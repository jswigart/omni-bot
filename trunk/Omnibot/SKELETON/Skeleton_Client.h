////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-02-11 20:03:50 -0800 (Mon, 11 Feb 2008) $
// $LastChangedRevision: 2438 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __SKELETON_CLIENT_H__
#define __SKELETON_CLIENT_H__

#include "Client.h"
#include "Skeleton_Config.h"

// class: Skeleton_Client
//		Skeleton Bot Class
class Skeleton_Client : public Client
{
public:

	NavFlags GetTeamFlag();
	NavFlags GetTeamFlag(int _team);

	void SendVoiceMacro(int _macroId) {};

	float GetGameVar(GameVar _var) const;
	float GetAvoidRadius(int _class) const;

	bool DoesBotHaveFlag(MapGoalPtr _mapgoal);

	Skeleton_Client();
	virtual ~Skeleton_Client();
protected:
};

#endif
