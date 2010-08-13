/*
 * drawable.h
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;

class DrawableData
{
public:
	virtual ptree data() = 0;
};

#endif /* DRAWABLE_H_ */
