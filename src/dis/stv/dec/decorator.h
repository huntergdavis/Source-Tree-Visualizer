/*
 * decorator.h
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#ifndef DECORATOR_H_
#define DECORATOR_H_

#include "../model/surrogate_tree_node.h"

class Decorator
{
public:
	virtual void decorate(SurrogateTreeNode* tree) = 0;
};

#endif /* DECORATOR_H_ */
