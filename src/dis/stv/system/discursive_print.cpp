/*
 * discursive_debug.cpp
 *
 *  Created on: Aug 19, 2010
 *      Author: Hunter Davis
 */

#include "./discursive_print.h"

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

	va_list args;
	va_start( args, printMessage );
	vprintf(printMessage.c_str(), args );
	va_end( args );
}

