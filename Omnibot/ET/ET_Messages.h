////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// Title: TF Message Structure Definitions
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TF_MESSAGES_H__
#define __TF_MESSAGES_H__

#include "Base_Messages.h"
#include "ET_Config.h"

#pragma pack(push)
#pragma pack(4)

//////////////////////////////////////////////////////////////////////////

enum ET_Event
{
	ET_EVENT_BEGIN = MSG_BASE_LAST,

	ET_MSG_FIRETEAM_CREATED,
	ET_MSG_FIRETEAM_DISBANDED,
	ET_MSG_FIRETEAM_JOINED,
	ET_MSG_FIRETEAM_LEFT,
	ET_MSG_FIRETEAM_INVITED,
	ET_MSG_FIRETEAM_PROPOSAL,
	ET_MSG_FIRETEAM_WARNED,
	ET_MSG_RECIEVEDAMMO,

	ET_EVENT_END
};

struct EvFireTeamCreated
{
	typedef MessageT<EvFireTeamCreated, ET_MSG_FIRETEAM_CREATED> Msg;

	GameEntity	mTeamLeader;
	int			mFireTeamNum;
};

struct EvFireTeamDisbanded
{
	typedef MessageT<EvFireTeamDisbanded, ET_MSG_FIRETEAM_DISBANDED> Msg;

	GameEntity	mTeamLeader;
	int			mFireTeamNum;
};

struct EvFireTeamJoined
{
	typedef MessageT<EvFireTeamJoined, ET_MSG_FIRETEAM_JOINED> Msg;

	GameEntity	mTeamLeader;
	int			mFireTeamNum;
};

struct EvFireTeamLeft
{
	typedef MessageT<EvFireTeamLeft, ET_MSG_FIRETEAM_LEFT> Msg;

	GameEntity	mTeamLeader;
	int			mFireTeamNum;
};

struct EvFireTeamInvited
{
	typedef MessageT<EvFireTeamInvited, ET_MSG_FIRETEAM_INVITED> Msg;

	GameEntity	mInviter;
	int			mFireTeamNum;
};

struct EvFireTeamProposal
{
	typedef MessageT<EvFireTeamProposal, ET_MSG_FIRETEAM_PROPOSAL> Msg;

	GameEntity	mTeamLeader;
	GameEntity	mInvitee;
	int			mFireTeamNum;
};

struct EvFireTeamWarning
{
	typedef MessageT<EvFireTeamWarning, ET_MSG_FIRETEAM_WARNED> Msg;

	GameEntity	mWarnedBy;
	int			mFireTeamNum;
};

#pragma pack(pop)

#endif
