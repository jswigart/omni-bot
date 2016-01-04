////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// Title: JA Message Structure Definitions
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __JA_MESSAGES_H__
#define __JA_MESSAGES_H__

#include "Base_Messages.h"
#include "JA_Config.h"

#pragma pack(push)
#pragma pack(4)

//////////////////////////////////////////////////////////////////////////
// Events

enum ET_Event
{
	JA_EVENT_BEGIN = MSG_BASE_LAST,

	JA_MSG_FEELFORCE,

	JA__EVENT_END
};

struct EvFeelForce
{
	typedef MessageT<EvFeelForce, JA_MSG_FEELFORCE> Msg;

	GameEntity		mInflictor;
	JA_ForcePower	mPower;
};

#pragma pack(pop)

#endif
