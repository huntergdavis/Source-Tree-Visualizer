/*
 * decorator.h
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#ifndef DECORATOR_H_
#define DECORATOR_H_

#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;

class Decorator
{
public:
	virtual void decorate(ptree* tree) = 0;
};

#endif /* DECORATOR_H_ */
