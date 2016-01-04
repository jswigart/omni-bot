////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __Q4_GAME_H__
#define __Q4_GAME_H__

class gmMachine;
class gmTableObject;

#include "IGame.h"
#include "Q4_Config.h"

// class: Q4_Game
//		Basic Game subclass
class Q4_Game : public IGame
{
public:
	bool Init( System & system );
	void StartGame();

	void AddBot( ParamsAddbot& addbot, bool _createnow = true );

	virtual Client *CreateGameClient();

	void GetTeamEnumeration( const IntEnum *&_ptr, int &num );
	void GetWeaponEnumeration( const IntEnum *&_ptr, int &num );

	Q4_Game();
	~Q4_Game();
protected:
	void InitScriptBinds( gmMachine *_machine );
	void InitScriptClasses( gmMachine *_machine, gmTableObject *_table );
	void InitScriptEvents( gmMachine *_machine, gmTableObject *_table );
	void InitScriptEntityFlags( gmMachine *_machine, gmTableObject *_table );
	void InitScriptPowerups( gmMachine *_machine, gmTableObject *_table );
	void InitScriptBuyMenu( gmMachine *_machine, gmTableObject *_table );

	static const float Q4_GetEntityClassTraceOffset( const TargetInfo &_target );
	static const float Q4_GetEntityClassAimOffset( const TargetInfo &_target );
};

#endif
