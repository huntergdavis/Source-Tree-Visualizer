/*
 * trapezoid_blocks.h
 *
 *  Created on: Oct 4, 2010
 *      Author: Mark Christensen
 */

#ifndef TRAPEZOID_BLOCKS_H_
#define TRAPEZOID_BLOCKS_H_

#include <list>
#include <math.h>
#include <boost/lexical_cast.hpp>
#include "./digitizer.h"
#include "./trapezoid_leader.h"
#include "../model/surrogate_tree_node.h"
#include "../system/discursive_system.h"

using namespace std;

class TrapezoidBlocks: public Digitizer
{
public:
	TrapezoidBlocks();
	virtual ~TrapezoidBlocks();
	DrawableData* digitize(SurrogateTreeNode* source);
private:
	DrawableData* data;
	list<TrapezoidLeader*> leaders;
	void initializeLeader(TrapezoidLeader* leader);
	void finalizeLeader(TrapezoidLeader* leader);
	bool step(DrawableData* data, TrapezoidLeader* leader);
	bool shouldSplit(SurrogateTreeNode* attractor, TrapezoidLeader* leader);
	double orientationBetween(SurrogateTreeNode* attractor, TrapezoidLeader* leader);
	double orientationBetweenSubtree(SurrogateTreeNode* attractor, TrapezoidLeader* leader);
	void drawBranch(TrapezoidLeader* leader, double startX, double startY, double orientation, double branchOrientation, double lengthPerLeaf, int leaves, double leafBranchSpacing, double growthUnit);
	double angleDiff(double ref, double compare);
	double angleFrom(double aX, double aY, double bX, double bY);
};

#endif /* TRAPEZOID_BLOCKS_H_ */
