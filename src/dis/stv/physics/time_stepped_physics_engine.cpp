/*
 * time_stepped_physics_engine.cpp
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#include "time_stepped_physics_engine.h"

TimeSteppedPhysicsEngine::TimeSteppedPhysicsEngine(long maxSteps, double stepTimeLength):maxSteps( maxSteps ), stepTimeLength( stepTimeLength )
{
}

void TimeSteppedPhysicsEngine::addMass(TimeSteppedPhysicsObject* mass)
{
	this->masses.push_back(mass);
}

void TimeSteppedPhysicsEngine::run()
{
	printf("Running simulator");
	for(int i = 0; i < this->maxSteps; i++)
	{
		printf("Step %d\n",i);
	}
}
