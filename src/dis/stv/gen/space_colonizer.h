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
#include "../system/discursive_system.h"


class SpaceColonizer : public Digitizer
{
private:
	DrawableData* data;
	int segLen;
	vector<ColonizationLeader*> leaders;
	void removeNodeFromSubtreeCenterOfMass(SurrogateTreeNode* source, SurrogateTreeNode* node);
	void calculateSubtreeCenterOfMass(SurrogateTreeNode* source);
	bool stepOrSplit(DrawableData* data, ColonizationLeader* leader);
	bool shouldSplit(SurrogateTreeNode* attractor, ColonizationLeader* leader);
	double orientationBetween(SurrogateTreeNode* attractor, ColonizationLeader* leader);
	double angleDiff(double ref, double compare);
	double angleFrom(double aX, double aY, double bX, double bY);

public:
	SpaceColonizer(int segmentLength);
	~SpaceColonizer();
	DrawableData* digitize(SurrogateTreeNode* source);
	void setColorMap(unordered_map<string, Magick::Color*>* colorMap);
};

#endif /* SPACE_COLONIZER_H_ */
