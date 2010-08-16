/*
 * time_stepped_physics_object.cpp
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#include "time_stepped_physics_object.h"

TimeSteppedPhysicsObject::TimeSteppedPhysicsObject(int mass, double x, double y):PhysicalMass(mass),SpatialObject(x,y)
{

}

void TimeSteppedPhysicsObject::update(TimeSteppedPhysicsObject* source)
{
	this->setMass( source->getMass() );
	this->setLocation(source->getX(), source->getY());
}
