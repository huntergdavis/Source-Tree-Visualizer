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
using namespace Magick;


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

// -------------------------------------------------------------------------
// API :: RepositoryAccess::ParseFilterKeywords
// PURPOSE :: parses filter keywords to internal keylist
//         :: private function shared by public functions
//         ::
// PARAMETERS :: std::String filterKeywords - comma sep list of keywords
// RETURN :: None
// -------------------------------------------------------------------------
void RepositoryAccess::ParseFilterKeywords(std::string filterKeywords)
{
	// split string by commas and push into global store
	// use a stringstream to split
    std::stringstream ss(filterKeywords);

    // store each delimited item in string
    std::string item;

    // split and push into global store
    while(std::getline(ss, item, ',')) {
		if(DoAnyFilterKeywordsMatch(item) == -1)
		{
			filterKeyStore.push_back(item);
		}
    }
};

// -------------------------------------------------------------------------
// API :: RepositoryAccess::ParseFilterKeywords
// PURPOSE :: parses filter keywords to internal keylist
//         :: private function shared by public functions
//         ::
// PARAMETERS :: std::String filterKeywords - comma sep list of keywords
// RETURN :: None
// -------------------------------------------------------------------------
void RepositoryAccess::ParseInverseFilterKeywords(std::string filterKeywords)
{
	// split string by commas and push into global store
	// use a stringstream to split
    std::stringstream ss(filterKeywords);

    // store each delimited item in string
    std::string item;

    // split and push into global store
    while(std::getline(ss, item, ',')) {
		if(DoAnyInverseFilterKeywordsMatch(item) == -1)
		{
			inverseFilterKeyStore.push_back(item);
		}
    }
};

// -------------------------------------------------------------------------
// API :: RepositoryAccess::AddFilterKeywords
// PURPOSE :: adds filter keywords to internal keylist
//         ::
//         ::
// PARAMETERS :: std::String filterKeywords - comma sep list of keywords
// RETURN :: None
// -------------------------------------------------------------------------
void RepositoryAccess::AddFilterKeywords(std::string filterKeyWords)
{
	ParseFilterKeywords(filterKeyWords);
};


// -------------------------------------------------------------------------
// API :: RepositoryAccess::AddInverseFilterKeywords
// PURPOSE :: adds filter keywords to internal keylist
//         ::
//         ::
// PARAMETERS :: std::String filterKeywords - comma sep list of keywords
// RETURN :: None
// -------------------------------------------------------------------------
void RepositoryAccess::AddInverseFilterKeywords(std::string filterKeyWords)
{
	ParseInverseFilterKeywords(filterKeyWords);
};


// -------------------------------------------------------------------------
// API :: RepositoryAccess::doAnyKeywordsMatch
// PURPOSE :: tests if any keywords match
//         :: returns the keyword number if so
//         ::
// PARAMETERS :: std::String filterKeywords - comma sep list of keywords
// RETURN :: int - keyword reference number if there's a match
// -------------------------------------------------------------------------
int RepositoryAccess::DoAnyFilterKeywordsMatch(std::string filterKeywords)
{
	// split string by commas and test against global store
	// use a stringstream to split
    std::stringstream ss(filterKeywords);

    // store each delimited item in string
    std::string item;

    // store the string iterations
    int keywordIterator = 0;

    // split and test global store
    while(std::getline(ss, item, ','))
    {
		keywordIterator++;
		for(std::vector<std::string>::iterator i = filterKeyStore.begin(); i != filterKeyStore.end(); ++i)
		{
			if(*i == item)
			{
				return keywordIterator;
			}
		}
    }

    return -1;
};

