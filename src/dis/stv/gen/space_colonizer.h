/*
 * space_colonizer.h
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#ifndef SPACE_COLONIZER_H_
#define SPACE_COLONIZER_H_

#include <math.h>
#include <boost/lexical_cast.hpp>
#include "digitizer.h"
#include "colonization_leader.h"
#include "../model/surrogate_tree_node.h"

class SpaceColonizer : public Digitizer
{
private:
	static const int TRUNK_LAYER = 0;
	static const int LEAF_LAYER = 0;
	int segLen;
	vector<ColonizationLeader*> leaders;
	void removeNodeFromSubtreeCenterOfMass(SurrogateTreeNode* source, SurrogateTreeNode* node);
	void calculateSubtreeCenterOfMass(SurrogateTreeNode* source);
	bool stepOrSplit(DrawableData* data, ColonizationLeader* leader);
	bool shouldSplit(SurrogateTreeNode* attractor, ColonizationLeader* leader);
	double orientationBetween(SurrogateTreeNode* attractor, ColonizationLeader* leader);
	double angleFrom(double aX, double aY, double bX, double bY);

public:
	SpaceColonizer(int segmentLength);
	DrawableData* digitize(SurrogateTreeNode* source);
};

#endif /* SPACE_COLONIZER_H_ */
