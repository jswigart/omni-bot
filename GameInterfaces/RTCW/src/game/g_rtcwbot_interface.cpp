/*
* RTCW <-> Omni-Bot interface source file.
*
*/

#include <sstream>
#include <iomanip>

extern "C"
{
#include "g_rtcwbot_interface.h"
};

#include "BotExports.h"
#include "RTCW_Config.h"
#include "RTCW_Messages.h"

#define OMNIBOT_MIN_ENG 1
#define OMNIBOT_MIN_MED 2
#define OMNIBOT_MIN_LT 2
#define OMNIBOT_MIN_SOL 1

void Bot_Event_EntityCreated( gentity_t *pEnt );

bool IsBot( gentity_t *e ) {
	return e->r.svFlags & SVF_BOT ? true : false;
}

//////////////////////////////////////////////////////////////////////////

const int MAX_CACHE = 32;
int g_NumSmokeGrenades = 0;
gentity_t       *g_SmokeGrenadeCache[MAX_CACHE] = {0};

struct BotEntity
{
	int16_t m_HandleSerial;
	bool m_NewEntity : 1;
	bool m_Used : 1;
};

BotEntity m_EntityHandles[MAX_GENTITIES];

//////////////////////////////////////////////////////////////////////////

extern "C" const char *_GetEntityName( gentity_t *_ent ) {
	static char newentname[ 256 ];
	char *name;

	newentname[ 0 ] = '\0';

	if ( _ent ) {
		if ( _ent->inuse && _ent->client ) {
			if ( _ent->client->pers.netname[0] ) {
				return _ent->client->pers.netname;
			} else
			{
				static char userinfo[MAX_INFO_STRING] = {0};
				trap_GetUserinfo( _ent - g_entities, userinfo, sizeof( userinfo ) );
				return Info_ValueForKey( userinfo, "name" );
			}
		}

		if ( _ent->track ) {
			strcpy( newentname, _ent->track );
		} else if ( _ent->scriptName ) {
			strcpy( newentname, _ent->scriptName );
		} else if ( _ent->targetname ) {
			strcpy( newentname, _ent->targetname );
		} else if ( _ent->message ) {
			strcpy( newentname, _ent->message );
		}

		name = newentname;

		if ( name ) {
			char undschar[] = { '-', 0 };
			char skipchar[] = { '[', ']', '#', '!', '*', '`',
				'^', '&', '<', '>', '+', '=', '|', '\'', '%',
				'.', ':', '/', 0 };
			char *curchar = NULL;
			char *tmp = NULL;
			char *tmpdst = NULL;
			tmp = name;
			tmpdst = name;

			while ( *tmp )
			{
				curchar = undschar;
				while ( *curchar )
				{
					if ( *tmp == *curchar ) {
						*tmp = '_';
						break;
					}
					curchar++;
				}
				curchar = skipchar;
				while ( *curchar )
				{
					if ( *tmp == *curchar ) {
						tmp++;
						break;
					}
					curchar++;
				}
				*tmpdst = *tmp;
				tmp++;
				tmpdst++;
			}

			*tmpdst = '\0';

			if ( !Q_stricmpn( "the ", name, 4 ) ) {
				return name + 4;
			}

			return name;
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

gentity_t *INDEXENT( const int _gameId ) {
	if ( _gameId >= 0 && _gameId < MAX_GENTITIES ) {
		switch ( _gameId )
		{
		case ENTITYNUM_WORLD: // world ent not marked inuse for some reason
			return &g_entities[ENTITYNUM_WORLD];
		default:
			return g_entities[_gameId].inuse ? &g_entities[_gameId] : 0;
		}
	}
	return 0;
}

int ENTINDEX( gentity_t *_ent ) {
	return _ent - g_entities;
}

gentity_t *EntityFromHandle( GameEntity _ent ) {
	int16_t index = _ent.GetIndex();
	if ( index >= 0 && index < MAX_GENTITIES ) {
		if ( m_EntityHandles[index].m_HandleSerial == _ent.GetSerial() && g_entities[index].inuse ) {
			return &g_entities[index];
		}
		if ( index == ENTITYNUM_WORLD ) {
			return &g_entities[ENTITYNUM_WORLD];
		}
	}
	return NULL;
}

GameEntity HandleFromEntity( gentity_t *_ent ) {
	if ( _ent ) {
		return GameEntity( _ent - g_entities, m_EntityHandles[_ent - g_entities].m_HandleSerial );
	} else {
		return GameEntity();
	}
}

//////////////////////////////////////////////////////////////////////////
enum { MaxDeferredGoals = 64 };
MapGoalDef g_DeferredGoals[MaxDeferredGoals];
int g_NumDeferredGoals = 0;
bool g_GoalSubmitReady = false;

void AddDeferredGoal( gentity_t *ent ) {
	if ( g_NumDeferredGoals >= MaxDeferredGoals - 1 ) {
		G_Error( "Deferred Goal Buffer Full!" );
		return;
	}

	MapGoalDef &goaldef = g_DeferredGoals[g_NumDeferredGoals++];
	MapGoalDef &goaldef2 = g_DeferredGoals[g_NumDeferredGoals++];

	switch ( ent->s.eType )
	{
	case ET_MG42_BARREL:
		{
			goaldef.Props.SetString( "Type","mountmg42" );
			goaldef.Props.SetEntity( "Entity",HandleFromEntity( ent ) );
			goaldef.Props.SetInt( "Team",( 1 << RTCW_TEAM_ALLIES ) | ( 1 << RTCW_TEAM_AXIS ) );
			goaldef.Props.SetString( "TagName",_GetEntityName( ent ) );
			goaldef.Props.SetInt( "InterfaceGoal",1 );

			// cs: this was done in rtcw_goalmanager before
			goaldef2.Props.SetString("Type","repairmg42");
			goaldef2.Props.SetEntity("Entity",HandleFromEntity(ent));
			goaldef2.Props.SetInt("Team",(1 << RTCW_TEAM_ALLIES)|(1 << RTCW_TEAM_AXIS));
			goaldef2.Props.SetString("TagName",_GetEntityName(ent));
			goaldef2.Props.SetInt("InterfaceGoal",1);
			break;
		}
	}
}

void SendDeferredGoals() {
	if ( g_GoalSubmitReady ) {
		for ( int i = 0; i < g_NumDeferredGoals; ++i )
		{
			gBotFunctions.pfnAddGoal( g_DeferredGoals[i] );
		}
		g_NumDeferredGoals = 0;
	}
}

//////////////////////////////////////////////////////////////////////////

static qboolean weaponCharged( playerState_t* ps, team_t team, int weapon ) {
	switch ( weapon )
	{
	case WP_PANZERFAUST:
		if ( level.time - ps->classWeaponTime < g_soldierChargeTime.integer * 0.80f ) {
			return qfalse;
		}
		break;
	case WP_DYNAMITE:
		if ( level.time - ps->classWeaponTime < g_engineerChargeTime.integer * 0.80f ) {
			return qfalse;
		}
		break;
	case WP_MEDKIT:
		if ( level.time - ps->classWeaponTime < g_medicChargeTime.integer * 0.25f ) {
			return qfalse;
		}
		break;
	case WP_AMMO:
		if ( level.time - ps->classWeaponTime < g_LTChargeTime.integer * 0.25f ) {
			return qfalse;
		}
		break;
	case WP_SMOKE_GRENADE:
		if ( level.time - ps->classWeaponTime < g_LTChargeTime.integer * 0.5f ) {
			return qfalse;
		}
		break;
	case WP_BINOCULARS:
		switch ( ps->stats[ STAT_PLAYER_CLASS ] )
		{
		case PC_LT:
			if ( level.time - ps->classWeaponTime < g_LTChargeTime.integer ) {
				return qfalse;
			}
			break;
		default:
			return qfalse;
		}
	}
	return qtrue;
}

static int CountPlayerClass( team_t team, int playerClass ) {
	int num = 0;
	int i, j;

	for ( i = 0; i < level.numConnectedClients; i++ )
	{
		j = level.sortedClients[i];

		if ( level.clients[j].sess.sessionTeam != team ) {
			continue;
		}

		if ( level.clients[j].sess.latchPlayerType != playerClass ) {
			continue;
		}
		num++;
	}
	return num;
}

int Bot_TeamGameToBot( int team ) {
	switch ( team )
	{
	case TEAM_RED:
		return RTCW_TEAM_AXIS;
	case TEAM_BLUE:
		return RTCW_TEAM_ALLIES;
	case TEAM_SPECTATOR:
		return OB_TEAM_SPECTATOR;
	default:
		return RTCW_TEAM_NONE;
	}
}

static int playerClassBotToGame( int playerClass ) {
	switch ( playerClass )
	{
	case RTCW_CLASS_SOLDIER:
		return PC_SOLDIER;
	case RTCW_CLASS_MEDIC:
		return PC_MEDIC;
	case RTCW_CLASS_ENGINEER:
		return PC_ENGINEER;
	case RTCW_CLASS_LIEUTENANT:
		return PC_LT;
	default:
		return -1;
	}
}

int Bot_PlayerClassGameToBot( int playerClass ) {
	switch ( playerClass )
	{
	case PC_SOLDIER:
		return RTCW_CLASS_SOLDIER;
	case PC_MEDIC:
		return RTCW_CLASS_MEDIC;
	case PC_ENGINEER:
		return RTCW_CLASS_ENGINEER;
	case PC_LT:
		return RTCW_CLASS_LIEUTENANT;
	default:
		return RTCW_CLASS_NULL;
	}
}

static int _weaponBotToGame( int weapon ) {
	switch ( weapon )
	{
	case RTCW_WP_KNIFE:
		return WP_KNIFE;
	case RTCW_WP_LUGER:
		return WP_LUGER;
	case RTCW_WP_MP40:
		return WP_MP40;
	case RTCW_WP_MAUSER:
		return WP_MAUSER;
	case RTCW_WP_GREN_AXIS:
		return WP_GRENADE_LAUNCHER;
	case RTCW_WP_PANZERFAUST:
		return WP_PANZERFAUST;
	case RTCW_WP_VENOM:
		return WP_VENOM;
	case RTCW_WP_FLAMETHROWER:
		return WP_FLAMETHROWER;
	case RTCW_WP_COLT:
		return WP_COLT;
	case RTCW_WP_THOMPSON:
		return WP_THOMPSON;
	case RTCW_WP_GARAND:
		return WP_GARAND;
	case RTCW_WP_GREN_ALLIES:
		return WP_GRENADE_PINEAPPLE;
	case RTCW_WP_ROCKET_LAUNCHER:
		return WP_ROCKET_LAUNCHER;
	case RTCW_WP_SNIPERRIFLE:
		return WP_SNIPERRIFLE;
	case RTCW_WP_STEN:
		return WP_STEN;
	case RTCW_WP_SYRINGE:
		return WP_MEDIC_SYRINGE;
	case RTCW_WP_AMMO_PACK:
		return WP_AMMO;
	case RTCW_WP_ARTY:
		return WP_ARTY;
	case RTCW_WP_DYNAMITE:
		return WP_DYNAMITE;
	case RTCW_WP_SNIPER:
		return WP_SNIPER;
	case RTCW_WP_MEDKIT:
		return WP_MEDKIT;
	case RTCW_WP_PLIERS:
		return WP_PLIERS;
	case RTCW_WP_SMOKE_GRENADE:
		return WP_SMOKE_GRENADE;
	case RTCW_WP_BINOCULARS:
		return WP_BINOCULARS;
	default:
		return WP_NONE;
	}
}

//bleh
static int _weaponBotToMpWeapon( int weapon ) {
	switch ( weapon )
	{
	case RTCW_WP_MP40:
		return 3;
	case RTCW_WP_THOMPSON:
		return 4;
	case RTCW_WP_STEN:
		return 5;
	case RTCW_WP_MAUSER:
	case RTCW_WP_GARAND:
	case RTCW_WP_SNIPERRIFLE:
	case RTCW_WP_SNIPER:
		return 6;
	case RTCW_WP_PANZERFAUST:
		return 8;
	case RTCW_WP_VENOM:
		return 9;
	case RTCW_WP_FLAMETHROWER:
		return 10;
	default:
		return RTCW_WP_NONE;
	}
}

int Bot_WeaponGameToBot( int weapon ) {
	switch ( weapon )
	{
	case WP_KNIFE:
		return RTCW_WP_KNIFE;
	case WP_LUGER:
		return RTCW_WP_LUGER;
	case WP_MP40:
		return RTCW_WP_MP40;
	case WP_MAUSER:
		return RTCW_WP_MAUSER;
	case WP_GRENADE_LAUNCHER:
		return RTCW_WP_GREN_AXIS;
	case WP_PANZERFAUST:
		return RTCW_WP_PANZERFAUST;
	case WP_VENOM:
		return RTCW_WP_VENOM;
	case WP_FLAMETHROWER:
		return RTCW_WP_FLAMETHROWER;
	case WP_COLT:
		return RTCW_WP_COLT;
	case WP_THOMPSON:
		return RTCW_WP_THOMPSON;
	case WP_GARAND:
		return RTCW_WP_GARAND;
	case WP_GRENADE_PINEAPPLE:
		return RTCW_WP_GREN_ALLIES;
	case WP_ROCKET_LAUNCHER:
		return RTCW_WP_ROCKET_LAUNCHER;
	case WP_SNIPERRIFLE:
		return RTCW_WP_SNIPERRIFLE;
	case WP_STEN:
		return RTCW_WP_STEN;
	case WP_MEDIC_SYRINGE:
		return RTCW_WP_SYRINGE;
	case WP_AMMO:
		return RTCW_WP_AMMO_PACK;
	case WP_ARTY:
		return RTCW_WP_ARTY;
	case WP_DYNAMITE:
		return RTCW_WP_DYNAMITE;
	case WP_SNIPER:
		return RTCW_WP_SNIPER;
	case WP_MEDKIT:
		return RTCW_WP_MEDKIT;
	case WP_PLIERS:
		return RTCW_WP_PLIERS;
	case WP_SMOKE_GRENADE:
		return RTCW_WP_SMOKE_GRENADE;
	case WP_BINOCULARS:
		return RTCW_WP_BINOCULARS;
	default:
		return RTCW_WP_NONE;
	}
}

static int Bot_HintGameToBot( gentity_t *_ent ) {
	int iHintType = CURSOR_HINT_NONE;
	if ( _ent && _ent->client ) {
		switch ( _ent->client->ps.serverCursorHint )
		{
		case HINT_PLAYER:
			iHintType = CURSOR_HINT_PLAYER;
			break;
		case HINT_ACTIVATE:
			iHintType = CURSOR_HINT_ACTIVATE;
			break;
		case HINT_DOOR:
			iHintType = CURSOR_HINT_DOOR;
			break;
		case HINT_DOOR_ROTATING:
			iHintType = CURSOR_HINT_DOOR_ROTATING;
			break;
		case HINT_DOOR_LOCKED:
			iHintType = CURSOR_HINT_DOOR_LOCKED;
			break;
		case HINT_DOOR_ROTATING_LOCKED:
			iHintType = CURSOR_HINT_DOOR_LOCKED;
			break;
		case HINT_MG42:
			iHintType = CURSOR_HINT_MG42;
			break;
		case HINT_BREAKABLE:
			iHintType = CURSOR_HINT_BREAKABLE;
			break;
		case HINT_BREAKABLE_DYNAMITE:
			iHintType = CURSOR_HINT_BREAKABLE_DYNAMITE;
			break;
		case HINT_CHAIR:
			iHintType = CURSOR_HINT_CHAIR;
			break;
		case HINT_ALARM:
			iHintType = CURSOR_HINT_ALARM;
			break;
		case HINT_HEALTH:
			iHintType = CURSOR_HINT_HEALTH;
			break;
		case HINT_TREASURE:
			iHintType = CURSOR_HINT_TREASURE;
			break;
		case HINT_KNIFE:
			iHintType = CURSOR_HINT_KNIFE;
			break;
		case HINT_LADDER:
			iHintType = CURSOR_HINT_LADDER;
			break;
		case HINT_BUTTON:
			iHintType = CURSOR_HINT_BUTTON;
			break;
		case HINT_WATER:
			iHintType = CURSOR_HINT_WATER;
			break;
		case HINT_WEAPON:
			iHintType = CURSOR_HINT_WEAPON;
			break;
		case HINT_AMMO:
			iHintType = CURSOR_HINT_AMMO;
			break;
		case HINT_POWERUP:
			iHintType = CURSOR_HINT_POWERUP;
			break;
		case HINT_HOLDABLE:
			iHintType = CURSOR_HINT_HOLDABLE;
			break;
		case HINT_INVENTORY:
			iHintType = CURSOR_HINT_INVENTORY;
			break;
		case HINT_SCENARIC:
			iHintType = CURSOR_HINT_SCENARIC;
			break;
		case HINT_EXIT:
			iHintType = CURSOR_HINT_EXIT;
			break;
		case HINT_PLYR_FRIEND:
			iHintType = CURSOR_HINT_PLYR_FRIEND;
			break;
		case HINT_PLYR_NEUTRAL:
			iHintType = CURSOR_HINT_PLYR_NEUTRAL;
			break;
		case HINT_PLYR_ENEMY:
			iHintType = CURSOR_HINT_PLYR_ENEMY;
			break;
		case HINT_PLYR_UNKNOWN:
			iHintType = CURSOR_HINT_PLYR_UNKNOWN;
			break;
		case HINT_BUILD:
			iHintType = CURSOR_HINT_BUILD;
			break;
		case HINT_DISARM:
			iHintType = CURSOR_HINT_DISARM;
			break;
		case HINT_REVIVE:
			iHintType = CURSOR_HINT_REVIVE;
			break;
		case HINT_DYNAMITE:
			iHintType = CURSOR_HINT_DYNAMITE;
			break;
		}
	}
	return iHintType;
}

qboolean Simple_EmplacedGunIsRepairable( gentity_t* ent ) {
	if ( Q_stricmp( ent->classname, "misc_mg42" ) ) {
		return qfalse;
	}
	if ( ent->s.frame == 0 ) {
		return qfalse;
	}
	return qtrue;
}

static int _choosePriWeap( int playerClass, int team ) {
	switch ( playerClass )
	{
	case RTCW_CLASS_SOLDIER:
		{
			int wpns[] =
			{
				// add shit as needed
				RTCW_WP_PANZERFAUST,
				RTCW_WP_FLAMETHROWER,
				RTCW_WP_MAUSER,
				RTCW_WP_VENOM,
			};
			int rInt = rand() % ( sizeof( wpns ) / sizeof( wpns[0] ) );
			return wpns[rInt];
		}
	case RTCW_CLASS_MEDIC:
		{
			if ( team == RTCW_TEAM_ALLIES ) {
				int wpns[] =
				{
					// add shit as needed
					RTCW_WP_THOMPSON,
				};
				int rInt = rand() % ( sizeof( wpns ) / sizeof( wpns[0] ) );
				return wpns[rInt];
			} else
			{
				int wpns[] =
				{
					// add shit as needed
					RTCW_WP_MP40,
				};
				int rInt = rand() % ( sizeof( wpns ) / sizeof( wpns[0] ) );
				return wpns[rInt];
			}
		}
	case RTCW_CLASS_ENGINEER:
		{
			if ( team == RTCW_TEAM_ALLIES ) {
				int wpns[] =
				{
					// add shit as needed
					RTCW_WP_THOMPSON,
				};
				int rInt = rand() % ( sizeof( wpns ) / sizeof( wpns[0] ) );
				return wpns[rInt];
			} else
			{
				int wpns[] =
				{
					// add shit as needed
					RTCW_WP_MP40,
				};
				int rInt = rand() % ( sizeof( wpns ) / sizeof( wpns[0] ) );
				return wpns[rInt];
			}
		}
	case RTCW_CLASS_LIEUTENANT:
		{
			if ( team == RTCW_TEAM_ALLIES ) {
				int wpns[] =
				{
					// add shit as needed
					RTCW_WP_THOMPSON,
					//RTCW_WP_STEN,
					RTCW_WP_MP40,
				};
				int rInt = rand() % ( sizeof( wpns ) / sizeof( wpns[0] ) );
				return wpns[rInt];
			} else
			{
				int wpns[] =
				{
					// add shit as needed
					RTCW_WP_MP40,
					//RTCW_WP_STEN,
					RTCW_WP_THOMPSON,
				};
				int rInt = rand() % ( sizeof( wpns ) / sizeof( wpns[0] ) );
				return wpns[rInt];
			}
		}
	default:
		return RTCW_WP_NONE;
	}
}

#define MAX_SMOKE_RADIUS 320.0
#define MAX_SMOKE_RADIUS_TIME 10000.0
#define UNAFFECTED_BY_SMOKE_DIST SQR( 100 )

gentity_t *Bot_EntInvisibleBySmokeBomb( vec3_t start, vec3_t end ) {
	gentity_t *ent = NULL;
	vec3_t smokeCenter;
	float smokeRadius;

	// if the target is close enough, vision is not affected by smoke bomb
	if ( DistanceSquared( start, end ) < UNAFFECTED_BY_SMOKE_DIST ) {
		//pfnPrintMessage("within unaffected dist");
		return 0;
	}

	//while (ent = G_FindSmokeBomb( ent ))
	for ( int i = 0; i < g_NumSmokeGrenades; ++i )
	{
		ent = g_SmokeGrenadeCache[i];

		if ( ent->s.effect1Time == 16 ) {
			//pfnPrintMessage("smoke not up yet");
			// xkan, the smoke has not really started yet, see weapon_smokeBombExplode
			// and CG_RenderSmokeGrenadeSmoke
			continue;
		}
		// check the distance
		VectorCopy( ent->s.pos.trBase, smokeCenter );
		// raise the center to better match the position of the smoke, see
		// CG_SpawnSmokeSprite().
		smokeCenter[2] += 32;
		// smoke sprite has a maximum radius of 640/2. and it takes a while for it to
		// reach that size, so adjust the radius accordingly.
		smokeRadius = MAX_SMOKE_RADIUS * ( ( level.time - ent->grenadeExplodeTime ) / MAX_SMOKE_RADIUS_TIME );
		if ( smokeRadius > MAX_SMOKE_RADIUS ) {
			smokeRadius = MAX_SMOKE_RADIUS;
		}
		// if distance from line is short enough, vision is blocked by smoke

		//float fColorRed[3] = { 1.0f, 0.0f, 0.0f };
		//float fColorGrn[3] = { 0.0f, 1.0f, 0.0f };

		if ( DistanceFromLineSquared( smokeCenter, start, end ) < ( smokeRadius * smokeRadius ) ) {
			//pfnPrintMessage("hid by smoke");
			//pfnAddTempDisplayLine(smokeCenter, end, fColorGrn);
			return ent;
		}
		//pfnAddTempDisplayLine(smokeCenter, start, fColorRed);
	}

	return 0;
}

void Bot_Util_AddGoal( const char *_type, gentity_t *_ent, int _team, const char *_tag, const char *_extrakey = 0, obUserData *_extraval = 0 ) {
	if ( IsOmnibotLoaded() ) {
		MapGoalDef goaldef;
		goaldef.Props.SetString( "Type",_type );
		goaldef.Props.SetEntity( "Entity",HandleFromEntity( _ent ) );
		goaldef.Props.SetInt( "Team",_team );
		goaldef.Props.SetVector( "Position", _ent->r.currentOrigin );
		goaldef.Props.SetString( "TagName",_tag );
		goaldef.Props.SetInt( "InterfaceGoal",1 );
		if ( _extrakey && _extraval ) {
			goaldef.Props.Set( _extrakey,*_extraval );
		}
		gBotFunctions.pfnAddGoal( goaldef );
	}
}

static int _GetEntityTeam( gentity_t *_ent ) {
	// hack, when the game joins clients again after warmup, they are temporarily ET_GENERAL entities(LAME)
	int t = _ent->s.eType;
	if ( _ent->client && ( _ent - g_entities ) < MAX_CLIENTS ) {
		t = ET_PLAYER;
	}

	switch ( t )
	{
	case ET_PLAYER:
		if ( _ent->client ) {
			return Bot_TeamGameToBot( _ent->client->sess.sessionTeam );
		}
	case ET_CORPSE:
		return Bot_TeamGameToBot( BODY_TEAM( _ent ) );
	case ET_MISSILE:
		if ( _ent->s.weapon == WP_DYNAMITE ) {
			return Bot_TeamGameToBot( _ent->s.teamNum % 4 );
		}
		// Let this fall through
	default:
		return Bot_TeamGameToBot( _ent->s.teamNum );
	}

	return 0;
}

static int _GetEntityClass( gentity_t *_ent ) {
	// hack, when the game joins clients again after warmup, they are temporarily ET_GENERAL entities(LAME)
	int t = _ent->s.eType;
	if ( _ent->client && ( _ent - g_entities ) < MAX_CLIENTS ) {
		t = ET_PLAYER;
	}

	switch ( t )
	{
	case ET_GENERAL:
		{
			if ( !Q_stricmp( _ent->classname, "func_invisible_user" ) ) {
				return ENT_CLASS_GENERIC_BUTTON;
			} else if ( !Q_stricmp( _ent->classname, "func_button" ) ) {
				return ENT_CLASS_GENERIC_BUTTON;
			} else if ( !Q_stricmp( _ent->classname, "props_chair_hiback" ) ) {
				return RTCW_CLASSEX_BROKENCHAIR;
			} else if ( !Q_stricmp( _ent->classname, "props_chair" ) ) {
				return RTCW_CLASSEX_BROKENCHAIR;
			} else if ( !Q_stricmp( _ent->classname, "props_chair_side" ) ) {
				return RTCW_CLASSEX_BROKENCHAIR;
			}
			else if(!Q_stricmp(_ent->classname, "info_player_deathmatch") ||
				!Q_stricmp(_ent->classname, "team_CTF_redspawn") ||
				!Q_stricmp(_ent->classname, "team_CTF_bluespawn") ||
				!Q_stricmp(_ent->classname, "info_player_spawn"))
			{
				return ENT_CLASS_GENERIC_PLAYERSTART;
			}
			break;
		}
	case ET_INVISIBLE:
		{
			if ( _ent->client ) {
				return ENT_CLASS_GENERIC_SPECTATOR;
			}
			break;
		}
	case ET_PLAYER:
		{
			if ( !_ent->client || ( _ent->entstate == STATE_INVISIBLE ) ||
				( _ent->client->sess.sessionTeam != TEAM_RED &&
				_ent->client->sess.sessionTeam != TEAM_BLUE ) ) {
					return ENT_CLASS_GENERIC_SPECTATOR;
			}

			return Bot_PlayerClassGameToBot( _ent->client->sess.latchPlayerType );
		}
	case ET_ITEM:
		{
			if ( !Q_strncmp( _ent->classname, "item_health", strlen( "item_health" ) ) ) {
				return ENT_CLASS_GENERIC_HEALTH;
			} else if ( !Q_strncmp( _ent->classname, "weapon_magicammo", strlen( "weapon_magicammo" ) ) ) {
				return ENT_CLASS_GENERIC_AMMO;
			} else if ( !Q_stricmp( _ent->classname, "item_treasure" ) ) {
				return RTCW_CLASSEX_TREASURE;
			} else if ( _ent->item && _ent->item->giType == IT_WEAPON ) {
				return RTCW_CLASSEX_WEAPON + Bot_WeaponGameToBot( _ent->item->giTag );
			}
			break;
		}
	case ET_CORPSE:
		{
			return RTCW_CLASSEX_CORPSE;
		}
	case ET_MISSILE:
		{
			// Register certain weapons as threats to avoid or whatever.
			switch ( _ent->s.weapon )
			{
			case WP_GRENADE_LAUNCHER:
			case WP_GRENADE_PINEAPPLE:
				return RTCW_CLASSEX_GRENADE;
			case WP_PANZERFAUST:
				return RTCW_CLASSEX_ROCKET;
			case WP_SMOKE_GRENADE:
				// smoke grenade without server flags is an a/s cannister ...
				if ( !(_ent->r.svFlags & SVF_SMOKEGRENADE) ) {
					return RTCW_CLASSEX_AIRSTRIKE;
				}
				break;
			case WP_SMOKETRAIL:
			case WP_ARTY:
				// cs: the airstrike bombs are also wp_arty. added aiName so a new client wouldn't
				//     need to be distributed.
				// TODO: fix this properly if a new client will be included in a release.
				if ( _ent->aiName && !Q_strncmp( _ent->aiName, "artillery", sizeof( "artillery" ) ) ) {
					return RTCW_CLASSEX_ARTY;
				}
				break;
			case WP_DYNAMITE:
				return RTCW_CLASSEX_DYNAMITE;
			default:
				if ( _ent->aiName && !Q_strncmp( _ent->aiName, "air strike", sizeof( "air strike" ) ) ) {
					return RTCW_CLASSEX_AIRSTRIKE;
				}
			}
			break;
		}
	case ET_FLAMETHROWER_CHUNK:
		{
			return RTCW_CLASSEX_FLAMECHUNK;
			break;
		}
	case ET_MOVER:
		{
			if ( !Q_stricmp( _ent->classname, "script_mover" ) ) {
				return RTCW_CLASSEX_VEHICLE;
			}
			/*			else if (!Q_stricmp(_ent->classname, "props_flamebarrel"))
			{
			if ((_ent->health > 0) && (_ent->takedamage == qtrue))
			{
			return RTCW_CLASSEX_BREAKABLE;
			}
			}*/
			else if ( !Q_stricmp( _ent->classname, "props_chair_hiback" ) ) {
				if ( ( _ent->health > 0 ) && ( _ent->takedamage == qtrue ) ) {
					return RTCW_CLASSEX_BREAKABLE;
				}
			} else if ( !Q_stricmp( _ent->classname, "props_chair" ) )      {
				if ( ( _ent->health > 0 ) && ( _ent->takedamage == qtrue ) ) {
					return RTCW_CLASSEX_BREAKABLE;
				}
			} else if ( !Q_stricmp( _ent->classname, "props_chair_side" ) )      {
				if ( ( _ent->health > 0 ) && ( _ent->takedamage == qtrue ) ) {
					return RTCW_CLASSEX_BREAKABLE;
				}
			}

			break;
		}
		/*	case ET_PROP:
		{
		if ((_ent->health > 0) && (_ent->takedamage == qtrue))
		{
		return RTCW_CLASSEX_BREAKABLE;
		}
		break;
		}*/
	case ET_MG42_BARREL:
		{
			if ( ( _ent->health > 0 ) &&
				( _ent->entstate != STATE_INVISIBLE ) ) {
					return RTCW_CLASSEX_MG42MOUNT;
			}
			break;
		}
	case ET_EXPLOSIVE:
		{
			if ( ( _ent->health > 0 ) && ( _ent->takedamage == qtrue ) ) {
				return RTCW_CLASSEX_BREAKABLE;
			}
			break;
		}
	case ET_OID_TRIGGER:
		{
			return ENT_CLASS_GENERIC_GOAL;
		}
		break;
	default:
		break;
	};
	return 0;
}

qboolean _EmplacedGunIsMountable( gentity_t *_ent ) {
	if ( g_OmniBotFlags.integer & OBF_DONT_MOUNT_GUNS ) {
		return qfalse;
	}
	return qtrue;
}

void Omnibot_Load_PrintMsg( const char *_msg ) {
	G_Printf( "Omni-bot: %s%s\n", S_COLOR_GREEN, _msg );
}

void Omnibot_Load_PrintErr( const char *_msg ) {
	G_Printf( "Omni-bot: %s%s\n", S_COLOR_RED, _msg );
}

inline void ConvertBit( int & srcValue, int & dstValue, int matchBit, int toBit )
{
	if ( srcValue & matchBit )
	{
		dstValue |= toBit;
		srcValue &= ~matchBit; // so we can debug bits we dont handle
	}
}

int g_LastScreenMessageTime = 0;

int obUtilBotContentsFromGameContents( int _contents ) {
	int iBotContents = 0;
	if ( _contents & CONTENTS_SOLID ) {
		iBotContents |= CONT_SOLID;
	}
	if ( _contents & CONTENTS_WATER ) {
		iBotContents |= CONT_WATER;
	}
	if ( _contents & CONTENTS_SLIME ) {
		iBotContents |= CONT_SLIME;
	}
	if ( _contents & CONTENTS_FOG ) {
		iBotContents |= CONT_FOG;
	}
	if ( _contents & CONTENTS_TELEPORTER ) {
		iBotContents |= CONT_TELEPORTER;
	}
	if ( _contents & CONTENTS_MOVER ) {
		iBotContents |= CONT_MOVER;
	}
	if ( _contents & CONTENTS_TRIGGER ) {
		iBotContents |= CONT_TRIGGER;
	}
	if ( _contents & CONTENTS_LAVA ) {
		iBotContents |= CONT_LAVA;
	}
	if ( _contents & CONTENTS_PLAYERCLIP ) {
		iBotContents |= CONT_PLYRCLIP;
	}
	return iBotContents;
}

int obUtilBotSurfaceFromGameSurface( int _surfaceflags ) {
	int iBotSurface = 0;
	if ( _surfaceflags & SURF_SLICK ) {
		iBotSurface |= SURFACE_SLICK;
	}
	if ( _surfaceflags & SURF_LADDER ) {
		iBotSurface |= SURFACE_LADDER;
	}
	return iBotSurface;
}

void Bot_Util_CheckForGoalEntity( GameEntity _ent ) {
	if ( IsOmnibotLoaded() ) {
		gentity_t *pEnt = EntityFromHandle( _ent );

		const char *pGoalName = _GetEntityName( pEnt );
		if ( pEnt->inuse ) {
			switch ( pEnt->s.eType )
			{
			case ET_ITEM:
				{
					char buffer[256] = {0};
					if ( !Q_stricmp( pEnt->classname, "team_CTF_redflag" ) ) {
						// allies flag
						if ( pEnt->s.otherEntityNum != -1 ) {
							pGoalName = _GetEntityName( &g_entities[pEnt->s.otherEntityNum] );
						}
						sprintf( buffer, "%s_dropped", pGoalName ? pGoalName : "allies_flag" );
						Bot_Util_AddGoal( "flag",pEnt,( 1 << RTCW_TEAM_ALLIES ),buffer );
						Bot_Util_AddGoal( "flagreturn",pEnt,( 1 << RTCW_TEAM_AXIS ),buffer );
					} else if ( !Q_stricmp( pEnt->classname, "team_CTF_blueflag" ) )        {
						// axis flag
						if ( pEnt->s.otherEntityNum != -1 ) {
							pGoalName = _GetEntityName( &g_entities[pEnt->s.otherEntityNum] );
						}
						sprintf( buffer, "%s_dropped", pGoalName ? pGoalName : "axis_flag" );

						Bot_Util_AddGoal( "flag",pEnt,( 1 << RTCW_TEAM_AXIS ),buffer );
						Bot_Util_AddGoal( "flagreturn",pEnt,( 1 << RTCW_TEAM_ALLIES ),buffer );
					}
					break;
				}
				/*case ET_MG42_BARREL: //CS: Using deferred goals now.
				{
				int team = 0;
				team |= (1 << RTCW_TEAM_ALLIES);
				team |= (1 << RTCW_TEAM_AXIS);
				pGoalName = _GetEntityName(pEnt);
				Bot_Util_AddGoal(pEnt, RTCW_GOAL_MG42MOUNT, team, pGoalName, NULL);
				break;
				}*/
			default:
				break;
			}
		}
	}
}

// helper stuff
qboolean InFieldOfVision( vec3_t viewangles, float fov, vec3_t angles ) {
	int i;
	float diff, angle;

	for ( i = 0; i < 2; i++ ) {
		angle = AngleMod( viewangles[i] );
		angles[i] = AngleMod( angles[i] );
		diff = angles[i] - angle;
		if ( angles[i] > angle ) {
			if ( diff > 180.0 ) {
				diff -= 360.0;
			}
		} else {
			if ( diff < -180.0 ) {
				diff += 360.0;
			}
		}
		if ( diff > 0 ) {
			if ( diff > fov * 0.5 ) {
				return qfalse;
			}
		} else {
			if ( diff < -fov * 0.5 ) {
				return qfalse;
			}
		}
	}
	return qtrue;
}

class RTCWInterface : public IEngineInterface
{
public:
	int AddBot( const MessageHelper &_data ) {
		if ( level.framenum < GAME_INIT_FRAMES ) {
			return -1;
		}

		OB_GETMSG( Msg_Addbot );

		int num;
		char userinfo[MAX_INFO_STRING] = {0};
		gentity_t* bot;
		num = trap_BotAllocateClient();

		if ( num < 0 ) {
			PrintError( "Could not add bot!" );
			PrintError( "No free slots!" );
			return -1;
		}

		std::stringstream guid;
		guid << "OMNIBOT" << std::setw( 2 ) << std::setfill( '0' ) << num << std::right << std::setw( 23 ) << "";

		bot = &g_entities[num];

		Info_SetValueForKey( userinfo, "name", pMsg->m_Name );
		Info_SetValueForKey( userinfo, "rate", "25000" );
		Info_SetValueForKey( userinfo, "snaps", "20" );
		Info_SetValueForKey( userinfo, "ip", "localhost" );
		Info_SetValueForKey( userinfo, "cl_guid", guid.str().c_str() );

		trap_SetUserinfo( num, userinfo );

		const char *s = 0;
		if ( ( s = ClientConnect( num, qtrue, qtrue ) ) != 0 ) {
			PrintError( va( "Could not connect bot: %s", s ) );
			num = -1;
		}
		return bot && bot->inuse ? num : -1;
	}

	void RemoveBot( const MessageHelper &_data ) {
		OB_GETMSG( Msg_Kickbot );
		if ( pMsg->m_GameId != Msg_Kickbot::InvalidGameId ) {
			if ( pMsg->m_GameId >= 0 && pMsg->m_GameId < MAX_CLIENTS ) {
				gentity_t *ent = &g_entities[pMsg->m_GameId];
				if ( IsBot( ent ) ) {
					trap_DropClient( pMsg->m_GameId, "disconnected" );
				}
			}
		} else
		{
			char cleanNetName[MAX_NETNAME];
			char cleanName[MAX_NAME_LENGTH];

			Q_strncpyz( cleanName, pMsg->m_Name, MAX_NAME_LENGTH );
			Q_CleanStr( cleanName );

			for ( int i = 0; i < MAX_CLIENTS; i++ )
			{
				if ( !g_entities[i].inuse ) {
					continue;
				}
				if ( !IsBot( &g_entities[i] ) ) {
					continue;
				}

				// clean stuff
				Q_strncpyz( cleanNetName, g_entities[i].client->pers.netname, MAX_NETNAME );
				Q_CleanStr( cleanNetName );

				if ( !Q_stricmp( cleanNetName, cleanName ) ) {
					trap_DropClient( i, "disconnected" );
				}
			}
		}
	}

	obResult ChangeTeam( int _client, int _newteam, const MessageHelper *_data ) {
		gentity_t* bot;
		char* teamName;

		bot = &g_entities[_client];

		if ( _newteam && g_botTeam.integer > 0 && g_botTeam.integer < 3 ) {
			_newteam = g_botTeam.integer;
		}

		// find a team if we didn't get one and we need one ;-)
		if ( _newteam != RTCW_TEAM_ALLIES && _newteam != RTCW_TEAM_AXIS ) {
			if ( ( _newteam == RANDOM_TEAM ) ||
				( bot->client->sess.sessionTeam != TEAM_RED &&
				bot->client->sess.sessionTeam != TEAM_BLUE ) ) {
					if ( TeamCount( _client, TEAM_BLUE ) <= TeamCount( _client, TEAM_RED ) ) {
						_newteam = RTCW_TEAM_ALLIES;
					} else
					{
						_newteam = RTCW_TEAM_AXIS;
					}
			} else
			{
				// remember old team
				_newteam = Bot_TeamGameToBot( bot->client->sess.sessionTeam );
			}
		}

		if ( _newteam == RTCW_TEAM_AXIS ) {
			teamName = "red";
		} else {
			teamName = "blue";
		}

		// always go to spectator first to solve problems on map restarts
		//SetTeam(bot, "spectator", qfalse);

		Msg_PlayerChooseEquipment *pMsg = 0;
		if ( _data ) {
			_data->Get2( pMsg );
		}
		if ( pMsg ) {
			if ( pMsg->m_WeaponChoice[0] ) {
				//bot->client->sess.latchPlayerWeapon = _weaponBotToGame(pMsg->m_WeaponChoice[0]);
				bot->client->sess.latchPlayerWeapon = _weaponBotToMpWeapon( pMsg->m_WeaponChoice[0] );
			}
		}

		{
			const int iBotTeam = Bot_TeamGameToBot( bot->client->sess.sessionTeam );
			const int iBotClass = Bot_PlayerClassGameToBot( bot->client->sess.latchPlayerType );

			//G_IsWeaponDisabled ?
			if ( !bot->client->sess.latchPlayerWeapon || iBotTeam != _newteam ) {
				//bot->client->sess.latchPlayerWeapon = _weaponBotToGame(_choosePriWeap(iBotClass, _newteam));
				bot->client->sess.latchPlayerWeapon = _weaponBotToMpWeapon( _choosePriWeap( iBotClass, _newteam ) );
			}
		}

		SetTeam( bot, teamName, qfalse );
		return Success;
	}

	obResult ChangeClass( int _client, int _newclass, const MessageHelper *_data ) {
		gentity_t* bot;
		int team;
		team_t predictedTeam = TEAM_FREE;

		bot = &g_entities[_client];

		// find playerclass if we didn't got one
		if ( _newclass <= RTCW_CLASS_NULL || _newclass >= RTCW_CLASS_MAX ) {
			if ( ( _newclass == RANDOM_CLASS ) || ( bot->client->sess.latchPlayerType < 0 ) ||
				( bot->client->sess.latchPlayerType >= NUM_PLAYER_CLASSES ) ) {
					//predictedTeam = bot->client->sess.sessionTeam;
					//if(predictedTeam!=TEAM_BLUE && predictedTeam!=TEAM_RED)
					//{
					if ( g_botTeam.integer == 1 ) {
						predictedTeam = TEAM_RED;
					} else if ( g_botTeam.integer == 2 ) {
						predictedTeam = TEAM_BLUE;
					} else if ( TeamCount( _client, TEAM_BLUE ) <= TeamCount( _client, TEAM_RED ) ) {
						predictedTeam = TEAM_BLUE;
					} else {
						predictedTeam = TEAM_RED;
					}
					//}

					if ( CountPlayerClass( predictedTeam, PC_ENGINEER ) < OMNIBOT_MIN_ENG ) {
						_newclass = RTCW_CLASS_ENGINEER;
					} else if ( CountPlayerClass( predictedTeam, PC_MEDIC ) < OMNIBOT_MIN_MED ) {
						_newclass = RTCW_CLASS_MEDIC;
					} else if ( CountPlayerClass( predictedTeam, PC_LT ) < OMNIBOT_MIN_LT ) {
						_newclass = RTCW_CLASS_LIEUTENANT;
					} else if ( CountPlayerClass( predictedTeam, PC_SOLDIER ) < OMNIBOT_MIN_SOL ) {
						_newclass = RTCW_CLASS_SOLDIER;
					} else {
						_newclass = Bot_PlayerClassGameToBot( rand() % NUM_PLAYER_CLASSES );
					}
			}
			else {
				_newclass = Bot_PlayerClassGameToBot( bot->client->sess.latchPlayerType );
			}
		}

		team = Bot_TeamGameToBot( bot->client->sess.sessionTeam );
		bot->client->sess.latchPlayerType = playerClassBotToGame( _newclass );

		Msg_PlayerChooseEquipment *pMsg = 0;
		if ( _data ) {
			_data->Get2( pMsg );
		}
		if ( pMsg ) {
			if ( pMsg->m_WeaponChoice[0] ) {
				bot->client->sess.latchPlayerWeapon = _weaponBotToMpWeapon( pMsg->m_WeaponChoice[0] );
			}
		}

		{
			const int iBotClass = Bot_PlayerClassGameToBot( bot->client->sess.latchPlayerType );
			if ( !bot->client->sess.latchPlayerWeapon || bot->client->sess.latchPlayerType != bot->client->sess.playerType ) {
				bot->client->sess.latchPlayerWeapon = _weaponBotToMpWeapon( _choosePriWeap( iBotClass, team ) );
			}
		}

		// commit suicide to ensure new class is used
		// CS: wait until 2 seconds before next spawn
		if ( bot->client->sess.latchPlayerType != bot->client->sess.playerType ) {
			//round end.
			if ( bot->client->ps.pm_flags & PMF_TIME_LOCKPLAYER ) {
				Cmd_Kill_f( bot );
			} else if ( !( bot->client->ps.pm_flags & PMF_LIMBO ) )     {
				bot->client->sess.botSuicide = qtrue;
			}

			// always clear this on class change. util scripts set it per class
			bot->client->sess.botSuicidePersist = qfalse;
		}

		return Success;
	}

	bool DebugLine( const float _start[3], const float _end[3], const obColor &_color, float _time ) {
		// for dedicated servers we tell the bot we can handle this function, so it doesn't open
		// an IPC channel.
		if ( g_dedicated.integer ) {
			return true;
		}

		return false;
	}

	bool DebugRadius( const float _pos[3], const float _radius, const obColor &_color, float _time ) {
		// for dedicated servers we tell the bot we can handle this function, so it doesn't open
		// an IPC channel.
		if ( g_dedicated.integer ) {
			return true;
		}

		return false;
	}

	void UpdateBotInput( int _client, const ClientInput &_input ) {
		static usercmd_t cmd;
		vec3_t angles, bodyangles, forward, right;
		gentity_t *bot = &g_entities[_client];

		// only causes problems
		bot->client->ps.pm_flags &= ~PMF_RESPAWNED;

		memset( &cmd, 0, sizeof( cmd ) );

		cmd.identClient = _client;
		cmd.serverTime = level.time;

		// Set the weapon
		cmd.weapon = _weaponBotToGame( _input.m_CurrentWeapon );

		// Process the bot keypresses.
		if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_RESPAWN ) ) {
			cmd.buttons |= BUTTON_ACTIVATE;
		}
		if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_ATTACK1 ) ) {
			cmd.buttons |= BUTTON_ATTACK;
		}
		if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_WALK ) ) {
			cmd.buttons |= BUTTON_WALKING;
		} else if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_SPRINT ) ) {
			cmd.buttons |= BUTTON_SPRINT;
		}
		if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_USE ) ) {
			cmd.buttons |= BUTTON_ACTIVATE;
		}

		// wbuttons
		if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_ATTACK2 ) ) {
			cmd.wbuttons |= WBUTTON_ATTACK2;
		}

		if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_DROP ) ) {
			cmd.wbuttons |= WBUTTON_DROP;
		}

		// if we have aim held
		if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_AIM ) ) {
			//if(!(bot->s.eFlags & EF_ZOOMING))
			switch ( bot->client->ps.weapon )
			{
			case WP_MAUSER:
				cmd.weapon = WP_SNIPERRIFLE;
				break;
			default:
				cmd.wbuttons |= WBUTTON_ZOOM;
				break;
			}
		} else
		{
			// if aim not held and we're zooming, zoom out.
			//if(bot->s.eFlags & EF_ZOOMING)
			//{
			//	trap_EA_Command(_client, "-zoom");
			//	//cmd.wbuttons |= WBUTTON_ZOOM;
			//}
		}

		if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_LEANLEFT ) ) {
			cmd.wbuttons |= WBUTTON_LEANLEFT;
		} else if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_LEANRIGHT ) ) {
			cmd.wbuttons |= WBUTTON_LEANRIGHT;
		}

		if ( bot->client->ps.weaponstate == WEAPON_RELOADING ) {
			// keep the same weapon while reloading
			cmd.weapon = bot->client->ps.weapon;
		} else if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_RELOAD ) )        {
			/*switch(_input.m_CurrentWeapon)
			{
			case RTCW_WP_SNIPERRIFLE:
			cmd.weapon = _weaponBotToGame(RTCW_WP_MAUSER);
			break;
			default:
			cmd.wbuttons |= WBUTTON_RELOAD;
			}*/
			cmd.wbuttons |= WBUTTON_RELOAD;
		}

		// don't process view angles and moving stuff when dead
		if ( bot->client->ps.pm_type >= PM_DEAD || bot->client->ps.pm_flags & ( PMF_LIMBO | PMF_TIME_LOCKPLAYER ) ) {
			// cant move in these states
			cmd.buttons &= ~BUTTON_ATTACK;
			cmd.wbuttons &= ~WBUTTON_ATTACK2;
		} else
		{
			float fMaxSpeed = 127.f;

			// Convert the bots vector to angles and set the view angle to the orientation
			vectoangles( _input.m_Facing, angles );
			SetClientViewAngle( bot, angles );

			if ( cmd.buttons & BUTTON_WALKING ) {
				fMaxSpeed = 64.f;
			}

			// Convert the move direction into forward and right moves to
			// take the bots orientation into account.

			// flatten the view angles so we get a proper fwd,right vector as relevent to movement.
			VectorCopy( angles,bodyangles );
			bodyangles[PITCH] = 0;

			AngleVectors( bodyangles, forward, right, NULL );
			const float fwd = DotProduct( forward, _input.m_MoveDir );
			const float rght = DotProduct( right, _input.m_MoveDir );

			cmd.forwardmove = (char)( fwd * fMaxSpeed );
			cmd.rightmove = (char)( rght * fMaxSpeed );

			if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_FWD ) || _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_MOVEUP ) ) {
				cmd.forwardmove = fMaxSpeed;
			}
			if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_BACK ) || _input.m_ButtonFlags.CheckFlag(BOT_BUTTON_MOVEDN) ) {
				cmd.forwardmove = -fMaxSpeed;
			}
			if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_RSTRAFE ) ) {
				cmd.rightmove = fMaxSpeed;
			}
			if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_LSTRAFE ) ) {
				cmd.rightmove = -fMaxSpeed;
			}
			if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_JUMP ) ) {
				cmd.upmove = fMaxSpeed;
			}
			if ( _input.m_ButtonFlags.CheckFlag( BOT_BUTTON_CROUCH ) ) {
				cmd.upmove = -fMaxSpeed;
			}
		}
		trap_BotUserCommand( _client, &cmd );
	}

	void BotCommand( int _client, const char *_cmd ) {
		trap_EA_Command( _client, (char *)_cmd );
	}

	obBool IsInPVS( const float _pos[3], const float _target[3] ) {
		return trap_InPVS( _pos, _target ) ? True : False;
	}

	obResult TraceLine( obTraceResult &_result, const float _start[3], const float _end[3],
		const AABB *_pBBox, int _mask, int _user, obBool _bUsePVS ) {
			trace_t tr;
			int iMask = 0;
			qboolean bInPVS = _bUsePVS ? trap_InPVS( _start, _end ) : qtrue;
			if ( bInPVS ) {
				// Set up the collision masks
				if ( _mask & TR_MASK_ALL ) {
					iMask = MASK_ALL;
				} else
				{
					if ( _mask & TR_MASK_SOLID ) {
						iMask |= MASK_SOLID;
					}
					if ( _mask & TR_MASK_PLAYER ) {
						iMask |= MASK_PLAYERSOLID;
					}
					if ( _mask & TR_MASK_SHOT ) {
						iMask |= MASK_SHOT;
					}
					if ( _mask & TR_MASK_OPAQUE ) {
						iMask |= MASK_OPAQUE;
					}
					if ( _mask & TR_MASK_WATER ) {
						iMask |= MASK_WATER;
					}
					if ( _mask & TR_MASK_PLAYERCLIP ) {
						iMask |= CONTENTS_PLAYERCLIP;
					}
					if ( _mask & TR_MASK_FLOODFILL ) {
						iMask |= CONTENTS_PLAYERCLIP | CONTENTS_SOLID;
					}

					if ( _mask & TR_MASK_SMOKEBOMB ) {
						gentity_t *pSmokeBlocker = Bot_EntInvisibleBySmokeBomb( (float*)_start, (float*)_end );
						if ( pSmokeBlocker ) {
							_result.m_Fraction = 0.0f;
							_result.m_HitEntity = HandleFromEntity( pSmokeBlocker );
							return Success;
						}
					}
				}

				trap_Trace( &tr, _start,
					_pBBox ? _pBBox->m_Mins : NULL,
					_pBBox ? _pBBox->m_Maxs : NULL,
					_end, _user, iMask );

				if ( ( tr.entityNum != ENTITYNUM_WORLD ) && ( tr.entityNum != ENTITYNUM_NONE ) ) {
					_result.m_HitEntity = HandleFromEntity( &g_entities[tr.entityNum] );
				} else {
					_result.m_HitEntity.Reset();
				}

				//_result.m_iUser1 = tr.surfaceFlags;

				// Fill in the bot traceflag.
				_result.m_Fraction = tr.fraction;
				_result.m_StartSolid = tr.startsolid;
				_result.m_Endpos[0] = tr.endpos[0];
				_result.m_Endpos[1] = tr.endpos[1];
				_result.m_Endpos[2] = tr.endpos[2];
				_result.m_Normal[0] = tr.plane.normal[0];
				_result.m_Normal[1] = tr.plane.normal[1];
				_result.m_Normal[2] = tr.plane.normal[2];
				_result.m_Contents = obUtilBotContentsFromGameContents( tr.contents );
				_result.m_Surface = obUtilBotSurfaceFromGameSurface( tr.surfaceFlags );
			} else
			{
				// Not in PVS
				_result.m_Fraction = 0.0f;
				_result.m_HitEntity.Reset();
			}
			return bInPVS ? Success : OutOfPVS;
	}

	GameEntity GetLocalGameEntity() {
		return EntityFromID( 0 );
	}

	virtual int GetPointContents( const float _pos[ 3 ] )
	{
		vec3_t vpos = { _pos[ 0 ], _pos[ 1 ], _pos[ 2 ] };
		const int iContents = trap_PointContents( vpos, -1 );
		return ConvertValue( iContents, ConvertContentsFlags, ConvertGameToBot );
	}

	virtual int ConvertValue( int value, ConvertType ctype, ConvertDirection cdir )
	{
		if ( cdir == ConvertGameToBot )
		{
			switch ( ctype )
			{
				case ConvertSurfaceFlags:
				{
					// clear flags we don't care about
					value &= ~( SURF_NOIMPACT | SURF_NOMARKS |
						SURF_HINT |
						SURF_NOLIGHTMAP | SURF_POINTLIGHT |
						SURF_LIGHTFILTER | SURF_ALPHASHADOW |
						SURF_NODLIGHT | SURF_METAL | SURF_WOOD |
						SURF_GRASS | SURF_GRAVEL |
						SURF_GLASS | SURF_SNOW |
						SURF_ROOF | SURF_RUBBLE | SURF_CARPET );

					int iBotSurface = 0;
					ConvertBit( value, iBotSurface, SURF_NONSOLID, SURFACE_NONSOLID );
					ConvertBit( value, iBotSurface, SURF_NOSTEPS, SURFACE_NOFOOTSTEP );
					ConvertBit( value, iBotSurface, SURF_SKY, SURFACE_SKY );
					ConvertBit( value, iBotSurface, SURF_SKIP, SURFACE_IGNORE );
					ConvertBit( value, iBotSurface, SURF_NODAMAGE, SURFACE_NOFALLDAMAGE );
					ConvertBit( value, iBotSurface, SURF_SLICK, SURFACE_SLICK );
					ConvertBit( value, iBotSurface, SURF_LADDER, SURFACE_LADDER );
					ConvertBit( value, iBotSurface, SURF_NODRAW, SURFACE_NODRAW );

					assert( value == 0 && "Unhandled flag" );
					return iBotSurface;
				}
				case ConvertContentsFlags:
				{
					const int CLIPSHOT_I_THINK = 0x00002000;

					value &= ~( CONTENTS_LIGHTGRID | CONTENTS_MISSILECLIP |
						CONTENTS_ITEM | CONTENTS_AREAPORTAL | CONTENTS_MONSTERCLIP |
						CONTENTS_CLUSTERPORTAL | CONTENTS_DONOTENTER |
						CONTENTS_DONOTENTER_LARGE | CONTENTS_ORIGIN |
						CONTENTS_STRUCTURAL | CONTENTS_TRANSLUCENT |
						CONTENTS_NODROP | CONTENTS_BODY | CONTENTS_CORPSE |
						CLIPSHOT_I_THINK );

					int iBotContents = 0;
					ConvertBit( value, iBotContents, CONTENTS_SOLID, CONT_SOLID );
					ConvertBit( value, iBotContents, CONTENTS_WATER, CONT_WATER );
					ConvertBit( value, iBotContents, CONTENTS_SLIME, CONT_SLIME );
					ConvertBit( value, iBotContents, CONTENTS_FOG, CONT_FOG );
					ConvertBit( value, iBotContents, CONTENTS_TELEPORTER, CONT_TELEPORTER );
					ConvertBit( value, iBotContents, CONTENTS_JUMPPAD, CONT_JUMPPAD );
					ConvertBit( value, iBotContents, CONTENTS_MOVER, CONT_MOVER );
					ConvertBit( value, iBotContents, CONTENTS_TRIGGER, CONT_TRIGGER );
					ConvertBit( value, iBotContents, CONTENTS_LAVA, CONT_LAVA );
					ConvertBit( value, iBotContents, CONTENTS_PLAYERCLIP, CONT_PLYRCLIP );
					ConvertBit( value, iBotContents, CONTENTS_DETAIL, CONT_NONSOLID );
					ConvertBit( value, iBotContents, CONTENTS_FOG, CONT_FOG );

					assert( value == 0 && "Unhandled flag" );
					return iBotContents;
				}
			}
		}
		else
		{
			switch ( ctype )
			{
				case ConvertSurfaceFlags:
				{
					int iBotSurface = 0;
					ConvertBit( value, iBotSurface, SURFACE_NONSOLID, SURF_NONSOLID );
					ConvertBit( value, iBotSurface, SURFACE_NOFOOTSTEP, SURF_NOSTEPS );
					ConvertBit( value, iBotSurface, SURFACE_SKY, SURF_SKY );
					ConvertBit( value, iBotSurface, SURFACE_IGNORE, SURF_SKIP );
					ConvertBit( value, iBotSurface, SURFACE_NOFALLDAMAGE, SURF_NODAMAGE );
					ConvertBit( value, iBotSurface, SURFACE_SLICK, SURF_SLICK );
					ConvertBit( value, iBotSurface, SURFACE_LADDER, SURF_LADDER );

					assert( value == 0 && "Unhandled flag" );
					return iBotSurface;
				}
				case ConvertContentsFlags:
				{
					int iBotContents = 0;
					ConvertBit( value, iBotContents, CONT_SOLID, CONTENTS_SOLID );
					ConvertBit( value, iBotContents, CONT_WATER, CONTENTS_WATER );
					ConvertBit( value, iBotContents, CONT_SLIME, CONTENTS_SLIME );
					ConvertBit( value, iBotContents, CONT_FOG, CONTENTS_FOG );
					ConvertBit( value, iBotContents, CONT_TELEPORTER, CONTENTS_TELEPORTER );
					ConvertBit( value, iBotContents, CONT_JUMPPAD, CONTENTS_JUMPPAD );
					ConvertBit( value, iBotContents, CONT_MOVER, CONTENTS_MOVER );
					ConvertBit( value, iBotContents, CONT_TRIGGER, CONTENTS_TRIGGER );
					ConvertBit( value, iBotContents, CONT_LAVA, CONTENTS_LAVA );
					ConvertBit( value, iBotContents, CONT_PLYRCLIP, CONTENTS_PLAYERCLIP );
					ConvertBit( value, iBotContents, CONT_NONSOLID, CONTENTS_DETAIL );
					ConvertBit( value, iBotContents, CONT_FOG, CONTENTS_FOG );

					assert( value == 0 && "Unhandled flag" );
					return iBotContents;
				}
			}
		}
		assert( 0 && "Unhandled conversion" );
		return 0;
	}

	GameEntity FindEntityInSphere( const float _pos[3], float _radius, GameEntity _pStart, int classId ) {
		// square it to avoid the square root in the distance check.
		gentity_t *pStartEnt = _pStart.IsValid() ? EntityFromHandle( _pStart ) : 0;

		const char *pClassName = 0;
		int iPlayerClass = 0;
		int iSpawnFlags = 0;
		switch ( classId )
		{
		case RTCW_CLASS_SOLDIER:
		case RTCW_CLASS_MEDIC:
		case RTCW_CLASS_ENGINEER:
		case RTCW_CLASS_LIEUTENANT:
		case RTCW_CLASS_ANY:
			iPlayerClass = classId != RTCW_CLASS_ANY ? classId : 0;
			pClassName = "player";
			break;
			//////////////////////////////////////////////////////////////////////////
		case RTCW_CLASSEX_MG42MOUNT:
			pClassName = "misc_mg42";
			break;
		case RTCW_CLASSEX_DYNAMITE:
			pClassName = "dynamite";
			break;
		case RTCW_CLASSEX_BREAKABLE:
			break;
		case RTCW_CLASSEX_CORPSE:
			pClassName = "corpse";
			break;
		case RTCW_CLASSEX_TREASURE:
			pClassName = "item_treasure";
			break;
		case RTCW_CLASSEX_GRENADE:
			pClassName = "grenade";
			break;
		case RTCW_CLASSEX_ROCKET:
			pClassName = "rocket";
			break;
		case RTCW_CLASSEX_ARTY:
			pClassName = "air strike";
			break;
		case RTCW_CLASSEX_AIRSTRIKE:
			pClassName = "air strike";
			break;
		case RTCW_CLASSEX_FLAMECHUNK:
			pClassName = "flamechunk";
			break;
		}

		if ( pClassName ) {
			float fSqRad = _radius * _radius;
			vec3_t toent;

			while ( ( pStartEnt = G_Find( pStartEnt, FOFS( classname ), pClassName ) ) != NULL )
			{
				if ( iPlayerClass && pStartEnt->client &&
					pStartEnt->client->sess.sessionTeam != iPlayerClass ) {
						continue;
				}

				if ( iSpawnFlags && !( pStartEnt->spawnflags & iSpawnFlags ) ) {
					continue;
				}

				VectorSubtract( _pos, pStartEnt->r.currentOrigin, toent );
				if ( VectorLengthSquared( toent ) < fSqRad ) {
					break;
				}
			}
			return HandleFromEntity( pStartEnt );
		}
		return GameEntity();
	}

	int GetEntityClass( const GameEntity _ent ) {
		gentity_t *pEnt = EntityFromHandle( _ent );
		return pEnt && pEnt->inuse ? _GetEntityClass( pEnt ) : RTCW_CLASS_NULL;
	}

	obResult GetEntityCategory( const GameEntity _ent, BitFlag32 &_category ) {
		obResult res = Success;
		gentity_t *pEnt = EntityFromHandle( _ent );
		if ( !pEnt ) {
			return InvalidEntity;
		}

		// hack, when the game joins clients again after warmup, they are temporarily ET_GENERAL entities(LAME)
		int t = pEnt->s.eType;
		if ( pEnt->client && ( pEnt - g_entities ) < MAX_CLIENTS ) {
			t = ET_PLAYER;
		}

		switch ( t )
		{
		case ET_GENERAL:
			{
				if ( !Q_stricmp( pEnt->classname, "func_invisible_user" ) ) {
					// The damage flags tells us the type.
					switch ( pEnt->s.dmgFlags )
					{
					case HINT_BUTTON:
						_category.SetFlag( ENT_CAT_TRIGGER );
						_category.SetFlag( ENT_CAT_NOLOS );
						break;
					}
				} else if ( !Q_stricmp( pEnt->classname, "func_button" ) ) {
					_category.SetFlag( ENT_CAT_TRIGGER );
					_category.SetFlag( ENT_CAT_NOLOS );
					// continue for now so it doesnt get regged
					//continue;
				} else if(!Q_stricmp(pEnt->classname, "info_player_deathmatch") ||
					!Q_stricmp(pEnt->classname, "team_CTF_redspawn") ||
					!Q_stricmp(pEnt->classname, "team_CTF_bluespawn") ||
					!Q_stricmp(pEnt->classname, "info_player_spawn"))
				{
					// don't fill up the bots sensory mem at start with these
					_category.SetFlag(ENT_CAT_INTERNAL);
				} else {
					res = InvalidEntity;
				}
				break;
			}
		case ET_PLAYER:
			{
				if ( !pEnt->client || ( pEnt->entstate == STATE_INVISIBLE ) ||
					( pEnt->client->ps.pm_type == PM_SPECTATOR ) ||
					( pEnt->client->sess.sessionTeam != TEAM_RED &&
					pEnt->client->sess.sessionTeam != TEAM_BLUE ) ) {
						res = InvalidEntity;
						break;
				}

				// Special case for dead players that haven't respawned.
				//if(pEnt->health <= 0)
				//{
				//	_category.SetFlag(ENT_CAT_MISC);
				//	break;
				//}
				if ( pEnt->health > GIB_HEALTH ) {
					if ( pEnt->client->ps.powerups[PW_INVULNERABLE] <= level.time ) {
						_category.SetFlag( ENT_CAT_SHOOTABLE );
					}

					_category.SetFlag( ENT_CAT_PLAYER );
				}
				break;
			}

		case ET_ITEM:
			{
				if ( !Q_strncmp( pEnt->classname, "item_health", strlen( "item_health" ) ) ) {
					_category.SetFlag( ENT_CAT_PICKUP );
					_category.SetFlag( ENT_CAT_PICKUP_HEALTH );
				} else if ( !Q_strncmp( pEnt->classname, "weapon_magicammo", strlen( "weapon_magicammo" ) ) )          {
					_category.SetFlag( ENT_CAT_PICKUP );
					_category.SetFlag( ENT_CAT_PICKUP_AMMO );
				} else if ( !Q_stricmp( pEnt->classname, "item_treasure" ) )        {
					_category.SetFlag( ENT_CAT_PICKUP );
				} else if ( pEnt->item && pEnt->item->giType == IT_WEAPON )      {
					_category.SetFlag( ENT_CAT_PICKUP );
					_category.SetFlag( ENT_CAT_PICKUP_WEAPON );
				} else {
					res = InvalidEntity;
				}
				break;
			}
		case ET_CORPSE:
			{
				_category.SetFlag( ENT_CAT_MISC );
				break;
			}
		case ET_MISSILE:
			{
				// Register certain weapons as threats to avoid or whatever.
				switch ( pEnt->s.weapon )
				{
				case WP_GRENADE_LAUNCHER:
				case WP_GRENADE_PINEAPPLE:
				case WP_PANZERFAUST:
				case WP_SMOKE_GRENADE:
				case WP_ARTY:
				case WP_DYNAMITE:
					_category.SetFlag( ENT_CAT_OBSTACLE );
					_category.SetFlag( ENT_CAT_PROJECTILE );
					break;
				default:
					if ( !Q_strncmp( pEnt->classname, "air strike", sizeof( "air strike" ) ) ) {
						_category.SetFlag( ENT_CAT_OBSTACLE );
						_category.SetFlag( ENT_CAT_PROJECTILE );
						break;
					} else {
						res = InvalidEntity;
					}
				}
				break;
			}
		case ET_FLAMETHROWER_CHUNK:
			{
				_category.SetFlag( ENT_CAT_PROJECTILE );
				break;
			}
		case ET_MOVER:
			{
				if ( !Q_stricmp( pEnt->classname, "script_mover" ) ) {
					if ( pEnt->model2 ) {
						_category.SetFlag( ENT_CAT_VEHICLE );
					} else {
						_category.SetFlag( ENT_CAT_MOVER );
					}

					_category.SetFlag( ENT_CAT_NOLOS );
					if ( ( pEnt->health > 0 ) && ( pEnt->takedamage == qtrue ) ) {
						_category.SetFlag( ENT_CAT_SHOOTABLE );
					}
				}
				/*				else if (!Q_stricmp(pEnt->classname, "props_flamebarrel"))
				{
				if ((pEnt->health > 0) && (pEnt->takedamage == qtrue))
				{
				_category.SetFlag(ENT_CAT_SHOOTABLE);
				}
				}*/
				else if ( !Q_stricmp( pEnt->classname, "props_chair_hiback" ) ) {
					if ( ( pEnt->health > 0 ) && ( pEnt->takedamage == qtrue ) ) {
						_category.SetFlag( ENT_CAT_SHOOTABLE );
					}
				} else if ( !Q_stricmp( pEnt->classname, "props_chair" ) )      {
					if ( ( pEnt->health > 0 ) && ( pEnt->takedamage == qtrue ) ) {
						_category.SetFlag( ENT_CAT_SHOOTABLE );
					}
				} else if ( !Q_stricmp( pEnt->classname, "props_chair_side" ) )      {
					if ( ( pEnt->health > 0 ) && ( pEnt->takedamage == qtrue ) ) {
						_category.SetFlag( ENT_CAT_SHOOTABLE );
					}
				}
				/*else if (!Q_stricmp(pCurrent->classname, "props_statue"))
				{
				info.m_EntityClass = ET_CLASSEX_BREAKABLE;
				info.m_EntityCategory = ENT_CAT_SHOOTABLE;
				}*/
				else {
					res = InvalidEntity;
				}
				break;
			}
			/*case ET_PROP:
			{
			if ((pEnt->health > 0) && (pEnt->takedamage == qtrue))
			{
			_category.SetFlag(ENT_CAT_SHOOTABLE);
			}
			break;
			}*/
		case ET_MG42_BARREL:
			{
				if ( ( pEnt->health > 0 ) &&
					( pEnt->entstate != STATE_INVISIBLE ) ) {
						_category.SetFlag( ENT_CAT_MOUNTEDWEAPON );
						_category.SetFlag( ENT_CAT_SHOOTABLE );
				} else {
					res = InvalidEntity;
				}
				break;
			}
			/*case ET_AAGUN:
			{
			if((pCurrent->health > 0) &&
			(pCurrent->entstate != STATE_INVISIBLE) &&
			(pCurrent->entstate != STATE_UNDERCONSTRUCTION))
			{
			}
			break;
			}*/
		case ET_EXPLOSIVE:
			{
				if ( ( pEnt->health > 0 ) && ( pEnt->takedamage == qtrue ) ) {
					_category.SetFlag( ENT_CAT_SHOOTABLE );
				} else {
					res = InvalidEntity;
				}
				break;
			}
			//case ET_CONSTRUCTIBLE:
			//{
			//if (G_ConstructionIsPartlyBuilt(pCurrent) &&
			//	!(pCurrent->spawnflags & CONSTRUCTIBLE_INVULNERABLE) &&
			//	 (pCurrent->constructibleStats.weaponclass == 0))
			//{
			//	info.m_EntityClass = ET_CLASSEX_BREAKABLE;
			//	info.m_EntityCategory = ENT_CAT_SHOOTABLE;
			//}
			//else
			//{
			//}
			//break;
			//}
		default:
			res = InvalidEntity;
			break;     // ignore this type.
		};
		return res;
	}

	obResult GetEntityFlags( const GameEntity _ent, BitFlag64 &_flags ) {
		gentity_t *pEnt = EntityFromHandle( _ent );

		if ( pEnt && pEnt->inuse ) {
			// Set any flags.
			if ( pEnt->s.eType == ET_PLAYER && pEnt->health > GIB_HEALTH && pEnt->health < 0
				&& g_OmniBotGib.integer ) {
					//ungibbed players are proned
					_flags.SetFlag( ENT_FLAG_PRONED );
					_flags.SetFlag( ENT_FLAG_VISTEST );
					_flags.SetFlag( RTCW_ENT_FLAG_INJURED );
			} else if ( pEnt->health <= 0 )      {
				_flags.SetFlag( ENT_FLAG_DEAD );
			}
			if ( pEnt->client && !IsBot( pEnt ) ) {
				_flags.SetFlag( ENT_FLAG_HUMANCONTROLLED );
			}

			if ( pEnt->waterlevel >= 3 ) {
				_flags.SetFlag( ENT_FLAG_UNDERWATER );
			} else if ( pEnt->waterlevel > 0 ) {
				_flags.SetFlag( ENT_FLAG_INWATER );
			}

			if ( pEnt->s.eFlags & EF_ZOOMING ) {
				_flags.SetFlag( ENT_FLAG_ZOOMING );
				_flags.SetFlag( ENT_FLAG_AIMING );
			}

			if ( pEnt->s.eFlags & EF_MG42_ACTIVE ) {
				_flags.SetFlag( RTCW_ENT_FLAG_MNT_MG42 );
			}

			if ( pEnt->s.eType == ET_MOVER ) {
				_flags.SetFlag( ENT_FLAG_VISTEST );
			}

			if ( pEnt->s.eType == ET_MG42_BARREL ) {
				_flags.SetFlag( ENT_FLAG_DEAD, Simple_EmplacedGunIsRepairable( pEnt ) != 0 );

				_flags.SetFlag( ENT_FLAG_VISTEST );
				if ( _EmplacedGunIsMountable( pEnt ) ) {
					_flags.SetFlag( RTCW_ENT_FLAG_ISMOUNTABLE );
				}
			}

			if ( pEnt->poisoned ) {
				_flags.SetFlag( RTCW_ENT_FLAG_POISONED );
			}

			if ( pEnt->client ) {
				if ( pEnt->client->ps.pm_flags & PMF_LADDER ) {
					_flags.SetFlag( ENT_FLAG_ONLADDER );
				}
				if ( pEnt->client->ps.pm_flags & PMF_DUCKED ) {
					_flags.SetFlag( ENT_FLAG_CROUCHED );
				}
				if ( pEnt->client->ps.groundEntityNum != ENTITYNUM_NONE ) {
					_flags.SetFlag( ENT_FLAG_ONGROUND );
				}
				if ( pEnt->client->ps.weaponstate == WEAPON_RELOADING ) {
					_flags.SetFlag( ENT_FLAG_RELOADING );
				}
				if ( pEnt->client->ps.powerups[PW_REDFLAG] || pEnt->client->ps.powerups[PW_BLUEFLAG] ) {
					_flags.SetFlag( RTCW_ENT_FLAG_CARRYINGGOAL );
				}
				if ( pEnt->client->ps.pm_flags & PMF_LIMBO ) {
					_flags.SetFlag( RTCW_ENT_FLAG_INLIMBO );
				}
			}

			// hack, when the game joins clients again after warmup, they are temporarily ET_GENERAL entities(LAME)
			int t = pEnt->s.eType;
			if ( pEnt->client && ( pEnt - g_entities ) < MAX_CLIENTS ) {
				t = ET_PLAYER;
			}

			switch ( t )
			{
			case ET_GENERAL:
				{
					_flags.SetFlag( ENT_FLAG_VISTEST );
					break;
				}
			case ET_PLAYER:
				{
					_flags.SetFlag( ENT_FLAG_VISTEST );
					if ( pEnt->health <= 0 ) {
						if ( !pEnt->r.linked || pEnt->health < GIB_HEALTH ) {
							_flags.SetFlag( ENT_FLAG_DISABLED );
						}
					}
					break;
					/*if(!pEnt->takedamage)
					_flags.SetFlag(ENT_FLAG_DISABLED);
					break;*/
				}
			case ET_TRAP: // checkpoints
				{
					if ( pEnt->touch == NULL ) {
						_flags.SetFlag( ENT_FLAG_DISABLED );
					}
				}
			case ET_CORPSE:
				{
					_flags.SetFlag( ENT_FLAG_VISTEST );
					if ( !pEnt->r.linked || pEnt->health < GIB_HEALTH ) {
						_flags.SetFlag( ENT_FLAG_DISABLED );
					}
					break;

					/*if(!pEnt->r.linked || !pEnt->takedamage)
					_flags.SetFlag(ENT_FLAG_DISABLED);
					break;*/
				}
			case ET_EXPLOSIVE:
				{
					_flags.SetFlag( ENT_FLAG_VISTEST );
					break;
				}
				/*			case ET_PROP:
				{
				_flags.SetFlag(ENT_FLAG_VISTEST);
				break;
				}*/
			case ET_MISSILE:
				{
					// Register certain weapons as threats to avoid or whatever.
					switch ( pEnt->s.weapon )
					{
					case WP_GRENADE_LAUNCHER:
					case WP_GRENADE_PINEAPPLE:
					case WP_PANZERFAUST:
					case WP_ARTY:
					case WP_SMOKE_GRENADE:
					case WP_DYNAMITE:
					default:
						_flags.SetFlag( ENT_FLAG_VISTEST );
					}
					break;
				}
			case ET_MG42_BARREL:
				{
					if ( ( pEnt->health < 0 ) ||
						( pEnt->entstate == STATE_INVISIBLE ) ) {
							_flags.SetFlag( ENT_FLAG_DEAD );
					}

					if(pEnt->r.ownerNum != pEnt->s.number)
					{
						gentity_t *owner = &g_entities[pEnt->r.ownerNum];
						if(owner && owner->active && owner->client && owner->s.eFlags & EF_MG42_ACTIVE)
							_flags.SetFlag(RTCW_ENT_FLAG_MOUNTED);
					}
					break;
				}
				//default:
				//	_flags.SetFlag(ENT_FLAG_VISTEST);
			}
		}
		return Success;
	}

	obResult GetEntityPowerups( const GameEntity _ent, BitFlag64 &_flags ) {
		gentity_t *pEnt = EntityFromHandle( _ent );

		if ( !pEnt ) {
			return InvalidEntity;
		}

		if ( pEnt && pEnt->client ) {
			if ( pEnt->client->ps.powerups[PW_INVULNERABLE] || ( pEnt->flags & FL_GODMODE ) ) {
				_flags.SetFlag( PWR_INVINCIBLE );
			}
			if ( pEnt->client->ps.powerups[PW_FIRE] ) {
				_flags.SetFlag( RTCW_PWR_FIRE );
			}
			if ( pEnt->client->ps.powerups[PW_ELECTRIC] ) {
				_flags.SetFlag( RTCW_PWR_ELECTRIC );
			}
			if ( pEnt->client->ps.powerups[PW_BREATHER] ) {
				_flags.SetFlag( RTCW_PWR_BREATHER );
			}
			if ( pEnt->client->ps.powerups[PW_NOFATIGUE] ) {
				_flags.SetFlag( RTCW_PWR_NOFATIGUE );
			}
			if ( pEnt->client->ps.powerups[PW_REDFLAG] ) {
				_flags.SetFlag( RTCW_PWR_REDFLAG );
			}
			if ( pEnt->client->ps.powerups[PW_BLUEFLAG] ) {
				_flags.SetFlag( RTCW_PWR_BLUEFLAG );
			}
			//if ( pEnt->client->ps.powerups[PW_BALL] ) {
			//	_flags.SetFlag( RTCW_PWR_BALL );
			//}
		}

		return Success;
	}

	obResult GetEntityEyePosition( const GameEntity _ent, float _pos[3] ) {
		if ( GetEntityPosition( _ent, _pos ) == Success ) {
			gentity_t *pEnt = EntityFromHandle( _ent );
			if ( pEnt && pEnt->client ) {
				_pos[2] += pEnt->client->ps.viewheight;
			}
			return Success;
		}
		return InvalidEntity;
	}

	obResult GetEntityBonePosition( const GameEntity _ent, int _boneid, float _pos[3] ) {
		// ET doesnt really support bones
		return GetEntityPosition( _ent, _pos );
	}

	obResult GetEntityOrientation( const GameEntity _ent, float _fwd[3], float _right[3], float _up[3] ) {
		gentity_t *pEnt = EntityFromHandle( _ent );
		if ( pEnt && pEnt->inuse ) {
			if ( pEnt->client ) {
				AngleVectors( pEnt->client->ps.viewangles, _fwd, _right, _up );
			} else
			{
				AngleVectors( pEnt->s.angles, _fwd, _right, _up );
			}
			return Success;
		}
		return InvalidEntity;
	}

	obResult GetEntityVelocity( const GameEntity _ent, float _velocity[3] ) {
		gentity_t *pEnt = EntityFromHandle( _ent );
		if ( pEnt && pEnt->inuse ) {
			_velocity[0] = pEnt->s.pos.trDelta[0];
			_velocity[1] = pEnt->s.pos.trDelta[1];
			_velocity[2] = pEnt->s.pos.trDelta[2];
			return Success;
		}
		return InvalidEntity;
	}

	obResult GetEntityPosition( const GameEntity _ent, float _pos[3] ) {
		gentity_t *pEnt = EntityFromHandle( _ent );
		if ( pEnt && pEnt->inuse ) {
			if ( !pEnt->client ) {
				// Need to do special case stuff for certain entities.
				if ( ( pEnt->s.eType == ET_EXPLOSIVE ) ||
					( pEnt->s.eType == ET_OID_TRIGGER ) ||
					!Q_stricmp( pEnt->classname, "trigger_multiple" ) ||
					!Q_stricmp( pEnt->classname, "func_commandpoint" ) ||
					!Q_stricmp( pEnt->classname, "plat_trigger" ) ||
					!Q_stricmp( pEnt->classname, "door_trigger" ) ||
					!Q_stricmp( pEnt->classname, "props_flamebarrel" ) ||
					!Q_stricmp( pEnt->classname, "props_chair_hiback" ) ||
					!Q_stricmp( pEnt->classname, "props_chair" ) ||
					!Q_stricmp( pEnt->classname, "props_chair_side" ) ) {
						// Center of bounding box, cuz currentOrigin isn't valid.
						/*if(!pEnt->r.linked && pEnt->parent && pEnt->parent->s.eType == ET_OID_TRIGGER)
						pEnt = pEnt->parent;*/

						_pos[0] = pEnt->r.currentOrigin[0] + ( ( pEnt->r.maxs[0] + pEnt->r.mins[0] ) * 0.5f );
						_pos[1] = pEnt->r.currentOrigin[1] + ( ( pEnt->r.maxs[1] + pEnt->r.mins[1] ) * 0.5f );
						_pos[2] = pEnt->r.currentOrigin[2] + ( ( pEnt->r.maxs[2] + pEnt->r.mins[2] ) * 0.5f );
						return Success;
				}
			}

			//ungibbed players, center / bottom
			//if ( pEnt->client && pEnt->health > GIB_HEALTH && pEnt->health < 1 ) {
			//	_pos[0] = pEnt->r.currentOrigin[0] + ( ( pEnt->r.maxs[0] + pEnt->r.mins[0] ) * 0.5f );
			//	_pos[1] = pEnt->r.currentOrigin[1] + ( ( pEnt->r.maxs[1] + pEnt->r.mins[1] ) * 0.5f );
			//	_pos[2] = pEnt->r.currentOrigin[2] + pEnt->r.mins[2];
			//	return Success;
			//}

			// Clients and entities not caught above will return normal position.
			_pos[0] = pEnt->r.currentOrigin[0];
			_pos[1] = pEnt->r.currentOrigin[1];
			_pos[2] = pEnt->r.currentOrigin[2];

			return Success;
		}

		return InvalidEntity;
	}

	obResult GetEntityLocalAABB( const GameEntity _ent, AABB &_aabb ) {
		gentity_t *pEnt = EntityFromHandle( _ent );
		if ( pEnt && pEnt->inuse ) {
			/*if(!pEnt->r.linked && pEnt->parent && pEnt->parent->s.eType == ET_OID_TRIGGER)
			pEnt = pEnt->parent;*/

			if ( !Q_stricmp( pEnt->classname, "func_explosive" ) ||
				!Q_stricmpn( pEnt->classname, "trigger_",8 ) ) {
					// find the midpt
					vec3_t pos;
					pos[0] = pEnt->r.currentOrigin[0] + ( ( pEnt->r.maxs[0] + pEnt->r.mins[0] ) * 0.5f );
					pos[1] = pEnt->r.currentOrigin[1] + ( ( pEnt->r.maxs[1] + pEnt->r.mins[1] ) * 0.5f );
					pos[2] = pEnt->r.currentOrigin[2] + ( ( pEnt->r.maxs[2] + pEnt->r.mins[2] ) * 0.5f );

					// figure out the local bounds from there
					_aabb.m_Mins[0] = pos[0] - pEnt->r.mins[0];
					_aabb.m_Mins[1] = pos[1] - pEnt->r.mins[1];
					_aabb.m_Mins[2] = pos[2] - pEnt->r.mins[2];
					_aabb.m_Maxs[0] = pos[0] - pEnt->r.maxs[0];
					_aabb.m_Maxs[1] = pos[1] - pEnt->r.maxs[1];
					_aabb.m_Maxs[2] = pos[2] - pEnt->r.maxs[2];
					return Success;
			}

			_aabb.m_Mins[0] = pEnt->r.mins[0];
			_aabb.m_Mins[1] = pEnt->r.mins[1];
			_aabb.m_Mins[2] = pEnt->r.mins[2];
			_aabb.m_Maxs[0] = pEnt->r.maxs[0];
			_aabb.m_Maxs[1] = pEnt->r.maxs[1];
			_aabb.m_Maxs[2] = pEnt->r.maxs[2];

			// hack for bad abs bounds
			if ( !Q_stricmp( pEnt->classname, "misc_mg42" ) ) {
				if ( _aabb.IsZero() ) {
					const float default_box_mins[] = { -8,-8,-8 };
					const float default_box_maxs[] = { 8,8,48 };
					_aabb.Set( default_box_mins,default_box_maxs );
				}
			}
			return Success;
		}
		return InvalidEntity;
	}

	obResult GetEntityWorldAABB( const GameEntity _ent, AABB &_aabb ) {
		gentity_t *pEnt = EntityFromHandle( _ent );
		if ( pEnt && pEnt->inuse ) {
			/*if(!pEnt->r.linked && pEnt->parent && pEnt->parent->s.eType == ET_OID_TRIGGER)
			pEnt = pEnt->parent;*/
			_aabb.m_Mins[0] = pEnt->r.absmin[0];
			_aabb.m_Mins[1] = pEnt->r.absmin[1];
			_aabb.m_Mins[2] = pEnt->r.absmin[2];
			_aabb.m_Maxs[0] = pEnt->r.absmax[0];
			_aabb.m_Maxs[1] = pEnt->r.absmax[1];
			_aabb.m_Maxs[2] = pEnt->r.absmax[2];

			// raise player bounds slightly since it appears to be in the ground a bit
			if ( pEnt->client )
			{
				_aabb.m_Mins[2] += 2.f;
				_aabb.m_Maxs[2] += 2.f;
			}

			// hack for bad abs bounds
			if ( !Q_stricmp( pEnt->classname, "misc_mg42" ) ) {
				if ( _aabb.IsZero() || !_aabb.Contains( pEnt->r.currentOrigin ) ) {
					float pos[3] = {0,0,0};
					const float default_box_mins[] = { -8,-8,-8 };
					const float default_box_maxs[] = { 8,8,48 };
					GetEntityPosition( _ent,pos );
					_aabb.Set( default_box_mins,default_box_maxs );
					_aabb.SetCenter( pos );
				}
			}

			return Success;
		}
		return InvalidEntity;
	}

	obResult GetEntityWorldOBB( const GameEntity _ent, float *_center, float *_axis0, float *_axis1, float *_axis2, float *_extents ) {
		gentity_t *pEnt = EntityFromHandle( _ent );
		if ( pEnt ) {
			vec3_t axis[3];
			AnglesToAxis( pEnt->r.currentAngles, axis );

			vec3_t boxCenter;
			boxCenter[0] = ( ( pEnt->r.maxs[0] + pEnt->r.mins[0] ) * 0.5f );
			boxCenter[1] = ( ( pEnt->r.maxs[1] + pEnt->r.mins[1] ) * 0.5f );
			boxCenter[2] = ( ( pEnt->r.maxs[2] + pEnt->r.mins[2] ) * 0.5f );

			vec3_t out;
			VectorCopy( pEnt->r.currentOrigin,out );
			for ( int i = 0; i < 3; ++i ) {
				vec3_t tmp;
				VectorScale( axis[i], boxCenter[i], tmp );
				VectorAdd( out, tmp, out );
			}
			VectorCopy( out, _center );
			VectorCopy( axis[0], _axis0 );
			VectorCopy( axis[1], _axis1 );
			VectorCopy( axis[2], _axis2 );
			_extents[0] = ( pEnt->r.maxs[0] - pEnt->r.mins[0] ) * 0.5f;
			_extents[1] = ( pEnt->r.maxs[1] - pEnt->r.mins[1] ) * 0.5f;
			_extents[2] = ( pEnt->r.maxs[2] - pEnt->r.mins[2] ) * 0.5f;

			return Success;
		}
		return InvalidEntity;
	}

	obResult GetEntityGroundEntity( const GameEntity _ent, GameEntity &moveent ) {
		gentity_t *pEnt = EntityFromHandle( _ent );
		if ( pEnt ) {
			if ( pEnt->s.groundEntityNum > 0 && pEnt->s.groundEntityNum < ENTITYNUM_MAX_NORMAL ) {
				moveent = HandleFromEntity( &g_entities[pEnt->s.groundEntityNum] );
			}
			return Success;
		}
		return InvalidEntity;
	}

	GameEntity GetEntityOwner( const GameEntity _ent ) {
		GameEntity owner;

		gentity_t *pEnt = EntityFromHandle( _ent );
		if ( pEnt && pEnt->inuse ) {
			// hack, when the game joins clients again after warmup, they are temporarily ET_GENERAL entities(LAME)
			int t = pEnt->s.eType;
			if ( pEnt->client && ( pEnt - g_entities ) < MAX_CLIENTS ) {
				t = ET_PLAYER;
			}

			switch ( t )
			{
			case ET_ITEM:
				{
					if ( !Q_stricmp( pEnt->classname, "team_CTF_redflag" ) || !Q_stricmp( pEnt->classname, "team_CTF_blueflag" ) ) {
						int iFlagEntNum = pEnt - g_entities;
						for ( int i = 0; i < MAX_CLIENTS; ++i )
						{
							if ( g_entities[i].client && g_entities[i].client->flagParent == iFlagEntNum ) {
								owner = HandleFromEntity( &g_entities[i] );
							}
						}
					}
					break;
				}
			case ET_MG42_BARREL:
				{
					if ( pEnt->r.ownerNum != pEnt->s.number ) {
						gentity_t *pOwner = &g_entities[pEnt->r.ownerNum];
						if ( pOwner && pOwner->active && pOwner->client && pOwner->s.eFlags & EF_MG42_ACTIVE ) {
							owner = HandleFromEntity( pOwner );
						}
					}
					break;
				}
			default:
				// -1 means theres no owner.
				if ( pEnt->r.ownerNum < MAX_GENTITIES ) {
					owner = HandleFromEntity( &g_entities[pEnt->r.ownerNum] );
				}
			}
		}
		return owner;
	}

	int GetEntityTeam( const GameEntity _ent ) {
		gentity_t *pEnt = EntityFromHandle( _ent );
		return pEnt && pEnt->inuse ? _GetEntityTeam( pEnt ) : RTCW_TEAM_NONE;
	}

	const char *GetEntityName( const GameEntity _ent ) {
		gentity_t *pEnt = EntityFromHandle( _ent );
		if ( pEnt ) {
			return _GetEntityName( pEnt );
		}
		return NULL;
	}

	obResult GetModel( GameModelInfo & modelOut, MemoryAllocator & alloc )
	{
		fileHandle_t f;
		const int fileSize = trap_FS_FOpenFile( va( "maps/%s.bsp", level.rawmapname ), &f, FS_READ );
		if ( fileSize > 0 )
		{
			char * mapBuffer = alloc.AllocateMemory( fileSize );
			trap_FS_Read( mapBuffer, fileSize, f );
			trap_FS_FCloseFile( f );

			Com_sprintf( modelOut.mModelName, sizeof( modelOut.mModelName ), "%s.bsp", level.rawmapname );
			Com_sprintf( modelOut.mModelType, sizeof( modelOut.mModelType ), "bsp" );
			modelOut.mDataBuffer = mapBuffer;
			modelOut.mDataBufferSize = fileSize;
			return InvalidParameter;
		}
		return Success;
	}

	obResult GetWorldModel( GameModelInfo & modelOut, MemoryAllocator & alloc )
	{
		Com_sprintf( modelOut.mModelName, sizeof( modelOut.mModelName ), "maps/%s.bsp", GetMapName() );
		Com_sprintf( modelOut.mModelType, sizeof( modelOut.mModelName ), "bsp" );
		return GetModel( modelOut, alloc );
	}

	obResult GetEntityModel( const GameEntity _ent, GameModelInfo & modelOut, MemoryAllocator & alloc )
	{
		gentity_t *pEnt = EntityFromHandle( _ent );
		if ( pEnt )
		{
			if ( pEnt->s.eType == ET_GAMEMODEL )
			{
				// only this type can be scaled?
				modelOut.mScale[ 0 ] = pEnt->s.angles2[ 0 ];
				modelOut.mScale[ 1 ] = pEnt->s.angles2[ 1 ];
				modelOut.mScale[ 2 ] = pEnt->s.angles2[ 2 ];
			}

			if ( G_FindConfigstringByIndex( pEnt->s.modelindex, CS_MODELS, MAX_MODELS, modelOut.mModelName, sizeof( modelOut.mModelName ) ) > 0 )
			{
				const int len = strlen( modelOut.mModelName );
				for ( int i = 0; i < len; ++i )
				{
					if ( modelOut.mModelName[ i ] == '.' )
						strncpy( modelOut.mModelType, &modelOut.mModelName[ i + 1 ], sizeof( modelOut.mModelName ) );
				}

				return GetModel( modelOut, alloc );
			}
		}
		return InvalidEntity;
	}

	obResult GetCurrentWeaponClip( const GameEntity _ent, FireMode _mode, int &_curclip, int &_maxclip ) {
		gentity_t *bot = EntityFromHandle( _ent );
		if ( bot && bot->inuse && bot->client ) {
			int iWeapon = bot->client->ps.weapon;

			_curclip = bot->client->ps.ammoclip[BG_FindClipForWeapon( (weapon_t)iWeapon )];

			// sanity check for non-clipped weapons
			switch ( iWeapon )
			{
			case WP_AMMO:
			case WP_BINOCULARS:
			case WP_DYNAMITE:
			case WP_FLAMETHROWER:
			case WP_GRENADE_PINEAPPLE:
			case WP_KNIFE:
			case WP_MEDKIT:
			case WP_PANZERFAUST:
			case WP_PLIERS:
			case WP_MEDIC_SYRINGE:
			case WP_SMOKE_GRENADE:
			case WP_GRENADE_LAUNCHER:
				_maxclip = 0;
				break;
			case WP_VENOM:
				// ugly hack to keep them from reloading until low. should be a weapon property.
				if (_curclip > 75) {
					_maxclip = _curclip;
				}
				break;
			default:
				_maxclip = GetAmmoTableData( iWeapon )->maxclip;
			}
			return Success;
		}
		return InvalidEntity;
	}

	int GetCurrentWeapons( const GameEntity ent, int weaponIds [], int maxWeapons )
	{
		gentity_t *bot = EntityFromHandle( ent );
		if ( bot && bot->inuse && bot->client )
		{
			int weaponCount = 0;

			for ( int i = 0; i < WP_NUM_WEAPONS; i++ )
			{
				if ( COM_BitCheck( bot->client->ps.weapons, i ) )
				{
					weaponIds[ weaponCount ] = Bot_WeaponGameToBot( i );
					if ( weaponIds[ weaponCount ] != RTCW_WP_NONE )
						++weaponCount;
				}
			}
			return weaponCount;
		}
		return 0;
	}

	obResult GetCurrentAmmo( const GameEntity _ent, int _weaponId, FireMode _mode, int &_cur, int &_max ) {
		gentity_t *bot = EntityFromHandle( _ent );
		if ( bot && bot->inuse && bot->client ) {
			int maxclip = 0;
			int ammoIndex = 0;

			_weaponId = _weaponBotToGame( _weaponId );
			ammoIndex = BG_FindAmmoForWeapon( (weapon_t)_weaponId );
			_cur = bot->client->ps.ammoclip[BG_FindClipForWeapon( (weapon_t)_weaponId )] +
				bot->client->ps.ammo[ammoIndex];

			// sanity check for non-clipped weapons
			switch ( _weaponId )
			{
			case WP_AMMO:
			case WP_BINOCULARS:
			case WP_DYNAMITE:
			case WP_FLAMETHROWER:
			case WP_GRENADE_PINEAPPLE:
			case WP_GRENADE_LAUNCHER:
			case WP_KNIFE:
			case WP_MEDKIT:
			case WP_PANZERFAUST:
			case WP_PLIERS:
			case WP_MEDIC_SYRINGE:
			case WP_SMOKE_GRENADE:
				maxclip = 0;
				break;
			default:
				maxclip = GetAmmoTableData( ammoIndex )->maxclip;
			}

			_max = maxclip * GetAmmoTableData( ammoIndex )->numClips;

			return Success;
		}
		return InvalidEntity;
	}

	int GetGameTime() {
		return level.time;
	}

	void GetGoals() {
		g_GoalSubmitReady = true;

		SendDeferredGoals();
		for ( int i = MAX_CLIENTS; i < level.num_entities; i++ )
		{
			gentity_t *e = &g_entities[i];
			const char *pGoalName = 0;

			if ( !e->inuse ) {
				continue;
			}

			pGoalName = _GetEntityName( e );
			switch ( e->s.eType )
			{
			case ET_GENERAL:
				{
					if ( !Q_stricmp( e->classname, "team_CTF_redflag" ) ) {
						// allies flag
						Bot_Util_AddGoal( "flag",e,( 1 << RTCW_TEAM_ALLIES ),pGoalName );
					} else if ( !Q_stricmp( e->classname, "team_CTF_blueflag" ) )        {
						// axis flag
						Bot_Util_AddGoal( "flag",e,( 1 << RTCW_TEAM_AXIS ),pGoalName );
					}
					break;
				}
			case ET_OID_TRIGGER:
				{
					//geekfeststarter's code for compat with ET goalnames
					//gentity_t *trav = NULL;

					//trav = G_Find( trav, FOFS(targetname), e->target );
					//pGoalName =  _GetEntityName(trav);

					pGoalName =  _GetEntityName( e );
					if ( e->spawnflags & ALLIED_OBJECTIVE ) {
						Bot_Util_AddGoal( "plant",e,( 1 << RTCW_TEAM_AXIS ),pGoalName );
					}
					if ( e->spawnflags & AXIS_OBJECTIVE ) {
						Bot_Util_AddGoal( "plant",e,( 1 << RTCW_TEAM_ALLIES ),pGoalName );
					}
					break;
				}
				/*case ET_MG42_BARREL:
				{
				int team = 0;
				team |= (1 << RTCW_TEAM_ALLIES);
				team |= (1 << RTCW_TEAM_AXIS);
				pGoalName = _GetEntityName(e);
				Bot_Util_AddGoal(e, RTCW_GOAL_MG42MOUNT, team, pGoalName, NULL);
				break;
				}*/
			case ET_MOVER:
				{
					int team = 0;
					if ( e->spawnflags & 32 ) {
						team = ( 1 << RTCW_TEAM_ALLIES );
					} else if ( e->spawnflags & 64 ) {
						team = ( 1 << RTCW_TEAM_AXIS );
					} else
					{
						team |= ( 1 << RTCW_TEAM_ALLIES );
						team |= ( 1 << RTCW_TEAM_AXIS );
					}
					pGoalName = _GetEntityName( e );
					if ( pGoalName && !Q_stricmp( e->classname, "script_mover" ) ) {
						Bot_Util_AddGoal( "mover",e,team,pGoalName );
					}
					break;
				}
			case ET_TRAP:
				{
					pGoalName = _GetEntityName( e );
					if ( !Q_stricmp( e->classname, "team_WOLF_checkpoint" ) ) {
						Bot_Util_AddGoal( "checkpoint",e,0,pGoalName );
					}
					break;
				}
			case ET_EXPLOSIVE:
				{
					pGoalName = _GetEntityName( e );
					if ( !Q_stricmp( e->classname, "func_explosive" ) ) {
						if ( e->spawnflags & 32 || ( e->spawnflags & 16 && e->health > 50 ) ) {
							Bot_Util_AddGoal( "explode",e,0,pGoalName );
						}

						// bridge in mp_tank has 2 extra dynamite actions which both teams
						// can use. they are not tied to an OID trigger .. bleh
						if ( e->spawnflags == 77 ||  e->spawnflags == 104 || ( e->spawnflags == 76 && !e->scriptName ) ) {
							Bot_Util_AddGoal( "plant",e,0,pGoalName );
						}
					}
					break;
				}
			}
		}
	}

	void GetPlayerInfo( obPlayerInfo &info ) {
		info.m_AvailableTeams |= ( 1 << RTCW_TEAM_ALLIES );
		info.m_AvailableTeams |= ( 1 << RTCW_TEAM_AXIS );

		info.m_MaxPlayers = level.maxclients;

		for ( int i = 0; i < MAX_CLIENTS; i++ )
		{
			if ( !g_entities[i].inuse ) {
				continue;
			}
			if ( !g_entities[i].client ) {
				continue;
			}
			if ( g_entities[i].client->pers.connected != CON_CONNECTED ) {
				continue;
			}

			GameEntity ge = HandleFromEntity( &g_entities[i] );

			info.m_Players[i].m_Team = GetEntityTeam( ge );
			info.m_Players[i].m_Class = GetEntityClass( ge );
			info.m_Players[i].m_Controller = IsBot( &g_entities[i] ) ?
				obPlayerInfo::Bot : obPlayerInfo::Human;
		}
	}

	obResult InterfaceSendMessage( const MessageHelper &_data, const GameEntity _ent ) {
		gentity_t *pEnt = EntityFromHandle( _ent );

		switch ( _data.GetMessageId() )
		{
			// general messages
		case GEN_MSG_GETGAMETYPE:
			{
				OB_GETMSG( Event_GameType );
				if ( pMsg ) {
					pMsg->m_GameType = g_gametype.integer;
				}
				break;
			}
		case GEN_MSG_SETCVAR:
			{
				OB_GETMSG( RTCW_CvarSet );
				if ( pMsg ) {
					trap_Cvar_Set( pMsg->m_Cvar, pMsg->m_Value );
				}
				break;
			}
		case GEN_MSG_GETCVAR:
			{
				OB_GETMSG( RTCW_CvarGet );
				if ( pMsg ) {
					pMsg->m_Value =
						trap_Cvar_VariableIntegerValue( pMsg->m_Cvar );
				}
				break;
			}
		case GEN_MSG_ISALIVE:
			{
				OB_GETMSG( Msg_IsAlive );
				if ( pMsg ) {
					if ( pEnt && pEnt->inuse && pEnt->client && pEnt->health > 0 &&
						pEnt->client->ps.pm_type == PM_NORMAL ) {
							pMsg->m_IsAlive = True;
					}
				}
				break;
			}
		case GEN_MSG_ISRELOADING:
			{
				OB_GETMSG( Msg_Reloading );
				if ( pMsg ) {
					if ( ( pEnt && pEnt->inuse && pEnt->client && pEnt->client->ps.weaponstate >= WEAPON_RAISING ) &&
						( pEnt->client->ps.weaponstate <= WEAPON_RELOADING ) ) {
							pMsg->m_Reloading = True;
					}
				}
				break;
			}
		case GEN_MSG_ISREADYTOFIRE:
			{
				OB_GETMSG( Msg_ReadyToFire );
				if ( pMsg ) {
					if ( pEnt && pEnt->inuse && pEnt->client ) {
						if ( pEnt->client->ps.weaponstate == WEAPON_READY || pEnt->client->ps.weaponstate == WEAPON_FIRING ) {
							pMsg->m_Ready = True;
						}
					}
				}
				break;
			}
		case GEN_MSG_GETEQUIPPEDWEAPON:
			{
				OB_GETMSG( WeaponStatus );
				if ( pMsg ) {
					if ( pEnt && pEnt->inuse && pEnt->client ) {
						pMsg->m_WeaponId = Bot_WeaponGameToBot( pEnt->client->ps.weapon );
					} else {
						pMsg->m_WeaponId = 0;
					}
				}
				break;
			}
		case GEN_MSG_GETMOUNTEDWEAPON:
			{
				OB_GETMSG( WeaponStatus );
				if ( pMsg && pEnt && pEnt->inuse && pEnt->client ) {
					pMsg->m_WeaponId = BG_PlayerMounted( pEnt->s.eFlags ) ? RTCW_WP_MOUNTABLE_MG42 : RTCW_WP_NONE;
				}
				break;
			}
		case GEN_MSG_GETWEAPONLIMITS:
			{
				OB_GETMSG( WeaponLimits );
				if ( pMsg ) {
					if ( pEnt && pEnt->inuse && pEnt->client &&
						pMsg->m_WeaponId == RTCW_WP_MOUNTABLE_MG42 &&
						BG_PlayerMounted( pEnt->client->ps.eFlags ) ) {
							pMsg->m_Limited = True;
							AngleVectors( pEnt->client->pmext.centerangles, pMsg->m_CenterFacing, NULL, NULL );

							pMsg->m_MinYaw = -pEnt->client->pmext.harc;
							pMsg->m_MaxYaw = pEnt->client->pmext.harc;
							pMsg->m_MinPitch = -pEnt->client->pmext.varc;
							pMsg->m_MaxPitch = pEnt->client->pmext.varc;
					} else {
						pMsg->m_Limited = False;
					}
				}
				//////////////////////////////////////////////////////////////////////////
				break;
			}
		case GEN_MSG_GETHEALTHARMOR:
			{
				OB_GETMSG( Msg_HealthArmor );
				if ( pMsg ) {
					// No Armor in ET
					pMsg->m_CurrentArmor = pMsg->m_MaxArmor = 0;
					if ( pEnt ) {
						switch ( pEnt->s.eType )
						{
						case ET_MG42_BARREL:
							if ( Simple_EmplacedGunIsRepairable( pEnt ) ) {
								pMsg->m_CurrentHealth = 0;
								pMsg->m_MaxHealth = MG42_MULTIPLAYER_HEALTH;
							} else {
								if ( pEnt->mg42BaseEnt > 0 ) {
									gentity_t *pBase = &g_entities[pEnt->mg42BaseEnt];
									pMsg->m_CurrentHealth = pBase->health;
									pMsg->m_MaxHealth = MG42_MULTIPLAYER_HEALTH;
								} else {
									// just in case
									pMsg->m_CurrentHealth = pEnt->health;
									pMsg->m_MaxHealth = MG42_MULTIPLAYER_HEALTH;
								}
							}
							break;
						default:
							if ( pEnt->client ) {
								pMsg->m_CurrentHealth = pEnt->client->ps.stats[STAT_HEALTH];
								pMsg->m_MaxHealth = pEnt->client->ps.stats[STAT_MAX_HEALTH];
							} else
							{
								pMsg->m_CurrentHealth = pEnt->takedamage ? pEnt->health : 0;
								pMsg->m_MaxHealth = 0;
							}
						}
					}
				}
				break;
			}
		case GEN_MSG_GETMAXSPEED:
			{
				OB_GETMSG( Msg_PlayerMaxSpeed );
				if ( pMsg && pEnt ) {
					pMsg->m_MaxSpeed = (float)g_speed.integer;
				}
				break;
			}
		case GEN_MSG_ISALLIED:
			{
				OB_GETMSG( Msg_IsAllied );
				if ( pMsg ) {
					gentity_t *pEntOther = EntityFromHandle( pMsg->m_TargetEntity );
					if ( pEntOther && pEnt ) {
						if ( ENTINDEX( pEntOther ) == ENTITYNUM_WORLD || ( !_GetEntityTeam( pEntOther )
							&& !Q_stricmp( pEntOther->classname, "player" ) ) ) {
								pMsg->m_IsAllied = True;
						} else
						{
							pMsg->m_IsAllied = _GetEntityTeam( pEnt ) == _GetEntityTeam( pEntOther ) ? True : False;
						}
					}
				}
				break;
			}
		case GEN_MSG_ISOUTSIDE:
			{
				OB_GETMSG( Msg_IsOutside );
				if ( pMsg ) {
					trace_t tr;
					vec3_t end;
					end[0] = pMsg->m_Position[0];
					end[1] = pMsg->m_Position[1];
					end[2] = pMsg->m_Position[2] + 4096;
					trap_Trace( &tr, pMsg->m_Position, NULL, NULL, end, -1, MASK_SOLID );

					if ( ( tr.fraction < 1.0 ) && !( tr.surfaceFlags & SURF_NOIMPACT ) ) {
						pMsg->m_IsOutside = False;
					} else {
						pMsg->m_IsOutside = True;
					}
				}
				break;
			}
		case GEN_MSG_CHANGENAME:
			{
				OB_GETMSG( Msg_ChangeName );
				if ( pMsg && pEnt && pEnt->client ) {
					char userinfo[MAX_INFO_STRING];
					trap_GetUserinfo( pEnt - g_entities, userinfo, MAX_INFO_STRING );
					Info_SetValueForKey( userinfo, "name", pMsg->m_NewName );
					trap_SetUserinfo( pEnt - g_entities, userinfo );
					ClientUserinfoChanged( pEnt - g_entities );
				}
				break;
			}
		case GEN_MSG_ENTITYKILL:
			{
				OB_GETMSG( Msg_KillEntity );
				if ( pMsg && pMsg->m_WhoToKill.IsValid() /*&& g_cheats.integer*/ ) {
					gentity_t *pWho = EntityFromHandle( pMsg->m_WhoToKill );
					if ( pWho ) {
						if ( pWho->s.eType == ET_EXPLOSIVE ) {
							func_explosive_explode(pWho, 0, 0, 1000, MOD_GRENADE);
						}
						else if (pWho->spawnflags & AXIS_OBJECTIVE || pWho->spawnflags & AXIS_OBJECTIVE) { // trigger_objective_info's
							G_UseTargets( pWho,  NULL );
							pWho->think = G_FreeEntity;
							pWho->nextthink = level.time + FRAMETIME;

							G_Script_ScriptEvent( pWho, "destroyed", "" );
						}
						else if ( pWho->takedamage ) {
							G_Damage( pWho, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, MOD_TELEFRAG );
						}
						else if (!pWho->neverFree ) {
							G_FreeEntity(pWho);
						}
						else {
							G_Printf("^1EntityKill failed for %d\n", pWho->s.number);
						}
					}
				}
				break;
			}
		case GEN_MSG_SERVERCOMMAND:
			{
				OB_GETMSG( Msg_ServerCommand );
				if ( pMsg && pMsg->m_Command[0] /*&& g_cheats.integer*/ ) {
					trap_SendConsoleCommand( EXEC_NOW, pMsg->m_Command );
				}
				break;
			}
		case GEN_MSG_GETFLAGSTATE:
			{
				OB_GETMSG( Msg_FlagState );
				if ( pMsg ) {
					/* CS: flagreturn position is not updated while its carried.
					if(pEnt)
					{
					if(!(pEnt->flags & FL_DROPPED_ITEM) && !(pEnt->flags & FL_NODRAW))
					pMsg->m_FlagState = S_FLAG_AT_BASE;
					else if(pEnt->flags & FL_NODRAW)
					pMsg->m_FlagState = S_FLAG_CARRIED;
					else if(pEnt->flags & FL_DROPPED_ITEM)
					pMsg->m_FlagState = S_FLAG_DROPPED;
					}
					*/
				}
				break;
			}
		case GEN_MSG_GETCONTROLLINGTEAM:
			{
				ControllingTeam *pMsg = _data.Get<ControllingTeam>();
				if ( pMsg ) {
					if ( pEnt && pEnt->s.eType == ET_TRAP ) {
						pMsg->m_ControllingTeam = Bot_TeamGameToBot( pEnt->count );
					}
				}
				break;
			}
		case GEN_MSG_GAMESTATE:
			{
				OB_GETMSG( Msg_GameState );
				if ( pMsg ) {
					//if (level.framenum < GAME_INIT_FRAMES)
					//{
					//	pMsg->m_GameState = GAME_STATE_INVALID;
					//	break;
					//}

					int iTimeLimit = (int)( g_timelimit.value * 60000 );
					int iMatchTime = level.time - level.startTime;
					pMsg->m_TimeLeft = ( iTimeLimit - iMatchTime ) / 1000.f;
					switch ( g_gamestate.integer )
					{
					case GS_PLAYING:
						pMsg->m_GameState = GAME_STATE_PLAYING;
						break;
					case GS_WARMUP_COUNTDOWN:
						pMsg->m_GameState = GAME_STATE_WARMUP_COUNTDOWN;
						break;
					case GS_WARMUP:
						pMsg->m_GameState = GAME_STATE_WARMUP;
						break;
					case GS_INTERMISSION:
						pMsg->m_GameState = GAME_STATE_INTERMISSION;
						break;
					case GS_WAITING_FOR_PLAYERS:
						pMsg->m_GameState = GAME_STATE_WAITINGFORPLAYERS;
						break;
					default:
						pMsg->m_GameState = GAME_STATE_INVALID;
						break;
					}
				}
				break;
			}
		case GEN_MSG_ENTITYSTAT:
			{
				OB_GETMSG( Msg_EntityStat );
				if ( pMsg && pEnt && pEnt->client ) {
					if ( !strcmp( pMsg->m_StatName, "kills" ) ) {
						pMsg->m_Result = obUserData( pEnt->client->pers.kills );
					} else if ( !strcmp( pMsg->m_StatName, "deaths" ) ) {
						pMsg->m_Result = obUserData( pEnt->client->pers.deaths );
					} else if ( !strcmp( pMsg->m_StatName, "gibs" ) ) {
						pMsg->m_Result = obUserData( pEnt->client->pers.gibs );
					} else if ( !strcmp( pMsg->m_StatName, "shots" ) ) {
						pMsg->m_Result = obUserData( pEnt->client->pers.acc_shots );
					} else if ( !strcmp( pMsg->m_StatName, "hits" ) ) {
						pMsg->m_Result = obUserData( pEnt->client->pers.acc_hits );
					} else if ( !strcmp( pMsg->m_StatName, "headshots" ) ) {
						pMsg->m_Result = obUserData( pEnt->client->pers.headshots );
					} else if ( !strcmp( pMsg->m_StatName, "ammo" ) ) {
						pMsg->m_Result = obUserData( pEnt->client->pers.ammoPacks );
					} else if ( !strcmp( pMsg->m_StatName, "health" ) ) {
						pMsg->m_Result = obUserData( pEnt->client->pers.medPacks );
					} else if ( !strcmp( pMsg->m_StatName, "accuracy" ) ) {
						float acc = 0.0f;
						if ( pEnt->client->pers.acc_shots > 0 ) {
							acc = ( (float)pEnt->client->pers.acc_hits / (float)pEnt->client->pers.acc_shots ) * 100.0f;
						}
						pMsg->m_Result = obUserData( acc );
					} else if ( !strcmp( pMsg->m_StatName, "hspercent" ) )        {
						float p = 0.0f;
						if ( pEnt->client->pers.headshots > 0 ) {
							p = ( (float)pEnt->client->pers.headshots / (float)pEnt->client->pers.acc_hits ) * 100.0f;
						}
						pMsg->m_Result = obUserData( p );
					} else if ( !strcmp( pMsg->m_StatName, "killratio" ) )        {
						float kr = pEnt->client->pers.kills;
						if ( pEnt->client->pers.deaths > 0 ) {
							kr = (float)pEnt->client->pers.kills / (float)pEnt->client->pers.deaths;
						}
						pMsg->m_Result = obUserData( kr );
					} else if ( !strcmp( pMsg->m_StatName, "respawns_left" ) ) {
						pMsg->m_Result = obUserData( pEnt->client->ps.persistant[PERS_RESPAWNS_LEFT] );
					}
					//else if(pEnt && pEnt->client && !Q_strcmp(pMsg->m_StatName, "score"))
					//	pMsg->m_Result = obUserData(0); // TODO:
				}
				break;
			}
		case GEN_MSG_TEAMSTAT:
			{
				OB_GETMSG( Msg_TeamStat );
				if ( pMsg ) {
					/*if(!strcmp(pMsg->m_StatName, "score"))
					pMsg->m_Result = obUserData(0);
					else if(!strcmp(pMsg->m_StatName, "deaths"))
					pMsg->m_Result = obUserData(0);*/
				}
				break;
			}
		case GEN_MSG_WPCHARGED:
			{
				OB_GETMSG( WeaponCharged );
				if ( pMsg && pEnt && pEnt->inuse && pEnt->client ) {
					pMsg->m_IsCharged =
						( weaponCharged( &pEnt->client->ps,pEnt->client->sess.sessionTeam,
						_weaponBotToGame( pMsg->m_Weapon ) ) ? True : False );
				}
				break;
			}
		case GEN_MSG_WPHEATLEVEL:
			{
				OB_GETMSG( WeaponHeatLevel );
				if ( pMsg && pEnt && pEnt->inuse && pEnt->client ) {
					pMsg->m_CurrentHeat = pEnt->client->ps.weapHeat[WP_STEN];
					pMsg->m_MaxHeat = MAX_WP_HEAT;
				}
				break;
			}
		case GEN_MSG_MOVERAT:
			{
				OB_GETMSG( Msg_MoverAt );
				if ( pMsg ) {
					/*Vector org(
					pMsg->m_Position[0],
					pMsg->m_Position[1],
					pMsg->m_Position[2]);
					Vector under(
					pMsg->m_Under[0],
					pMsg->m_Under[1],
					pMsg->m_Under[2]);

					trace_t tr;
					UTIL_TraceLine(org, under, MASK_SOLID, NULL, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);

					if(tr.DidHitNonWorldEntity() && !tr.m_pEnt->IsPlayer())
					{
					pMsg->m_Entity = HandleFromEntity(tr.m_pEnt);
					}*/
				}
				break;
			}
			//////////////////////////////////////////////////////////////////////////
		case RTCW_MSG_GOTOLIMBO:
			{
				OB_GETMSG( RTCW_GoLimbo );
				if ( pMsg ) {
					// Dens: don't forget to look if the pEnt SHOULD really be in limbo
					// this check prevenst extra bodies and weird spectator behaviour
					int limbo_health = GIB_HEALTH;
					if ( pEnt && pEnt->inuse && pEnt->client && pEnt->health > limbo_health &&
						pEnt->client->ps.pm_type == PM_DEAD && !( pEnt->client->ps.pm_flags & PMF_LIMBO ) ) {
							Cmd_BotTapOut_f( pEnt );
							pMsg->m_GoLimbo = True;
					} else
					{
						pMsg->m_GoLimbo = False;
					}
				}
				break;
			}
		case RTCW_MSG_ISMEDICNEAR:
			{
				OB_GETMSG( RTCW_MedicNear );
				if ( pMsg ) {
					if ( pEnt && pEnt->inuse && pEnt->client &&
						pEnt->client->ps.pm_type == PM_DEAD && !( pEnt->client->ps.pm_flags & PMF_LIMBO ) ) {
							pMsg->m_MedicNear = ( pEnt->client->ps.viewlocked == 7 ) && g_gamestate.integer == GS_PLAYING ? True : False;
					} else
					{
						pMsg->m_MedicNear = Invalid;
					}
				}
				break;
			}
		case RTCW_MSG_REINFORCETIME:
			{
				OB_GETMSG( RTCW_ReinforceTime );
				if ( pMsg ) {
					if ( g_gamestate.integer == GS_WARMUP || g_gamestate.integer == GS_WARMUP_COUNTDOWN ) {
						pMsg->m_ReinforceTime = 1;
					} else if ( pEnt && pEnt->inuse && pEnt->client ) {
						if ( pEnt->client->sess.sessionTeam == TEAM_RED ) {
							pMsg->m_ReinforceTime = g_redlimbotime.integer -
								( ( level.dwRedReinfOffset + level.time - level.startTime ) %
								g_redlimbotime.integer );
						} else if ( pEnt->client->sess.sessionTeam == TEAM_BLUE ) {
							pMsg->m_ReinforceTime = g_bluelimbotime.integer -
								( ( level.dwBlueReinfOffset + level.time - level.startTime ) %
								g_bluelimbotime.integer );
						}
					}
				}
				break;
			}
		case RTCW_MSG_GETGUNHEALTH:
			{
				OB_GETMSG( RTCW_MG42Health );
				if ( pMsg ) {
					gentity_t *pGunEntity = EntityFromHandle( pMsg->m_MG42Entity );
					if ( pGunEntity && pGunEntity->inuse && pGunEntity->r.linked &&
						pGunEntity->entstate == STATE_DEFAULT ) {
							if ( pGunEntity->mg42BaseEnt != -1 ) {
								pMsg->m_Health = g_entities[pGunEntity->mg42BaseEnt].health;
							} else {
								pMsg->m_Health = pGunEntity->health;
							}
					} else
					{
						pMsg->m_Health = -1;
					}
				}
				break;
			}
		case RTCW_MSG_GETGUNHEAT:
			{
				OB_GETMSG( RTCW_WeaponHeatLevel );
				if ( pMsg ) {
					gentity_t *pGunEntity = EntityFromHandle( pMsg->m_Entity );
					if ( pEnt && pEnt->client && pGunEntity ) {
						pMsg->m_Current = pEnt->client->ps.weapHeat[WP_STEN];
						pMsg->m_Max = MAX_WP_HEAT;
					} else
					{
						pMsg->m_Current = -1;
						pMsg->m_Max = -1;
					}
				}
				break;
			}
		case RTCW_MSG_ISGUNMOUNTED:
			{
				OB_GETMSG( RTCW_MG42MountedPlayer );
				if ( pMsg ) {
					gentity_t *pGunEntity = EntityFromHandle( pMsg->m_MG42Entity );
					if ( pGunEntity && pGunEntity->inuse && ( pGunEntity->r.ownerNum < level.maxclients ) ) {
						pMsg->m_MountedEntity = HandleFromEntity( &g_entities[pGunEntity->r.ownerNum] );
					} else {
						pMsg->m_MountedEntity.Reset();
					}
				}
				break;
			}
		case RTCW_MSG_ISGUNREPAIRABLE:
			{
				OB_GETMSG( RTCW_MG42MountedRepairable );
				if ( pMsg ) {
					gentity_t *pGunEntity = EntityFromHandle( pMsg->m_MG42Entity );
					if ( pEnt && pGunEntity && pGunEntity->inuse ) {
						pMsg->m_Repairable = G_EmplacedGunIsRepairable( pGunEntity, pEnt ) == qtrue ? True : False;
					} else {
						pMsg->m_Repairable = False;
					}
				}
				break;
			}
		case RTCW_MSG_MOUNTEDMG42INFO:
			{
				OB_GETMSG( RTCW_MG42Info );
				if ( pMsg ) {
					if ( pEnt && pEnt->inuse && pEnt->client && BG_PlayerMounted( pEnt->client->ps.eFlags ) ) {
						AngleVectors( pEnt->client->pmext.centerangles, pMsg->m_CenterFacing, NULL, NULL );
						//AngleVectors(pEnt->client->pmext.mountedWeaponAngles, pMsg->, NULL, NULL);

						pMsg->m_MinHorizontalArc = -pEnt->client->pmext.harc;
						pMsg->m_MaxHorizontalArc = pEnt->client->pmext.harc;
						pMsg->m_MinVerticalArc = -pEnt->client->pmext.varc;
						pMsg->m_MaxVerticalArc = pEnt->client->pmext.varc;
					}
				}
				break;
			}
		case RTCW_MSG_WPOVERHEATED:
			{
				OB_GETMSG( RTCW_WeaponOverheated );
				if ( pMsg ) {
					if ( pEnt && pEnt->inuse && pEnt->client ) {
						int iCurHeat = pEnt->client->ps.weapHeat[_weaponBotToGame( pMsg->m_Weapon )];
						int iMaxHeat = GetAmmoTableData( _weaponBotToGame( pMsg->m_Weapon ) )->maxHeat;
						pMsg->m_IsOverheated = iMaxHeat ? ( ( iCurHeat >= iMaxHeat ) ? True : False ) : False;
					}
				}
				break;
			}
		case RTCW_MSG_PICKWEAPON:
		case RTCW_MSG_PICKWEAPON2:
			{
				OB_GETMSG( RTCW_SelectWeapon );
				if ( pMsg ) {
					if ( pEnt && pEnt->inuse && pEnt->client ) {
						if ( _data.GetMessageId() == RTCW_MSG_PICKWEAPON2 ) {
							pEnt->client->sess.playerWeapon2 = _weaponBotToGame( pMsg->m_Selection );
							pEnt->client->sess.latchPlayerWeapon2 = _weaponBotToGame( pMsg->m_Selection );
						} else {
							//CS: /kill 2 seconds before next spawn
							if ( !( pEnt->client->ps.pm_flags & PMF_LIMBO ) && pEnt->client->sess.playerWeapon != _weaponBotToMpWeapon( pMsg->m_Selection ) ) {
								pEnt->client->sess.botSuicide = qtrue;
							}

							//pEnt->client->sess.playerWeapon = _weaponBotToGame(pMsg->m_Selection);
							pEnt->client->sess.playerWeapon = _weaponBotToMpWeapon( pMsg->m_Selection );
							pEnt->client->sess.latchPlayerWeapon = _weaponBotToMpWeapon( pMsg->m_Selection );
						}
						pMsg->m_Good = True;
					} else {
						pMsg->m_Good = False;
					}
				}
				break;
			}
		case RTCW_MSG_GETHINT:
			{
				OB_GETMSG( RTCW_CursorHint );
				if ( pMsg ) {
					if ( pEnt && pEnt->client ) {
						pMsg->m_Type = Bot_HintGameToBot( pEnt );
						pMsg->m_Value = pEnt->client->ps.serverCursorHintVal;
					}
				}
				break;
			}
		case RTCW_MSG_CHANGESPAWNPOINT:
			{
				OB_GETMSG( RTCW_SpawnPoint );
				if ( pMsg ) {
					if ( pEnt && pEnt->client ) {
						pEnt->client->sess.spawnObjectiveIndex = pMsg->m_SpawnPoint;
					}
				}
				break;
			}
		case RTCW_MSG_GETSPAWNPOINT:
			{
				OB_GETMSG( RTCW_GetSpawnPoint );
				if ( pMsg ) {
					if ( pEnt && pEnt->client ) {
						pMsg->m_SpawnPoint = pEnt->client->sess.spawnObjectiveIndex;
					}
				}
				break;
			}
		case RTCW_MSG_GHASFLAG:
			{
				OB_GETMSG( RTCW_HasFlag );
				if ( pMsg ) {
					if ( pEnt && pEnt->inuse && pEnt->client && ( pEnt->health >= 0 ) ) {
						if ( pEnt->client->ps.powerups[PW_REDFLAG] || pEnt->client->ps.powerups[PW_BLUEFLAG] ) {
							pMsg->m_HasFlag = True;
						}
					}
				}
				break;
			}
		case RTCW_MSG_GDYNDESTROYABLE:
			{
				OB_GETMSG( RTCW_Destroyable );
				if ( pMsg ) {
					gentity_t *pDestroyable = EntityFromHandle( pMsg->m_Entity );
					if ( pEnt && pEnt->inuse && pDestroyable && pDestroyable->inuse ) {
						if ( pDestroyable->s.eType == ET_OID_TRIGGER
							|| pDestroyable->spawnflags == 77 ||  pDestroyable->spawnflags == 104
							|| ( pDestroyable->spawnflags == 76 && !pDestroyable->scriptName ) )
						{
							pMsg->m_State = CONST_DESTROYABLE;
						}
					}
				}
				break;
			}
		case RTCW_MSG_GEXPLOSIVESTATE:
			{
				OB_GETMSG( RTCW_ExplosiveState );
				if ( pMsg ) {
					gentity_t *pExplo = EntityFromHandle( pMsg->m_Explosive );
					if ( pExplo && pExplo->inuse ) {
						if ( pExplo->s.eType == ET_MISSILE ) {
							pMsg->m_State = ( pExplo->s.teamNum < 4 ) ? XPLO_ARMED : XPLO_UNARMED;
						}
					}
				}
				break;
			}
		case RTCW_MSG_GCANBEGRABBED:
			{
				OB_GETMSG( RTCW_CanBeGrabbed );
				if ( pMsg ) {
					//cs: testing flagstate
					//pMsg->m_CanBeGrabbed = True;

					gentity_t *pFlagEnt = EntityFromHandle( pMsg->m_Entity );
					if ( pEnt && pEnt->client && pFlagEnt ) {
						// DUPLICATE ERROR CHECK, so BG_CanItemBeGrabbed doesn't screw up.
						if ( pFlagEnt->s.modelindex < 1 || pFlagEnt->s.modelindex >= bg_numItems ) {
							//Com_Error( ERR_DROP, "BG_CanItemBeGrabbed: index out of range" );
							pMsg->m_CanBeGrabbed = Invalid;
						} else
						{
							pMsg->m_CanBeGrabbed =
								BG_CanItemBeGrabbed( &pFlagEnt->s, &pEnt->client->ps ) ? True : False;
						}
					}
				}
				break;
			}
		case RTCW_MSG_SENDPM:
			{
				OB_GETMSG( RTCW_SendPM );
				if ( pMsg ) {
					if ( pEnt && pEnt->client ) {
						G_PrivateMessage( pEnt, pMsg->m_TargetName, pMsg->m_Message, qtrue );
					}
				}
				break;
			}
		case RTCW_MSG_SETSUICIDE:
			{
				OB_GETMSG( RTCW_SetSuicide );
				if ( pMsg ) {
					if ( pEnt && pEnt->client ) {
						if ( pMsg->m_Suicide > 0 ) {
							pEnt->client->sess.botSuicide = qtrue;
							if ( pMsg->m_Persist > 0 ) {
								pEnt->client->sess.botSuicidePersist = qtrue;
							}
						} else
						{
							pEnt->client->sess.botSuicide = qfalse;
							pEnt->client->sess.botSuicidePersist = qfalse;
						}
					}
				}
				break;
			}
		case RTCW_MSG_DISABLEBOTPUSH:
			{
				OB_GETMSG( RTCW_DisableBotPush );
				if ( pMsg ) {
					if ( pEnt && pEnt->client ) {
						if ( pMsg->m_Push > 0 ) {
							pEnt->client->sess.botPush = qfalse;
						} else
						{
							pEnt->client->sess.botPush = qtrue;
						}
					}
				}
				break;
			}
		case RTCW_MSG_GETPLAYERCLASS:
			{
				OB_GETMSG( RTCW_GetPlayerClass );
				if ( pMsg ) {
					gentity_t *pPlayer = EntityFromHandle( pMsg->m_Entity );
					int t = pPlayer->s.eType;
					if ( pPlayer->client && ( pPlayer - g_entities ) < MAX_CLIENTS ) {
						t = ET_PLAYER;
					}

					if ( t == ET_PLAYER ) {
						pMsg->m_PlayerClass = Bot_PlayerClassGameToBot( pEnt->client->sess.latchPlayerType );
					}
				}
				break;
			}
		default:
			{
				return UnknownMessageType;
			}
		}
		return Success;
	}

	void PrintError( const char *_error ) {
		if ( _error ) {
			G_Printf( "%s%s\n", S_COLOR_RED, _error );
		}
	}

	void PrintMessage( const char *_msg ) {
		if ( _msg ) {
			// et console doesn't support tabs, so
			const int BufferSize = 1024;
			char tmpbuffer[BufferSize] = {};
			const char *src = _msg;
			char *dest = tmpbuffer;
			while ( *src != 0 )
			{
				if ( *src == '\t' ) {
					for ( int i = 0; i < 4; ++i )
						*dest++ = ' ';
					src++;
				} else
				{
					*dest++ = *src++;
				}
			}
			G_Printf( "%s%s\n", S_COLOR_GREEN, tmpbuffer );
		}
	}

	const char *GetMapName() {
		char serverinfo[MAX_INFO_STRING];

		trap_GetServerinfo( serverinfo, sizeof( serverinfo ) );
		Q_strncpyz( level.rawmapname, Info_ValueForKey( serverinfo, "mapname" ), sizeof( level.rawmapname ) );
		return level.rawmapname;
	}

	void GetMapExtents( AABB &_aabb ) {
		_aabb.m_Mins[0] = -2048.0f;
		_aabb.m_Mins[1] = -2048.0f;
		_aabb.m_Mins[2] = -65535.0f;

		_aabb.m_Maxs[0] = 2048.0f;
		_aabb.m_Maxs[1] = 2048.0f;
		_aabb.m_Maxs[2] = 65535.0f;

		/*if(level.mapcoordsValid)
		{
		_aabb.m_Mins[0] = level.mapcoordsMins[0] * 2.F;
		_aabb.m_Mins[1] = level.mapcoordsMins[1] * 2.F;
		_aabb.m_Mins[2] = -65535.0f;

		_aabb.m_Maxs[0] = level.mapcoordsMaxs[0] * 2.F;
		_aabb.m_Maxs[1] = level.mapcoordsMaxs[1] * 2.F;
		_aabb.m_Maxs[2] = 65535.0f;
		}
		else
		{
		memset(&_aabb, 0, sizeof(AABB));
		}*/
	}

	GameEntity EntityByName( const char *_name ) {
		gentity_t *pEnt = G_FindByTargetname( NULL, _name );
		return HandleFromEntity( pEnt );
	}

	GameEntity EntityFromID( const int _gameId ) {
		gentity_t *pEnt = INDEXENT( _gameId );
		return pEnt ? HandleFromEntity( pEnt ) : GameEntity();
	}

	int IDFromEntity( const GameEntity _ent ) {
		gentity_t *pEnt = EntityFromHandle( _ent );
		if ( pEnt ) {
			gentity_t *pStart = g_entities;
			int iIndex = pEnt - pStart;
			assert( iIndex >= 0 );
			return ( iIndex < MAX_GENTITIES ) ? iIndex : -1;
		}
		return -1;
	}

	bool DoesEntityStillExist( const GameEntity &_hndl ) {
		return _hndl.IsValid() ? EntityFromHandle( _hndl ) != NULL : false;
	}

	int GetAutoNavFeatures( AutoNavFeature *_feature, int _max ) {
		int iNumFeatures = 0;
		for ( int i = MAX_CLIENTS; i < level.num_entities; i++ )
		{
			gentity_t *e = &g_entities[i];

			if ( !e->inuse ) {
				continue;
			}

			////////////////////////////////////////////////////////////////////////
			_feature[iNumFeatures].m_Type = 0;
			_feature[iNumFeatures].m_TravelTime = 0;
			for ( int x = 0; x < 3; ++i )
			{
				_feature[iNumFeatures].m_Position[x] = e->r.currentOrigin[x];
				_feature[iNumFeatures].m_TargetPosition[x] = e->r.currentOrigin[x];
				_feature[iNumFeatures].m_Bounds.m_Mins[0] = 0.f;
				_feature[iNumFeatures].m_Bounds.m_Maxs[0] = 0.f;
				AngleVectors( e->s.angles, _feature[iNumFeatures].m_Facing, NULL, NULL );
			}

			_feature[iNumFeatures].m_Bounds.m_Mins[0] = e->r.absmin[0];
			_feature[iNumFeatures].m_Bounds.m_Mins[1] = e->r.absmin[1];
			_feature[iNumFeatures].m_Bounds.m_Mins[2] = e->r.absmin[2];
			_feature[iNumFeatures].m_Bounds.m_Maxs[0] = e->r.absmin[0];
			_feature[iNumFeatures].m_Bounds.m_Maxs[1] = e->r.absmin[1];
			_feature[iNumFeatures].m_Bounds.m_Maxs[2] = e->r.absmin[2];
			//////////////////////////////////////////////////////////////////////////
			if ( e->classname ) {
				if ( !Q_stricmp( e->classname, "team_CTF_redspawn" ) ) {
					_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_PLAYERSTART_TEAM1;
				} else if ( !Q_stricmp( e->classname, "team_CTF_bluespawn" ) ) {
					_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_PLAYERSTART_TEAM2;
				} else if ( !Q_stricmp( e->classname, "info_player_deathmatch" ) ||
					!Q_stricmp( e->classname, "info_player_spawn" ) ) {
						_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_PLAYERSTART;
				} else if ( !Q_stricmp( e->classname, "target_teleporter" ) )        {
					_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_TELEPORTER;
					gentity_t *pTarget = G_PickTarget( e->target );
					if ( pTarget ) {
						_feature[iNumFeatures].m_TargetPosition[0] = pTarget->r.currentOrigin[0];
						_feature[iNumFeatures].m_TargetPosition[1] = pTarget->r.currentOrigin[1];
						_feature[iNumFeatures].m_TargetPosition[2] = pTarget->r.currentOrigin[2];
					}
				} else if ( !Q_stricmp( e->classname, "team_CTF_redflag" ) )        {
					_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_FLAG;
				} else if ( !Q_stricmp( e->classname, "team_CTF_blueflag" ) )        {
					_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_FLAG;
				}
			}

			if ( _feature[iNumFeatures].m_Type != 0 ) {
				iNumFeatures++;
			}
		}
		return iNumFeatures;
	}

	const char *GetGameName() {
		return GAME_VERSION;
	}

	const char *GetModName() {
		return OMNIBOT_MODNAME;
	}

	const char *GetModVers() {
		return OMNIBOT_MODVERSION;
	}

	const char *GetBotPath() {
		return Omnibot_GetLibraryPath();
	}

	const char *GetLogPath() {
		static char logpath[512];
		trap_Cvar_VariableStringBuffer( "fs_homepath", logpath, sizeof( logpath ) );
		return Omnibot_FixPath( logpath );
	}
};

