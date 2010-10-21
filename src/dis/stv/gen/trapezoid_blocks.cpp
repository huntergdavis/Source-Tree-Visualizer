/*
 * trapezoid_blocks.cpp
 *
 *  Created on: Oct 4, 2010
 *      Author: Mark Christensen
 */

#include "./trapezoid_blocks.h"

TrapezoidBlocks::TrapezoidBlocks():data(new DrawableData()), colorMap(NULL)
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

void TrapezoidBlocks::setColorMap(unordered_map<string, Magick::Color*>* colorMap)
{
	this->colorMap = colorMap;
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
	double enough = 3.14159 / 5;
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

Magick::Color* TrapezoidBlocks::getColor(string color)
{
	Magick::Color* result = NULL;
	if(this->colorMap != NULL)
	{
		result = (*this->colorMap)[color];
	}
	DiscursiveDebugPrint("color","Color lookup: %s -> %p (%d,%d,%d)\n", color.c_str(), result, result->redQuantum(), result->greenQuantum(), result->blueQuantum());
	return result;
}

void TrapezoidBlocks::setDatumColor(MinDrawableDatum* datum, string color)
{
	if(this->colorMap != NULL && datum != NULL)
	{
		DiscursiveDebugPrint("color","Node(%s): Setting datum color to '%s'\n",datum->source->data[TreeNodeKey::NAME].c_str(),color.c_str());
		datum->color = (*this->colorMap)[color];
	}
	else
	{
		DiscursiveDebugPrint("color","Node(%s): Can't set color from '%s'\n",datum->source->data[TreeNodeKey::NAME].c_str(),color.c_str());
	}
}

void TrapezoidBlocks::drawBranch(TrapezoidLeader* leader, double startX, double startY, double orientation, double branchOrientation, double lengthPerLeaf, vector<SurrogateTreeNode*> leaves, double leafBranchSpacing, double growthUnit)
{

	bool left = true;
	int length = leaves.size() * lengthPerLeaf;
	// Add approx width of this branch
	length += 3 * log((int)leader->getBaseWidth() + 2);


//	printf("Drawing branch (%d) @ (%f,%f) of length %d towards %f\n", leaves, startX, startY, length, branchOrientation);


	MinDrawableDatum* datum;
	SurrogateTreeNode* node;
	vector<SurrogateTreeNode*>::iterator iter = leaves.begin();

	// Add stem for leaves
	datum = (MinDrawableDatum*)malloc(sizeof(MinDrawableDatum));
	datum->locationX = startX;
	datum->locationY = startY;
	datum->angle = branchOrientation;
	datum->extent = length;
	datum->source = leader->getSourceSet();
//	datum->mass = leader->getEndWidth();
	datum->mass = 2.0 * leaves.size();
	this->setDatumColor(datum, datum->source->data[TreeNodeKey::COLOR]);
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

	SurrogateTreeNode* curr;

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
		node = *iter;
		++iter;
		datum->source = node;
		this->setDatumColor(datum, datum->source->data[TreeNodeKey::COLOR]);
		this->data->insert(LEAF_LAYER,datum);
	}
	// Side leaves
//	for(int i = 0; i < leaves - leafModifier; i++)
	int i = 0;
	for(;iter != leaves.end(); ++iter)
	{
		node = *iter;
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
		datum->source = node;
		this->setDatumColor(datum, datum->source->data[TreeNodeKey::COLOR]);
		data->insert(LEAF_LAYER,datum);
		left = !left;
		i++;
	}
}

double TrapezoidBlocks::splitLocation(LeafSplit* leaf, double spacing, bool logGrowth)
{
	if(logGrowth)
	{
		// +1 keeps log(0) from occurring.
		return leaf->baseLength + (pow(log(leaf->subCount + 1),0.5) * spacing);
	}
	else
	{
		return leaf->baseLength + (leaf->subCount * spacing);
	}
}

void TrapezoidBlocks::drawMicroBranch(LeafSplit* leaf, double spacing, double growthUnit, Magick::Color* branchColor, Magick::Color* leafColor, bool logGrowth)
{
	MinDrawableDatum* datum;
	double stemLength = this->splitLocation(leaf, spacing, logGrowth) + leaf->baseLength;

	// Add stem
	datum = (MinDrawableDatum*)malloc(sizeof(MinDrawableDatum));
	datum->locationX = leaf->rootX;
	datum->locationY = leaf->rootY;
	datum->angle = leaf->orientation;
	datum->extent = stemLength;
	datum->mass = 3.0;
	datum->massRatio = 1;
	datum->source = NULL;
	datum->color = branchColor;
	//this->setColor(datum, datum->source->data[TreeNodeKey::COLOR]);
	this->data->insert(TRUNK_LAYER,datum);

	// Add leaf
	datum = (MinDrawableDatum*)malloc(sizeof(MinDrawableDatum));
	datum->locationX = leaf->rootX + ((stemLength + growthUnit) * cos(leaf->orientation));
	datum->locationY = leaf->rootY - ((stemLength + growthUnit) * sin(leaf->orientation));
	datum->angle = leaf->orientation;
	datum->extent = growthUnit;
	datum->mass = growthUnit;
	datum->source = NULL;
	datum->color = leafColor;
	this->data->insert(LEAF_LAYER,datum);
}

