/*
 * system_monitor.h
 *
 *  Created on: Nov 4, 2010
 *      Author: Mark Christensen
 */

#ifndef SYSTEM_MONITOR_H_
#define SYSTEM_MONITOR_H_

#include <stdio.h>
#include "../stv/system/discursive_system.h"

namespace System
{
	const bool DEBUG = true;
	void debugProcessMemory(const char* message);
}


#endif /* SYSTEM_MONITOR_H_ */
