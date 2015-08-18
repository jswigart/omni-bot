////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __OF_GAME_H__
#define __OF_GAME_H__

class gmMachine;
class gmTableObject;

#include "IGame.h"
#include "TF_Game.h"

// class: OF_Game
//		Game Type for Open Fortress.
class OF_Game : public TF_Game
{
public:
	bool Init( System & system );

	virtual Client *CreateGameClient();
	
	void GetTeamEnumeration( const IntEnum *&_ptr, int &num );

	OF_Game();
	~OF_Game();
protected:
	void InitScriptEntityFlags( gmMachine *_machine, gmTableObject *_table );

	static const float OF_GetEntityClassTraceOffset( const TargetInfo &_target );
	static const float OF_GetEntityClassAimOffset( const TargetInfo &_target );
};

#endif
