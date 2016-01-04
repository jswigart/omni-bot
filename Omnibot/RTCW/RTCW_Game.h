////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __RTCW_GAME_H__
#define __RTCW_GAME_H__

class gmMachine;
class gmTableObject;

#include "IGame.h"

// class: RTCW_Game
//		Game Type for Returnto Castle Wolfenstein
class RTCW_Game : public IGame
{
public:
	bool Init( System & system );

	virtual void StartGame();
	virtual Client *CreateGameClient();

	void AddBot( ParamsAddbot& addbot, bool _createnow = true );
	void ClientJoined( const EvClientConnected *_msg );
	
	void GetTeamEnumeration( const IntEnum *&_ptr, int &num );
	void GetWeaponEnumeration( const IntEnum *&_ptr, int &num );

	RTCW_Game();
	~RTCW_Game();
protected:
	void InitScriptBinds( gmMachine *_machine );
	void InitScriptClasses( gmMachine *_machine, gmTableObject *_table );
	void InitScriptEntityFlags( gmMachine *_machine, gmTableObject *_table );
	void InitScriptEvents( gmMachine *_machine, gmTableObject *_table );
	void InitScriptPowerups( gmMachine *_machine, gmTableObject *_table );
	void InitVoiceMacros( gmMachine *_machine, gmTableObject *_table );

	// Commands
	void InitCommands();

	static const float RTCW_GetEntityClassTraceOffset( const TargetInfo &_target );
	static const float RTCW_GetEntityClassAimOffset( const TargetInfo &_target );
};

#endif
