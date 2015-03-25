////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __Q4_CLIENT_H__
#define __Q4_CLIENT_H__

#include "Client.h"
#include "Q4_Config.h"

// class: Q4_Client
//		Q4 Bot Class
class Q4_Client : public Client
{
public:
	NavFlags GetTeamFlag(int _team) const;
	void GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags );

	void SendVoiceMacro(int _macroId) {};

	float GetGameVar(GameVar _var) const;

	bool DoesBotHaveFlag(MapGoalPtr _mapgoal);

	void SetupBehaviorTree();

	Q4_Client();
	virtual ~Q4_Client();
protected:
};

#endif
