/*
 * discursive_debug.cpp
 *
 *  Created on: Aug 19, 2010
 *      Author: Hunter Davis
 */

#include "./discursive_debug.h"

extern DiscursiveDebugAgent debugAgent;

// -------------------------------------------------------------------------
// API :: DiscursiveDebugPrint
// PURPOSE :: DEBUG PRINTING
//         :: prints debugMessage with printf of varagrs parameters if 
//         :: any keywords match
// PARAMETERS :: std::string debugKeywords, std::string debugMessage,...
// RETURN :: None
// -------------------------------------------------------------------------
void DiscursiveDebugPrint(std::string debugKeyWords,std::string debugMessage,...)
{
	if(debugAgent.doAnyKeywordsMatch(debugKeyWords) >= 0)
	{
		va_list args;
		va_start( args, debugMessage );
		vprintf(debugMessage.c_str(), args );
		va_end( args );
	}
}


// -------------------------------------------------------------------------
// API :: DiscursiveDebugAgent::DiscursiveDebugAgent()
// PURPOSE :: instantiation of DiscursiveDebugAgent
//         :: instatiates DiscursiveDebugAgent
//         :: 
// PARAMETERS :: none
// RETURN :: None
// -------------------------------------------------------------------------
DiscursiveDebugAgent::DiscursiveDebugAgent()
{
};

// -------------------------------------------------------------------------
// API :: DiscursiveDebugAgent::DiscursiveDebugAgent(std::string debugKeywords)
// PURPOSE :: instantiation of DiscursiveDebugAgent
//         :: instatiates DiscursiveDebugAgent with keywords passed comma delim
//         :: 
// PARAMETERS :: std::string debugKeywords
// RETURN :: None
// -------------------------------------------------------------------------
DiscursiveDebugAgent::DiscursiveDebugAgent(std::string debugKeywords)
{
	AddDebugKeywords(debugKeywords);
};

// -------------------------------------------------------------------------
// API :: DiscursiveDebugAgent::~DiscursiveDebugAgent()
// PURPOSE :: destructor  of DiscursiveDebugAgent
//         :: destructs DiscursiveDebugAgent
//         :: 
// PARAMETERS :: none
// RETURN :: None
// -------------------------------------------------------------------------
DiscursiveDebugAgent::~DiscursiveDebugAgent()
{
}

// -------------------------------------------------------------------------
// API :: DiscursiveDebugAgent::AddDebugKeywords
// PURPOSE :: adds debug keywords to internal keylist
//         :: 
//         :: 
// PARAMETERS :: std::String debugKeywords - comma sep list of keywords
// RETURN :: None
// -------------------------------------------------------------------------
void DiscursiveDebugAgent::AddDebugKeywords(std::string debugKeywords)
{
	ParseDebugKeywords(debugKeywords);
}

// -------------------------------------------------------------------------
// API :: DiscursiveDebugAgent::doAnyKeywordsMatch
// PURPOSE :: tests if any keywords match
//         :: returns the keyword number if so
//         :: 
// PARAMETERS :: std::String debugKeywords - comma sep list of keywords
// RETURN :: int - keyword reference number if there's a match
// -------------------------------------------------------------------------
int DiscursiveDebugAgent::doAnyKeywordsMatch(std::string debugKeywords)
{
	// split string by commas and test against global store
	// use a stringstream to split
    std::stringstream ss(debugKeywords);
    
    // store each delimited item in string
    std::string item;
    
    // store the string iterations
    int keywordIterator = 0;
    
    // split and test global store
    while(std::getline(ss, item, ',')) 
    {
		keywordIterator++;
		for(std::vector<std::string>::iterator i = debugKeyStore.begin(); i != debugKeyStore.end(); ++i)
		{
			if(*i == item)
			{
				return keywordIterator;
			}
		}		   
    }		
    
    return -1;
}

// -------------------------------------------------------------------------
// API :: DiscursiveDebugAgent::ParseDebugKeywords
// PURPOSE :: parses debug keywords to internal keylist
//         :: private function shared by public functions
//         :: 
// PARAMETERS :: std::String debugKeywords - comma sep list of keywords
// RETURN :: None
// -------------------------------------------------------------------------
void DiscursiveDebugAgent::ParseDebugKeywords(std::string debugKeywords)
{
	// split string by commas and push into global store
	// use a stringstream to split
    std::stringstream ss(debugKeywords);
    
    // store each delimited item in string
    std::string item;
    
    // split and push into global store
    while(std::getline(ss, item, ',')) {
		if(doAnyKeywordsMatch(item) == -1)
		{
			debugKeyStore.push_back(item);
		}
    }	
}