void Bot_Interface_InitHandles() {
	for ( int i = 0; i < MAX_GENTITIES; ++i )
	{
		m_EntityHandles[i].m_HandleSerial = 1;
		m_EntityHandles[i].m_NewEntity = false;
		m_EntityHandles[i].m_Used = false;
	}
}

int Bot_Interface_Init() {
	if ( g_OmniBotEnable.integer == 0 ) {
		G_Printf( "%s%s\n", S_COLOR_GREEN,
			"Omni-bot is currently disabled with \"omnibot_enable 0\"" );
		return true;
	}

	/*
	for(int i = 0; i < MAX_GENTITIES; ++i)
	{
	m_EntityHandles[i].m_HandleSerial = 1;
	m_EntityHandles[i].m_NewEntity = false;
	m_EntityHandles[i].m_Used = false;
	}
	*/

	g_GoalSubmitReady = false;

	gGameFunctions = new RTCWInterface;
	omnibot_error err = Omnibot_LoadLibrary( RTCW_VERSION_LATEST,
		"omnibot_rtcw", Omnibot_FixPath( g_OmniBotPath.string ) );
	if ( err == BOT_ERROR_NONE ) {
		return true;
	}
	return false;
}

int Bot_Interface_Shutdown() {
	if ( IsOmnibotLoaded() ) {
		gBotFunctions.pfnShutdown();
	}
	Omnibot_FreeLibrary();
	return 1;
}