void TrapezoidBlocks::drawBranchAdv(TrapezoidLeader* leader, double startX, double startY, double orientation, double baseLength, double leafBranchSpacing, double growthUnit)
{
	//printf("Drawing microbranches for node '%s'\n", leader->getSourceSet()->data[TreeNodeKey::NAME].c_str());
	// Create fractal datastruct
	// and draw branches
	if(leader->leaves->size() > 0)
	{
		LeafSplit* head;
		LeafSplit* fract;
		LeafSplit* parent;
		queue<LeafSplit*> splits;
		double split;
		vector<SurrogateTreeNode*>::iterator iter = leader->leaves->begin();
		head = (LeafSplit*)malloc(sizeof(LeafSplit));
		memset(head,0,sizeof(LeafSplit));
		head->rootX = startX;
		head->rootY = startY;
		head->orientation = orientation;
		head->baseLength = baseLength;
		SurrogateTreeNode* curr = *iter;
		head->subCount = leader->leaves->size() - 1;
		splits.push(head);
		// Draw this new stem
		this->drawMicroBranch(head, leafBranchSpacing, growthUnit, this->getColor(leader->getSourceSet()->data[TreeNodeKey::COLOR]), this->getColor(curr->data[TreeNodeKey::COLOR]), true);
		// Move to second leaf in list
		++iter;
		// Loop over remaining children
		for(; iter != leader->leaves->end(); ++iter)
		{
			parent = splits.front();
			curr = *iter;
			if(parent == head)
			{
				split = this->splitLocation(parent,leafBranchSpacing,true);
			}
			else
			{
				split = this->splitLocation(parent,leafBranchSpacing/4);
			}
			fract = (LeafSplit*)malloc(sizeof(LeafSplit));
			memset(fract,0,sizeof(LeafSplit));
			splits.push(fract);
			fract->baseLength = baseLength;

			// Assign new node to parent
			// Remove parent from queue if both children are now set
			if(parent->left == NULL)
			{
				fract->subCount = ceil(parent->subCount / 2.0) - 1;
				fract->orientation = this->angleAdd(parent->orientation,3.14159/4);
				fract->rootX = parent->rootX + (split * cos(parent->orientation));
				fract->rootY = parent->rootY - (split * sin(parent->orientation));
				// Set new node to left child
				parent->left = fract;
			}
			else
			{
				fract->subCount = floor(parent->subCount / 2.0) - 1;
				fract->orientation = this->angleAdd(parent->orientation,-3.14159/4);
				fract->rootX = parent->rootX + (split * cos(parent->orientation));
				fract->rootY = parent->rootY - (split * sin(parent->orientation));
				// Set new node to right child
				parent->right = fract;
				// Remove parent from list
				splits.pop();
			}
			this->drawMicroBranch(fract, leafBranchSpacing/4, growthUnit, this->getColor(leader->getSourceSet()->data[TreeNodeKey::COLOR]), this->getColor(curr->data[TreeNodeKey::COLOR]));
		}
		// Empty stack
		while(!splits.empty())
		{
			splits.pop();
		}

		// Free allocated fractal objects
		this->reclaim(head);
	}
}

void TrapezoidBlocks::reclaim(LeafSplit* leaf)
{
	if(leaf != NULL)
	{
		this->reclaim(leaf->left);
		leaf->left = NULL;
		this->reclaim(leaf->right);
		leaf->right = NULL;
		free(leaf);
	}
}

double TrapezoidBlocks::angleAdd(double origin, double addAmount)
{

	double newAngle = origin + addAmount;
	if(newAngle < 0)
	{
		newAngle += 2 * 3.14159;
	}
	if(newAngle > (2 * 3.14159))
	{
		newAngle -= 2 * 3.14159;
	}

	return newAngle;
}

