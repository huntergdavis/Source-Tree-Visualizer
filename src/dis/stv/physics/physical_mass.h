/*
 * physical_mass.h
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#ifndef PHYSICAL_MASS_H_
#define PHYSICAL_MASS_H_
#include "../system/discursive_system.h"


class PhysicalMass
{
private:
	int mass;

public:
	PhysicalMass(int mass);
	double accelerationFromForce(double forceMagnitude);
	void setMass( int mass );
	int getMass();
};

#endif /* PHYSICAL_MASS_H_ */
