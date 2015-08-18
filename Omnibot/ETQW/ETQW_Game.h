////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __ETQW_GAME_H__
#define __ETQW_GAME_H__

class gmMachine;
class gmTableObject;

#include "IGame.h"

// class: ETQW_Game
//		Game Type for Enemy-Territory.
class ETQW_Game : public IGame
{
public:
	bool Init( System & system );

	virtual Client *CreateGameClient();

	int GetVersionNum() const;
	const char *GetGameAbbrev() const;
	const char *GetGameName() const;

	void AddBot( Msg_Addbot &_addbot, bool _createnow = true );

	void ClientJoined( const Event_SystemClientConnected *_msg );
	
	void GetTeamEnumeration( const IntEnum *&_ptr, int &num );
	void GetWeaponEnumeration( const IntEnum *&_ptr, int &num );

	ETQW_Game();
	virtual ~ETQW_Game();
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

	static const float ETQW_GetEntityClassTraceOffset( const TargetInfo &_target );
	static const float ETQW_GetEntityClassAimOffset( const TargetInfo &_target );
};

#endif
