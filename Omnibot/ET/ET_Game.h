////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __ET_GAME_H__
#define __ET_GAME_H__

class gmMachine;
class gmTableObject;

#include "IGame.h"

// class: ET_Game
//		Game Type for Enemy-Territory.
class ET_Game : public IGame
{
public:
	bool Init( System & system );

	virtual Client *CreateGameClient();

	int GetVersionNum() const;
	const char *GetDLLName() const;
	const char *GetGameName() const;
	const char *GetModSubFolder() const;
	const char *GetNavSubfolder() const;
	const char *GetScriptSubfolder() const;
	const char *GetGameDatabaseAbbrev() const;
	NavigatorID GetDefaultNavigator() const;
	bool ReadyForDebugWindow() const;

	virtual bool GetAnalyticsKeys( GameAnalytics::Keys & keys );

	void AddBot( Msg_Addbot &_addbot, bool _createnow = true );

	void ClientJoined( const Event_SystemClientConnected *_msg );
	
	void GetTeamEnumeration( const IntEnum *&_ptr, int &num );
	void GetWeaponEnumeration( const IntEnum *&_ptr, int &num );

	virtual bool AddWeaponId( const char * weaponName, int weaponId );

	static int CLASSEXoffset;
	static bool IsETBlight, IsBastardmod;

	ET_Game();
	virtual ~ET_Game();
protected:

	void GetGameVars( GameVars &_gamevars );

	// Script support.
	void InitScriptBinds( gmMachine *_machine );
	void InitScriptCategories( gmMachine *_machine, gmTableObject *_table );
	void InitScriptClasses( gmMachine *_machine, gmTableObject *_table );
	void InitScriptSkills( gmMachine *_machine, gmTableObject *_table );
	void InitScriptEvents( gmMachine *_machine, gmTableObject *_table );
	void InitScriptEntityFlags( gmMachine *_machine, gmTableObject *_table );
	void InitScriptPowerups( gmMachine *_machine, gmTableObject *_table );
	void InitVoiceMacros( gmMachine *_machine, gmTableObject *_table );

	// Commands
	void InitCommands();

	static const float ET_GetEntityClassTraceOffset( const TargetInfo &_target );
	static const float ET_GetEntityClassAimOffset( const TargetInfo &_target );

	StringBuffer mExtraWeaponNames;
};

#endif
