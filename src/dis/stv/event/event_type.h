/*
 * event_type.h
 *
 *  Created on: Aug 19, 2010
 *      Author: programmer
 */

#ifndef EVENT_TYPE_H_
#define EVENT_TYPE_H_

#include "../system/discursive_system.h"


class EventType
{
public:
	virtual bool EventType::operator==(const EventType& other) const = 0;
};

#endif /* EVENT_TYPE_H_ */
