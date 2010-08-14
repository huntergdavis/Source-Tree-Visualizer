/*
 * spatial_displacement.h
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#ifndef SPATIAL_DISPLACEMENT_H_
#define SPATIAL_DISPLACEMENT_H_

#include "decorator.h"
#include <boost/property_tree/ptree.hpp>
#include <stdlib.h>

using boost::property_tree::ptree;
using boost::property_tree::basic_ptree;

class SpatialDisplacement : public Decorator
{
public:
	void decorate(ptree* tree);

private:
	void expand(ptree tree);
	int count(ptree tree);
};

#endif /* SPATIAL_DISPLACEMENT_H_ */
