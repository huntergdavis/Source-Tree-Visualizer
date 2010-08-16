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
	this->count(tree);
	// Second, float weighted surrogate nodes into position
	this->expand(tree);
}

// Sorted in increasing order
void SpatialDisplacement::insertOrderedBy(vector<SurrogateTreeNode*>& list, SurrogateTreeNode* tree, string property)
{
	SurrogateTreeNode* node;
	for(vector<SurrogateTreeNode*>::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		node = *iter;
		printf("%ld < %ld?\n",atol(tree->data[property].c_str()),atol(node->data[property].c_str()));
		if(atol(tree->data[property].c_str()) < atol(node->data[property].c_str()))
		{
			list.insert(iter,tree);
			break;
		}
	}
}

void SpatialDisplacement::expand(SurrogateTreeNode* tree)
{
	if(tree->children.size() > 0)
	{
		// Determine initial layout based on creation time of child nodes.
		// Files are split into different list than directories.
		vector<SurrogateTreeNode*> files;
		vector<SurrogateTreeNode*> dirs;
		SurrogateTreeNode* node = NULL;
		for(vector<SurrogateTreeNode*>::iterator iter = tree->children.begin(); iter != tree->children.end(); ++iter)
		{
			node = *iter;
			// Directory
			if(node->children.size() > 0)
			{
				this->insertOrderedBy(dirs,node,"creation_time");
			}
			// File
			else
			{
				this->insertOrderedBy(files,node,"creation_time");
			}
		}

		// Set tether radius based on number of children
		int tetherRadius = (((tree->children.size() - 1) / 2)*10) + 10;
		// Layout nodes.  Order from middle outward by creation time (oldest in the middle)
	}
	else
	{
		printf("No children for node '%s'\n", tree->data["name"].c_str());
	}
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
	tree->data["size"] = boost::lexical_cast<string>(sum);
	printf("Size of tree %s is %s\n",tree->data["name"].c_str(),tree->data["size"].c_str());
	return sum;
}
