/*
 * space_colonizer.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#include "space_colonizer.h"

SpaceColonizer::SpaceColonizer(int segmentLength):segLen(segmentLength)
{
}

// Updates center of mass by removing values of child
void SpaceColonizer::removeNodeFromSubtreeCenterOfMass(SurrogateTreeNode* source, SurrogateTreeNode* node)
{
	int childWeight = atoi(node->data["size"].c_str());
	double childX = atof(node->data["x"].c_str());
	double childY = atof(node->data["y"].c_str());

	int scomWeight = atoi(source->data["scomWeight"].c_str());
	double scomX = atof(source->data["scomX"].c_str());
	double scomY = atof(source->data["scomY"].c_str());

	int newWeight = scomWeight - childWeight;
	int newX = atof(source->data["x"].c_str());
	int newY = atof(source->data["y"].c_str());
	if(newWeight > 0)
	{
		newX = (scomX * scomWeight - childX * childWeight)/newWeight;
		newY = (scomY * scomWeight - childY * childWeight)/newWeight;
	}

	// Save new SCoM values
	source->data["scomX"] = boost::lexical_cast<string>(newX);
	source->data["scomY"] = boost::lexical_cast<string>(newY);
	source->data["scomWeight"] = boost::lexical_cast<string>(newWeight);
}

// Puts value of SCoM(x,y) under "scomX" and "scomY" parameters resp.
// Total mass of children is stored under "scomWeight"
void SpaceColonizer::calculateSubtreeCenterOfMass(SurrogateTreeNode* source)
{
	// Start with own location as SCoM
	// This is the degenerate case for leaf nodes
	double scomX = atof(source->data["x"].c_str());
	double scomY = atof(source->data["y"].c_str());
	int totalWeight = 0;
	if(source->children.size() > 0)
	{
		// To store values for averaging
		int childWeight = 0;
		double totalX = 0;
		double totalY = 0;
		// Calculate SCoM for each child
		SurrogateTreeNode* child;
		for(vector<SurrogateTreeNode*>::iterator iter = source->children.begin(); iter != source->children.end(); ++iter)
		{
			child = *iter;
			childWeight = atoi(child->data["size"].c_str());
			this->calculateSubtreeCenterOfMass(child);
			totalWeight += childWeight;
			totalX += (atof(child->data["scomX"].c_str()) * childWeight);
			totalY += (atof(child->data["scomY"].c_str()) * childWeight);
		}
		// Average together <ScoM, weight> child pairs
		scomX = totalX / totalWeight;
		scomY = totalY / totalWeight;
	}
	// Save SCoM values
	source->data["scomX"] = boost::lexical_cast<string>(scomX);
	source->data["scomY"] = boost::lexical_cast<string>(scomY);
	source->data["scomWeight"] = boost::lexical_cast<string>(totalWeight);
}

double SpaceColonizer::angleFrom(double aX, double aY, double bX, double bY)
{
	double attractorAngle;
	double dx = bX - aX;
	double dy = bY - aY;
	double dist = sqrt(dx * dx + dy * dy);
	if(dist != 0)
	{
		attractorAngle = acos(dx/dist);
		if(dy < 0)
		{
			attractorAngle = 2 * 3.14159 - attractorAngle;
		}
	}
//	if(dx != 0)
//	{
//		if(dy > 0)
//		{
//			attractorAngle = atan(dy/dx);
//		}
//		else if(dy < 0)
//		{
//			attractorAngle = 3.14159 - atan(dy/dx);
//		}
//		else
//		{
//			if(dx > 0)
//			{
//				attractorAngle = 0;
//			}
//			else
//			{
//				attractorAngle = 3.14159;
//			}
//		}
//	}
//	else
//	{
//		if(dy > 0)
//		{
//			attractorAngle = 3.14159 / 2;
//		}
//		else if(dy < 0)
//		{
//			attractorAngle = 3 * 3.14159 / 4;
//		}
//		else
//		{
//			attractorAngle = 0;
//		}
//	}
	return attractorAngle;
}

double SpaceColonizer::orientationBetween(SurrogateTreeNode* attractor, ColonizationLeader* leader)
{
	double leaderX = leader->getLocationX();
	double leaderY = leader->getLocationY();
	double attractorX = atof(attractor->data["x"].c_str());
	double attractorY = atof(attractor->data["y"].c_str());

	return this->angleFrom(leaderX, leaderY, attractorX, attractorY);
}

bool SpaceColonizer::shouldSplit(SurrogateTreeNode* attractor, ColonizationLeader* leader)
{
	double currentOrientation = leader->getOrientation();
	// Split if angle from leader to attractor is
	// 'enough' more then current orientation
	double enough = 3.14159 / 3;

	double attractorAngle = orientationBetween(attractor, leader);

	return enough < fabs(attractorAngle - currentOrientation);
}

// Returns true if # of leaders changed
bool SpaceColonizer::stepOrSplit(DrawableData* data, ColonizationLeader* leader)
{
	bool modified = false;
	SurrogateTreeNode* source = leader->getSourceSet();
	//printf("Leader has %d attractors.\n", source->children.size());
	// Look at all attractors and determine if a split should occur
	// or if the leader should just "step"
	bool split = false;
	SurrogateTreeNode* attractor;
	vector<SurrogateTreeNode*>::iterator iter;
	if(source->children.size() > 1)
	{
		for(iter = source->children.begin(); iter < source->children.end(); ++iter)
		{
			attractor = *iter;
			if(this->shouldSplit(attractor,leader))
			{
				//printf("Adding leader\n");
				split = true;
				// Create new Leader with child set that caused split
				double attractorAngle = orientationBetween(attractor, leader);
				printf("Adding leader at (%f, %f) pointed at %f with SCoM @ (%f,%f)\n",leader->getLocationX(),leader->getLocationY(),attractorAngle, atof(attractor->data["scomX"].c_str()),atof(attractor->data["scomY"].c_str()));
				ColonizationLeader* newLeader = new ColonizationLeader(leader->getLocationX(),leader->getLocationY(),attractorAngle, attractor);
				this->leaders.push_back(newLeader);
				modified = true;
				break;
			}
		}
	}
	// Step or stop if we didn't split
	if(!split)
	{
		double x = leader->getLocationX();
		double y = leader->getLocationY();
		// If there are multiple targets, we always step
		if(source->children.size() > 1)
		{
			// Calculate angle towards subtree center of mass
			double childX = atof(source->data["scomX"].c_str());
			double childY = atof(source->data["scomY"].c_str());
			double orientation = this->angleFrom(x,y,childX,childY);
			double dx = this->segLen * cos(orientation);
			double dy = this->segLen * sin(orientation);

			// Insert new DrawDatum
			data->insert(TRUNK_LAYER,new DrawableDatum(x, y, orientation, this->segLen, atoi(source->data["size"].c_str())));
			// Step leader towards SCoM
			leader->setLocation(x+dx, y+dy);
			leader->setOrientation(orientation);
		}
		else
		{
			SurrogateTreeNode* child = NULL;
			if(source->children.size() > 0)
			{
				child = *(source->children.begin());
			}
			//double childX = atof(child->data["x"].c_str());
			//double childY = atof(child->data["y"].c_str());
			double childX = atof(source->data["scomX"].c_str());
			double childY = atof(source->data["scomY"].c_str());
			double dx = childX - x;
			double dy = childY - y;
			// If there is only a single entry, we step if far enough
			if(sqrt(dx * dx + dy * dy) >= this->segLen)
			{
				//printf("Distance to CoM: %f\n",sqrt(dx * dx + dy * dy));
				// Calculate angle towards subtree center of mass
				double orientation = this->angleFrom(x,y,childX,childY);
				double ndx = this->segLen * cos(orientation);
				double ndy = this->segLen * sin(orientation);

				// Insert new DrawDatum
				data->insert(TRUNK_LAYER,new DrawableDatum(x, y, orientation, this->segLen, atoi(source->data["size"].c_str())));
				// Step leader towards SCoM
				leader->setLocation(x+ndx, y+ndy);
				leader->setOrientation(orientation);
			}
			// Otherwise we stop and remove leader from list
			else
			{
				// If single entry has children, update data set for leader
				// instead of removing
				if(child != NULL && child->children.size() > 0)
				{
					leader->setSourceSet(child);
				}
				// Otherwise remove
				else
				{
					//printf("Removing leader\n");
					vector<ColonizationLeader*>::iterator colonIter;
					for(colonIter = this->leaders.begin(); colonIter < this->leaders.end(); ++colonIter)
					{
						if(leader == *colonIter)
						{
							break;
						}
					}
					this->leaders.erase(colonIter);
					modified = true;

					// Insert new Leaf DrawDatum
					printf("Adding leaf @ (%f,%f)\n",x,y);
					data->insert(LEAF_LAYER,new DrawableDatum(x, y, 0, this->segLen, this->segLen));
				}
			}
		}
	}
	else
	{
		// Update source SCoM
		this->removeNodeFromSubtreeCenterOfMass(source, attractor);
		// Remove from child list
		source->children.erase(iter);
	}
	return modified;
}

DrawableData* SpaceColonizer::digitize(SurrogateTreeNode* source)
{
	// Initialize drawable data set
	DrawableData* data = new DrawableData();

	// Precalculate subtree center of mass (SCoM) for each node
	this->calculateSubtreeCenterOfMass(source);

	// Add first leader
	ColonizationLeader* starter = new ColonizationLeader(0,0,3.14159/2,source);
	this->leaders.push_back(starter);

	// Run stepOrSplit() over each leader while there are leaders
	ColonizationLeader* leader;
	while(this->leaders.size() > 0)
	{
		//printf("Leaders: %d\n", this->leaders.size());
		for(vector<ColonizationLeader*>::iterator iter = this->leaders.begin(); iter < this->leaders.end(); ++iter)
		{
			leader = *iter;
			// If leader list changed
			if(this->stepOrSplit(data,leader))
			{
				// Restart loop
				break;
			}
		}
	}

	return data;
}
