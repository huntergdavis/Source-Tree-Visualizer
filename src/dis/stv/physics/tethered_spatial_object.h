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
#include "../system/discursive_system.h"


class TetheredSpatialObject : public SpatialObject
{
private:
	double tetherX;
	double tetherY;
	double tetherRadius;

public:
	TetheredSpatialObject(double x, double y, double tetherX, double tetherY, double tetherRadius);
	void updateLocation(double timeDelta);
	double getTetherX();
	double getTetherY();
	double getTetherRadius();
};

#endif /* TETHERED_SPATIAL_OBJECT_H_ */
