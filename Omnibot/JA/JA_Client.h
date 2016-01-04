////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __JA_CLIENT_H__
#define __JA_CLIENT_H__

#include "Client.h"
#include "JA_Config.h"

// class: JA_Client
//		Extended client class for Jedi-Academy.
class JA_Client : public Client
{
public:
	friend class JA_Game;

	void Init( int _gameid );

	NavFlags GetTeamFlag( int _team ) const;

	void GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags );

	void SendVoiceMacro( int _macroId );

	void ProcessGotoNode( const Path &_path );

	float GetGameVar( GameVar _var ) const;

	bool DoesBotHaveFlag( MapGoalPtr _mapgoal );

	bool CanBotSnipe();
	bool GetSniperWeapon( int &nonscoped, int &scoped );

	float GetBreakableTargetDist() const
	{
		return mBreakableTargetDistance;
	}

	void SetupBehaviorTree();

	JA_Client();
	virtual ~JA_Client();
protected:
	void ProcessEvent( const Message &message, CallbackParameters & cb );
	int HandleVoiceMacroEvent( const EvVoiceMacro* msg );

	float	 mBreakableTargetDistance;
};

#endif
