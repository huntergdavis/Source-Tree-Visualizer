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

void DiscursiveDebugPrint(std::string debugMessage,...)
{
	if(debugLevel == 1)
	{
		va_list args;
		va_start( args, debugMessage );
		vprintf(debugMessage.c_str(), args );
		va_end( args );
	}
}

