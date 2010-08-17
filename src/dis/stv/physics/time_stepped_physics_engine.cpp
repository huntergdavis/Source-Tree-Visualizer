/*
 * time_stepped_physics_engine.cpp
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#include "time_stepped_physics_engine.h"

TimeSteppedPhysicsEngine::TimeSteppedPhysicsEngine(double gravityX, double gravityY, long maxSteps, double stepTimeLength):gravityX( gravityX ), gravityY( gravityY ), repulsionX( 2 ), repulsionY( 2 ), bufferLocation( 0 ), maxSteps( maxSteps ), stepTimeLength( stepTimeLength )
{
}

void TimeSteppedPhysicsEngine::addMass(TimeSteppedPhysicsObject* mass)
{
	TimeSteppedPhysicsObject** massSet = (TimeSteppedPhysicsObject**)malloc(2*sizeof(TimeSteppedPhysicsObject*));
	massSet[0] = mass;
	massSet[1] = mass->clone();
	this->masses.push_back(massSet);
}

void TimeSteppedPhysicsEngine::run()
{
	printf("Running simulator");
	for(int i = 0; i < this->maxSteps; i++)
	{
		printf("Step %d\n",i);
		int nextGen = ( this->bufferLocation + 1 ) % 2;
		vector<TimeSteppedPhysicsObject**>::iterator iter = this->masses.begin();
		TimeSteppedPhysicsObject** massSet;
		TimeSteppedPhysicsObject* current;
		for(;iter != this->masses.end(); ++iter)
		{
			massSet = *iter;
			// Copy all old values to the "future" generation
			current = massSet[nextGen];
			current->update(massSet[this->bufferLocation]);
			// Calculate forces for this object from current generation
			// :: Gravity ::
			// This has the effect of normalizing the gravity to a max
			// value of 1 for the 'heaviest' node.
			current->applyForce(this->gravityX*current->getMass(),this->gravityY*current->getMass());
			// :: Repulsion ::
			vector<TimeSteppedPhysicsObject**>::iterator rep = this->masses.begin();
			TimeSteppedPhysicsObject** resSet;
			TimeSteppedPhysicsObject* prev;
			for(;rep != this->masses.end(); ++rep)
			{
				// We don't apply repulsion to ourselves
				if(rep != iter)
				{
					resSet = *rep;
					prev = resSet[this->bufferLocation];
					// Linear repulsion:
					//                               Mass of other
					// Force = Repulsion_Constant x -----------------
					//                               Mass of self
					current->applyForce(this->repulsionX*prev->getMass()/current->getMass(),this->repulsionY*prev->getMass()/current->getMass());
				}
			}
			// Update values for node
			current->step(this->stepTimeLength);
		}

		// Update buffer location
		this->bufferLocation = nextGen;
	}
}
