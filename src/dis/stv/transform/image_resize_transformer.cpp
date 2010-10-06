/*
 * image_resize_transformer.cpp
 *
 *  Created on: Oct 4, 2010
 *      Author: Mark Christensen
 */

#include "./image_resize_transformer.h"
//
//ImageResizeTransformer::ImageResizeTransformer()
//{
//}
//
//ImageResizeTransformer::~ImageResizeTransformer()
//{
//}

void ImageResizeTransformer::transform(va_list args)
{
	Image* canvas = va_arg(args,Image*);
	int width = va_arg(args,int);
	int height = va_arg(args,int);
//	printf("%d x %d\n",width,height);
	canvas->scale(Geometry(width,height));
}
