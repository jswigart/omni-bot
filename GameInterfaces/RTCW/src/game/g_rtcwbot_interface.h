/*
* ET <-> Omni-Bot interface header file.
*
*/

#ifndef __G_RTCW_INTERFACE_H__
#define __G_RTCW_INTERFACE_H__

//#include "q_shared.h"
#include "g_local.h"

//#define NO_BOT_SUPPORT

// IMPORTANT: when changed this has to be copied manually to GAMEVERSION (g_local.h)
#define OMNIBOT_NAME "Omni-Bot"

#define OMNIBOT_MODNAME "rtcwmain"
#define OMNIBOT_MODVERSION "1.41"

//////////////////////////////////////////////////////////////////////////
// g_OmniBotFlags bits
enum BotFlagOptions
{
	OBF_DONT_XPSAVE         = ( 1 << 0 ), // Disables XPSave for bots
	OBF_DONT_MOUNT_TANKS    = ( 1 << 1 ), // Bots cannot mount tanks
	OBF_DONT_MOUNT_GUNS     = ( 1 << 2 ), // Bots cannot mount emplaced guns
	OBF_DONT_SHOW_BOTCOUNT  = ( 1 << 3 ), // Don't count bots
	OBF_NEXT_FLAG           = ( 1 << 4 ), // mod specific flags start from here
	OBF_NO_SPREE_ANNOUNCE	= ( 1 << 5 ), // no killing spree / multikill announce for bots
	OBF_FAST_BOTS		= ( 1 << 6 ), // slightly faster movement for bots
};
//////////////////////////////////////////////////////////////////////////

int Bot_Interface_Init();
void Bot_Interface_InitHandles();
int Bot_Interface_Shutdown();

void Bot_Interface_Update();

int  Bot_Interface_ConsoleCommand();

//void Bot_Util_AddGoal(gentity_t *_ent, int _goaltype, int _team, const char *_tag, obUserData *_bud);
void Bot_Util_SendTrigger( gentity_t *_ent, gentity_t *_activator, const char *_tagname, const char *_action );
qboolean Bot_Util_CheckForSuicide( gentity_t *ent );

int Bot_WeaponGameToBot( int weapon );
int Bot_TeamGameToBot( int team );
int Bot_PlayerClassGameToBot( int playerClass );

void Bot_Queue_EntityCreated( gentity_t *pEnt );
void Bot_Event_EntityDeleted( gentity_t *pEnt );

//////////////////////////////////////////////////////////////////////////

void Bot_Event_ClientConnected( int _client, qboolean _isbot );
void Bot_Event_ClientDisConnected( int _client );
void Bot_Event_Drowning( int _client );

void Bot_Event_TakeDamage( int _client, gentity_t *_ent );
void Bot_Event_Death( int _client, gentity_t *_killer, const char *_meansofdeath );
void Bot_Event_KilledSomeone( int _client, gentity_t *_victim, const char *_meansofdeath );
void Bot_Event_Revived( int _client, gentity_t *_whodoneit );
void Bot_Event_Healed( int _client, gentity_t *_whodoneit );

void Bot_Event_FireWeapon( int _client, int _weaponId, gentity_t *_projectile );
void Bot_Event_PreTriggerMine( int _client, gentity_t *_mine );
void Bot_Event_PostTriggerMine( int _client, gentity_t *_mine );
void Bot_Event_MortarImpact( int _client, vec3_t _pos );

void Bot_Event_Spectated( int _client, int _who );

void Bot_Event_ChatMessage( int _client, gentity_t *_source, int _type, const char *_message );
void Bot_Event_VoiceMacro( int _client, gentity_t *_source, int _type, const char *_message );

void Bot_Event_Sound( int _client, int _sndtype, gentity_t *_source, float *_origin, const char *_name );
void Bot_Event_RecievedAmmo( int _client, gentity_t *_whodoneit );

// goal helpers
void Bot_AddDynamiteGoal( gentity_t *_ent, int _team, const char *_tag );
void Bot_AddFallenTeammateGoals( gentity_t *_teammate, int _team );
void AddDeferredGoal( gentity_t *ent );

#endif
