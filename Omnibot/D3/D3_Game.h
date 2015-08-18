////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __D3_GAME_H__
#define __D3_GAME_H__

class gmMachine;
class gmTableObject;

#include "IGame.h"
#include "D3_Config.h"

// class: D3_Game
class D3_Game : public IGame
{
public:
	bool Init( System & system );
	void StartGame();

	virtual Client *CreateGameClient();
			
	void GetTeamEnumeration( const IntEnum *&_ptr, int &num );
	void GetWeaponEnumeration( const IntEnum *&_ptr, int &num );

	D3_Game();
	~D3_Game();
protected:
	void InitScriptBinds( gmMachine *_machine );
	void InitScriptClasses( gmMachine *_machine, gmTableObject *_table );
	void InitScriptEvents( gmMachine *_machine, gmTableObject *_table );
	void InitScriptEntityFlags( gmMachine *_machine, gmTableObject *_table );
	void InitScriptPowerups( gmMachine *_machine, gmTableObject *_table );

	static const float D3_GetEntityClassTraceOffset( const TargetInfo &_target );
	static const float D3_GetEntityClassAimOffset( const TargetInfo &_target );
};

#endif
