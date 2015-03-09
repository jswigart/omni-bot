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

	int GetVersionNum() const;
	const char *GetDLLName() const;
	const char *GetGameName() const;
	const char *GetModSubFolder() const;
	const char *GetNavSubfolder() const;
	const char *GetScriptSubfolder() const;
	const char *GetGameDatabaseAbbrev() const { return "doom3"; }
	NavigatorID GetDefaultNavigator() const;

	const char *FindClassName(obint32 _classId);

	void GetTeamEnumeration(const IntEnum *&_ptr, int &num);
	void GetWeaponEnumeration(const IntEnum *&_ptr, int &num);
	
	D3_Game() {};
	virtual ~D3_Game() {};
protected:

	void GetGameVars(GameVars &_gamevars);

	// Script support.
	void InitScriptBinds(gmMachine *_machine);
	void InitScriptClasses(gmMachine *_machine, gmTableObject *_table);
	void InitScriptEvents(gmMachine *_machine, gmTableObject *_table);
	void InitScriptEntityFlags(gmMachine *_machine, gmTableObject *_table);
	void InitScriptPowerups(gmMachine *_machine, gmTableObject *_table);

	static const float D3_GetEntityClassTraceOffset(const int _class, const BitFlag64 &_entflags);
	static const float D3_GetEntityClassAimOffset(const int _class, const BitFlag64 &_entflags);
};

#endif
