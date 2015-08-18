////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __MC_GAME_H__
#define __MC_GAME_H__

class gmMachine;
class gmTableObject;

#include "IGame.h"
#include "MC_Config.h"

// class: MC_Game
//		Game Type for Half-life 2 Deathmatch
class MC_Game : public IGame
{
public:
	bool Init( System & system );

	void InitScriptBinds( gmMachine *_machine );

	virtual Client *CreateGameClient();
		
	void GetTeamEnumeration( const IntEnum *&_ptr, int &num );
	void GetWeaponEnumeration( const IntEnum *&_ptr, int &num );

	MC_Game();
	virtual ~MC_Game();
protected:
	void InitScriptClasses( gmMachine *_machine, gmTableObject *_table );
	void InitScriptEvents( gmMachine *_machine, gmTableObject *_table );
	void InitScriptCategories( gmMachine *_machine, gmTableObject *_table );
	void InitScriptBotButtons( gmMachine *_machine, gmTableObject *_table );
	void InitScriptTraceMasks( gmMachine *_machine, gmTableObject *_table );
	void InitScriptModules( gmMachine *_machine, gmTableObject *_table );

	// Commands
	void InitCommands();

	static const float MC_GetEntityClassTraceOffset( const TargetInfo &_target );
	static const float MC_GetEntityClassAimOffset( const TargetInfo &_target );
};

#endif
