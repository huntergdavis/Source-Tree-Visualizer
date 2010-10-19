/*
 * repository_access.cpp
 *
 *  Created on: Sept 22, 2010
 *      Author: HunterD
 */


#include "repository_access.h"
#include <Magick++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../model/surrogate_tree_node.h"
#include "../dec/spatial_displacement.h"
#include "../draw/scanline_artist.h"
#include "../gen/space_colonizer.h"
#include "../main/configuration_agent.h"
using namespace Magick;

extern ConfigurationAgent configAgent;

int RepositoryAccess::WriteJPGFromCanvas(Image* canvas)
{
	// Tree image size parameters
	// Generate Image Name with preceding integer and increment it
	std::stringstream integerPlusFileName;
	integerPlusFileName << "./out/";
	integerPlusFileName << this->jpgIndex;
	integerPlusFileName << this->fileName;
	std::string name = integerPlusFileName.str();
	this->jpgIndex++;

	DiscursivePrint("Writing JPEG:  %s\n",name.c_str());

	try
	{
		// Write the image to a file with a preceding integer
		canvas->write(name.c_str());
	}
	catch( Exception &error_ )
	{
		DiscursiveError(error_.what());
	}

	return 0;
};

void RepositoryAccess::InsertByPathName(SurrogateTreeNode* tree, string pathname, long time, int drawSubLeafs)
{
	// should we continue to draw sub leaves?
	int continueDrawingSubLeaves = drawSubLeafs;

	// Split off first part of path
	int firstIndex = pathname.find("/");
	if(firstIndex == 0)
	{
		// Our path started with a slash but we already have a root
		// So remove first slash and ignore
		pathname = pathname.substr(1,pathname.length()-1);
		firstIndex = pathname.find("/");
	}
	if(firstIndex == -1)
	{
		// We have no more path left.  Just a single entry (leaf)
		// test for keyword failure
		if(configAgent.DoesThisStringContainFilterKeywords(pathname) < 0)
		{
			continueDrawingSubLeaves = 0;
		}

		if((continueDrawingSubLeaves > 0) || (drawFilteredLeaves == 1))
		{
			// We have no more path left.  Just a single entry (leaf)
			SurrogateTreeNode* file = new SurrogateTreeNode();

			// set all configuration properties from filter items
			configAgent.AddFilterPropertiesToTreeNode(file,pathname);

			// set other properties on the node
			string timeStr = boost::lexical_cast<string>(time);
			file->set(TreeNodeKey::CREATION_TIME, timeStr);
			file->set(TreeNodeKey::NAME, pathname);
			file->set(TreeNodeKey::REVISIONCREATED, localRevs);
			file->set(TreeNodeKey::DRAWNODE, continueDrawingSubLeaves);
			//printf("Adding node '%s' @ time %ld\n",pathname.c_str(),time);
			tree->children->push_back(file);
		}
	}
	else
	{
		// Split first string out for this nodes name
		string name = pathname.substr(0,firstIndex);
		// test for keyword failure  in folder name
		if(configAgent.DoesThisStringContainFilterKeywords(name) < 0)
		{
			continueDrawingSubLeaves = 0;
		}
		if((continueDrawingSubLeaves > 0) || (drawFilteredLeaves == 1))
		{
			// Look for node in children
			SurrogateTreeNode* node = NULL;

			vector<SurrogateTreeNode*>::iterator iter = tree->children->begin();
			for(;iter != tree->children->end(); ++iter)
			{
				SurrogateTreeNode* local = *iter;
				string nameComp = local->data[TreeNodeKey::NAME];
				//printf("Comparing %s to %s\n",nameComp.c_str(),name.c_str());
				if(!nameComp.compare(name))
				{
					// Found node
					node = (*iter);
					// Update node time if necessary
					if(time < atol(node->data[TreeNodeKey::CREATION_TIME].c_str()))
					{
						//printf("Updating time of node[\"%s\"] to %ld from %ld\n", name.c_str(), time, atol(node->data[TreeNodeKey::CREATION_TIME].c_str()));
						node->set(TreeNodeKey::CREATION_TIME, boost::lexical_cast<string>(time));
						node->set(TreeNodeKey::REVISIONCREATED, localRevs);
						node->set(TreeNodeKey::DRAWNODE, continueDrawingSubLeaves);

						// set all configuration properties from filter items
						configAgent.AddFilterPropertiesToTreeNode(node,name);
					}
					break;
				}
			}
			// If child not found, add new node
			if(node == NULL)
			{
				node = new SurrogateTreeNode();
				string timeStr = boost::lexical_cast<string>(time);
				node->set(TreeNodeKey::CREATION_TIME, timeStr);
				node->set(TreeNodeKey::NAME, name);
				node->set(TreeNodeKey::REVISIONCREATED, localRevs);
				node->set(TreeNodeKey::DRAWNODE, continueDrawingSubLeaves);

				// set all configuration properties from filter items
				configAgent.AddFilterPropertiesToTreeNode(node,name);

				//printf("Adding node '%s' @ time %ld\n",name.c_str(),time);
				tree->children->push_back(node);
			}
			// Else, use found node

			// Parse rest of string
			this->InsertByPathName(node, pathname.substr(firstIndex+1,(pathname.length() - firstIndex - 1)),time,continueDrawingSubLeaves);
		}
		//this->InsertByPathName(tree, pathname.substr(firstIndex+1,(pathname.length() - firstIndex - 1)),time,continueDrawingSubLeaves);
	}
}


