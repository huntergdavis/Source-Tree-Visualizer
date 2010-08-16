/*
 * tethered_spatial_object.cpp
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#include "tethered_spatial_object.h"

TetheredSpatialObject::TetheredSpatialObject(double x, double y, double tetherX, double tetherY, double tetherRadius) : SpatialObject(x,y), tetherX( tetherX ), tetherY( tetherY ), tetherRadius( tetherRadius )
{
}

void TetheredSpatialObject::applyAcceleration(double x, double)
{
}
