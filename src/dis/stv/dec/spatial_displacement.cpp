/*
 * spatial_displacement.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#include "spatial_displacement.h"

using namespace std;

void SpatialDisplacement::decorate(SurrogateTreeNode* tree)
{
	// Iterate over each node with children
	// First, count
	for(vector<SurrogateTreeNode*>::iterator iter = tree->children.begin(); iter != tree->children.end(); ++iter)
	{
		this->count(*iter);
	}
	// Second, float weighted surrogate nodes into position
	for(vector<SurrogateTreeNode*>::iterator iter = tree->children.begin(); iter != tree->children.end(); ++iter)
	{
		this->expand(*iter);
	}
}

void SpatialDisplacement::expand(SurrogateTreeNode* tree)
{
}

int SpatialDisplacement::count(SurrogateTreeNode* tree)
{
	// Count all children plus ourselves (initial 1)
	int sum = 1;
	for(vector<SurrogateTreeNode*>::iterator iter = tree->children.begin(); iter != tree->children.end(); ++iter)
	{
		sum += this->count(*iter);
	}
	// Assign count to data
	(*tree)["size"] = boost::lexical_cast<string>(sum).c_str();
	// Check if tree has data, then it is a file
	// Else it is a directory and needs a node
	return 0;
}
