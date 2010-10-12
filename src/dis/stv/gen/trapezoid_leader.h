/*
 * trapezoid_leader.h
 *
 *  Created on: Oct 05, 2010
 *      Author: Mark Christensen
 */

#ifndef TRAPEZOID_LEADER_H_
#define TRAPEZOID_LEADER_H_

#include "math.h"
#include "../model/surrogate_tree_node.h"
#include "../system/discursive_system.h"
//
//struct LeafBranch
//{
//	vector<SurrogateTreeNode*>* leaves;
//};

class TrapezoidLeader
{
private:
	double length;
	double baseX;
	double baseY;
	double baseWidth;
	double endWidth;
	double orientation;
	double initSpacer;
	SurrogateTreeNode* sourceSet;


public:
	vector<SurrogateTreeNode*>* leaves;
	TrapezoidLeader(double baseX, double baseY, double baseWidth, double orientation, double initSpacer, SurrogateTreeNode* sourceSet);
	~TrapezoidLeader();
	double getBaseX();
	double getBaseY();
	double getBaseWidth();
	double getEndWidth();
	void setEndWidth(double endWidth);
	double getX();
	double getY();
	double getOrientation();
	int getLength();
	double step(double stepSize);
	void setOrientation(double orientation);
	SurrogateTreeNode* getSourceSet();
	void setSourceSet(SurrogateTreeNode* newSourceSet);
};

#endif /* TRAPEZOID_LEADER_H_ */
