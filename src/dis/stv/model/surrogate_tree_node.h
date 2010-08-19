/*
 * surrogate_tree_node.h
 *
 *  Created on: Aug 13, 2010
 *      Author: programmer
 */

#ifndef SURROGATE_TREE_NODE_H_
#define SURROGATE_TREE_NODE_H_

#include <vector>
#include <boost/lexical_cast.hpp>
#include "surrogate.h"
#include "property_transformer.h"

using namespace std;

class SurrogateTreeNode : public Surrogate
{
public:
	vector<SurrogateTreeNode*> children;

	SurrogateTreeNode* getChildWithName();
	double findMin(string property);
	double findMax(string property);
	void scale(string property, double factor);
	void transform(string property, PropertyTransformer* transformer);
};

#endif /* SURROGATE_TREE_NODE_H_ */
