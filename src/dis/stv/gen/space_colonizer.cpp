/*
 * space_colonizer.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#include "space_colonizer.h"

SpaceColonizer::SpaceColonizer(int segmentLength)
{
	this->segLen = segmentLength;
}

// Updates center of mass by removing values of child
void SpaceColonizer::removeNodeFromSubtreeCenterOfMass(SurrogateTreeNode* source, SurrogateTreeNode* node)
{
	int childWeight = atoi(node->data["size"].c_str());
	double childX = atof(node->data["x"].c_str());
	double childY = atof(node->data["y"].c_str());

	int scomWeight = atoi(source->data["scomWeight"].c_str());
	double scomX = atof(source->data["scomX"].c_str());
	double scomY = atof(source->data["scomY"].c_str());

	int newWeight = scomWeight - childWeight;
	int newX = atof(source->data["x"].c_str());
	int newY = atof(source->data["y"].c_str());
	if(newWeight > 0)
	{
		newX = (scomX * scomWeight - childX * childWeight)/newWeight;
		newY = (scomY * scomWeight - childY * childWeight)/newWeight;
	}

	// Save new SCoM values
	source->data["scomX"] = newX;
	source->data["scomY"] = newY;
	source->data["scomWeight"] = newWeight;
}

// Puts value of SCoM(x,y) under "scomX" and "scomY" parameters resp.
// Total mass of children is stored under "scomWeight"
void SpaceColonizer::calculateSubtreeCenterOfMass(SurrogateTreeNode* source)
{
	// Start with own location as SCoM
	// This is the degenerate case for leaf nodes
	double scomX = atof(source->data["x"].c_str());
	double scomY = atof(source->data["y"].c_str());
	int totalWeight = 0;
	if(source->children.size() > 0)
	{
		// To store values for averaging
		int childWeight = 0;
		double totalX = 0;
		double totalY = 0;
		// Calculate SCoM for each child
		SurrogateTreeNode* child;
		for(vector<SurrogateTreeNode*>::iterator iter = source->children.begin(); iter != source->children.end(); ++iter)
		{
			child = *iter;
			childWeight = atoi(child->data["size"].c_str());
			this->calculateSubtreeCenterOfMass(child);
			totalWeight += childWeight;
			totalX += (atof(child->data["scomX"].c_str()) * childWeight);
			totalY += (atof(child->data["scomY"].c_str()) * childWeight);
		}
		// Average together <ScoM, weight> child pairs
		scomX = totalX / totalWeight;
		scomY = totalY / totalWeight;
	}
	// Save SCoM values
	source->data["scomX"] = scomX;
	source->data["scomY"] = scomY;
	source->data["scomWeight"] = totalWeight;
}

void SpaceColonizer::stepOrSplit(SurrogateTreeNode* source)
{

}

DrawableData* SpaceColonizer::digitize(SurrogateTreeNode* source)
{
	// Initialize drawable data set
	//DrawableData* data = new DrawableData();

	// Precalculate subtree center of mass (SCoM) for each node
	this->calculateSubtreeCenterOfMass(source);

	// Run stepOrSplit() over each leader


	return NULL;//data;
}
