////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __MC_CLIENT_H__
#define __MC_CLIENT_H__

#include "Client.h"
#include "MC_Config.h"
#include "MC_Messages.h"

// class: MC_Client
//		Client for Half-life 2 Deathmatch
class MC_Client : public Client
{
public:
	NavFlags GetTeamFlag( int _team ) const;
	void GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags );
	
	void SendVoiceMacro( int _macroId );

	float GetGameVar( GameVar _var ) const;

	bool DoesBotHaveFlag( MapGoalPtr _mapgoal );

	void ProcessEvent( const Message & message, CallbackParameters & cb );

	void Update();

	MC_Client();
	virtual ~MC_Client();
protected:
	bool CanUseModule( MC_Module mod ) const;
};

#endif
