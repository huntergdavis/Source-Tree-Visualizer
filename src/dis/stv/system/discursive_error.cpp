/*
 * discursive_error.cpp
 *
 *  Created on: Aug 19, 2010
 *      Author: Hunter Davis
 */

#include "./discursive_error.h"

void DiscursiveError(std::string error)
{
	printf("%s",error.c_str());
	exit(1);
}
