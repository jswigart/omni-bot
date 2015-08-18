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
	const char *GetGameAbbrev() const;
	const char *GetGameName() const;
	
	virtual bool GetAnalyticsKeys( GameAnalytics::Keys & keys );

	void GetTeamEnumeration( const IntEnum *&_ptr, int &num );

	ETF_Game();
	~ETF_Game();
protected:
	void InitScriptEntityFlags( gmMachine *_machine, gmTableObject *_table );
	void InitScriptPowerups( gmMachine *_machine, gmTableObject *_table );

	static const float ETF_GetEntityClassTraceOffset( const TargetInfo &_target );
	static const float ETF_GetEntityClassAimOffset( const TargetInfo &_target );
};

#endif
