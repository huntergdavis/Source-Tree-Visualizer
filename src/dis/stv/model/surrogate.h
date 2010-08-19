/*
 * surrogate.h
 *
 *  Created on: Aug 13, 2010
 *      Author: programmer
 */

#ifndef SURROGATE_H_
#define SURROGATE_H_

#include <unordered_map>
#include <string>
#include "property_transformer.h"

using namespace std;

class Surrogate
{
public:
	unordered_map<string, string> data;
	virtual double findMin(string property) = 0;
	virtual double findMax(string property) = 0;
	virtual void scale(string property, double factor) = 0;
	virtual void transform(string property, PropertyTransformer* transformer) = 0;
};

#endif /* SURROGATE_H_ */
