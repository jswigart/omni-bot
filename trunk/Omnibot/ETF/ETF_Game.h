////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __ETF_GAME_H__
#define __ETF_GAME_H__

class Waypoint;
class gmMachine;
class gmTableObject;

#include "IGame.h"
#include "TF_Game.h"

// class: ETF_Game
//		Game Type for Enemy Territory Fortress.
class ETF_Game : public TF_Game
{
public:
	bool Init( System & system );

	//void AddBot(const std::string &_name, int _team, int _class, const std::string _profile, bool _createnow);

	virtual Client *CreateGameClient();

	int GetVersionNum() const;
	const char *GetDLLName() const;
	const char *GetModSubFolder() const;
	const char *GetGameName() const;
	const char *GetNavSubfolder() const;
	const char *GetScriptSubfolder() const;
	const char *GetGameDatabaseAbbrev() const { return "etf"; }
	virtual NavigatorID GetDefaultNavigator() const { return NAVID_RECAST; }
	bool ReadyForDebugWindow() const { return true; }
	virtual const char *IsDebugDrawSupported() const;

	virtual bool GetAnalyticsKeys( GameAnalyticsKeys & keys );

	void GetTeamEnumeration(const IntEnum *&_ptr, int &num);
	
	ETF_Game() { }
	virtual ~ETF_Game() {}
protected:

	void GetGameVars(GameVars &_gamevars);

	// Script support.
	void InitScriptEntityFlags(gmMachine *_machine, gmTableObject *_table);
	void InitScriptPowerups(gmMachine *_machine, gmTableObject *_table);

	static const float ETF_GetEntityClassTraceOffset(const int _class, const BitFlag64 &_entflags);
	static const float ETF_GetEntityClassAimOffset(const int _class, const BitFlag64 &_entflags);
	static const void ETF_GetEntityVisDistance(float &_distance, const TargetInfo &_target, const Client *_client);
};

#endif