void TrapezoidBlocks::drawBranches(TrapezoidLeader* leader, int leavesPerBranch, double lengthPerLeaf)
{
	SurrogateTreeNode* source = leader->getSourceSet();

	double growthUnit = 20.0;
	double leafBranchSpacing = 2 * growthUnit;

	double orientation = leader->getOrientation();
	double intro = this->branchIntro(leader,growthUnit);
	double startX = leader->getBaseX() + (intro * cos(orientation));
	double startY = leader->getBaseY() - (intro * sin(orientation));

	if(atoi(source->data[TreeNodeKey::DEPTH].c_str()) != FINAL_BRANCH)
	{
//		int leaves = 0;
		vector<SurrogateTreeNode*> leaves;
		SurrogateTreeNode* node;

		bool left = (orientation > (3.14159/2));
		double branchOrientation;
		for(vector<SurrogateTreeNode*>::iterator iter = leader->leaves->begin(); iter != leader->leaves->end(); ++iter)
		{
			node = *iter;
	//		if(node->children->size() == 0)
	//		{
	//			// Note new leaf
			leaves.push_back(node);
			// Draw leaves + branch
			if(leaves.size() == leavesPerBranch)
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
				leaves.clear();
			}
	//		}
		}
		if(leaves.size() > 0)
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
			leaves.clear();
		}
	}
	else
	{
		// We are at a final branch and need to do the advance branching
		this->drawBranchAdv(leader, leader->getX(), leader->getY(), orientation, lengthPerLeaf * 1.25, growthUnit, growthUnit);
	}
}

double TrapezoidBlocks::branchIntro(TrapezoidLeader* leader, double growthUnit)
{
	double initialSpacer = 15 * growthUnit;
	return initialSpacer;
}

double TrapezoidBlocks::leafSpacing(int depth, int leaves, double growthUnit)
{
	double branchSpacer = 0;
//	SurrogateTreeNode* source = leader->getSourceSet();

//	if(atoi(source->data[TreeNodeKey::DEPTH].c_str()) > FINAL_BRANCH + 1)
	if(depth > FINAL_BRANCH)
	{
		double leavesPerBranch = 15.0;
		double leafBranchSpacing = 2 * growthUnit;
//		branchSpacer = ceil(((source->children->size() / leavesPerBranch) + 1) * leafBranchSpacing);
		branchSpacer = ceil(((leaves / leavesPerBranch) + 1) * leafBranchSpacing);

		// Spoof
		branchSpacer = 2 * leafBranchSpacing;
	}
	return branchSpacer;
}

void TrapezoidBlocks::initializeLeader(TrapezoidLeader* leader)
{
	double lengthPerLeaf = 20.0;

	// Re-orient to predefined orientation
	leader->setOrientation(atof(leader->getSourceSet()->data[TreeNodeKey::ANGLE].c_str()));
//	printf("--- Changed orientation to %f\n",leader->getOrientation());

	// Set initial step where leaves will go
	leader->step(this->branchIntro(leader,lengthPerLeaf) + this->leafSpacing(atoi(leader->getSourceSet()->data[TreeNodeKey::DEPTH].c_str()),leader->getSourceSet()->children->size(),lengthPerLeaf));

	this->leaders.push_back(leader);
}

void TrapezoidBlocks::finalizeLeader(TrapezoidLeader* leader)
{
//	printf("Finalizing leader\n");
//	fflush(stdout);
	double lengthPerLeaf = 20.0;
	int leaves = leader->leaves->size();
	this->drawBranches(leader,15,lengthPerLeaf);


	SurrogateTreeNode* source = leader->getSourceSet();


	// Create draw objects from leader
	MinDrawableDatum* datum = (MinDrawableDatum*)malloc(sizeof(MinDrawableDatum));
	datum->locationX = (int)leader->getBaseX();
	datum->locationY = (int)leader->getBaseY();
	datum->angle = leader->getOrientation();
	datum->extent = this->branchIntro(leader,lengthPerLeaf) + this->leafSpacing(atoi(leader->getSourceSet()->data[TreeNodeKey::DEPTH].c_str()),leaves,lengthPerLeaf);
	if(leader->getLength() > datum->extent)
	{
		datum->extent = leader->getLength();
	}
	datum->mass = leader->getBaseWidth();
	datum->massRatio = (leader->getEndWidth()/datum->mass);
	datum->source = source;
	this->setDatumColor(datum, datum->source->data[TreeNodeKey::COLOR]);
//	printf("Adding leader branch at (%d,%d) with dims [l: %d, m: %d, r: %f] pointed at %f\n",datum->locationX,datum->locationY, datum->extent, datum->mass, datum->massRatio, datum->angle);
	data->insert(TRUNK_LAYER,datum);

	// Remove all references to the children
	source->children->clear();
	leader->setSourceSet(NULL);

	// Clear node
	delete source;

	// Delete leader
	delete leader;
}
