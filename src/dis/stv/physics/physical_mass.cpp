/*
 * physical_mass.cpp
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#include "physical_mass.h"

PhysicalMass::PhysicalMass(int mass):mass( mass )
{
}

double PhysicalMass::accelerationFromForce(double forceMagnitude)
{
	return forceMagnitude / this->mass;
}

void PhysicalMass::setMass(int mass)
{
	this->mass = mass;
}

int PhysicalMass::getMass()
{
	return this->mass;
}
