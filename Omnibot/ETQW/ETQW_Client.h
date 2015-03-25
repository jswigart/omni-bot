////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __ETQW_CLIENT_H__
#define __ETQW_CLIENT_H__

#include "Client.h"
#include "ETQW_Config.h"

// class: ETQW_Client
//		Extended client class for Enemy-Territory.
class ETQW_Client : public Client
{
public:
	friend class ETQW_Game;

	void Init( int _gameid );

	NavFlags GetTeamFlag( int _team ) const;

	void GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags );

	void SendVoiceMacro( int _macroId );

	void ProcessGotoNode( const Path &_path );

	float GetGameVar( GameVar _var ) const;

	bool DoesBotHaveFlag( MapGoalPtr _mapgoal );

	bool CanBotSnipe();
	bool GetSniperWeapon( int &nonscoped, int &scoped );

	bool GetSkills( gmMachine *machine, gmTableObject *tbl );

	float GetBreakableTargetDist() const
	{
		return mBreakableTargetDistance;
	}

	void SetupBehaviorTree();

	ETQW_Client();
	virtual ~ETQW_Client();
protected:
	void ProcessEvent( const MessageHelper &_message, CallbackParameters &_cb );
	int HandleVoiceMacroEvent( const MessageHelper &_message );

	float	 mBreakableTargetDistance;
};

#endif
