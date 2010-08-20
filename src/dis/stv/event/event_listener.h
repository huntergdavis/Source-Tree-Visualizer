/*
 * event_listener.h
 *
 *  Created on: Aug 19, 2010
 *      Author: programmer
 */

#ifndef EVENT_LISTENER_H_
#define EVENT_LISTENER_H_

#include "event_driven_object.h"
#include "event.h"
#include "../system/discursive_system.h"


class EventListener
{
public:
	virtual void eventOccurred(Event* event) = 0;
};

#endif /* EVENT_LISTENER_H_ */
