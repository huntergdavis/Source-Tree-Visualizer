/*
 * discursive_debug.h
 *
 *  Created on: Aug 19, 2010
 *      Author: Hunter Davis
 */

#ifndef DISCURSIVE_DEBUG_H_
#define DISCURSIVE_DEBUG_H_

#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void DiscursiveDebugPrint(std::string debugKeyWords,std::string debugMessage,...);

class DiscursiveDebugAgent
{
private:
	std::vector<std::string> debugKeyStore;
	void ParseDebugKeywords(std::string debugKeywords);
public:
	DiscursiveDebugAgent();
	DiscursiveDebugAgent(std::string debugKeywords);
	~DiscursiveDebugAgent();
	void AddDebugKeywords(std::string debugKeywords);
	int doAnyKeywordsMatch(std::string debugKeywords);
};

#endif /* DISCURSIVE_DEBUG_H_ */
