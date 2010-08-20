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
#include <string>

int GetDiscursiveDebugLevel();
void SetDiscursiveDebugLevel(int level);
void DebugPrint(std::string debugMessage,...);

#endif /* DISCURSIVE_DEBUG_H_ */
