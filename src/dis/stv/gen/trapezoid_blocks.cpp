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
	double attractorAngle = 0.0;
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
	double enough = 3.14159 / 2.3;
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
	double STEP_SIZE = 1.0;
	bool shouldModify = false;

	SurrogateTreeNode* source = leader->getSourceSet();
//	printf("Source contains %d subtrees\n",source->children->size());
	fflush(stdout);
	double scomX = atof(source->data[TreeNodeKey::SCOMX].c_str());
	double scomY = atof(source->data[TreeNodeKey::SCOMY].c_str());
	if(!(scomX == leader->getBaseX() && scomY == leader->getBaseY()))
	{
		double x = leader->getX();
		double y = leader->getY();
		double dx = scomX - x;
		double dy = scomY - y;
		double dist = sqrt(dx*dx + dy*dy);

		double bx = leader->getBaseX();
		double by = leader->getBaseY();
		double dbx = scomX - bx;
		double dby = scomY - by;
		double bdist = sqrt(dbx*dbx + dby*dby);

	//	printf("Distance(<%d[%d],%d[%d]> -> <%d,%d>) @ %f is %f\n",(int)x,(int)leader->getBaseX(),(int)y,(int)leader->getBaseY(),(int)scomX,(int)scomY,leader->getOrientation(),dist);

		shouldModify = (dist >= (2.5 * STEP_SIZE) && bdist >= (5.0 * STEP_SIZE));
	}

	if(shouldModify)
	{
		// If we have children, check to see if we need to split
		// one off to a new leader
		if(source->children->size() > 0)
		{
			bool split = false;
			bool leaf;
			SurrogateTreeNode* attractor;
			vector<SurrogateTreeNode*>::iterator iter;
			for(iter = source->children->begin(); iter < source->children->end(); ++iter)
			{
				attractor = *iter;
				leaf = (attractor->children->size() == 0);
				// Always split off leaves (0 children)
				split = this->shouldSplit(attractor,leader) || leaf;
				if(split)
				{
					if(!leaf)
					{
						double attractorX = atof(attractor->data[TreeNodeKey::SCOMX].c_str());
						double attractorY = atof(attractor->data[TreeNodeKey::SCOMY].c_str());

						// Create new Leader with child set that caused split
						double attractorAngle = orientationBetweenSubtree(attractor, leader);
						if(attractorAngle == 0)
						{
							attractorAngle = leader->getOrientation();
						}
						int subtreeMass = atoi(attractor->data[TreeNodeKey::SIZE].c_str());

						// Calculate trunk width offset
						double trunkWidth = leader->getBaseWidth() / 2;

						double trunkOrthOrientation = 0;
						double orientation = leader->getOrientation();
						if(attractorAngle < orientation)
						{
							trunkOrthOrientation = orientation - (3.14159/2);
							if(trunkOrthOrientation < 0)
							{
								trunkOrthOrientation += 2 * 3.14159;
							}
						}
						else
						{
							trunkOrthOrientation = orientation + (3.14159/2);
							if(trunkOrthOrientation > 2 * 3.14159)
							{
								trunkOrthOrientation -= 2 * 3.14159;
							}
						}
						// Still something wrong with layout distances
						double dx = attractorX - leader->getX();
						double dy = attractorY - leader->getY();
						if(sqrt(dx * dx + dy * dy) < 2)
						{
							attractorAngle = trunkOrthOrientation;
						}

						double _x = leader->getX();// + (trunkWidth * cos(trunkOrthOrientation));
						double _y = leader->getY();// + (trunkWidth * sin(trunkOrthOrientation));

						printf("Adding leader (%s/%s) at (%f, %f) toward %f with SCoM @ (%f,%f)\n",attractor->data[TreeNodeKey::NAME].c_str(),source->data[TreeNodeKey::NAME].c_str(),_x,_y,attractorAngle,attractorX, attractorY);
						TrapezoidLeader* newLeader = new TrapezoidLeader(_x,_y, subtreeMass, attractorAngle, trunkWidth, attractor);
						this->initializeLeader(newLeader);
						// Only allow 1 new leader per step
					}
					break;
				}
			}
			if(split)
			{
				// Remove child we spawned a new leader with
				source->children->erase(iter);
				if(leaf)
				{
					leader->leaves->push_back(attractor);
				}
			}
			// Step leader
			leader->step(STEP_SIZE);
		}
		else
		{
			shouldModify = false;
		}

	}
	if(!shouldModify)
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
					double attractorX = atof(attractor->data[TreeNodeKey::SCOMX].c_str());
					double attractorY = atof(attractor->data[TreeNodeKey::SCOMY].c_str());
					printf("Hit CoM.  Adding leader (%s/%s) at (%f, %f) pointed at %f with SCoM @ (%f,%f)\n",attractor->data[TreeNodeKey::NAME].c_str(),source->data[TreeNodeKey::NAME].c_str(),leader->getX(),leader->getY(),attractorAngle,attractorX, attractorY);
					int subtreeMass = atoi(attractor->data[TreeNodeKey::SIZE].c_str());
					endMass += subtreeMass;
					TrapezoidLeader* newLeader = new TrapezoidLeader(leader->getX(),leader->getY(), subtreeMass, attractorAngle, 0, attractor);
					this->initializeLeader(newLeader);
