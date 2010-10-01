/*
 * discursive_error.cpp
 *
 *  Created on: Aug 19, 2010
 *      Author: Hunter Davis
 */

#include "./discursive_error.h"

void DiscursiveError(std::string error, ...)
{
//	DiscursivePrint("\n%s\n",error.c_str());
	char* printMessage = (char*)malloc((error.length() + 3) * sizeof(char));
	sprintf(printMessage,"\n%s\n",error.c_str());

	va_list args;
	va_start( args, error);
	vprintf(printMessage, args );
	va_end( args );
	exit(1);
}
