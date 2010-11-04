/*
 * system_monitor.cpp
 *
 *  Created on: Nov 4, 2010
 *      Author: Mark Christensen
 */

#include "./system_monitor.h"

void System::debugProcessMemory(const char* message)
{
	if(System::DEBUG)
	{
		// Open process status file
		FILE *f = fopen("/proc/self/stat","r");
		if(f)
		{
			unsigned long vm = 0;
			// Skip unneeded rows
			for(int i = 0; i < 22; ++i)
			{
				fscanf(f,"%*s");
			}
			// Read VM row
			fscanf(f,"%lu",&vm);
			// Close proc file
			fclose(f);

			DiscursiveDebugPrint("system","[%s][MEM] %d bytes (%dkb)\n", message, vm, vm/1024);
		}
		else
		{
			DiscursiveDebugPrint("system","[PROC] Unable to open '/proc/self/status'\n");
		}
	}
}
