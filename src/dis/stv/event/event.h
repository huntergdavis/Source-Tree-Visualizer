/*
 * event.h
 *
 *  Created on: Aug 19, 2010
 *      Author: programmer
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "event_type.h"
#include "../system/discursive_system.h"


class Event
{
private:
	EventType* type;
public:
	virtual const EventType& getType() = 0;
};

#endif /* EVENT_H_ */
