/*
 * time_stepped_physics_engine.h
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#ifndef TIME_STEPPED_PHYSICS_ENGINE_H_
#define TIME_STEPPED_PHYSICS_ENGINE_H_

#include "physics_engine.h"
#include "time_stepped_physics_object.h"

class TimeSteppedPhysicsEngine : public PhysicsEngine
{
private:
	long maxSteps;
	double stepTimeLength;
public:
	TimeSteppedPhysicsEngine(long maxSteps, double stepTimeLength);
	void addMass(TimeSteppedPhysicsObject* mass);
	void run();
};

#endif /* TIME_STEPPED_PHYSICS_ENGINE_H_ */
