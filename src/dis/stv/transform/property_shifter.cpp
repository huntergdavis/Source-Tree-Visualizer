/*
 * property_shifter.cpp
 *
 *  Created on: Aug 18, 2010
 *      Author: programmer
 */

#include "property_shifter.h"

PropertyShifter::PropertyShifter(double shiftAmount):shiftAmount(shiftAmount)
{

}

double PropertyShifter::transform(double initialValue)
{
	return initialValue + shiftAmount;
}
