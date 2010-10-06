/*
 * colonization_leader.h
 *
 *  Created on: Aug 18, 2010
 *      Author: Mark Christensen
 */

#ifndef COLONIZATION_LEADER_H_
#define COLONIZATION_LEADER_H_

#include "math.h"
#include "../model/surrogate_tree_node.h"
#include "../system/discursive_system.h"


class ColonizationLeader
{
private:
	int length;
	double locationX;
	double locationY;
	double orientation;
	SurrogateTreeNode* sourceSet;

public:
	ColonizationLeader(double locationX, double locationY, double orientation, SurrogateTreeNode* sourceSet);
	~ColonizationLeader();
	double getLocationX();
	double getLocationY();
	double getOrientation();
	int getLength();
	int step();
	void setLocation(double x, double y);
	void setOrientation(double orientation);
	SurrogateTreeNode* getSourceSet();
	void setSourceSet(SurrogateTreeNode* newSourceSet);
};

#endif /* COLONIZATION_LEADER_H_ */
