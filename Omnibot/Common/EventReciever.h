////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __EVENTRECIEVER_H__
#define __EVENTRECIEVER_H__

#include "Omni-Bot_Types.h"
#include "Base_Messages.h"

class CallbackParameters;

// class: EventReciever
//		Base class to provide event processing capabilities to subclasses
class EventReciever
{
public:
	void SendEvent( const Message & message, uint32_t targetState = 0 );

	EventReciever();
	virtual ~EventReciever();
protected:
	virtual void ProcessEventImpl( const Message & message, uint32_t targetState );
	virtual void ProcessEvent( const Message & message, CallbackParameters & cb );
};

#endif
