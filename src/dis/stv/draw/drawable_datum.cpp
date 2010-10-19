/*
 * drawable_datum.cpp
 *
 *  Created on: Aug 18, 2010
 *      Author: programmer
 */

#include "drawable_datum.h"


DrawableDatum::DrawableDatum(double locationX, double locationY, double angle, double extent, double mass):locationX(locationX), locationY(locationY),angle(angle),extent(extent),mass(mass)
{
}

double DrawableDatum::getAngle()
{
	return this->angle;
}

double DrawableDatum::getLocationX()
{
	return this->locationX;
}

double DrawableDatum::getLocationY()
{
	return this->locationY;
}

double DrawableDatum::getExtent()
{
	return this->extent;
}

double DrawableDatum::getMass()
{
	return this->mass;
}


