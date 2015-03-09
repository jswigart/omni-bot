////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart@gmail.com $
// $LastChangedDate: 2013-03-07 21:15:21 -0600 (Thu, 07 Mar 2013) $
// $LastChangedRevision: 837 $
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __GAMEEVENTS_H__
#define __GAMEEVENTS_H__

template<int LEN>
struct StringParm
{
	StringParm( const char * str )
	{
		strncpy( mBuffer, str, sizeof( str ) );
		mBuffer[ LEN-1 ] = 0;
	}
	char mBuffer[ LEN ];
};

typedef StringParm<32> StringParm32;

template<int EVENT_ID, typename PAYLOAD>
struct GameEvent
{
	enum { EventId = EVENT_ID };

	typedef PAYLOAD Payload;
	
	static void CallBot( int gameId, const PAYLOAD & payload )
	{
		gBotFunctions.pfnSendEvent( gameId, MessageHelper(EventId, &d, sizeof(d)) );
	}
	/*static void CallGame( GameEntity dest, const PAYLOAD & payload )
	{
		gGameFunctions->InterfaceSendMessage()
	}*/
	static const Payload * FromData( const void * msgPayload, const size_t msgSize )
	{
		if ( msgSize == sizeof( Payload ) )
			return static_cast<const Payload *>( msgPayload );
		return NULL;
	}
};

#endif