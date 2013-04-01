/*
 * OF <-> Omni-Bot interface header file.
 * 
 */

#ifndef __G_OFBOT_INTERFACE_H__
#define __G_OFBOT_INTERFACE_H__

//#include "q_shared.h"
#include "g_local.h"
#include "g_q3f_flag.h"
#include "g_q3f_team.h"
#include "g_q3f_playerclass.h"
#include "g_q3f_weapon.h"
#include "g_q3f_mapents.h"
#include "bg_q3f_util.h"

//#define NO_BOT_SUPPORT

// IMPORTANT: when changed this has to be copied manually to GAMEVERSION (g_local.h)
#define OMNIBOT_NAME "etf"

#define OMNIBOT_MODNAME "etf"
#define OMNIBOT_MODVERSION "1.7"

//////////////////////////////////////////////////////////////////////////
// g_OmniBotFlags bits
enum BotFlagOptions
{
//  OBF_DONT_XPSAVE         = (1<<0), // Disables XPSave for bots 
//  OBF_DONT_MOUNT_TANKS    = (1<<1), // Bots cannot mount tanks 
//  OBF_DONT_MOUNT_GUNS     = (1<<2), // Bots cannot mount emplaced guns
	OBF_DONT_SHOW_BOTCOUNT = (1 << 0),	// Don't count bots
	OBF_NEXT_FLAG = (1 << 1),	// mod specific flags start from here
};

//////////////////////////////////////////////////////////////////////////

int             Bot_Interface_Init();
void            Bot_Interface_InitHandles();
void            Bot_Interface_Shutdown();

void            Bot_Interface_Update();

int             Bot_Interface_ConsoleCommand(int skiparg);

//void Bot_Util_AddGoal(gentity_t *_ent, int _goaltype, int _team, const char *_tag, obUserData *_bud);
void            Bot_Util_SendTrigger(gentity_t * _ent, gentity_t * _activator, const char *_tagname, const char *_action);
void            Bot_AddDisarmGoal(gentity_t *_ent, int _team);

int             Bot_WeaponGameToBot(int weapon, int clsnum);
int             Bot_TeamGameToBot(int team);
int             Bot_TeamGameBitsToBotBits(int teams);
int             Bot_PlayerClassGameToBot(int playerClass);

void            Bot_Queue_EntityCreated(gentity_t * pEnt);
void            Bot_Event_EntityDeleted(gentity_t * pEnt);

void            Bot_TeleporterSource(gentity_t * pEnt);
void            Bot_TeleporterDest(gentity_t * pEnt);

//////////////////////////////////////////////////////////////////////////

void            Bot_Event_GameStarted();
void            Bot_Event_GameEnded(int _winningteam);

void            Bot_Event_ClientConnected(gentity_t * _player, qboolean _isbot);
void            Bot_Event_ClientDisConnected(gentity_t * _player);

void            Bot_Event_ResetWeapons(gentity_t * _player);
void            Bot_Event_AddWeapon(gentity_t * _player, int _weaponId);

void            Bot_Event_TakeDamage(gentity_t * _player, gentity_t * _ent);
void            Bot_Event_Death(gentity_t * _victim, gentity_t * _killer, const char *_meansofdeath);
void            Bot_Event_KilledSomeone(gentity_t * _attacker, gentity_t * _victim, const char *_meansofdeath);
void            Bot_Event_Healed(gentity_t * _player, gentity_t * _whodoneit);

void            Bot_Event_Build_MustBeOnGround(gentity_t * _player, int _buildable);
void            Bot_Event_Build_CantBuild(gentity_t * _player, int _buildable);
void            Bot_Event_Build_AlreadyBuilt(gentity_t * _player, int _buildable);
void            Bot_Event_Build_NotEnoughAmmo(gentity_t * _player, int _buildable);
void            Bot_Event_BuildCancelled(gentity_t * _player, int _buildable);

void            Bot_Event_RadarDetectedEnemy(gentity_t * _player, gentity_t * _ent);

