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

//typedef __gnu_cxx::hash_map<const char*, const char*> Surrogate;

class Surrogate
{
public:
	std::unordered_map<std::string, std::string> data;
};

#endif /* SURROGATE_H_ */
