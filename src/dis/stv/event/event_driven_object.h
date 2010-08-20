/*
 * event_driven_object.h
 *
 *  Created on: Aug 19, 2010
 *      Author: Mark Christensen
 */

#ifndef EVENT_DRIVEN_OBJECT_H_
#define EVENT_DRIVEN_OBJECT_H_

#include "event.h"
#include "event_type.h"
#include "event_listener.h"
#include "../system/discursive_system.h"


class EventDrivenObjecct
{
public:
	virtual bool registerListener(const EventType& type, const EventListener& listener);
};

#endif /* EVENT_DRIVEN_OBJECT_H_ */