// -------------------------------------------------------------------------
// API :: RepositoryAccess::doAnyInverseFilterKeywordsMatch
// PURPOSE :: tests if any keywords match
//         :: returns the keyword number if so
//         ::
// PARAMETERS :: std::String filterKeywords - comma sep list of keywords
// RETURN :: int - keyword reference number if there's a match
// -------------------------------------------------------------------------
int RepositoryAccess::DoAnyInverseFilterKeywordsMatch(std::string filterKeywords)
{
	// split string by commas and test against global store
	// use a stringstream to split
    std::stringstream ss(filterKeywords);

    // store each delimited item in string
    std::string item;

    // store the string iterations
    int keywordIterator = 0;

    // split and test global store
    while(std::getline(ss, item, ','))
    {
		keywordIterator++;
		for(std::vector<std::string>::iterator i = inverseFilterKeyStore.begin(); i != inverseFilterKeyStore.end(); ++i)
		{
			if(*i == item)
			{
				return keywordIterator;
			}
		}
    }

    return -1;
};

// -------------------------------------------------------------------------
// API :: RepositoryAccess::DoesThisStringContainFilterKeywords
// PURPOSE :: tests if any internal filters match
//         :: returns the keyword number if so
//         ::
// PARAMETERS :: std::String textualData - string to search
// RETURN :: int - first keyword reference number if there's a match
//        :: int - returns zero if there are no filterKeyStore items
// -------------------------------------------------------------------------
int RepositoryAccess::DoesThisStringContainFilterKeywords(std::string textualData)
{
    // store the string iterations
    int keyWordIterator = 0;
    size_t found = 0;



    // check for negative keyword matches and fail if found
	for(std::vector<std::string>::iterator i = inverseFilterKeyStore.begin(); i != inverseFilterKeyStore.end(); ++i)
	{
		found = textualData.find(*i);
		if(found != std::string::npos)
		{
			return -1;
		}
	}

    // if no negatives and there is no keystore size, return
    if(filterKeyStore.size() == 0)
    {
    	return 0;
    }

    // check for positive keyword matches
	for(std::vector<std::string>::iterator i = filterKeyStore.begin(); i != filterKeyStore.end(); ++i)
	{
		keyWordIterator++;
		found = textualData.find(*i);
		if(found != std::string::npos)
		{
			return keyWordIterator;
		}
	}

	// if nothing is found in either bin, and we have both bins, we fail
	return -1;
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
		if(DoesThisStringContainFilterKeywords(pathname) < 0)
		{
			continueDrawingSubLeaves = 0;
		}

		if((continueDrawingSubLeaves > 0) || (drawFilteredLeaves == 1))
		{
			// We have no more path left.  Just a single entry (leaf)
			SurrogateTreeNode* file = new SurrogateTreeNode();
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
		// test for keyword failure
		if(DoesThisStringContainFilterKeywords(name) < 0)
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

void RepositoryAccess::RemoveByPathName(SurrogateTreeNode* tree, string pathname)
{
	bool removeNode = false;
	DiscursiveDebugPrint("STN OP","RemoveByPathName( %s )\n", pathname.c_str());
	// Split off first part of path
	int firstIndex = pathname.find("/");
	// If path separator is missing, we only have filename or unadorned dir
	// If path separator is last character, we are removing a directory
	if(firstIndex == -1 || firstIndex == pathname.length() - 1 )
	{
		removeNode = true;
	}
	if(firstIndex == 0)
	{
		// Our path started with a slash but we already have a root
		// So remove first slash and restart
		pathname = pathname.substr(1,pathname.length()-1);
		//firstIndex = pathname.find("/");
		this->RemoveByPathName(tree,pathname);
	}
	else
	{
		// Split first string out for this nodes name
		string name = pathname.substr(0,firstIndex);
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
				break;
			}
		}
		// If child not found, specified path was bad.
		if(node == NULL)
		{
			// Log error and exit
			DiscursiveDebugPrint("STN OP","Unable to delete '%s' under node '%s'\n",pathname.c_str(),tree->data[TreeNodeKey::NAME].c_str());
		}
		// Else, use found node
		else
		{
			if(removeNode)
			{
				DiscursiveDebugPrint("STN OP","Removing node %s\n", pathname.c_str());
				// Delete item from child list
				tree->children->erase(iter);
				// Remove node
				delete node;
			}
			else
			{
				// Parse rest of string
				this->RemoveByPathName(node, pathname.substr(firstIndex+1,(pathname.length() - firstIndex - 1)));
			}
		}
	}
}


