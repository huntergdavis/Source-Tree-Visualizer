/*
 * transform_factory.h
 *
 *  Created on: Oct 4, 2010
 *      Author: Mark Christensen
 */

#ifndef TRANSFORM_FACTORY_H_
#define TRANSFORM_FACTORY_H_

#include "./transformer.h"
#include "./image_magick_transformer.h"
#include "./image_resize_transformer.h"

class TransformFactory {
public:
	static const int IMAGE_MAGICK_TRANSFORMER = 0;
	static const int IMAGE_RESIZE_TRANSFORMER = 1;
//
//	static Transformer<T>* getInstance(int instanceType, int argc, ...);

	static void transform(int transformType, int argc, ...);
};

#endif /* TRANSFORM_FACTORY_H_ */
