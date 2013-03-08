////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: plastovicka $
// $LastChangedDate: 2012-07-10 07:28:45 -0500 (Tue, 10 Jul 2012) $
// $LastChangedRevision: 677 $
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __DEBUGCONNECTION_H__
#define __DEBUGCONNECTION_H__

#include "connection.h"
//#include "common.pb.h"

#ifdef ENABLE_REMOTE_DEBUGGING

#define SET_IF_DIFF( cachedMsg, msgDiff, newvalue, var ) \
	if ( (cachedMsg).var() != (newvalue) ) {\
	(msgDiff).set_##var( (newvalue) ); \
	}

namespace RemoteLib {
	class DebugConnection : public Connection {
	public:
		virtual void onConnect();
		virtual void onDisconnect();

		Remote::Game	cachedState;
		Remote::Game	updateState;

		DebugConnection( TcpSocket & socket ) : Connection( socket ) {}
	};
};

#endif
#endif
