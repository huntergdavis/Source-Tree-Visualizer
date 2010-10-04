/*
 * discursive_time.cpp
 *
 *  Created on: Oct 4th, 2010
 *      Author: Hunter Davis
 */

#include "./discursive_time.h"


// -------------------------------------------------------------------------
// API :: DiscursiveTime::DiscursiveTime
// PURPOSE :: instantiation of DiscursiveTime
//         :: sets values to passed in values
// PARAMETERS :: std::string keyValue - key name, struct timeval tv; keyTime - key time
// RETURN :: None
// -------------------------------------------------------------------------
DiscursiveTime::DiscursiveTime(std::string keyName, struct timeval keyTime)
{
	defaultKey = keyName;
	defaultTime = keyTime;
}

// -------------------------------------------------------------------------
// API :: DiscursiveTime::DiscursiveTime
// PURPOSE :: instantiation of DiscursiveTime
//         :: sets values to defaults
// PARAMETERS ::
// RETURN :: None
// -------------------------------------------------------------------------
DiscursiveTime::DiscursiveTime()
{
	defaultKey = "";
	gettimeofday(&defaultTime,NULL);
}


// -------------------------------------------------------------------------
// API :: DiscursiveTime::Tic
// PURPOSE :: sets a 'tic' value, the initial of a time pair
//         ::
// PARAMETERS :: std::string ticType - storage hash for retrieval of tic later
// RETURN :: None
// -------------------------------------------------------------------------
void DiscursiveTime::Tic(std::string ticType)
{
	if(ticType != defaultKey)
	{
		DiscursivePrint("Failure to Lookup ticType %s, setting tictype %s instead\n",ticType.c_str(),defaultKey.c_str());
	}

	gettimeofday(&defaultTime, NULL);
};

// -------------------------------------------------------------------------
// API :: DiscursiveTime::Toc
// PURPOSE :: returns a 'toc' value, the second of a time pair
//         :: returns value of matching pair to ticType
// PARAMETERS :: std::string ticType - the name of the tic value to compare to
// RETURN :: None
// -------------------------------------------------------------------------
struct timeval DiscursiveTime::Toc(std::string ticType)
{
	if(ticType != defaultKey)
	{
		DiscursivePrint("Failure to Lookup ticType %s, returning tictype %s instead\n",ticType.c_str(),defaultKey.c_str());
	}

	struct timeval result;
	struct timeval tocValue;
	gettimeofday(&tocValue, NULL);

	if(TimeValSubtract(&result, &tocValue,&defaultTime) == 1)
	{
		DiscursiveError("Timing Function failed with a negative number.....");
	}


	// return the difference in tic toc values
	return result;
};


// -------------------------------------------------------------------------
// API :: DiscursiveTime::PrintToc
// PURPOSE :: prints a 'toc' value, the second of a time pair
//         :: prints value of matching pair to ticType
// PARAMETERS :: std::string ticType - the name of the tic value to compare to
// RETURN :: None
// -------------------------------------------------------------------------
void  DiscursiveTime::PrintToc(std::string ticType)
{
	if(ticType != defaultKey)
	{
		DiscursivePrint("Failure to Lookup ticType %s, returning tictype %s instead\n",ticType.c_str(),defaultKey.c_str());
	}

	struct timeval tocValue;
	struct timeval resultValue;
	gettimeofday(&tocValue, NULL);

	if(TimeValSubtract(&resultValue, &tocValue, &defaultTime) == 1)
	{
		DiscursiveError("Timing Function failed with a negative number.....");
	}

	DiscursivePrint("Elapsed : %ld seconds, %ld microseconds\n", resultValue.tv_sec,resultValue.tv_usec);
};

int DiscursiveTime::TimeValSubtract(struct timeval *result, struct timeval *x, struct timeval *y)
 {
   /* Perform the carry for the later subtraction by updating y. */
   if (x->tv_usec < y->tv_usec) {
	 int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
	 y->tv_usec -= 1000000 * nsec;
	 y->tv_sec += nsec;
   }
   if (x->tv_usec - y->tv_usec > 1000000) {
	 int nsec = (x->tv_usec - y->tv_usec) / 1000000;
	 y->tv_usec += 1000000 * nsec;
	 y->tv_sec -= nsec;
   }

   /* Compute the time remaining to wait.
	  tv_usec is certainly positive. */
   result->tv_sec = x->tv_sec - y->tv_sec;
   result->tv_usec = x->tv_usec - y->tv_usec;

   /* Return 1 if result is negative. */
   return x->tv_sec < y->tv_sec;
 }

