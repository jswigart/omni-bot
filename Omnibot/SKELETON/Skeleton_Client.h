////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __SKELETON_CLIENT_H__
#define __SKELETON_CLIENT_H__

#include "Client.h"
#include "Skeleton_Config.h"

// class: Skeleton_Client
//		Skeleton Bot Class
class Skeleton_Client : public Client
{
public:
	NavFlags GetTeamFlag(int _team) const;
	void GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags );

	void SendVoiceMacro(int _macroId) {};

	float GetGameVar(GameVar _var) const;

	bool DoesBotHaveFlag(MapGoalPtr _mapgoal);

	Skeleton_Client();
	virtual ~Skeleton_Client();
protected:
};

#endif
