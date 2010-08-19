/*
 * property_inverter.cpp
 *
 *  Created on: Aug 18, 2010
 *      Author: programmer
 */

#include "property_inverter.h"

PropertyInverter::PropertyInverter(double inversionBase):inversionBase(inversionBase)
{
}

double PropertyInverter::transform(double initialValue)
{
	return this->inversionBase - initialValue;
}
