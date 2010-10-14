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

    if(filterKeyStore.size() == 0)
    {
    	return 0;
    }

	for(std::vector<std::string>::iterator i = filterKeyStore.begin(); i != filterKeyStore.end(); ++i)
	{
		keyWordIterator++;
		found = textualData.find(*i);
		if(found != std::string::npos)
		{
			return keyWordIterator;
		}
	}

	return -1;
};


