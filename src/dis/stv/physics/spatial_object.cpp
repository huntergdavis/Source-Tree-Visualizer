/*
 * spatial_object.cpp
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#include "spatial_object.h"

SpatialObject::SpatialObject(double x, double y) : x( x ), y( y )
{
}

void SpatialObject::setLocation(double x, double y)
{
	this->x = x;
	this->y = y;
}
double SpatialObject::getX()
{
	return this->x;
}
double SpatialObject::getY()
{
	return this->y;
}
