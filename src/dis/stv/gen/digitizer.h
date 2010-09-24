/*
 * digitizer.h
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#ifndef DIGITIZER_H_
#define DIGITIZER_H_

#include "../draw/drawable_data.h"
#include "../model/surrogate_tree_node.h"
#include "../system/discursive_system.h"

static const int TRUNK_LAYER = 0;
static const int LEAF_LAYER = 1;
static const int DEBUG_LAYER = 2;

class Digitizer
{
public:
	virtual DrawableData* digitize(SurrogateTreeNode* source) = 0;
};

#endif /* DIGITIZER_H_ */
