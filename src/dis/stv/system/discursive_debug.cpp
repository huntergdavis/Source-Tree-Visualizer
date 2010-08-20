/*
 * discursive_debug.cpp
 *
 *  Created on: Aug 19, 2010
 *      Author: Hunter Davis
 */

#include "./discursive_debug.h"

int debugLevel;

int GetDiscursiveDebugLevel()
{
	return debugLevel;
}
void SetDiscursiveDebugLevel(int level)
{
	debugLevel = level;
}

void DebugPrint(std::string debugMessage)
{
	if(debugLevel == 1)
	{
		printf("%s",debugMessage.c_str());
	}
}

