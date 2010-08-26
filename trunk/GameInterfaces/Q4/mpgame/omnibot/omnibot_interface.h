#pragma once

#include "Omni-Bot.h"

class omnibot_interface
{
public:	
	//////////////////////////////////////////////////////////////////////////
	// omnibot_flags bits
	enum BotFlagOptions
	{		
		OBF_DONT_SHOW_BOTCOUNT	= (1<<0), // Don't count bots
		OBF_NEXT_FLAG			= (1<<2), // mod specific flags start from here
	};
	//////////////////////////////////////////////////////////////////////////

	static bool IsBot(int _clientnum);

	static bool InitBotInterface();
	static void ShutdownBotInterface();
	static void UpdateBotInterface();
	static void Bot_SendTrigger(const TriggerInfo &_triggerInfo);
	static void OmnibotCommand( const idCmdArgs &args );

	static void MapRestart();

	static void DrawHud();

	//////////////////////////////////////////////////////////////////////////

	static void Bot_Event_ClientConnected(int _client, bool _isbot);
	static void Bot_Event_ClientDisConnected(int _client);

	static void Bot_Event_Spectated(int _client, int _who);

	static void Bot_Event_TakeDamage(int _client, idEntity *_ent);
	static void Bot_Event_Death(int _client, idEntity *_killer, const char *_meansofdeath);
	static void Bot_Event_KilledSomeone(int _client, idEntity *_victim, const char *_meansofdeath);
	static void Bot_Event_ResetWeapons(int _client);
	static void Bot_Event_AddWeapon(int _client, int _weaponId);
	static void Bot_Event_FireWeapon(int _client, int _weaponId, idEntity *_projectile);

	static void Bot_Event_Sound(idEntity *_source, int _sndtype, const char *_name);

	static void Notify_ChatMsg(idPlayer *_player, const char *_msg);
	static void Notify_TeamChatMsg(idPlayer *_player, const char *_msg);

	static void Bot_Queue_EntityCreated(idEntity *pEnt);	
	static void Bot_Event_EntityDeleted(idEntity *pEnt);
	
private:
	static void InitOldBots();

	static void Bot_Event_EntityCreated(idEntity *pEnt);
};

int obUtilGetWeaponId(const char *_weaponName);
idEntity *EntityFromHandle(GameEntity _ent);
GameEntity HandleFromEntity(idEntity *_ent);

