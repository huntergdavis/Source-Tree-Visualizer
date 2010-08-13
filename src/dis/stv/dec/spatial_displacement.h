/*
 * spatial_displacement.h
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#ifndef SPATIAL_DISPLACEMENT_H_
#define SPATIAL_DISPLACEMENT_H_

#include "decorator.h"

using boost::property_tree::ptree;

class SpatialDisplacement : public Decorator
{
public:
	void decorate(ptree* tree);
};

#endif /* SPATIAL_DISPLACEMENT_H_ */
