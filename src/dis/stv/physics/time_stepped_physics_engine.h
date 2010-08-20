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
#include "../system/discursive_system.h"


class TimeSteppedPhysicsEngine
{
private:
	static const double MIN_DISTANCE = 0.5;

	double repulsionX;
	double repulsionY;
	int bufferLocation;
	long maxSteps;
	double stepTimeLength;
	double minDiff;
	vector<TimeSteppedPhysicsObject**> masses;
	bool shouldContinue(int step, double minChange);

public:
	TimeSteppedPhysicsEngine(long maxSteps, double stepTimeLength);
	TimeSteppedPhysicsEngine(double minDiff, double stepTimeLength);
	void addMass(TimeSteppedPhysicsObject* mass);
	void run();
};

#endif /* TIME_STEPPED_PHYSICS_ENGINE_H_ */
