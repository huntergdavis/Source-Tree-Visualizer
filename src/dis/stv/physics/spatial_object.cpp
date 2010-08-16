/*
 * spatial_object.cpp
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#include "spatial_object.h"

SpatialObject::SpatialObject() : x( 0 ), y( 0 ), ddx( 0 ), ddy( 0 )
{
}

SpatialObject::SpatialObject(double x, double y) : x( x ), y( y ), ddx( 0 ), ddy( 0 )
{
}

void SpatialObject::setAcceleration(double x, double y)
{
	this->ddx = x;
	this->ddy = y;
}
void SpatialObject::applyAcceleration(double x, double y)
{
	this->ddx += x;
	this->ddy += y;
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
double SpatialObject::getAccelX()
{
	return this->ddx;
}
double SpatialObject::getAccelY()
{
	return this->ddy;
}
