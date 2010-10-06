/*
 * trapezoid_blocks.cpp
 *
 *  Created on: Oct 4, 2010
 *      Author: Mark Christensen
 */

#include "./trapezoid_blocks.h"

TrapezoidBlocks::TrapezoidBlocks():data(new DrawableData())
{
}

TrapezoidBlocks::~TrapezoidBlocks()
{
	if(this->data != NULL)
	{
		delete(this->data);
		this->data = NULL;
	}
}

double TrapezoidBlocks::angleFrom(double aX, double aY, double bX, double bY)
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
			attractorAngle = 2 * 3.14159265 - attractorAngle;
		}
	}
	return attractorAngle;
}

double TrapezoidBlocks::orientationBetween(SurrogateTreeNode* attractor, TrapezoidLeader* leader)
{
	double leaderX = leader->getX();
	double leaderY = leader->getY();
	double attractorX = atof(attractor->data[TreeNodeKey::X].c_str());
	double attractorY = atof(attractor->data[TreeNodeKey::Y].c_str());

	return this->angleFrom(leaderX, -leaderY, attractorX, -attractorY);
}

double TrapezoidBlocks::orientationBetweenSubtree(SurrogateTreeNode* attractor, TrapezoidLeader* leader)
{
	double leaderX = leader->getX();
	double leaderY = leader->getY();
	double attractorX = atof(attractor->data[TreeNodeKey::SCOMX].c_str());
	double attractorY = atof(attractor->data[TreeNodeKey::SCOMY].c_str());

	return this->angleFrom(leaderX, -leaderY, attractorX, -attractorY);
}

bool TrapezoidBlocks::shouldSplit(SurrogateTreeNode* attractor, TrapezoidLeader* leader)
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

double TrapezoidBlocks::angleDiff(double ref, double compare)
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

// Returns true if this leader was stepped.
bool TrapezoidBlocks::step(DrawableData* data, TrapezoidLeader* leader)
{
	bool shouldModify;

	SurrogateTreeNode* source = leader->getSourceSet();
//	printf("Source contains %d subtrees\n",source->children->size());
	fflush(stdout);
	double scomX = atof(source->data[TreeNodeKey::SCOMX].c_str());
	double scomY = atof(source->data[TreeNodeKey::SCOMY].c_str());
//	double x =  atof(source->data[TreeNodeKey::X].c_str());
//	double y =  atof(source->data[TreeNodeKey::Y].c_str());
	double x = leader->getX();
	double y = leader->getY();
	double dx = scomX - x;
	double dy = scomY - y;
	double dist = sqrt(dx*dx + dy*dy);

//	printf("Distance(<%d,%d> -> <%d,%d>) @ %f is %f\n",(int)x,(int)y,(int)scomX,(int)scomY,leader->getOrientation(),dist);

	shouldModify = (dist >= 2.0);

	if(shouldModify)
	{
		// If we have children, check to see if we need to split
		// one off to a new leader
		if(source->children->size() > 0)
		{
			bool split = false;
			SurrogateTreeNode* attractor;
			vector<SurrogateTreeNode*>::iterator iter;
			for(iter = source->children->begin(); iter < source->children->end(); ++iter)
			{
				attractor = *iter;
				split = this->shouldSplit(attractor,leader); // || attractor->children->size() == 0;
				if(split)
				{
					if(attractor->children->size() > 0)
					{
						double attractorX = atof(source->data[TreeNodeKey::SCOMX].c_str());
						double attractorY = atof(source->data[TreeNodeKey::SCOMY].c_str());

						// Create new Leader with child set that caused split
						double attractorAngle = orientationBetweenSubtree(attractor, leader);
						int subtreeMass = atoi(source->data[TreeNodeKey::SIZE].c_str());
//						printf("Adding new leader at (%f, %f) pointed at %f with SCoM @ (%f,%f)\n",leader->getX(),leader->getY(),attractorAngle,attractorX, attractorY);
						TrapezoidLeader* newLeader = new TrapezoidLeader(leader->getX(),leader->getY(), subtreeMass, attractorAngle, attractor);

						this->leaders.push_back(newLeader);
						// Only allow 1 new leader per step
					}
					break;
				}
			}
			if(split)
			{
				// Remove child we spawned a new leader with
				source->children->erase(iter);
			}
		}
		// Step leader
		leader->step(1.0);
	}
	else
	{
		// If we are at the CoM, split off remaining children with leaders
		if(source->children->size() > 0)
		{
			double attractorAngle;
			SurrogateTreeNode* attractor;
			int endMass = 0;
			for(vector<SurrogateTreeNode*>::iterator iter = source->children->begin(); iter < source->children->end(); ++iter)
			{
				attractor = *iter;
				if(attractor->children->size() > 0)
				{
					// Create new Leader with child set that caused split
					attractorAngle = orientationBetweenSubtree(attractor, leader);
					double attractorX = atof(source->data[TreeNodeKey::SCOMX].c_str());
					double attractorY = atof(source->data[TreeNodeKey::SCOMY].c_str());
//					printf("Adding new leader at (%f, %f) pointed at %f with SCoM @ (%f,%f)\n",leader->getX(),leader->getY(),attractorAngle,attractorX, attractorY);
					int subtreeMass = atoi(source->data[TreeNodeKey::SIZE].c_str());
					endMass += subtreeMass;
					TrapezoidLeader* newLeader = new TrapezoidLeader(leader->getX(),leader->getY(), subtreeMass, attractorAngle, attractor);
					this->leaders.push_back(newLeader);
				}
			}
			leader->setEndWidth(endMass);
			source->children->clear();
		}
	}
	// Return true only if we modified this leader
	return shouldModify;
}

