////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __FF_GAME_H__
#define __FF_GAME_H__

class Waypoint;
class gmMachine;
class gmTableObject;

#include "IGame.h"
#include "TF_Game.h"

// class: FF_Game
//		Game Type for Fortress Forever.
class FF_Game : public TF_Game
{
public:
	bool Init();

	//void AddBot(const std::string &_name, int _team, int _class, const std::string _profile, bool _createnow);

	virtual Client *CreateGameClient();

	int GetVersionNum() const;
	const char *GetDLLName() const;
	const char *GetModSubFolder() const;
	const char *GetGameName() const;
	const char *GetNavSubfolder() const;
	const char *GetScriptSubfolder() const;
	const char *GetGameDatabaseAbbrev() const { return "ff"; }
	virtual eNavigatorID GetDefaultNavigator() const { return NAVID_NAVMESH; }

	virtual bool RendersToGame() const { return true; }

	ClientPtr &GetClientFromCorrectedGameId(int _gameid);

	void GetTeamEnumeration(const IntEnum *&_ptr, int &num);

	FF_Game() { }
	virtual ~FF_Game() {}
protected:

	void GetGameVars(GameVars &_gamevars);

	//static const float FF_GetEntityClassTraceOffset(const int _class, const BitFlag64 &_entflags);
	//static const float FF_GetEntityClassAimOffset(const int _class, const BitFlag64 &_entflags);
};

#endif
