////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "EventReciever.h"
#include "IGame.h"
#include "ScriptManager.h"

EventReciever::EventReciever()
{
}

EventReciever::~EventReciever()
{
}

void EventReciever::SendEvent( const Message & message, uint32_t  targetState )
{
	ProcessEventImpl( message, targetState );
}

void EventReciever::ProcessEventImpl( const Message & message, uint32_t  targetState )
{
	// Subclasses can override this function to perform additional stuff
	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
	CallbackParameters cb( message.Id(), pMachine );
	ProcessEvent( message, cb );
}

void EventReciever::ProcessEvent( const Message & message, CallbackParameters & cb )
{
	message; cb;
}