void            Bot_Event_BuildableDamaged(gentity_t * _player, int _type, gentity_t * _buildEnt);
void            Bot_Event_DispenserBuilding(gentity_t * _player, gentity_t * _buildEnt);
void            Bot_Event_DispenserBuilt(gentity_t * _player, gentity_t * _buildEnt);
void            Bot_Event_DispenserEnemyUsed(gentity_t * _player, gentity_t * _enemyUser);
void            Bot_Event_DispenserDestroyed(gentity_t * _player, gentity_t * _attacker);
void            Bot_Event_SentryUpgraded(gentity_t * _player, int _level);
void            Bot_Event_SentryBuilding(gentity_t * _player, gentity_t * _buildEnt);
void            Bot_Event_SentryBuilt(gentity_t * _player, gentity_t * _buildEnt);
void            Bot_Event_SentryDestroyed(gentity_t * _player, gentity_t * _attacker);
void            Bot_Event_SentrySpottedEnemy(gentity_t * _player, gentity_t * _enemy);
void            Bot_Event_SentryAimed(gentity_t * _player, gentity_t * _buildEnt, vec3_t _dir);
void            Bot_Event_DetpackBuilding(gentity_t * _player, gentity_t * _buildEnt);
void            Bot_Event_DetpackBuilt(gentity_t * _player, gentity_t * _buildEnt);
void            Bot_Event_DetpackDetonated(gentity_t * _player);
void            Bot_Event_DispenserDetonated(gentity_t * _player);
void            Bot_Event_DispenserDismantled(gentity_t * _player);
void            Bot_Event_SentryDetonated(gentity_t * _player);
void            Bot_Event_SentryDismantled(gentity_t * _player);

void            Bot_Event_CantDisguiseAsTeam(gentity_t * _player, int _disguiseTeam);
void            Bot_Event_CantDisguiseAsClass(gentity_t * _player, int _disguiseClass);
void            Bot_Event_Disguising(gentity_t * _player, int _disguiseTeam, int _disguiseClass);
void            Bot_Event_Disguised(gentity_t * _player, int _disguiseTeam, int _disguiseClass);
void            Bot_Event_DisguiseLost(gentity_t * _player);
void            Bot_Event_UnCloaked(gentity_t * _player);
void            Bot_Event_CantCloak(gentity_t * _player);
void            Bot_Event_Cloaked(gentity_t * _player);

void            Bot_Event_SendSentryStatsToBot(gentity_t * _sentry);
void            Bot_Event_SendSupplyStatsToBot(gentity_t * _supplystation);

void            Bot_Event_GotDispenserAmmo(gentity_t * _player);
void            Bot_Event_GaveSpannerArmor(gentity_t * _engy, gentity_t * _target, int _before, int _after);
void            Bot_Event_GotSpannerArmor(gentity_t * _target, gentity_t * _engy, int _before, int _after);
void            Bot_Event_GaveMedicHealth(gentity_t * _medic, gentity_t * _target, int _before, int _after);
void            Bot_Event_GotMedicHealth(gentity_t * _target, gentity_t * _medic, int _before, int _after);
void            Bot_Event_Infected(gentity_t * _target, gentity_t * _infector);
void            Bot_Event_Cured(gentity_t * _curee, gentity_t * _curer);
void            Bot_Event_MedicCall(gentity_t * _player);
void            Bot_Event_EngineerCall(gentity_t *_player);

void            Bot_Event_FireWeapon(gentity_t * _player, int _weaponId, gentity_t * _projectile);

void            Bot_Event_Spectated(gentity_t * _player, int _who);

void            Bot_Event_ChatMessage(gentity_t * _to, gentity_t * _source, int _type, const char *_message);

void            Bot_Event_Sound(gentity_t * _source, int _sndtype, const char *_name);

void            Bot_Event_GoalInfo(gentity_t * _entity, int _type, int _teamflags);

// goal helpers
void            GetEntityCenter(gentity_t * ent, vec3_t pos);

qboolean        InFieldOfVision(vec3_t viewangles, float fov, vec3_t angles);

#endif
