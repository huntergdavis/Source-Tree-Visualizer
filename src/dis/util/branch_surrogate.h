/*
 * branch_surrogate.h
 *
 *  Created on: Aug 11, 2010
 *      Author: anonymous
 */

#ifndef BRANCH_SURROGATE_H_
#define BRANCH_SURROGATE_H_

class BranchSurrogate : public Surrogate
{
public:
	// -------------------------------------------------------------------------
	// API :: Surrogate::weight
	// PURPOSE ::
	//         ::
	// PARAMETERS ::
	// RETURN ::
	// -------------------------------------------------------------------------
	virtual double weight()
	{
		return 0.0;
	}

	// -------------------------------------------------------------------------
	// API :: Surrogate::weight
	// PURPOSE ::
	//         ::
	// PARAMETERS ::
	// RETURN ::
	// -------------------------------------------------------------------------
	virtual double x()
	{
		return 0.0;
	}

	// -------------------------------------------------------------------------
	// API :: Surrogate::weight
	// PURPOSE ::
	//         ::
	// PARAMETERS ::
	// RETURN ::
	// -------------------------------------------------------------------------
	virtual double y()
	{
		return 0.0;
	}
};


#endif /* BRANCH_SURROGATE_H_ */
