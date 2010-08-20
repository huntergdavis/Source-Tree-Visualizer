/*
 * property_shifter.h
 *
 *  Created on: Aug 18, 2010
 *      Author: programmer
 */

#ifndef PROPERTY_SHIFTER_H_
#define PROPERTY_SHIFTER_H_

#include "property_transformer.h"
#include "../system/discursive_system.h"


class PropertyShifter : public PropertyTransformer
{
private:
	double shiftAmount;
public:
	PropertyShifter(double shiftAmount);
	double transform(double initialValue);
};

#endif /* PROPERTY_SHIFTER_H_ */
