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

class gmMachine;
class gmTableObject;

#include "IGame.h"
#include "TF_Game.h"

// class: FF_Game
//		Game Type for Fortress Forever.
class FF_Game : public TF_Game
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
	const char *GetGameDatabaseAbbrev() const;
	virtual NavigatorID GetDefaultNavigator() const
	{
		return NAVID_RECAST;
	}

	virtual bool RendersToGame() const
	{
		return true;
	}

	ClientPtr &GetClientFromCorrectedGameId( int _gameid );

	void GetTeamEnumeration( const IntEnum *&_ptr, int &num );

	FF_Game();
	~FF_Game();
protected:
	void GetGameVars( GameVars &_gamevars );
};

#endif
