/*
 * time_stepped_physics_engine.cpp
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#include "time_stepped_physics_engine.h"

TimeSteppedPhysicsEngine::TimeSteppedPhysicsEngine(double gravityX, double gravityY, long maxSteps, double stepTimeLength):gravityX( gravityX ), gravityY( gravityY ), repulsionX( 1.25 ), repulsionY( 1.25 ), bufferLocation( 0 ), maxSteps( maxSteps ), stepTimeLength( stepTimeLength )
{
}

void TimeSteppedPhysicsEngine::addMass(TimeSteppedPhysicsObject* mass)
{
	printf("Adding mass[%d]\n",mass->getMass());
	TimeSteppedPhysicsObject** massSet = (TimeSteppedPhysicsObject**)malloc(2*sizeof(TimeSteppedPhysicsObject*));
	massSet[0] = mass;
	massSet[1] = mass->clone();
	this->masses.push_back(massSet);
}

void TimeSteppedPhysicsEngine::run()
{
	printf("Running simulator\n");
	for(int i = 0; i < this->maxSteps; i++)
	{

		printf("Step %d\n",i);
		int nextGen = ( this->bufferLocation + 1 ) % 2;
		vector<TimeSteppedPhysicsObject**>::iterator iter = this->masses.begin();
		TimeSteppedPhysicsObject** massSet;
		TimeSteppedPhysicsObject* current;
		int item = 0;
		for(;iter != this->masses.end(); ++iter)
		{
			printf("Forces for item #%d\n",item++);
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
					//                               Mass of other x Mass of self
					// Force = Repulsion_Constant x ------------------------------
					//                                  Distance  x  Distance
					double dx = current->getLocationX() - prev->getLocationX();
					double dy = current->getLocationY() - prev->getLocationY();
					double distance = sqrt(dx * dx + dy * dy);
					double massTotal = prev->getMass()*current->getMass();
					int xOrientation = 1;
					if(prev->getLocationX() > current->getLocationX())
					{
						xOrientation = -1;
					}
					int yOrientation = 1;
					if(prev->getLocationY() > current->getLocationY())
					{
						yOrientation = -1;
					}
					current->applyForce(xOrientation * this->repulsionX*massTotal/(distance * distance),yOrientation * this->repulsionY*massTotal/(distance * distance));
				}
			}
			// Update position for node
			current->step(this->stepTimeLength);
		}
		// Update buffer location
		this->bufferLocation = nextGen;
	}
}
