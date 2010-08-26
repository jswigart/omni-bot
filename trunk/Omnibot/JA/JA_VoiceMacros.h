////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-07-05 03:18:03 -0700 (Sat, 05 Jul 2008) $
// $LastChangedRevision: 3023 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __JA_VOICEMACROS_H__
#define __JA_VOICEMACROS_H__

class Client;

typedef enum
{
	VCHAT_NONE,

	VCHAT_ATTACK_POSITION,
	VCHAT_ATTACK_PRIMARY,
	VCHAT_ATTACK_SECONDARY,
	VCHAT_DEFEND_GUNS,	
	VCHAT_DEFEND_POSITION,
	VCHAT_DEFEND_PRIMARY,
	VCHAT_DEFEND_SECONDARY,
	VCHAT_REPLY_COMING,
	VCHAT_REPLY_GO,
	VCHAT_REPLY_NO,
	VCHAT_REPLY_STAY,
	VCHAT_REPLY_YES,
	VCHAT_REQUEST_ASSISTANCE,
	VCHAT_REQUEST_DEMO,
	VCHAT_REQUEST_HEAVYWEAPS,
	VCHAT_REQUEST_MEDIC,
	VCHAT_REQUEST_SUPPLIES,
	VCHAT_REQUEST_TECH,
	VCHAT_SPOT_AIR,
	VCHAT_SPOT_DEFENSES,
	VCHAT_SPOT_EMPLACED,
	VCHAT_SPOT_SNIPER,
	VCHAT_SPOT_TROOPS,
	VCHAT_TACTICS_COVER,
	VCHAT_TACTICS_FALLBACK,
	VCHAT_TACTICS_FOLLOW,
	VCHAT_TACTICS_HOLD,
	VCHAT_TACTICS_SPLITUP,
	VCHAT_TACTICS_TOGETHER,

	VCHAT_TEAM_NUMMESSAGES, // LEAVE THIS AFTER THE TEAM MESSAGES

	VCHAT_GLOBAL_NUMMESSAGES, // LEAVE THIS AFTER THE GLOBAL MESSAGES

	// This must stay last.
	NUM_JA_VCHATS
} eVChat;

// class: JA_VoiceMacros
class JA_VoiceMacros
{
public:

	static int GetVChatId(const char *_string);
	static void SendVoiceMacro(Client *_bot, int _msg);

protected:
};

#endif
