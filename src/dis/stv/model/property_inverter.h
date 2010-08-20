/*
 * property_inverter.h
 *
 *  Created on: Aug 18, 2010
 *      Author: programmer
 */

#ifndef PROPERTY_INVERTER_H_
#define PROPERTY_INVERTER_H_

#include "property_transformer.h"
#include "../system/discursive_system.h"


class PropertyInverter : public PropertyTransformer
{
private:
	double inversionBase;
public:
	PropertyInverter(double inversionBase);
	double transform(double initialValue);
};

#endif /* PROPERTY_INVERTER_H_ */
