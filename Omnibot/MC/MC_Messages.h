////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// Title: MC Message Structure Definitions
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __MC_MESSAGES_H__
#define __MC_MESSAGES_H__

#include "Base_Messages.h"
#include "MC_Config.h"

#pragma pack(push)
#pragma pack(4)

//////////////////////////////////////////////////////////////////////////

enum MC_Events
{
	MC_MSG_BEGIN = MSG_BASE_LAST,
	MC_MSG_PLAYER_SPREE,
	MC_MSG_PLAYER_SPREE_END,
	MC_MSG_SPREEWAR_START,
	MC_MSG_SPREEWAR_END,
	MC_MSG_LEVEL_UP
};

//////////////////////////////////////////////////////////////////////////

struct EvPlayerSpree
{
	typedef MessageT<EvPlayerSpree, MC_MSG_PLAYER_SPREE> Msg;

	GameEntity		mPlayer;
	int				mKills;
};

struct EvPlayerSpreeEnd
{
	typedef MessageT<EvPlayerSpreeEnd, MC_MSG_PLAYER_SPREE_END> Msg;

	GameEntity		mPlayer;
	GameEntity		mByWho;
	int				mKills;
};

struct EvSpreeWarStart
{
	typedef MessageT<EvSpreeWarStart, MC_MSG_SPREEWAR_START> Msg;

	GameEntity		mVictim;
};

struct EvSpreeWarEnd
{
	typedef MessageT<EvSpreeWarEnd, MC_MSG_SPREEWAR_END> Msg;

};

struct EvLevelUp
{
	typedef MessageT<EvLevelUp, MC_MSG_LEVEL_UP> Msg;

	int				mLevel;
};

#pragma pack(pop)

#endif
