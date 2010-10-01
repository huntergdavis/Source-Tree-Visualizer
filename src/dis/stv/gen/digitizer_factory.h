/*
 * digitizer_factory.h
 *
 *  Created on: Oct 1, 2010
 *      Author:Mark Christensen
 */

#ifndef DIGITIZER_FACTORY_H_
#define DIGITIZER_FACTORY_H_

#include "./digitizer.h"
#include "./space_colonizer.h"

class DigitizerFactory {
public:
	static const int SPACE_COLONIZER = 0;

	static Digitizer* getInstance(int instanceType, int argc, ...);
};

#endif /* DIGITIZER_FACTORY_H_ */
