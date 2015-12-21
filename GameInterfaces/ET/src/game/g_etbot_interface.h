/*
 * ET <-> Omni-Bot interface header file.
 *
 */

#ifndef __G_ETBOT_INTERFACE_H__
#define __G_ETBOT_INTERFACE_H__

//#include "q_shared.h"
#include "g_local.h"

//#define NO_BOT_SUPPORT

// IMPORTANT: when changed this has to be copied manually to GAMEVERSION (g_local.h)
#define OMNIBOT_NAME "Omni-Bot:etmain"

#define OMNIBOT_MODNAME "etmain"
#define OMNIBOT_MODVERSION "2.60"


//////////////////////////////////////////////////////////////////////////
// g_OmniBotFlags bits
enum BotFlagOptions
{
	OBF_DONT_XPSAVE = ( 1 << 0 ), // Disables XPSave for bots 
	OBF_DONT_MOUNT_TANKS = ( 1 << 1 ), // Bots cannot mount tanks 
	OBF_DONT_MOUNT_GUNS = ( 1 << 2 ), // Bots cannot mount emplaced guns
	OBF_DONT_SHOW_BOTCOUNT = ( 1 << 3 ), // Don't count bots
	OBF_GIBBING = ( 1 << 4 ), // Bots will target ungibbed enemies
	OBF_TRIGGER_MINES = ( 1 << 5 ), // Bots will trigger team and spotted mines
	OBF_SHOVING = ( 1 << 6 ), // Bots can use g_shove
	OBF_NEXT_FLAG = ( 1 << 16 ), // mod specific flags start from here
};
//////////////////////////////////////////////////////////////////////////

int Bot_Interface_Init();
void Bot_Interface_InitHandles();
int Bot_Interface_Shutdown();

void Bot_Interface_Update();

void Bot_Interface_ConsoleCommand();

qboolean Bot_Util_AllowPush( int weaponId );
qboolean Bot_Util_CheckForSuicide( gentity_t *ent );
const char *_GetEntityName( gentity_t *_ent );

//void Bot_Util_AddGoal(gentity_t *_ent, int _goaltype, int _team, const char *_tag, obUserData *_bud);
void Bot_Util_SendTrigger( gentity_t *_ent, gentity_t *_activator, const char *_tagname, const char *_action );

int Bot_WeaponGameToBot( int weapon );
int Bot_TeamGameToBot( int team );
int Bot_PlayerClassGameToBot( int playerClass );

void Bot_EntityQueued( gentity_t * ent );
void Bot_EntityDeleted( gentity_t * ent );

//////////////////////////////////////////////////////////////////////////

void Bot_Event_ClientConnected( gentity_t * _player, qboolean _isbot );
void Bot_Event_ClientDisConnected( gentity_t * _player );

void Bot_Event_Spawn( gentity_t * _player );

void Bot_Event_TakeDamage( gentity_t * _player, gentity_t * _attacker, const char * damagetype, float damage );
void Bot_Event_Death( gentity_t * _victim, gentity_t * _killer, const char * damagetype );
void Bot_Event_KilledSomeone( gentity_t * _attacker, gentity_t * _victim, const char *_meansofdeath );
void Bot_Event_Revived( gentity_t * _player, gentity_t * _whodoneit );
void Bot_Event_Healed( gentity_t * _player, gentity_t * _whodoneit );
void Bot_Event_RecievedAmmo( gentity_t * _player, gentity_t * _whodoneit );

void Bot_Event_FireWeapon( gentity_t * _player, int _weaponId, gentity_t *_projectile );
void Bot_Event_PreTriggerMine( gentity_t * _player, gentity_t *_mine );
void Bot_Event_PostTriggerMine( gentity_t * _player, gentity_t *_mine );
void Bot_Event_MortarImpact( gentity_t * _player, vec3_t _pos, float radius );

void Bot_Event_Spectated( gentity_t * _player, int _who );

void Bot_Event_ChatMessage( gentity_t * _player, gentity_t *_source, int _type, const char *_message );
void Bot_Event_VoiceMacro( gentity_t * _player, gentity_t *_source, int _type, const char *_message );

void Bot_Event_Sound( gentity_t *_source, int _sndtype, const char *_name );

void Analytic_Event( const char * eventName, const float * optionalPos );
void Analytic_EventWithValue( const char * eventName, float value, const float * optionalPos );

void Bot_Event_FireTeamCreated( gentity_t * _player, gentity_t *leader, int _fireteamnum );
void Bot_Event_FireTeamDestroyed( gentity_t * _player, gentity_t *leader, int _fireteamnum );
void Bot_Event_JoinedFireTeam( gentity_t * _player, gentity_t *leader, int _fireteamnum );
void Bot_Event_LeftFireTeam( gentity_t * _player, gentity_t *leader, int _fireteamnum );
void Bot_Event_InviteFireTeam( gentity_t * _player, gentity_t * _invitee, int _fireteamnum );
void Bot_Event_FireTeam_Proposal( gentity_t * _player, gentity_t * _proposed, int _fireteamnum );
void Bot_Event_FireTeam_Warn( gentity_t * _player, gentity_t * _warned, int _fireteamnum );

// goal helpers
void Bot_AddDynamiteGoal( gentity_t *_ent, int _team, const char *_tag );
void Bot_AddFallenTeammateGoals( gentity_t *_teammate, int _team );
void AddDeferredGoal( gentity_t *ent );
void UpdateGoalEntity( gentity_t *oldent, gentity_t *newent );
void GetEntityCenter( gentity_t *ent, vec3_t pos );

#endif
