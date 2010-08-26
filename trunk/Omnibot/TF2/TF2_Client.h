////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-07-05 03:18:03 -0700 (Sat, 05 Jul 2008) $
// $LastChangedRevision: 3023 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TF2_CLIENT_H__
#define __TF2_CLIENT_H__

#include "TF_Client.h"
#include "TF2_Config.h"

// class: TF2_Client
//		TF2 Bot Class
class TF2_Client : public TF_Client
{
public:

	//NavFlags GetTeamFlag();
	//NavFlags GetTeamFlag(int _team);

	//void SendVoiceMacro(int _macroId) {};

	//float GetGameVar(GameVar _var) const;
	////float GetAvoidRadius(int _class) const;

	//bool DoesBotHaveFlag(MapGoalPtr _mapgoal);

	void ProcessEvent(const MessageHelper &_message, CallbackParameters &_cb);

	void SetupBehaviorTree();

	TF2_Client();
	virtual ~TF2_Client();
protected:

};

#endif
