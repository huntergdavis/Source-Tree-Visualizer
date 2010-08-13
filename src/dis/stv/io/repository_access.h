/*
 * repository_access.h
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#ifndef REPOSITORY_ACCESS_H_
#define REPOSITORY_ACCESS_H_

#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;

class RepositoryAccess
{
public:
	virtual ptree* retrieve() = 0;
};

#endif /* REPOSITORY_ACCESS_H_ */