void Bot_Interface_Update() {
	if ( IsOmnibotLoaded() ) {
		char buf[1024] = {0};

		/*if (level.framenum == GAME_INIT_FRAMES)
		Bot_Event_StartGame();*/

		//////////////////////////////////////////////////////////////////////////
		{
			// time triggers for Omni-bot
			if ( level.framenum % 20 == 0 ) {
				if ( !level.twoMinute && ( g_timelimit.value * 60000 - ( level.time - level.startTime ) ) < 120000 ) {
					level.twoMinute = qtrue;
					Bot_Util_SendTrigger( NULL, NULL, "two minute warning.", "twominute" );
				}

				if ( !level.thirtySecond && ( g_timelimit.value * 60000 - ( level.time - level.startTime ) ) < 30000 ) {
					level.thirtySecond = qtrue;
					Bot_Util_SendTrigger( NULL, NULL, "thirty second warning.", "thirtysecond" );
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		{
			static float serverGravity = 0.0f;
			if ( serverGravity != g_gravity.value ) {
				Event_SystemGravity d = { -g_gravity.value };
				gBotFunctions.pfnSendGlobalEvent( MessageHelper( GAME_GRAVITY, &d, sizeof( d ) ) );
				serverGravity = g_gravity.value;
			}
			static int cheatsEnabled = 0;
			if ( g_cheats.integer != cheatsEnabled ) {
				Event_SystemCheats d = { g_cheats.integer ? True : False };
				gBotFunctions.pfnSendGlobalEvent( MessageHelper( GAME_CHEATS, &d, sizeof( d ) ) );
				cheatsEnabled = g_cheats.integer;
			}
		}

		int iNumBots = 0;
		for ( int i = 0; i < MAX_CLIENTS; i++ )
		{
			if ( !g_entities[i].inuse ) {
				continue;
			}
			if ( !g_entities[i].client ) {
				continue;
			}
			if ( g_entities[i].client->pers.connected != CON_CONNECTED ) {
				continue;
			}

			// Send a spectated message to bots that are being spectated.
			if ( ( g_entities[i].client->sess.sessionTeam == TEAM_SPECTATOR ) &&
				( g_entities[i].client->sess.spectatorState == SPECTATOR_FOLLOW ) ) {
					int iDestination = g_entities[i].client->sess.spectatorClient;
					Bot_Event_Spectated( iDestination, i );
			}

			// fake handle server commands (to prevent server command overflow)
			if ( ( g_entities[i].inuse == qtrue ) && IsBot( &g_entities[i] ) ) {
				++iNumBots;
				while ( trap_BotGetServerCommand( i, buf, sizeof( buf ) ) )
				{
				}
			}
		}

		if ( !( g_OmniBotFlags.integer & OBF_DONT_SHOW_BOTCOUNT ) ) {
			if ( g_OmniBotPlaying.integer != iNumBots ) {
				g_OmniBotPlaying.integer = iNumBots;
				trap_Cvar_Set( "omnibot_playing", va( "%i", iNumBots ) );
			}
		}
		else
		{
			if ( g_OmniBotPlaying.integer != -1 ) {
				g_OmniBotPlaying.integer = -1;
				trap_Cvar_Set( "omnibot_playing", "-1" );
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// Register any pending entity updates.
		for ( int i = 0; i < MAX_GENTITIES; ++i )
		{
			if ( m_EntityHandles[i].m_NewEntity && g_entities[i].inuse ) {
				m_EntityHandles[i].m_NewEntity = false;
				Bot_Event_EntityCreated( &g_entities[i] );
			}
		}
		SendDeferredGoals();
		//////////////////////////////////////////////////////////////////////////
		// Call the libraries update.
		gBotFunctions.pfnUpdate();
		//////////////////////////////////////////////////////////////////////////
	}
}

//////////////////////////////////////////////////////////////////////////
qboolean Bot_Util_CheckForSuicide( gentity_t *ent ) {
	if ( ent && ent->client ) {
		// Omni-bot: used for class changes, bot will /kill 2 seconds before spawn
		if ( ent->client->sess.botSuicide == qtrue ) {
			if ( ent->client->sess.sessionTeam == TEAM_RED && !ent->client->ps.powerups[PW_BLUEFLAG] ) {
				if ( ( g_redlimbotime.integer - ( ( level.dwRedReinfOffset + level.time - level.startTime ) % g_redlimbotime.integer ) ) < 2000 ) {
					Cmd_Kill_f( ent );
					if ( !ent->client->sess.botSuicidePersist || g_gamestate.integer != GS_PLAYING ) {
						ent->client->sess.botSuicide = qfalse;
					}

					return qtrue;
				}
			} else if ( ent->client->sess.sessionTeam == TEAM_BLUE && !ent->client->ps.powerups[PW_REDFLAG] )      {
				if ( ( g_bluelimbotime.integer - ( ( level.dwBlueReinfOffset + level.time - level.startTime ) % g_bluelimbotime.integer ) ) < 2000 ) {
					Cmd_Kill_f( ent );
					if ( !ent->client->sess.botSuicidePersist || g_gamestate.integer != GS_PLAYING ) {
						ent->client->sess.botSuicide = qfalse;
					}

					return qtrue;
				}
			}
		}
	}
	return qfalse;
}

//////////////////////////////////////////////////////////////////////////
int Bot_Interface_ConsoleCommand() {
	if ( IsOmnibotLoaded() ) {
		Arguments args;
		for ( int i = 0; i < trap_Argc(); ++i )
		{
			trap_Argv( i, args.m_Args[args.m_NumArgs++], Arguments::MaxArgLength );
		}
		gBotFunctions.pfnConsoleCommand( args );
	} else
	{
		G_Printf( "%s%s\n", S_COLOR_RED, "Omni-bot not loaded." );
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////
void Bot_Event_ClientConnected( int _client, qboolean _isbot ) {
	if ( IsOmnibotLoaded() ) {
		Event_SystemClientConnected d;
		d.m_GameId = _client;
		d.m_IsBot = _isbot == qtrue ? True : False;
		gBotFunctions.pfnSendGlobalEvent( MessageHelper( GAME_CLIENTCONNECTED, &d, sizeof( d ) ) );
	}
}

void Bot_Event_ClientDisConnected( int _client ) {
	if ( IsOmnibotLoaded() ) {
		Event_SystemClientDisConnected d = { _client };
		gBotFunctions.pfnSendGlobalEvent( MessageHelper( GAME_CLIENTDISCONNECTED, &d, sizeof( d ) ) );
	}
}

void Bot_Event_Drowning( int _client ) {
	if ( IsOmnibotLoaded() ) {
		if ( IsBot( &g_entities[_client] ) ) {
			gBotFunctions.pfnSendEvent( _client, MessageHelper( RTCW_EVENT_DROWNING ) );
		}
	}
}

void Bot_Event_TakeDamage( int _client, gentity_t *_ent ) {
	if ( IsOmnibotLoaded() ) {
		if ( IsBot( &g_entities[_client] ) ) {
			Event_TakeDamage d = { HandleFromEntity( _ent ) };
			gBotFunctions.pfnSendEvent( _client, MessageHelper( PERCEPT_FEEL_PAIN, &d, sizeof( d ) ) );
		}
	}
}

void Bot_Event_Death( int _client, gentity_t *_killer, const char *_meansofdeath ) {
	if ( IsOmnibotLoaded() ) {
		if ( IsBot( &g_entities[_client] ) ) {
			Event_Death d;
			d.m_WhoKilledMe = HandleFromEntity( _killer );
			Q_strncpyz( d.m_MeansOfDeath,
				_meansofdeath ? _meansofdeath : "<unknown>", sizeof( d.m_MeansOfDeath ) );
			gBotFunctions.pfnSendEvent( _client, MessageHelper( MESSAGE_DEATH, &d, sizeof( d ) ) );
		}
	}
}

void Bot_Event_Healed( int _client, gentity_t *_whodoneit ) {
	if ( IsOmnibotLoaded() ) {
		if ( IsBot( &g_entities[_client] ) ) {
			Event_Healed d = { HandleFromEntity( _whodoneit ) };
			gBotFunctions.pfnSendEvent( _client, MessageHelper( MESSAGE_HEALED, &d, sizeof( d ) ) );
		}
	}
}

void Bot_Event_RecievedAmmo( int _client, gentity_t *_whodoneit ) {
	if ( IsOmnibotLoaded() ) {
		if ( IsBot( &g_entities[_client] ) ) {
			Event_Ammo d = { HandleFromEntity( _whodoneit ) };
			gBotFunctions.pfnSendEvent( _client, MessageHelper( RTCW_EVENT_RECIEVEDAMMO, &d, sizeof( d ) ) );
		}
	}
}

void Bot_Event_Revived( int _client, gentity_t *_whodoneit ) {
	if ( IsOmnibotLoaded() ) {
		if ( IsBot( &g_entities[_client] ) ) {
			Event_Revived d = { HandleFromEntity( _whodoneit ) };
			gBotFunctions.pfnSendEvent( _client, MessageHelper( MESSAGE_REVIVED, &d, sizeof( d ) ) );
		}
	}
}

void Bot_Event_KilledSomeone( int _client, gentity_t *_victim, const char *_meansofdeath ) {
	if ( IsOmnibotLoaded() ) {
		if ( IsBot( &g_entities[_client] ) ) {
			Event_KilledSomeone d;
			d.m_WhoIKilled = HandleFromEntity( _victim );
			Q_strncpyz( d.m_MeansOfDeath,
				_meansofdeath ? _meansofdeath : "<unknown>",
				sizeof( d.m_MeansOfDeath ) / sizeof( d.m_MeansOfDeath[0] ) );
			gBotFunctions.pfnSendEvent( _client, MessageHelper( MESSAGE_KILLEDSOMEONE, &d, sizeof( d ) ) );
		}
	}
}

void Bot_Event_FireWeapon( int _client, int _weaponId, gentity_t *_projectile ) {
	if ( IsOmnibotLoaded() ) {
		if ( IsBot( &g_entities[_client] ) ) {
			Event_WeaponFire d = {0};
			d.m_WeaponId = _weaponId;
			d.m_Projectile = HandleFromEntity( _projectile );
			d.m_FireMode = Primary;
			gBotFunctions.pfnSendEvent( _client, MessageHelper( ACTION_WEAPON_FIRE, &d, sizeof( d ) ) );
		}
	}
}

void Bot_Event_Spectated( int _client, int _who ) {
	if ( IsOmnibotLoaded() ) {
		if ( IsBot( &g_entities[_client] ) ) {
			Event_Spectated d = { _who };
			gBotFunctions.pfnSendEvent( _client, MessageHelper( MESSAGE_SPECTATED, &d, sizeof( d ) ) );
		}
	}
}

void Bot_Event_ChatMessage( int _to, gentity_t *_source, int _type, const char *_message ) {
	if ( IsOmnibotLoaded() ) {
		if ( IsBot( &g_entities[_to] ) ) {
			int iMsg = PERCEPT_HEAR_GLOBALCHATMSG;
			switch ( _type )
			{
			case SAY_ALL:
				iMsg = PERCEPT_HEAR_GLOBALCHATMSG;
				break;
			case SAY_TEAM:
				//		case SAY_TEAMNL:
				iMsg = PERCEPT_HEAR_TEAMCHATMSG;
				break;
			case PRIVATE_MESSAGE:
				iMsg = PERCEPT_HEAR_PRIVCHATMSG;
				break;
			}

			Event_ChatMessage d;
			d.m_WhoSaidIt = HandleFromEntity( _source );
			Q_strncpyz( d.m_Message, _message ? _message : "<unknown>",
				sizeof( d.m_Message ) / sizeof( d.m_Message[0] ) );
			gBotFunctions.pfnSendEvent( _to, MessageHelper( iMsg, &d, sizeof( d ) ) );
		}
	}
}

void Bot_Event_VoiceMacro( int _client, gentity_t *_source, int _type, const char *_message ) {
	if ( IsOmnibotLoaded() ) {
		if ( IsBot( &g_entities[_client] ) ) {
			int iMessageId = PERCEPT_HEAR_GLOBALVOICEMACRO;
			if ( _type == SAY_TEAM ) {
				iMessageId = PERCEPT_HEAR_TEAMVOICEMACRO;
			}

			Event_VoiceMacro d;
			d.m_WhoSaidIt = HandleFromEntity( _source );
			Q_strncpyz( d.m_MacroString, _message ? _message : "<unknown>",
				sizeof( d.m_MacroString ) / sizeof( d.m_MacroString[0] ) );
			gBotFunctions.pfnSendEvent( _client, MessageHelper( iMessageId, &d, sizeof( d ) ) );
		}
	}
}

void Bot_Event_Sound( gentity_t *_source, int _sndtype, const char *_name ) {
	if ( IsOmnibotLoaded() ) {
		Event_Sound d = {};
		d.m_Source = HandleFromEntity( _source );
		d.m_SoundType = _sndtype;
		gGameFunctions->GetEntityPosition( d.m_Source,d.m_Origin );
		Q_strncpyz( d.m_SoundName, _name ? _name : "<unknown>", sizeof( d.m_SoundName ) / sizeof( d.m_SoundName[0] ) );
		gBotFunctions.pfnSendGlobalEvent( MessageHelper( GAME_SOUND, &d, sizeof( d ) ) );
	}
}

void Bot_Event_EntityCreated( gentity_t *pEnt ) {
	if ( pEnt && IsOmnibotLoaded() ) {
		// Get common properties.
		const int iEntNum = pEnt - g_entities;
		GameEntity ent = HandleFromEntity( pEnt );
		int iClass = gGameFunctions->GetEntityClass( ent );
		if ( iClass ) {
			Event_EntityCreated d;
			d.m_Entity = GameEntity( iEntNum, m_EntityHandles[iEntNum].m_HandleSerial );

			d.m_EntityClass = iClass;
			gGameFunctions->GetEntityCategory( ent, d.m_EntityCategory );
			gBotFunctions.pfnSendGlobalEvent( MessageHelper( GAME_ENTITYCREATED, &d, sizeof( d ) ) );
			m_EntityHandles[iEntNum].m_Used = true;
		}

		Bot_Util_CheckForGoalEntity( ent );
	}
}

extern "C"
{
	void Bot_Queue_EntityCreated( gentity_t *pEnt ) {
		if ( pEnt ) {
			m_EntityHandles[pEnt - g_entities].m_NewEntity = true;
		}
	}
	void Bot_Event_EntityDeleted( gentity_t *pEnt ) {
		if ( pEnt ) {
			const int iEntNum = pEnt - g_entities;
			if ( IsOmnibotLoaded() ) {
				Event_EntityDeleted d;
				d.m_Entity = GameEntity( iEntNum, m_EntityHandles[iEntNum].m_HandleSerial );
				gBotFunctions.pfnSendGlobalEvent( MessageHelper( GAME_ENTITYDELETED, &d, sizeof( d ) ) );
			}
			m_EntityHandles[iEntNum].m_Used = false;
			m_EntityHandles[iEntNum].m_NewEntity = false;
			while ( ++m_EntityHandles[iEntNum].m_HandleSerial == 0 ) {}
		}
	}

	//////////////////////////////////////////////////////////////////////////

	void Bot_Util_SendTrigger( gentity_t *_ent, gentity_t *_activator, const char *_tagname, const char *_action ) {
		if ( IsOmnibotLoaded() ) {
			TriggerInfo triggerInfo;
			triggerInfo.m_Entity = HandleFromEntity( _ent );
			Q_strncpyz( triggerInfo.m_TagName, _tagname, TriggerBufferSize );
			Q_strncpyz( triggerInfo.m_Action, _action, TriggerBufferSize );
			gBotFunctions.pfnSendTrigger( triggerInfo );
		}
	}

	void Bot_AddDynamiteGoal( gentity_t *_ent, int _team, const char *_tag ) {
		if ( _team == TEAM_RED ) {
			Bot_Util_AddGoal( "defuse",_ent,( 1 << RTCW_TEAM_ALLIES ), _tag );
		} else {
			Bot_Util_AddGoal( "defuse",_ent,( 1 << RTCW_TEAM_AXIS ), _tag );
		}
	}

	void Bot_AddFallenTeammateGoals( gentity_t *_teammate, int _team ) {
		if ( _team == TEAM_RED ) {
			Bot_Util_AddGoal( "revive",_teammate,( 1 << RTCW_TEAM_AXIS ),_GetEntityName( _teammate ) );
		} else if ( _team == TEAM_BLUE ) {
			Bot_Util_AddGoal( "revive",_teammate,( 1 << RTCW_TEAM_ALLIES ),_GetEntityName( _teammate ) );
		}
	}
};