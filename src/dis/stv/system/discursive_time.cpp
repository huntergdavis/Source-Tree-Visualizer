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
//         :: instantiates a default keyname 'tic'
// PARAMETERS :: std::string keyValue - key name, struct timeval tv; keyTime - key time
// RETURN :: None
// -------------------------------------------------------------------------
DiscursiveTime::DiscursiveTime(std::string keyName)
{
	struct timeval defaultTime;
	gettimeofday(&defaultTime,NULL);
	keyTimeMap[keyName] = defaultTime;

	// initialize the running total for this key to be zero
	defaultTime.tv_sec = 0;
	defaultTime.tv_usec = 0;
	keyTimeRunningTotal[keyName] = defaultTime;
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
	struct timeval defaultTime;
	gettimeofday(&defaultTime,NULL);
	keyTimeMap[""] = defaultTime;

	// initialize the running total for this key to be zero
	defaultTime.tv_sec = 0;
	defaultTime.tv_usec = 0;
	keyTimeRunningTotal[""] = defaultTime;
}

// -------------------------------------------------------------------------
// API :: DiscursiveTime::~DiscursiveTime
// PURPOSE :: disinstantiation of DiscursiveTime
//         ::
// PARAMETERS ::
// RETURN :: None
// -------------------------------------------------------------------------
DiscursiveTime::~DiscursiveTime()
{

};



// -------------------------------------------------------------------------
// API :: DiscursiveTime::Tic
// PURPOSE :: sets a 'tic' value, the initial of a time pair
//         ::
// PARAMETERS :: std::string ticType - storage hash for retrieval of tic later
// RETURN :: None
// -------------------------------------------------------------------------
void DiscursiveTime::Tic(std::string ticType)
{
	struct timeval defaultTime;
	// loop over all stored time values looking for ticType
	// if not found, initialize the total time to be zero
	bool foundTicType = 0;
	for( std::map<std::string,struct timeval>::iterator i=keyTimeMap.begin(); i!=keyTimeMap.end(); ++i)
	{
		if( (*i).first == ticType )
		{
			foundTicType = 1;
		}
	}
	if(foundTicType == 0)
	{
		// initialize the running total for this key to be zero
		defaultTime.tv_sec = 0;
		defaultTime.tv_usec = 0;
		keyTimeRunningTotal[ticType] = defaultTime;
	}
	gettimeofday(&defaultTime, NULL);
	keyTimeMap[ticType] = defaultTime;

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
	// loop over all stored time values looking for ticType
	bool foundTicType = 0;
	struct timeval ticTypeTimeVal;
	for( std::map<std::string,struct timeval>::iterator ii=keyTimeMap.begin(); ii!=keyTimeMap.end(); ++ii)
	{
		if( (*ii).first == ticType )
		{
			foundTicType = 1;
			ticTypeTimeVal = (*ii).second;
		}
	}

	// error out if toc called on non-existant tic value
	if(foundTicType == 0)
	{
		DiscursiveError("Could not find tic type %s for toc call\n",ticType.c_str());
	}

	// subtract time values, check if in correct range
	struct timeval result;
	struct timeval tocValue;
	gettimeofday(&tocValue, NULL);
	timersub(&tocValue,&ticTypeTimeVal,&result);

	// update the running total for this ticType
	struct timeval originalTotal = keyTimeRunningTotal[ticType];
	struct timeval runningTotal;
	timeradd(&originalTotal,&result,&runningTotal);
	keyTimeRunningTotal[ticType] = runningTotal;
	
	// debug print values
	DiscursiveDebugPrint("time","Elapsed : %ld seconds, %ld microseconds\n", result.tv_sec,result.tv_usec);

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
	struct timeval resultValue = Toc(ticType);

	DiscursivePrint("Elapsed : %ld seconds, %ld microseconds\n", resultValue.tv_sec,resultValue.tv_usec);
};


// -------------------------------------------------------------------------
// API :: DiscursiveTime::PrintRunningTotals
// PURPOSE :: loops through all total pairs and prints out their values
//         ::
// PARAMETERS :: None
// RETURN :: None
// -------------------------------------------------------------------------
void  DiscursiveTime::PrintRunningTotals()
{
	// loop over all stored time value totals and print them
	DiscursivePrint("Running Totals of all Tic/Toc Pairs:\n");
	for( std::map<std::string,struct timeval>::iterator ii=keyTimeRunningTotal.begin(); ii!=keyTimeRunningTotal.end(); ++ii)
	{
		DiscursivePrint("Key Value: %s = %ld seconds, %ld useconds\n",(*ii).first.c_str(),(*ii).second.tv_sec,(*ii).second.tv_usec);
	}

};





