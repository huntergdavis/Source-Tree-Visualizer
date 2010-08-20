/*
 * fixed_attractor.h
 *
 *  Created on: Aug 17, 2010
 *      Author: programmer
 */

#ifndef FIXED_ATTRACTOR_H_
#define FIXED_ATTRACTOR_H_

#include "spatial_object.h"
#include "time_stepped_physics_object.h"
#include "../system/discursive_system.h"


class FixedAttractor : public TimeSteppedPhysicsObject, public SpatialObject
{
public:
	FixedAttractor(int inverseMass, double x, double y);
	double updateLocation(double timeDelta);
	double step(double stepSize);
	TimeSteppedPhysicsObject* clone();
	void update(TimeSteppedPhysicsObject* source);
	void applyForce(double forceX, double forceY);
	double getLocationX();
	double getLocationY();
};

#endif /* FIXED_ATTRACTOR_H_ */
