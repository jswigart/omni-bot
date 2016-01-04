////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TF_CLIENT_H__
#define __TF_CLIENT_H__

#include "Client.h"
#include "TF_Config.h"
#include "TF_Messages.h"

// class: TF_Client
//		Bot Client for FF Game
class TF_Client : public Client
{
public:
	typedef enum
	{
		TF_FL_DISGUISEDISABLED = Client::NUM_INTERNAL_FLAGS,
		TF_FL_CLOAKED,

		// THIS MUST BE LAST
		TF_NUM_INTERNAL_FLAGS
	} TF_InternalFlags;

	void Init(int _gameid);

	void Update();

	void SendVoiceMacro(int _macroId) {};

	float GetGameVar(GameVar _var) const;

	bool DoesBotHaveFlag(MapGoalPtr _mapgoal);

	bool CanBotSnipe();
	bool GetSniperWeapon(int &nonscoped, int &scoped);

	void ProcessGotoNode( const PathInterface::PathCorner corners[ 2 ], const size_t numEdges );
	void ProcessGotoNode(const Path &_path);
	
	void SetupBehaviorTree();

	TF_Client();
	virtual ~TF_Client();
protected:

	void ProcessEvent(const Message &_message, CallbackParameters &_cb);

	bool	mDoubleJumping;
	float	mDoubleJumpHeight;
};

#endif
