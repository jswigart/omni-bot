////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __HL2DM_GAME_H__
#define __HL2DM_GAME_H__

class gmMachine;
class gmTableObject;

#include "IGame.h"
#include "HL2DM_Config.h"

// class: HL2DM_Game
//		Game Type for Half-life 2 Deathmatch
class HL2DM_Game : public IGame
{
public:
	bool Init( System & system );

	void InitScriptBinds( gmMachine *_machine );

	virtual Client *CreateGameClient();

	int GetVersionNum() const;
	const char *GetGameAbbrev() const;
	const char *GetGameName() const;

	virtual bool RendersToGame() const
	{
		return true;
	}
	
	void GetTeamEnumeration( const IntEnum *&_ptr, int &num );
	void GetWeaponEnumeration( const IntEnum *&_ptr, int &num );

	HL2DM_Game();
	virtual ~HL2DM_Game();
protected:
	void InitScriptClasses( gmMachine *_machine, gmTableObject *_table );
	void InitScriptEvents( gmMachine *_machine, gmTableObject *_table );
	void InitScriptTraceMasks( gmMachine *_machine, gmTableObject *_table );

	// Commands
	void InitCommands();

	static const float HL2DM_GetEntityClassTraceOffset( const TargetInfo &_target );
	static const float HL2DM_GetEntityClassAimOffset( const TargetInfo &_target );
};

#endif
