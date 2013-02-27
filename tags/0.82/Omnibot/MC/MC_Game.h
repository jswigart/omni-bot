////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MC_GAME_H__
#define __MC_GAME_H__

class Waypoint;
class gmMachine;
class gmTableObject;

#include "IGame.h"
#include "MC_Config.h"

// class: MC_Game
//		Game Type for Half-life 2 Deathmatch
class MC_Game : public IGame
{
public:
	bool Init();

	void InitScriptBinds(gmMachine *_machine);

	void RegisterNavigationFlags(PathPlannerBase *_planner);
	void RegisterPathCheck(PathPlannerWaypoint::pfbWpPathCheck &_pfnPathCheck);

	virtual Client *CreateGameClient();

	eNavigatorID GetDefaultNavigator() const ;

	int GetVersionNum() const;
	const char *GetDLLName() const;
	const char *GetGameName() const;
	const char *GetModSubFolder() const;
	const char *GetNavSubfolder() const;
	const char *GetScriptSubfolder() const;
	const char *GetGameDatabaseAbbrev() const { return "MC"; }

	ClientPtr &GetClientFromCorrectedGameId(int _gameid);

	const char *FindClassName(obint32 _classId);

	void GetTeamEnumeration(const IntEnum *&_ptr, int &num);
	void GetWeaponEnumeration(const IntEnum *&_ptr, int &num);

	MC_Game() {};
	virtual ~MC_Game() {};
protected:

	void GetGameVars(GameVars &_gamevars);

	// Script support.
	void InitScriptClasses(gmMachine *_machine, gmTableObject *_table);
	void InitScriptEvents(gmMachine *_machine, gmTableObject *_table);
	void InitScriptCategories(gmMachine *_machine, gmTableObject *_table);
	void InitScriptBotButtons(gmMachine *_machine, gmTableObject *_table);
	void InitScriptTraceMasks(gmMachine *_machine, gmTableObject *_table);

	void InitScriptModules(gmMachine *_machine, gmTableObject *_table);
	void InitScriptUpgrades(gmMachine *_machine, gmTableObject *_table);

	// Commands
	void InitCommands();

	static const float MC_GetEntityClassTraceOffset(const int _class, const BitFlag64 &_entflags);
	static const float MC_GetEntityClassAimOffset(const int _class, const BitFlag64 &_entflags);
};

#endif
