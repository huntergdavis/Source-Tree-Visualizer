/*
 * surrogate_tree_node.cpp
 *
 *  Created on: Aug 18, 2010
 *      Author: programmer
 */

#include "surrogate_tree_node.h"

double SurrogateTreeNode::findMin(string property)
{
	double min = atof(this->data[property].c_str());
	double childMin;
	SurrogateTreeNode* node;
	for(vector<SurrogateTreeNode*>::iterator iter = this->children.begin(); iter != this->children.end(); ++iter)
	{
		node = *iter;
		childMin = node->findMin(property);
		if(childMin < min)
		{
			min = childMin;
		}
	}
	return min;
}

double SurrogateTreeNode::findMax(string property)
{
	double max = atof(this->data[property].c_str());
	double childMax;
	SurrogateTreeNode* node;
	for(vector<SurrogateTreeNode*>::iterator iter = this->children.begin(); iter != this->children.end(); ++iter)
	{
		node = *iter;
		childMax = node->findMax(property);
		if(childMax > max)
		{
			max = childMax;
		}
	}
	return max;
}

void SurrogateTreeNode::scale(string property, double factor)
{
	SurrogateTreeNode* node;
	double currValue;
	double newValue;
	for(vector<SurrogateTreeNode*>::iterator iter = this->children.begin(); iter != this->children.end(); ++iter)
	{
		node = *iter;
		currValue = atof(node->data[property].c_str());
		newValue = factor * currValue;
		node->data[property] = boost::lexical_cast<string>(newValue);
		node->scale(property, factor);
	}
}

void SurrogateTreeNode::transform(string property, PropertyTransformer* transformer)
{
	SurrogateTreeNode* node;
	double currValue;
	double newValue;
	for(vector<SurrogateTreeNode*>::iterator iter = this->children.begin(); iter != this->children.end(); ++iter)
	{
		node = *iter;
		currValue = atof(node->data[property].c_str());
		newValue = transformer->transform(currValue);
		//printf("Setting '%s' to %f\n", property.c_str(),newValue);
		node->data[property] = boost::lexical_cast<string>(newValue);
		node->transform(property, transformer);
	}
}
