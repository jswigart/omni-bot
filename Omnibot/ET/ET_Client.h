////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __ET_CLIENT_H__
#define __ET_CLIENT_H__

#include "Client.h"
#include "ET_Config.h"

// class: ET_Client
//		Extended client class for Enemy-Territory.
class ET_Client : public Client
{
public:
	friend class ET_Game;

	void Init( int _gameid );
	void UpdateBotInput();

	NavFlags GetTeamFlag( int _team ) const;

	void GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags );

	void SendVoiceMacro( int _macroId );

	void ProcessGotoNode( const PathInterface::PathCorner corners[ 2 ], const size_t numEdges );
	void ProcessGotoNode( const Path &_path );

	float GetGameVar( GameVar _var ) const;

	bool DoesBotHaveFlag( MapGoalPtr _mapgoal );
	bool IsFlagGrabbable( MapGoalPtr _mapgoal );

	bool CanBotSnipe();
	bool GetSniperWeapon( int &nonscoped, int &scoped );

	bool GetSkills( gmMachine *machine, gmTableObject *tbl );

	float GetBreakableTargetDist() const
	{
		return mBreakableTargetDistance;
	}

	void SetupBehaviorTree();

	ET_Client();
	virtual ~ET_Client();
protected:
	void ProcessEvent( const Message & message, CallbackParameters & cb );
	int HandleVoiceMacroEvent( const EvVoiceMacro* msg );

	float	 mBreakableTargetDistance;
};

#endif
