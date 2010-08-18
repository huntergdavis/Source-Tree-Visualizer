/*
 * space_colonizer.h
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#ifndef SPACE_COLONIZER_H_
#define SPACE_COLONIZER_H_

#include "digitizer.h"
#include "../model/surrogate_tree_node.h"

class SpaceColonizer : public Digitizer
{
private:
	int segLen;
	void removeNodeFromSubtreeCenterOfMass(SurrogateTreeNode* source, SurrogateTreeNode* node);
	void calculateSubtreeCenterOfMass(SurrogateTreeNode* source);
	void stepOrSplit(SurrogateTreeNode* source);

public:
	SpaceColonizer(int segmentLength);
	DrawableData* digitize(SurrogateTreeNode* source);
};

#endif /* SPACE_COLONIZER_H_ */
