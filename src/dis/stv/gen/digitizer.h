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

class Digitizer
{
public:
	virtual DrawableData* digitize(SurrogateTreeNode* source) = 0;
};

#endif /* DIGITIZER_H_ */
