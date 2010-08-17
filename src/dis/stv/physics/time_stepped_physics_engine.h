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

class TimeSteppedPhysicsEngine
{
private:
	double gravityX;
	double gravityY;
	double repulsionX;
	double repulsionY;
	int bufferLocation;
	long maxSteps;
	double stepTimeLength;
	vector<TimeSteppedPhysicsObject**> masses;
public:
	TimeSteppedPhysicsEngine(double gravityX, double gravityY, long maxSteps, double stepTimeLength);
	void addMass(TimeSteppedPhysicsObject* mass);
	void run();
};

#endif /* TIME_STEPPED_PHYSICS_ENGINE_H_ */
