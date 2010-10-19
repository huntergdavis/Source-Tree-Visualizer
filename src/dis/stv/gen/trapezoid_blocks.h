/*
 * trapezoid_blocks.h
 *
 *  Created on: Oct 4, 2010
 *      Author: Mark Christensen
 */

#ifndef TRAPEZOID_BLOCKS_H_
#define TRAPEZOID_BLOCKS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include <queue>
#include <math.h>
#include <boost/lexical_cast.hpp>
#include <unordered_map>
#include "./digitizer.h"
#include "./trapezoid_leader.h"
#include "../model/surrogate_tree_node.h"
#include "../system/discursive_system.h"

using namespace std;

struct LeafSplit
{
	double rootX;
	double rootY;
	double orientation;
	double baseLength;
	int subCount;
	LeafSplit* left;
	LeafSplit* right;
};

class TrapezoidBlocks: public Digitizer
{
public:
	static const int FINAL_BRANCH = 2;

	TrapezoidBlocks();
	virtual ~TrapezoidBlocks();
	DrawableData* digitize(SurrogateTreeNode* source);
	void setColorMap(unordered_map<string, Magick::Color*>* colorMap);
private:
	DrawableData* data;
	list<TrapezoidLeader*> leaders;
	unordered_map<string, Magick::Color*>* colorMap;
	void initializeLeader(TrapezoidLeader* leader);
	void finalizeLeader(TrapezoidLeader* leader);
	bool step(DrawableData* data, TrapezoidLeader* leader);
	bool shouldSplit(SurrogateTreeNode* attractor, TrapezoidLeader* leader);
	double orientationBetween(SurrogateTreeNode* attractor, TrapezoidLeader* leader);
	double orientationBetweenSubtree(SurrogateTreeNode* attractor, TrapezoidLeader* leader);
	void drawBranches(TrapezoidLeader* leader, int leavesPerBranch, double lengthPerLeaf);
	void drawBranch(TrapezoidLeader* leader, double startX, double startY, double orientation, double branchOrientation, double lengthPerLeaf, int leaves, double leafBranchSpacing, double growthUnit);
	void drawBranchAdv(TrapezoidLeader* leader, double startX, double startY, double orientation, double baseLength, double leafBranchSpacing, double growthUnit);
	void drawMicroBranch(LeafSplit* leaf, double spacing, double growthUnit, Magick::Color* branchColor, Magick::Color* leafColor, bool logGrowth = false);
	double angleDiff(double ref, double compare);
	double angleFrom(double aX, double aY, double bX, double bY);
	double angleAdd(double origin, double addAmount);
	double branchIntro(TrapezoidLeader* leader, double growthUnit);
	double leafSpacing(int depth, int leaves, double growthUnit);
	double splitLocation(LeafSplit* leaf, double spacing, bool logGrowth = false);
	void reclaim(LeafSplit* leaf);
	Magick::Color* getColor(string color);
	void setDatumColor(MinDrawableDatum* datum, string color);
};

#endif /* TRAPEZOID_BLOCKS_H_ */
