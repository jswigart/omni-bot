////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __JA_GAME_H__
#define __JA_GAME_H__

class gmMachine;
class gmTableObject;

#include "IGame.h"

// class: JA_Game
//		Game Type for Jedi-Academy.
class JA_Game : public IGame
{
public:
	bool Init( System & system );

	virtual Client *CreateGameClient();

	void AddBot( ParamsAddbot & parms, bool _createnow = true );

	void ClientJoined( const EvClientConnected *msg );

	void GetTeamEnumeration( const IntEnum *&ptr, int &num );
	void GetWeaponEnumeration( const IntEnum *&ptr, int &num );

	JA_Game();
	virtual ~JA_Game();
protected:
	// Script support.
	void InitScriptClasses( gmMachine *machine, gmTableObject *table );
	void InitScriptEvents( gmMachine *machine, gmTableObject *table );
	void InitScriptEntityFlags( gmMachine *machine, gmTableObject *table );
	void InitScriptPowerups( gmMachine *machine, gmTableObject *table );
	void InitScriptContentFlags( gmMachine *machine, gmTableObject *table );
	void InitScriptBotButtons( gmMachine *machine, gmTableObject *table );
	void InitVoiceMacros( gmMachine *machine, gmTableObject *table );

	// Commands
	void InitCommands();

	static const float JA_GetEntityClassTraceOffset( const TargetInfo &_target );
	static const float JA_GetEntityClassAimOffset( const TargetInfo &_target );
};

#endif
