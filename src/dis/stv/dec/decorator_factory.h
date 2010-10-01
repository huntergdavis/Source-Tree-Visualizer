/*
 * decorator_factory.h
 *
 *  Created on: Oct 1, 2010
 *      Author: anonymous
 */

#ifndef DECORATOR_FACTORY_H_
#define DECORATOR_FACTORY_H_

#include "./decorator.h"
#include "./spatial_displacement.h"

class DecoratorFactory {
public:
	static const int SPATIAL_DISPLACEMENT_NAIVE = 0;
	static const int SPATIAL_DISPLACEMENT_LEAF_CLUSTERING = 1;

	static Decorator* getInstance(int instanceType, int argc, ...);
};

#endif /* DECORATOR_FACTORY_H_ */
