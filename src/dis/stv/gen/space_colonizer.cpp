/*
 * space_colonizer.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#include "space_colonizer.h"

SpaceColonizer::SpaceColonizer(int segmentLength):data(new DrawableData()),segLen(segmentLength)
{
}

Digitizer::~Digitizer()
{
}

SpaceColonizer::~SpaceColonizer()
{
	if(this->data != NULL)
	{
//		DiscursivePrint("Destroying SpaceColonizer\n");
		delete(this->data);
		this->data = NULL;
	}
}

// Updates center of mass by removing values of child
void SpaceColonizer::removeNodeFromSubtreeCenterOfMass(SurrogateTreeNode* source, SurrogateTreeNode* node)
{
	int childWeight = atoi(node->data[TreeNodeKey::SIZE].c_str());
	double childX = atof(node->data[TreeNodeKey::X].c_str());
	double childY = atof(node->data[TreeNodeKey::Y].c_str());

	int scomWeight = atoi(source->data[TreeNodeKey::SCOMWEIGHT].c_str());
	double scomX = atof(source->data[TreeNodeKey::SCOMX].c_str());
	double scomY = atof(source->data[TreeNodeKey::SCOMY].c_str());

	int newWeight = scomWeight - childWeight;
	int newX = atof(source->data[TreeNodeKey::X].c_str());
	int newY = atof(source->data[TreeNodeKey::Y].c_str());
	if(newWeight > 0)
	{
		newX = (scomX * scomWeight - childX * childWeight)/newWeight;
		newY = (scomY * scomWeight - childY * childWeight)/newWeight;
	}

	// Save new SCoM values
	source->set(TreeNodeKey::SCOMX, boost::lexical_cast<string>(newX));
	source->set(TreeNodeKey::SCOMY, boost::lexical_cast<string>(newY));
	source->set(TreeNodeKey::SCOMWEIGHT,boost::lexical_cast<string>(newWeight));
}

// Puts value of SCoM(x,y) under TreeNodeKey::SCOMX and TreeNodeKey::SCOMY parameters resp.
// Total mass of children is stored under TreeNodeKey::SCOMWEIGHT
void SpaceColonizer::calculateSubtreeCenterOfMass(SurrogateTreeNode* source)
{
	// Start with own location as SCoM
	// This is the degenerate case for leaf nodes
	double scomX = atof(source->data[TreeNodeKey::X].c_str());
	double scomY = atof(source->data[TreeNodeKey::Y].c_str());
	int totalWeight = 0;
	if(source->children->size() > 0)
	{
		// To store values for averaging
		int childWeight = 0;
		double totalX = 0;
		double totalY = 0;
		// Calculate SCoM for each child
		SurrogateTreeNode* child;
		for(vector<SurrogateTreeNode*>::iterator iter = source->children->begin(); iter != source->children->end(); ++iter)
		{
			child = *iter;
			childWeight = atoi(child->data[TreeNodeKey::SIZE].c_str());
			this->calculateSubtreeCenterOfMass(child);
			totalWeight += childWeight;
			totalX += (atof(child->data[TreeNodeKey::X].c_str()) * childWeight);
			totalY += (atof(child->data[TreeNodeKey::Y].c_str()) * childWeight);
		}
		// Average together <ScoM, weight> child pairs
		scomX = totalX / totalWeight;
		scomY = totalY / totalWeight;
	}
	// Save SCoM values
	source->set(TreeNodeKey::SCOMX, boost::lexical_cast<string>(scomX));
	source->set(TreeNodeKey::SCOMY, boost::lexical_cast<string>(scomY));
	source->set(TreeNodeKey::SCOMWEIGHT, boost::lexical_cast<string>(totalWeight));
}

double SpaceColonizer::angleFrom(double aX, double aY, double bX, double bY)
{
	double attractorAngle;
	double dx = bX - aX;
	double dy = bY - aY; // Has to be flipped because of stupid drawing orientation rules (y increases down)
	double dist = sqrt(dx * dx + dy * dy);
	if(dist != 0)
	{
		attractorAngle = acos(dx/dist);
		if(dy < 0)
		{
			attractorAngle = 2 * 3.14159 - attractorAngle;
		}
	}
	return attractorAngle;
}

double SpaceColonizer::orientationBetween(SurrogateTreeNode* attractor, ColonizationLeader* leader)
{
	double leaderX = leader->getLocationX();
	double leaderY = leader->getLocationY();
	double attractorX = atof(attractor->data[TreeNodeKey::X].c_str());
	double attractorY = atof(attractor->data[TreeNodeKey::Y].c_str());

	return this->angleFrom(leaderX, -leaderY, attractorX, -attractorY);
}

bool SpaceColonizer::shouldSplit(SurrogateTreeNode* attractor, ColonizationLeader* leader)
{
	double currentOrientation = leader->getOrientation();
	// Split if angle from leader to attractor is
	// 'enough' more then current orientation
	double enough = 3.14159 * leader->getLength() / 12;
	if(enough > 3.14159 / 3)
	{
		enough = 3.14159 / 3;
	}

	double attractorAngle = orientationBetween(attractor, leader);

	return enough < fabs(attractorAngle - currentOrientation);
}

double SpaceColonizer::angleDiff(double ref, double compare)
{
	double TWO_PI = 2 * 3.14159;
	while(ref < 0)
	{
		ref += TWO_PI;
	}
	while(compare < 0)
	{
		compare += TWO_PI;
	}
	double diff = compare - ref;
	while(diff > TWO_PI)
	{
		diff -= TWO_PI;
	}

	return diff;
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
	if(source->children->size() > 1)
	{
		for(iter = source->children->begin(); iter < source->children->end(); ++iter)
		{
			attractor = *iter;
			if(this->shouldSplit(attractor,leader))
			{
				//printf("Adding leader\n");
				split = true;
				// Create new Leader with child set that caused split
				ColonizationLeader* newLeader = new ColonizationLeader(leader->getLocationX(),leader->getLocationY(),leader->getOrientation(), attractor);
//				double attractorAngle = orientationBetween(attractor, leader);
//				DiscursiveDebugPrint("Adding leader at (%f, %f) pointed at %f with SCoM @ (%f,%f)\n",leader->getLocationX(),leader->getLocationY(),attractorAngle, atof(attractor->data[TreeNodeKey::SCOMX].c_str()),atof(attractor->data[TreeNodeKey::SCOMY].c_str()));
//				ColonizationLeader* newLeader = new ColonizationLeader(leader->getLocationX(),leader->getLocationY(),attractorAngle, attractor);

				this->leaders.push_back(newLeader);
				modified = true;
				// Add debug marker for attractor
				//data->insert(DEBUG_LAYER,new DrawableDatum(atof(source->data[TreeNodeKey::X].c_str()), atof(source->data[TreeNodeKey::Y].c_str()), 0, 5, atoi(source->data[TreeNodeKey::SIZE].c_str())));
				break;
			}
		}
	}
	// Step or stop if we didn't split
	if(!split)
	{
//		leader->step(1.0);
		double x = leader->getLocationX();
		double y = leader->getLocationY();
		// If there are multiple targets, we always step
		if(source->children->size() > 1)
		{
			// Calculate angle towards subtree center of mass
			double childX = atof(source->data[TreeNodeKey::SCOMX].c_str());
			double childY = atof(source->data[TreeNodeKey::SCOMY].c_str());
			//double orientation = this->angleFrom(x,-y,childX,-childY);
			double orientation = this->angleFrom(x,-y,childX,-childY);
//			double maxAngleChange = 3.14159/(48.0 * log(fabs(leader->getLocationX() - atof(attractor->data[TreeNodeKey::X].c_str()))));
			double maxAngleChange = 3.14159 * leader->getLength() / 96.0;
			double angleDiff = this->angleDiff(leader->getOrientation(), orientation);
			if(fabs(angleDiff) > maxAngleChange)
			{
				if(angleDiff < 0)
				{
					orientation = leader->getOrientation() - maxAngleChange;
				}
				else
				{
					orientation = leader->getOrientation() + maxAngleChange;
				}
			}
			double dx = this->segLen * cos(orientation);
			double dy = -this->segLen * sin(orientation);

			// Insert new DrawDatum
			MinDrawableDatum* datum = (MinDrawableDatum*)malloc(sizeof(MinDrawableDatum));
			datum->locationX = x;
			datum->locationY = y;
			datum->angle = orientation;
			datum->extent = this->segLen;
			datum->mass = atoi(source->data[TreeNodeKey::SIZE].c_str());
			//new MinDrawableDatum(x, y, orientation, this->segLen, atoi(source->data[TreeNodeKey::SIZE].c_str()));
			data->insert(TRUNK_LAYER,datum);
			// Step leader towards SCoM
			leader->setLocation(x+dx, y+dy);
			leader->setOrientation(orientation);
		}
		else
		{
			SurrogateTreeNode* child = NULL;
			if(source->children->size() > 0)
			{
				child = *(source->children->begin());
			}
			//double childX = atof(child->data[TreeNodeKey::X].c_str());
			//double childY = atof(child->data[TreeNodeKey::Y].c_str());
			double childX = atof(source->data[TreeNodeKey::SCOMX].c_str());
			double childY = atof(source->data[TreeNodeKey::SCOMY].c_str());
			double dx = childX - x;
			double dy = childY - y;
			// If there is only a single entry, we step if far enough
			if(sqrt(dx * dx + dy * dy) >= 2*this->segLen)
			{
				//printf("Distance to CoM: %f\n",sqrt(dx * dx + dy * dy));
				// Calculate angle towards subtree center of mass
				//double orientation = this->angleFrom(x,-y,childX,-childY);
				double orientation = this->angleFrom(x,-y,childX,-childY);
				double maxAngleChange = 3.14159 * leader->getLength() /48.0;
				double angleDiff = this->angleDiff(leader->getOrientation(), orientation);
				if(fabs(angleDiff) > maxAngleChange)
				{
					if(angleDiff < 0)
					{
						orientation = leader->getOrientation() - maxAngleChange;
					}
					else
					{
						orientation = leader->getOrientation() + maxAngleChange;
					}
				}
				double ndx = this->segLen * cos(orientation);
				double ndy = -this->segLen * sin(orientation);

				// Insert new DrawDatum
				MinDrawableDatum* datum = (MinDrawableDatum*)malloc(sizeof(MinDrawableDatum));
				datum->locationX = x;
				datum->locationY = y;
				datum->angle = orientation;
				datum->extent = this->segLen;
				datum->mass = atoi(source->data[TreeNodeKey::SIZE].c_str());
				// new DrawableDatum(x, y, orientation, this->segLen, atoi(source->data[TreeNodeKey::SIZE].c_str()))
				data->insert(TRUNK_LAYER,datum);
				// Step leader towards SCoM
				leader->setLocation(x+ndx, y+ndy);
				leader->setOrientation(orientation);
			}
			// Otherwise we stop and remove leader from list
			else
			{
				// If single entry has children, update data set for leader
				// instead of removing
				if(child != NULL && child->children->size() > 0)
				{
					// Remove this child from its parent
					source->children->clear();
					// Redirect leader to follow child
					leader->setSourceSet(child);
					// Delete parent
					delete source;
				}
				// Otherwise remove
				else
				{
					//printf("Removing leader\n");
					DiscursiveDebugPrint("Removing leader at (%f, %f)\n",leader->getLocationX(),leader->getLocationY());
					vector<ColonizationLeader*>::iterator colonIter;
					for(colonIter = this->leaders.begin(); colonIter < this->leaders.end(); ++colonIter)
					{
						if(leader == *colonIter)
						{
							break;
						}
					}
					this->leaders.erase(colonIter);
					// Delete leader
					delete leader;
					// Delete node
					//delete source;
					modified = true;

					// Insert new Leaf DrawDatum
					DiscursiveDebugPrint("Adding leaf @ (%f,%f)\n",x,y);
					MinDrawableDatum* datum = (MinDrawableDatum*)malloc(sizeof(MinDrawableDatum));
					datum->locationX = x;
					datum->locationY = y;
					datum->angle = 0;
					datum->extent = this->segLen;
					datum->mass = this->segLen;
					// new DrawableDatum(x, y, 0, this->segLen, this->segLen)
					data->insert(LEAF_LAYER,datum);
					// Debugging
					//data->insert(DEBUG_LAYER,new DrawableDatum(childX, childY, 0, 5, 1));
				}
			}
		}
	}
	else
	{
		// Update source SCoM
		this->removeNodeFromSubtreeCenterOfMass(source, attractor);
		// Remove from child list
		source->children->erase(iter);
	}
	return modified;
}

DrawableData* SpaceColonizer::digitize(SurrogateTreeNode* source)
{
	// Initialize drawable data set

	// Precalculate subtree center of mass (SCoM) for each node
	this->calculateSubtreeCenterOfMass(source);

	// Add first leader
	double x = atof(source->data[TreeNodeKey::X].c_str());
	double y = atof(source->data[TreeNodeKey::Y].c_str());
	DiscursiveDebugPrint("Adding starter at (%f, %f) pointed at %f with SCoM @ (%f,%f)\n",x,y,3.14159/2, atof(source->data[TreeNodeKey::SCOMX].c_str()),atof(source->data[TreeNodeKey::SCOMY].c_str()));
	ColonizationLeader* starter = new ColonizationLeader(x,y,3.14159/2,source);
	this->leaders.push_back(starter);

	// Run stepOrSplit() over each leader while there are leaders
	ColonizationLeader* leader;
	while(this->leaders.size() > 0)
	{
		bool restart = false;
		//printf("Leaders: %d\n", this->leaders.size());
		for(vector<ColonizationLeader*>::iterator iter = this->leaders.begin(); iter < this->leaders.end(); ++iter)
		{
			leader = *iter;
			// If leader list changed
			if(this->stepOrSplit(data,leader))
			{
				// Restart loop
				restart = true;
				break;
			}
		}
//		if(restart)
//		{
//			if(leader == NULL)
//			{
//				DiscursivePrint("Leader is null\n");
//				fflush(stdout);
//			}
//			else
//			{
//				delete(leader);
//			}
//		}
	}

	return data;
}
