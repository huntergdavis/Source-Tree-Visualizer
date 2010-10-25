/*
 * discursive_debug.cpp
 *
 *  Created on: Aug 19, 2010
 *      Author: Hunter Davis
 */

#include "./discursive_print.h"
#include "../main/configuration_agent.h"
extern ConfigurationAgent configAgent;

// -------------------------------------------------------------------------
// API :: DiscursivePrint
// PURPOSE :: it's printf that can be updated to work with guis etc
//         ::
// PARAMETERS :: std::string printMessage - printf first argument
//            :: ... - any formatting arguments or parameters to printf
// RETURN :: void - this is a 100% replacement for printf
// -------------------------------------------------------------------------
void DiscursivePrint(std::string printMessage,...)
{
	int printMode = configAgent.returnOptionByName("printMode");

	if(printMode == 0)
	{
		va_list args;
		va_start( args, printMessage );
		vprintf(printMessage.c_str(), args );
		va_end( args );
	}
	else if(printMode == 1)
	{
		FILE * outFile;
		std::string outputFileName = configAgent.returnPrintModeFileName();
		outFile = fopen (outputFileName.c_str(),"a");
		va_list args;
		va_start( args, printMessage );
		vfprintf(outFile,printMessage.c_str(), args );
		va_end( args );
		fclose (outFile);
	}
	else if(printMode == 2)
	{
		// ignore 2 - it's the ignore flag
	}
	else
	{
		DiscursiveError("Incorrect Print Mode Set %d\n",printMode);
	}
}

