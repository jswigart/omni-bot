////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: plastovicka $
// $LastChangedDate: 2012-07-13 04:21:10 -0500 (Fri, 13 Jul 2012) $
// $LastChangedRevision: 681 $
//
////////////////////////////////////////////////////////////////////////////////



#ifdef ENABLE_REMOTE_DEBUGGING

namespace RemoteLib {
	void DebugConnection::onConnect() {
		EngineFuncs::ConsoleMessage( va( "Connecting to %s", getIp() ) );
	}

	void DebugConnection::onDisconnect() {
		EngineFuncs::ConsoleMessage( va( "Remote disconnected %s", getIp() ) );
	}
};

#endif
