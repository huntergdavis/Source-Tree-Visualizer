/*
 * time_stepped_physics_object.h
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#ifndef TIME_STEPPED_PHYSICS_OBJECT_H_
#define TIME_STEPPED_PHYSICS_OBJECT_H_

#include <stdio.h>
#include "physical_mass.h"
#include "../system/discursive_system.h"


class TimeSteppedPhysicsObject : public PhysicalMass
{
public:
	TimeSteppedPhysicsObject(int mass);
	virtual double step(double stepSize) = 0;
	virtual TimeSteppedPhysicsObject* clone() = 0;
	virtual void update(TimeSteppedPhysicsObject* source) = 0;
	virtual void applyForce(double forceX, double forceY) = 0;
	virtual double getLocationX() = 0;
	virtual double getLocationY() = 0;
};



#endif /* TIME_STEPPED_PHYSICS_OBJECT_H_ */
