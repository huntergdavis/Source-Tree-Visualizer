/*
 * digitizer_factory.cpp
 *
 *  Created on: Oct 1, 2010
 *      Author: Mark Christensen
 */

#include "digitizer_factory.h"

Digitizer* DigitizerFactory::getInstance(int instanceType, int argc, ...)
{
	Digitizer* result = NULL;

	switch(instanceType)
	{
	case SPACE_COLONIZER:
	{
		va_list args;

		int segLen = 0;
		va_start( args, argc );
		switch(argc)
		{
		case 1:
			segLen = va_arg(args,int);
			break;
		default:
			DiscursiveError("Unable to create SpaceColonizer object with %d arguments.\n  Requires 1 arguments.\n", argc);
			break;
		}
		va_end( args );
		result = (Digitizer*)(new SpaceColonizer(segLen));
		break;
	}
	case SIMPLE_TRAPEZOIDER:
	{
		va_list args;

		va_start( args, argc );
		switch(argc)
		{
		case 0:
			break;
		default:
			DiscursiveError("Unable to create SpaceColonizer object with %d arguments.\n  Requires 0 arguments.\n", argc);
			break;
		}
		va_end( args );
		result = (Digitizer*)(new TrapezoidBlocks());
		break;
	}
	default:
		DiscursiveError("Request for invalid Digitizer type '%d'\n", instanceType);
		break;
	}

	return result;
}
