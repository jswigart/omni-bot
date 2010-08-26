////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-07-05 03:18:03 -0700 (Sat, 05 Jul 2008) $
// $LastChangedRevision: 3023 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __D3_CLIENT_H__
#define __D3_CLIENT_H__

#include "Client.h"
#include "D3_Config.h"

// class: D3_Client
//		D3 Bot Class
class D3_Client : public Client
{
public:

	NavFlags GetTeamFlag();
	NavFlags GetTeamFlag(int _team);

	void SendVoiceMacro(int _macroId) {};

	float GetGameVar(GameVar _var) const;
	float GetAvoidRadius(int _class) const;

	bool DoesBotHaveFlag(MapGoalPtr _mapgoal);

	void SetupBehaviorTree();

	D3_Client();
	virtual ~D3_Client();
protected:

};

#endif
