/*
 * surrogate_tree_node.cpp
 *
 *  Created on: Aug 18, 2010
 *      Author: programmer
 */

#include "surrogate_tree_node.h"
#include <sstream>

//const string TreeNodeKey::DEPTH = "depth";
//const string TreeNodeKey::X = "x";
//const string TreeNodeKey::Y = "y";
//const string TreeNodeKey::SIZE = "size";
//const string TreeNodeKey::NAME = "name";
//const string TreeNodeKey::ROOT = "root";
//const string TreeNodeKey::SCOMX = "scomX";
//const string TreeNodeKey::SCOMY = "scomY";
//const string TreeNodeKey::SCOMWEIGHT = "scomWeight";
//const string TreeNodeKey::SCOMWEIGHT = "creation_time";


SurrogateTreeNode::SurrogateTreeNode():children(new vector<SurrogateTreeNode*>()), isFile(true)
{
}

SurrogateTreeNode::~SurrogateTreeNode()
{
//	printf("Freeing 0x%x\n", this);
//	fflush(stdout);
//	printf("--- (%s)\n", this->data[TreeNodeKey::NAME].c_str());
//	fflush(stdout);
//	if(this->data[TreeNodeKey::NAME].compare("ExampleScripts") == 0)
//	{
//		int* a;
//		free(a);
//	}
	SurrogateTreeNode* node;
	if(this->children != NULL)
	{
//		printf("--- Freeing %d children\n", this->children->size());
//		fflush(stdout);
		for(vector<SurrogateTreeNode*>::iterator iter = this->children->begin(); iter != this->children->end(); ++iter)
		{
			node = *iter;
			if(node != NULL)
			{
				delete node;
			}
			//this->children->erase(iter);
		}
//		printf("--- Done freeing children\n");
//		fflush(stdout);
		this->children->clear();
		delete(this->children);
	}

	// this should work but doesnt
//	this->data.clear();
	// Print all data in data!
	this->data.clear();
//	for(unordered_map<string,string>::iterator iter = this->data.begin(); iter != this->data.end(); ++iter)
//	{
//		this->data.erase(iter);
////		delete iter->first;
////		delete iter->second;
//	}
}

void SurrogateTreeNode::set(string key, string value)
{
	unordered_map<string, string>::iterator existing = this->data.find(key);
	if(existing != this->data.end())
	{
		this->data.erase(key);
//		delete existing;
	}
	this->data[key] = value;
}

void SurrogateTreeNode::set(string key, int value)
{
	std::string stringValue;
	std::stringstream intToString;
	intToString << value;
	stringValue = intToString.str();
	unordered_map<string, string>::iterator existing = this->data.find(key);
	if(existing != this->data.end())
	{
		// delete existing;
		this->data.erase(key);
	}
	this->data[key] = stringValue;
}

double SurrogateTreeNode::findMin(string property)
{
	double min = atof(this->data[property].c_str());
	double childMin;
	SurrogateTreeNode* node;
	for(vector<SurrogateTreeNode*>::iterator iter = this->children->begin(); iter != this->children->end(); ++iter)
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
	for(vector<SurrogateTreeNode*>::iterator iter = this->children->begin(); iter != this->children->end(); ++iter)
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

	currValue = atof(this->data[property].c_str());
	newValue = factor * currValue;
	this->set(property, boost::lexical_cast<string>(newValue));

	for(vector<SurrogateTreeNode*>::iterator iter = this->children->begin(); iter != this->children->end(); ++iter)
	{
		node = *iter;
		node->scale(property, factor);
	}
}

void SurrogateTreeNode::transform(string property, PropertyTransformer* transformer)
{
	SurrogateTreeNode* node;
	double currValue;
	double newValue;

	currValue = atof(this->data[property].c_str());
	newValue = transformer->transform(currValue);
	//printf("Setting '%s' to %f\n", property.c_str(),newValue);
	this->set(property,boost::lexical_cast<string>(newValue));

	for(vector<SurrogateTreeNode*>::iterator iter = this->children->begin(); iter != this->children->end(); ++iter)
	{
		node = *iter;
		node->transform(property, transformer);
	}
}

// -------------------------------------------------------------------------
// API :: SurrogateTreeNode::returnXml()
// PURPOSE :: recursively returns XML for this and all sub tree nodes
//         ::
// PARAMETERS ::
// RETURN :: std::string - containing xml values generated
// -------------------------------------------------------------------------
std::string SurrogateTreeNode::returnXml()
{
	// temp storage for returned XML
	std::string returnXml = "<node> ";

	// first loop over all data for this node and put into XML
	for(unordered_map<string, string>::const_iterator dataIterator = this->data.begin(); dataIterator != this->data.end(); ++dataIterator)
	{
		// add an opening tag of first string
		returnXml += "<";
		returnXml += dataIterator->first;
		returnXml += ">";

		// add the data item as the second string
		returnXml += dataIterator->second;

		// add a closing tag of first string
		returnXml += "</";
		returnXml += dataIterator->first;
		returnXml += "> ";

	}

	// now find any children and do the same
	SurrogateTreeNode* childNode;
	for(vector<SurrogateTreeNode*>::iterator iter = this->children->begin(); iter != this->children->end(); ++iter)
	{
		returnXml += "\n<child>";
		childNode = *iter;
		returnXml += childNode->returnXml();
		returnXml += "</child>\n";
	}

	returnXml += "</node>\n";
	return returnXml;
};

