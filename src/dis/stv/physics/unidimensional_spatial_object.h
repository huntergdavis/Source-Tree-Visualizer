/*
 * tethered_spatial_object.h
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#ifndef TETHERED_SPATIAL_OBJECT_H_
#define TETHERED_SPATIAL_OBJECT_H_

#include <math.h>
#include <stdio.h>
#include "spatial_object.h"

class UnidimensionalSpatialObject : public SpatialObject
{
public:
	UnidimensionalSpatialObject(double location);
	double updateLocation(double timeDelta);
};

#endif /* TETHERED_SPATIAL_OBJECT_H_ */
