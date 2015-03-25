////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __HL2DM_CLIENT_H__
#define __HL2DM_CLIENT_H__

#include "Client.h"
#include "HL2DM_Config.h"

// class: HL2DM_Client
//		Client for Half-life 2 Deathmatch
class HL2DM_Client : public Client
{
public:
	NavFlags GetTeamFlag(int _team) const;

	void GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags );

	void SendVoiceMacro(int _macroId) {};

	float GetGameVar(GameVar _var) const;

	bool DoesBotHaveFlag(MapGoalPtr _mapgoal);

	HL2DM_Client();
	virtual ~HL2DM_Client();
protected:
};

#endif
