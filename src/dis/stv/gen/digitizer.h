/*
 * digitizer.h
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#ifndef DIGITIZER_H_
#define DIGITIZER_H_

#include "../draw/drawable_data.h"
#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;

class Digitizer
{
public:
	virtual DrawableData* digitize(const ptree* source) = 0;
};

#endif /* DIGITIZER_H_ */