//					this->leaders.push_back(newLeader);
				}
				else
				{
					leader->leaves->push_back(attractor);
				}
			}
			if(endMass < 2)
			{
				endMass = 2;
			}
			if(endMass > leader->getBaseWidth())
			{
				endMass = leader->getBaseWidth();
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
//	printf("Adding starter at (%f, %f) pointed at %f with SCoM @ (%f,%f)\n",x,y,attractorAngle, attractorX, attractorY);

	if(attractorAngle == 0)
	{
		attractorAngle = (3.14159/2);
	}

	TrapezoidLeader* starter = new TrapezoidLeader(x,y,atoi(source->data[TreeNodeKey::SIZE].c_str()),attractorAngle,0,source);
	this->initializeLeader(starter);

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

void TrapezoidBlocks::drawBranch(TrapezoidLeader* leader, double startX, double startY, double orientation, double branchOrientation, double lengthPerLeaf, int leaves, double leafBranchSpacing, double growthUnit)
{

	bool left = true;
	int length = leaves * lengthPerLeaf;
	// Add approx width of this branch
	length += (int)leader->getBaseWidth();


//	printf("Drawing branch (%d) @ (%f,%f) of length %d towards %f\n", leaves, startX, startY, length, branchOrientation);


	MinDrawableDatum* datum;

	// Add stem for leaves
	datum = (MinDrawableDatum*)malloc(sizeof(MinDrawableDatum));
	datum->locationX = startX;
	datum->locationY = startY;
	datum->angle = branchOrientation;
	datum->extent = length;
//	datum->mass = leader->getEndWidth();
	datum->mass = 2.0 * leaves;
	if(datum->mass > 4.0 )
	{
		datum->massRatio = (4.0/datum->mass);
	}
	else
	{
		datum->massRatio = 1;
	}
//	printf("Adding draw object at (%d,%d) with length %d pointed at %f\n",datum->locationX,datum->locationY, datum->extent,datum->angle);
	this->data->insert(TRUNK_LAYER,datum);

	// Insert leaves
	double xShift = growthUnit * cos(branchOrientation);
	double yShift = -growthUnit * sin(branchOrientation);
	double leafX = growthUnit * cos(orientation);
	double leafY = -growthUnit * sin(orientation);

	startX += (length * cos(branchOrientation));  // Make local adjustment to start position
	startY -= (length * sin(branchOrientation));  // Make local adjustment to start position

	// Odd number of leaves?
	int leafModifier = 1;
	if(leafModifier == 1)
	{
		// End leaf
		datum = (MinDrawableDatum*)malloc(sizeof(MinDrawableDatum));
		datum->mass = growthUnit;

		datum->locationX = startX + xShift;
		datum->locationY = startY + yShift;
		datum->angle = branchOrientation;
		datum->extent = length;
		this->data->insert(LEAF_LAYER,datum);
	}
	// Side leaves
	for(int i = 0; i < leaves - leafModifier; i++)
	{
		datum = (MinDrawableDatum*)malloc(sizeof(MinDrawableDatum));
		datum->mass = growthUnit;
		datum->locationX = startX					// Start reference
				- (xShift * (1 + 2 * (i / 2)));		// Shift "down" branch
		if(left)
		{
			datum->locationX += leafX;
		}
		else
		{
			datum->locationX -= leafX;
		}
		datum->locationY = startY					// Start reference
				- (yShift * (1 + 2 * (i / 2)));		// Shift "down" branch
		if(left)
		{
			datum->locationY += leafY;
		}
		else
		{
			datum->locationY -= leafY;
		}
		datum->angle = branchOrientation;
		datum->extent = length;
		data->insert(LEAF_LAYER,datum);
		left = !left;
	}
}

void TrapezoidBlocks::drawBranches(TrapezoidLeader* leader, int leavesPerBranch, double lengthPerLeaf)
{
	SurrogateTreeNode* source = leader->getSourceSet();

	double growthUnit = 50.0;
	double leafBranchSpacing = 2 * growthUnit;

	double orientation = leader->getOrientation();
	double intro = this->branchIntro(leader,growthUnit);
	double startX = leader->getBaseX() + (intro * cos(orientation));
	double startY = leader->getBaseY() - (intro * sin(orientation));

	int leaves = 0;
	SurrogateTreeNode* node;

	bool left = (orientation < (3.14159/2));
	double branchOrientation;
	for(vector<SurrogateTreeNode*>::iterator iter = leader->leaves->begin(); iter != leader->leaves->end(); ++iter)
	{
		node = *iter;
//		if(node->children->size() == 0)
//		{
//			// Note new leaf
			leaves++;
			// Draw leaves + branch
			if(leaves == leavesPerBranch)
			{
				if(left)
				{
					branchOrientation = orientation - (3.14159/2);
					if(branchOrientation < 0)
					{
						branchOrientation += 2 * 3.14159;
					}
				}
				else
				{
					branchOrientation = orientation + (3.14159/2);
					if(branchOrientation > 2 * 3.14159)
					{
						branchOrientation -= 2 * 3.14159;
					}
				}
				// Shift start location
				startX += (leafBranchSpacing * cos(orientation));
				startY -= (leafBranchSpacing * sin(orientation));
				// Draw branch
				this->drawBranch(leader, startX, startY, orientation, branchOrientation, lengthPerLeaf, leaves, leafBranchSpacing, growthUnit);
				// Swap sides
				left = !left;
				// Reset count
				leaves = 0;
			}
//		}
	}
	if(leaves > 0)
	{
		if(left)
		{
			branchOrientation = orientation - (3.14159/2);
			if(branchOrientation < 0)
			{
				branchOrientation += 2 * 3.14159;
			}
		}
		else
		{
			branchOrientation = orientation + (3.14159/2);
			if(branchOrientation > 2 * 3.14159)
			{
				branchOrientation -= 2 * 3.14159;
			}
		}
		// Shift start location
		startX += (leafBranchSpacing * cos(orientation));
		startY -= (leafBranchSpacing * sin(orientation));
		// Draw branch
		this->drawBranch(leader, startX, startY, orientation, branchOrientation, lengthPerLeaf, leaves, leafBranchSpacing, growthUnit);
	}
}

double TrapezoidBlocks::branchIntro(TrapezoidLeader* leader, double growthUnit)
{
	double initialSpacer = 7.5 * growthUnit;
	return initialSpacer;
}

double TrapezoidBlocks::leafSpacing(TrapezoidLeader* leader, double growthUnit)
{
	SurrogateTreeNode* source = leader->getSourceSet();
	double leavesPerBranch = 5.0;
	double leafBranchSpacing = 2 * growthUnit;
	double branchSpacer = ceil(((source->children->size() / leavesPerBranch) + 1) * leafBranchSpacing);
	return branchSpacer;
}

void TrapezoidBlocks::initializeLeader(TrapezoidLeader* leader)
{
	double lengthPerLeaf = 50.0;

	// Set initial step where leaves will go
	leader->step(this->branchIntro(leader,lengthPerLeaf) + this->leafSpacing(leader,lengthPerLeaf));

	this->leaders.push_back(leader);
}

void TrapezoidBlocks::finalizeLeader(TrapezoidLeader* leader)
{
//	printf("Finalizing leader\n");
//	fflush(stdout);
	double lengthPerLeaf = 50.0;
	this->drawBranches(leader,5,lengthPerLeaf);


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
//	printf("Adding leader branch at (%d,%d) with dims [l: %d, m: %d, r: %f] pointed at %f\n",datum->locationX,datum->locationY, datum->extent, datum->mass, datum->massRatio, datum->angle);
	data->insert(TRUNK_LAYER,datum);

	// Clear node
	delete source;

	// Delete leader
	delete leader;
}
