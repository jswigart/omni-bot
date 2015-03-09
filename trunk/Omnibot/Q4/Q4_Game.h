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

	void AddBot(Msg_Addbot &_addbot, bool _createnow = true);
	
	virtual Client *CreateGameClient();

	int GetVersionNum() const;
	const char *GetDLLName() const;
	const char *GetGameName() const;
	const char *GetModSubFolder() const;
	const char *GetNavSubfolder() const;
	const char *GetScriptSubfolder() const;
	const char *GetGameDatabaseAbbrev() const { return "quake4"; }
	NavigatorID GetDefaultNavigator() const;
	
	const char *FindClassName(obint32 _classId);

	void GetTeamEnumeration(const IntEnum *&_ptr, int &num);
	void GetWeaponEnumeration(const IntEnum *&_ptr, int &num);
	
	Q4_Game() {};
	virtual ~Q4_Game() {};
protected:

	void GetGameVars(GameVars &_gamevars);

	// Script support.
	void InitScriptBinds(gmMachine *_machine);
	void InitScriptClasses(gmMachine *_machine, gmTableObject *_table);
	void InitScriptEvents(gmMachine *_machine, gmTableObject *_table);
	void InitScriptEntityFlags(gmMachine *_machine, gmTableObject *_table);
	void InitScriptPowerups(gmMachine *_machine, gmTableObject *_table);
	void InitScriptBuyMenu(gmMachine *_machine, gmTableObject *_table);

	static const float Q4_GetEntityClassTraceOffset(const int _class, const BitFlag64 &_entflags);
	static const float Q4_GetEntityClassAimOffset(const int _class, const BitFlag64 &_entflags);
};

#endif
