/*
 * discursive_time.h
 *
 *  Created on: Oct 4th, 2010
 *      Author: Hunter Davis
 */

#ifndef DISCURSIVE_TIME_H_
#define DISCURSIVE_TIME_H_

#include <stdio.h>
#include <string>
#include <sys/time.h>
#include <time.h>
#include "./discursive_print.h"
#include "./discursive_debug.h"
#include "./discursive_error.h"



class DiscursiveTime
{
private:
	std::string defaultKey;
	struct timeval defaultTime;


public:
	DiscursiveTime(std::string keyName, struct timeval tv);
	DiscursiveTime();
	void Tic(std::string ticType);
	struct timeval Toc(std::string ticType);
	void PrintToc(std::string ticType);
	int TimeValSubtract(struct timeval *result, struct timeval *x, struct timeval *y);
};


#endif /* DISCURSIVE_TIME_H_ */
