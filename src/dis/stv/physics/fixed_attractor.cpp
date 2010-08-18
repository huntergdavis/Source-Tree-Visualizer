/*
 * fixed_attractor.cpp
 *
 *  Created on: Aug 17, 2010
 *      Author: Mark Christensen
 */

#include "fixed_attractor.h"

FixedAttractor::FixedAttractor(int inverseMass, double x, double y):TimeSteppedPhysicsObject(inverseMass),SpatialObject(x,y)
{
}

double FixedAttractor::step(double stepSize)
{
	// Do nothing
	return 0;
}

TimeSteppedPhysicsObject* FixedAttractor::clone()
{
	TimeSteppedPhysicsObject* c = new FixedAttractor(this->getMass(), this->getX(), this->getY());
	return c;
}

void FixedAttractor::update(TimeSteppedPhysicsObject* source)
{
	// Set location and mass from source
	this->setMass(source->getMass());
	this->setLocation(source->getLocationX(), source->getLocationY());
}

void FixedAttractor::applyForce(double forceX, double forceY)
{
	// Do nothing
}

double FixedAttractor::getLocationX()
{
	return this->getX();
}

double FixedAttractor::getLocationY()
{
	return this->getY();
}

double FixedAttractor::updateLocation(double timeDelta)
{
	// Do nothing
	return 0;
}
