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
void SpatialDisplacement::insertOrderedBy(vector<SurrogateTreeNode*>* list, SurrogateTreeNode* tree, string property)
{
	printf("Inserting '%s' from %s.  Current size: %d\n",property.c_str(),tree->data["name"].c_str(), list->size());
	SurrogateTreeNode* node;
	bool inserted = false;
	for(vector<SurrogateTreeNode*>::iterator iter = list->begin(); iter != list->end(); ++iter)
	{
		node = *iter;
		printf("%ld < %ld?\n",atol(tree->data[property].c_str()),atol(node->data[property].c_str()));
		if(atol(tree->data[property].c_str()) < atol(node->data[property].c_str()))
		{
			list->insert(iter,tree);
			inserted = true;
			break;
		}
	}
	if(!inserted)
	{
		printf("Inserting at end\n");
		list->push_back(tree);
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
				printf("Inserting into dirs...\n");
				this->insertOrderedBy(&dirs,node,"creation_time");
			}
			// File
			else
			{
				printf("Inserting into files...\n");
				this->insertOrderedBy(&files,node,"creation_time");
			}
		}

		// Map for retrieving resutls
		unordered_map<SurrogateTreeNode*,TreeDisplacementNode*> simPairs;
		int gap = 10;
		// If total is even, use initial offset
		int offset = 0;
		if((dirs.size() + files.size()) % 2 == 0)
		{
			offset = -gap/2;
		}
		// Set tether radius based on number of children
		int tetherRadius = (((tree->children.size() - 1) / 2)*gap) + 11 + offset;
		printf("Tether radius is %d\n", tetherRadius);
		// Layout nodes.  Order from middle outward by creation time (oldest in the middle)
		TimeSteppedPhysicsEngine* engine = new TimeSteppedPhysicsEngine(0,3,5000,0.2);

		// Layout directories first
		bool left = true;
		int dist = 0;
		TreeDisplacementNode* treeNode;
		int i = 0;
		for(; i < (int)dirs.size(); i++)
		{
			dist = (i+1)/2;
			if(left)
			{
				treeNode = new TreeDisplacementNode(atoi(dirs[i]->data["size"].c_str()),offset-dist*gap,0,0,0,tetherRadius);
			}
			else
			{
				treeNode = new TreeDisplacementNode(atoi(dirs[i]->data["size"].c_str()),offset+dist*gap,0,0,0,tetherRadius);
			}
			printf("Adding node '%s' at initial position (%f,%f)\n", dirs[i]->data["name"].c_str(),treeNode->getX(), treeNode->getY());
			simPairs[dirs[i]] = treeNode;
			engine->addMass(treeNode);
			left = !left;
		}
		// Now layout files
		for(int j = 0; j < (int)files.size(); j++)
		{
			dist = ((i + j + 1)/2);
			if(left)
			{
				treeNode = new TreeDisplacementNode(atoi(files[j]->data["size"].c_str()),offset-dist*gap,0,0,0,tetherRadius);
			}
			else
			{
				treeNode = new TreeDisplacementNode(atoi(files[j]->data["size"].c_str()),offset+dist*gap,0,0,0,tetherRadius);
			}
			printf("Adding node '%s' at initial position (%f,%f)\n", files[j]->data["name"].c_str(),treeNode->getX(), treeNode->getY());
			simPairs[files[j]] = treeNode;
			engine->addMass(treeNode);
			left = !left;
		}
		// Run simulation
		engine->run();
		// List new positions
		for(vector<SurrogateTreeNode*>::iterator iter = tree->children.begin(); iter != tree->children.end(); ++iter)
		{
			node = *iter;
			treeNode = simPairs[node];
			node->data["x"] = boost::lexical_cast<string>(treeNode->getX());
			node->data["y"] = boost::lexical_cast<string>(treeNode->getY());
			printf("%s @ (%s,%s)\n",node->data["name"].c_str(),node->data["x"].c_str(),node->data["y"].c_str());
		}
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
