/*
 * transform_factory.cpp
 *
 *  Created on: Oct 4, 2010
 *      Author: Mark Christensen
 */

#include "./transform_factory.h"
//
//template <typename T>
void TransformFactory::transform(int transformType, int argc, ...)
{
	// Load args
	va_list args;
	va_start(args, argc);

	switch(transformType)
	{
	case IMAGE_MAGICK_TRANSFORMER:
	{
		switch(argc)
		{
		case 3:
			break;
		default:
			DiscursiveError("Unable to create ImageMagickTransformer object with %d arguments.\n  Requires %d arguments.\n", argc, 3);
			break;
		}
		ImageMagickTransformer::transform(args);
		break;
	}
	case IMAGE_RESIZE_TRANSFORMER:
		switch(argc)
		{
		case 3:
			break;
		default:
			DiscursiveError("Unable to create ImageMagickTransformer object with %d arguments.\n  Requires %d arguments.\n", argc, 3);
			break;
		}
		ImageResizeTransformer::transform(args);
		break;
	default:
		DiscursiveError("Request for invalid Transformer type '%d'.  No transformation performed.\n", transformType);
		break;
	}

	va_end(args);
//	return result;
}
//
//void TransformFactory::transform(int transformType, ...)
//{
//	va_list args;
//	va_start(args, transformType);
//	typename T::transform(args);
//	va_end(args);
//};

