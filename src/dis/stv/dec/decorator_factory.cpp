/*
 * decorator_factory.cpp
 *
 *  Created on: Oct 1, 2010
 *      Author: anonymous
 */

#include "decorator_factory.h"

Decorator* DecoratorFactory::getInstance(int instanceType, int argc, ...)
{
	Decorator* result = NULL;

	switch(instanceType)
	{
	case SPATIAL_DISPLACEMENT_NAIVE:
	{
		va_list args;

		int width = 0;
		int height = 0;
		double scaleWidth = SpatialDisplacement::SCALE_WIDTH;
		double scaleHeight = SpatialDisplacement::SCALE_HEIGHT;
		va_start( args, argc );
		switch(argc)
		{
		case 2:
			width = va_arg(args,int);
			height = va_arg(args,int);
			break;
		case 4:
			width = va_arg(args,int);
			height = va_arg(args,int);
			scaleWidth = va_arg(args,double);
			scaleHeight = va_arg(args,double);
			break;
		default:
			DiscursiveError("Unable to create SpatialDisplacement object with %d arguments.\n  Requires 2 or 4 arguments.\n", argc);
			break;
		}
		va_end( args );
		result = (Decorator*)(new SpatialDisplacement(width, height, scaleWidth, scaleHeight));
		break;
	}
	case SPATIAL_DISPLACEMENT_LEAF_CLUSTERING:
	{
		va_list args;

		int width = 0;
		int height = 0;
		va_start( args, argc );
		switch(argc)
		{
		case 2:
			width = va_arg(args,int);
			height = va_arg(args,int);
			break;
		default:
			DiscursiveError("Unable to create SpatialDisplacementLeafless object with %d arguments.\n  Requires 2 arguments.\n", argc);
			break;
		}
		va_end( args );
		result = (Decorator*)(new SpatialDisplacementLeafless(width, height));
		break;
	}
	default:
		DiscursiveError("Request for invalid Decorator type '%d'\n", instanceType);
		break;
	}

	return result;
}
