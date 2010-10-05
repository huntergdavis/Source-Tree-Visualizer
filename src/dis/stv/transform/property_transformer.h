/*
 * property_transformer.h
 *
 *  Created on: Aug 18, 2010
 *      Author: programmer
 */

#ifndef PROPERTY_TRANSFORMER_H_
#define PROPERTY_TRANSFORMER_H_

#include "../system/discursive_system.h"


class PropertyTransformer
{
public:
	virtual double transform(double initialValue) = 0;
};

#endif /* PROPERTY_TRANSFORMER_H_ */