DrawableData* TrapezoidBlocks::digitize(SurrogateTreeNode* source)
{
	// Add first leader
	double x = atof(source->data[TreeNodeKey::X].c_str());
	double y = atof(source->data[TreeNodeKey::Y].c_str());

	double attractorX = atof(source->data[TreeNodeKey::SCOMX].c_str());
	double attractorY = atof(source->data[TreeNodeKey::SCOMY].c_str());

	double attractorAngle = this->angleFrom(x, -y, attractorX, -attractorY);
	DiscursiveDebugPrint("trapezoid_blocks","Adding starter at (%f, %f) pointed at %f with SCoM @ (%f,%f)\n",x,y,attractorAngle, attractorX, attractorY);

	TrapezoidLeader* starter = new TrapezoidLeader(x,y,atoi(source->data[TreeNodeKey::SIZE].c_str()),attractorAngle,source);
	this->leaders.push_back(starter);

	// Run step() over each leader while there are leaders
	TrapezoidLeader* leader;
	unsigned int oldSize;
	list<TrapezoidLeader*>::iterator iter;
	bool removed;
	while(this->leaders.size() > 0)
	{
//		printf("Loop start %d\n",this->leaders.size());
//		fflush(stdout);
		removed = false;
//		printf("Loop begins at %p\n",this->leaders.begin());
		for(iter = this->leaders.begin(); iter != this->leaders.end(); ++iter)
		{
//			printf("Step\n");
			fflush(stdout);
			leader = *iter;
			oldSize = this->leaders.size();
			if(!this->step(data,leader))
			{
//				printf("Removing leader\n");
				fflush(stdout);
				this->finalizeLeader(leader);
				removed = true;
				break;
			}
			if(oldSize != this->leaders.size())
			{
				break;
			}
		}
		if(removed)
		{
			this->leaders.erase(iter);
		}
	}

	return data;
}

void TrapezoidBlocks::finalizeLeader(TrapezoidLeader* leader)
{
//	printf("Finalizing leader\n");
	fflush(stdout);
	SurrogateTreeNode* source = leader->getSourceSet();
	// Remove all references to the children
	source->children->clear();
	leader->setSourceSet(NULL);

	// Create draw objects from leader
	MinDrawableDatum* datum = (MinDrawableDatum*)malloc(sizeof(MinDrawableDatum));
	datum->locationX = (int)leader->getBaseX();
	datum->locationY = (int)leader->getBaseY();
	datum->angle = leader->getOrientation();
	datum->extent = leader->getLength();
	datum->mass = leader->getBaseWidth();
	datum->massRatio = (leader->getEndWidth()/datum->mass);
//	printf("Adding draw object at (%d,%d) with length %d pointed at %f\n",datum->locationX,datum->locationY, datum->extent,datum->angle);
	data->insert(TRUNK_LAYER,datum);

	// Clear node
	delete source;

	// Delete leader
	delete leader;
}
