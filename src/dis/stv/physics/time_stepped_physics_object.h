/*
 * time_stepped_physics_object.h
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#ifndef TIME_STEPPED_PHYSICS_OBJECT_H_
#define TIME_STEPPED_PHYSICS_OBJECT_H_

#include "physical_mass.h"
#include "spatial_object.h"

class TimeSteppedPhysicsObject : public PhysicalMass, SpatialObject
{
public:
	TimeSteppedPhysicsObject(int mass, double x, double y);
	void update(TimeSteppedPhysicsObject* source);
};



#endif /* TIME_STEPPED_PHYSICS_OBJECT_H_ */
