/*
 * surrogate_tree_node.h
 *
 *  Created on: Aug 13, 2010
 *      Author: programmer
 */

#ifndef SURROGATE_TREE_NODE_H_
#define SURROGATE_TREE_NODE_H_

#include <vector>
#include "surrogate.h"

using namespace std;

class SurrogateTreeNode : public Surrogate
{
public:
	vector<SurrogateTreeNode*> children;

	SurrogateTreeNode* getChildWithName();
};

#endif /* SURROGATE_TREE_NODE_H_ */
