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
	
	virtual bool GetAnalyticsKeys( GameAnalytics::Keys & keys );

	void AddBot( ParamsAddbot & parms, bool _createnow = true );

	void ClientJoined( const EvClientConnected *msg );
	
	void GetTeamEnumeration( const IntEnum *&_ptr, int &num );
	void GetWeaponEnumeration( const IntEnum *&_ptr, int &num );

	virtual bool AddWeaponId( const char * weaponName, int weaponId );

	static int CLASSEXoffset;
	static bool IsETBlight, IsBastardmod;

	ET_Game();
	virtual ~ET_Game();
protected:
	void InitScriptBinds( gmMachine *_machine );
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
