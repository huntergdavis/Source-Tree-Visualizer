/*
 * unidimensional_spatial_object.cpp
 *
 *  Created on: Aug 17, 2010
 *      Author: Mark Christensen
 */

#include "unidimensional_spatial_object.h"


UnidimensionalSpatialObject::UnidimensionalSpatialObject(double location):SpatialObject( location, 0 )
{
}

double UnidimensionalSpatialObject::updateLocation(double timeDelta)
{
	double ddx = this->getAccelX() * (timeDelta * timeDelta);
	this->setLocation(this->getX() + ddx,this->getY());
	return ddx;
}
