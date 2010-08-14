/*
 * spatial_displacement.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#include "spatial_displacement.h"

using namespace std;

void SpatialDisplacement::decorate(ptree* tree)
{
	// Iterate over each node with children
	// First, count
	for(ptree::iterator iter = tree->begin(); iter != tree->end(); ++iter)
	{
		this->count(iter->second.data());
	}
	// Second, float weighted surrogate nodes into position
	for(ptree::iterator iter = tree->begin(); iter != tree->end(); ++iter)
	{
		this->expand(iter);
	}
}

void SpatialDisplacement::expand(ptree tree)
{
}

int SpatialDisplacement::count(ptree tree)
{
	// Count all children
	int childSum = 0;
	for(ptree::iterator iter = tree.begin(); iter != tree.end(); ++iter)
	{
		childSum += this->count(*iter);
	}
	// Assign count to data
	// Check if ptree has data, then it is a file
	Surrogate* node = tree.data();
	// Else it is a directory and needs a node

}
