/*
* ETF <-> Omni-Bot interface source file.
* 
*/

#include <sstream>
#include <iomanip>

extern          "C"
{
	#include "g_bot_interface.h"

	void Cmd_Kill_f(gentity_t * ent);
};

#include "BotExports.h"
#include "ETF_Config.h"
#include "TF_Messages.h"

void            Bot_Event_EntityCreated(gentity_t * pEnt);

bool IsBot(gentity_t * e)
{
	return e->r.svFlags & SVF_BOT ? true : false;
}

//////////////////////////////////////////////////////////////////////////

const int       MAX_CACHE = 32;
int             g_NumGasGrenades = 0;
gentity_t      *g_GasGrenadeCache[MAX_CACHE] = { 0 };

/*struct BotGoalInfo
{
	char            m_GoalName[MAX_QPATH];
	int             m_GoalType;
	int             m_GoalTeam;
	gentity_t      *m_Entity;
	bool            m_Used;
};*/

struct BotEntity
{
	obint16         m_HandleSerial;
	bool            m_NewEntity:1;
	bool            m_Used:1;
};

BotEntity       m_EntityHandles[MAX_GENTITIES];

struct GrenadePrime
{
	int             m_PrimeTime;
	bool            m_Throw;
	int             m_GrenType;
	int             m_GrenClass;
};

GrenadePrime    m_GrenadePrime[MAX_CLIENTS];

//GrenadePrime  m_GrenadePrime2[MAX_CLIENTS];

//////////////////////////////////////////////////////////////////////////

extern          "C" const char *_GetEntityName(gentity_t * _ent)
{
	static char     newentname[256];

	newentname[0] = '\0';

	if(_ent)
	{
		if(_ent->inuse && _ent->client)
		{
			if(_ent->client->pers.netname[0])
				return _ent->client->pers.netname;
			else
			{
				static char     userinfo[MAX_INFO_STRING] = { 0 };
				trap_GetUserinfo(_ent - g_entities, userinfo, sizeof(userinfo));
				return Info_ValueForKey(userinfo, "name");
			}
		}

		if(_ent->mapdata && _ent->mapdata->groupname && _ent->mapdata->groupname->data->d.strdata)
			Q_strncpyz(newentname, _ent->mapdata->groupname->data->d.strdata, 256);
		else if(_ent->targetname)
			Q_strncpyz(newentname, _ent->targetname, 256);
		else
			Com_sprintf(newentname, 256, "%s_%i", _ent->classname, _ent - g_entities);

		return newentname;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

gentity_t      *INDEXENT(const int _gameId)
{
	if(_gameId >= 0 && _gameId < MAX_GENTITIES)
	{
		switch (_gameId)
		{
			case ENTITYNUM_WORLD:	// world ent not marked inuse for some reason
				return &g_entities[ENTITYNUM_WORLD];
			default:
				return g_entities[_gameId].inuse ? &g_entities[_gameId] : 0;
		}
	}
	return 0;
}

int ENTINDEX(gentity_t * _ent)
{
	return _ent - g_entities;
}

gentity_t      *EntityFromHandle(GameEntity _ent)
{
	obint16         index = _ent.GetIndex();

	if(m_EntityHandles[index].m_HandleSerial == _ent.GetSerial() && g_entities[index].inuse)
		return &g_entities[index];
	if(index == ENTITYNUM_WORLD)
		return &g_entities[ENTITYNUM_WORLD];
	return NULL;
}

GameEntity HandleFromEntity(gentity_t * _ent)
{
	if(_ent)
	{
		int             index = ENTINDEX(_ent);

		return GameEntity(index, m_EntityHandles[index].m_HandleSerial);
	}
	else
		return GameEntity();
}

//////////////////////////////////////////////////////////////////////////

void GetEntityCenter(gentity_t * ent, vec3_t _pos)
{
	_pos[0] = ent->r.currentOrigin[0] + ((ent->r.maxs[0] + ent->r.mins[0]) * 0.5f);
	_pos[1] = ent->r.currentOrigin[1] + ((ent->r.maxs[1] + ent->r.mins[1]) * 0.5f);
	_pos[2] = ent->r.currentOrigin[2] + ((ent->r.maxs[2] + ent->r.mins[2]) * 0.5f);
}

void GetCenterTop(gentity_t * ent, const vec3_t _in, vec3_t _pos)
{
	_pos[0] = _in[0] + ((ent->r.maxs[0] + ent->r.mins[0]) * 0.5f);
	_pos[1] = _in[1] + ((ent->r.maxs[1] + ent->r.mins[1]) * 0.5f);
	_pos[2] = _in[2] + (ent->r.maxs[2]);//ent->r.currentOrigin[2] + ((ent->r.maxs[2] + ent->r.mins[2]) * 0.5f);
}

//////////////////////////////////////////////////////////////////////////

int Bot_TeamGameToBot(int team)
{
	switch (team)
	{
		case Q3F_TEAM_RED:
			return ETF_TEAM_RED;
		case Q3F_TEAM_BLUE:
			return ETF_TEAM_BLUE;
		case Q3F_TEAM_YELLOW:
			return ETF_TEAM_YELLOW;
		case Q3F_TEAM_GREEN:
			return ETF_TEAM_GREEN;
		case Q3F_TEAM_SPECTATOR:
			return OB_TEAM_SPECTATOR;
		default:
			return ETF_TEAM_NONE;
	}
}

int _teamBotToGame(int team)
{
	switch (team)
	{
		case ETF_TEAM_RED:
			return Q3F_TEAM_RED;
		case ETF_TEAM_BLUE:
			return Q3F_TEAM_BLUE;
		case ETF_TEAM_YELLOW:
			return Q3F_TEAM_YELLOW;
		case ETF_TEAM_GREEN:
			return Q3F_TEAM_GREEN;
		case OB_TEAM_SPECTATOR:
			return Q3F_TEAM_SPECTATOR;
		default:
			return Q3F_TEAM_FREE;
	}
}

static int playerClassBotToGame(int playerClass)
{
	switch (playerClass)
	{
		case TF_CLASS_SCOUT:
			return Q3F_CLASS_RECON;
		case TF_CLASS_SNIPER:
			return Q3F_CLASS_SNIPER;
		case TF_CLASS_SOLDIER:
			return Q3F_CLASS_SOLDIER;
		case TF_CLASS_DEMOMAN:
			return Q3F_CLASS_GRENADIER;
		case TF_CLASS_MEDIC:
			return Q3F_CLASS_PARAMEDIC;
		case TF_CLASS_HWGUY:
			return Q3F_CLASS_MINIGUNNER;
		case TF_CLASS_PYRO:
			return Q3F_CLASS_FLAMETROOPER;
		case TF_CLASS_SPY:
			return Q3F_CLASS_AGENT;
		case TF_CLASS_ENGINEER:
			return Q3F_CLASS_ENGINEER;
		case TF_CLASS_CIVILIAN:
			return Q3F_CLASS_CIVILIAN;
		default:
			return -1;
	}
}

int Bot_PlayerClassGameToBot(int playerClass)
{
	switch (playerClass)
	{
		case Q3F_CLASS_RECON:
			return TF_CLASS_SCOUT;
		case Q3F_CLASS_SNIPER:
			return TF_CLASS_SNIPER;
		case Q3F_CLASS_SOLDIER:
			return TF_CLASS_SOLDIER;
		case Q3F_CLASS_GRENADIER:
			return TF_CLASS_DEMOMAN;
		case Q3F_CLASS_PARAMEDIC:
			return TF_CLASS_MEDIC;
		case Q3F_CLASS_MINIGUNNER:
			return TF_CLASS_HWGUY;
		case Q3F_CLASS_FLAMETROOPER:
			return TF_CLASS_PYRO;
		case Q3F_CLASS_AGENT:
			return TF_CLASS_SPY;
		case Q3F_CLASS_ENGINEER:
			return TF_CLASS_ENGINEER;
		case Q3F_CLASS_CIVILIAN:
			return TF_CLASS_CIVILIAN;
		default:
			return TF_CLASS_NONE;
	}
}

#define WP_DETPACK 99
#define WP_GRENADE1 100
#define WP_GRENADE2 101
static int _weaponBotToGame(int weapon)
{
	switch (weapon)
	{
		case TF_WP_AXE:
		case TF_WP_CROWBAR:
		case TF_WP_KNIFE:
		case TF_WP_MEDKIT:
		case TF_WP_SPANNER:
			return WP_AXE;
		case TF_WP_SHOTGUN:
			return WP_SHOTGUN;
		case TF_WP_SUPERSHOTGUN:
			return WP_SUPERSHOTGUN;
		case TF_WP_NAILGUN:
			return WP_NAILGUN;
		case TF_WP_SUPERNAILGUN:
			return WP_SUPERNAILGUN;
		case TF_WP_GRENADE_LAUNCHER:
			return WP_GRENADE_LAUNCHER;
		case TF_WP_ROCKET_LAUNCHER:
			return WP_ROCKET_LAUNCHER;
		case TF_WP_SNIPER_RIFLE:
			return WP_SNIPER_RIFLE;
		case TF_WP_RAILGUN:
			return WP_RAILGUN;
		case TF_WP_FLAMETHROWER:
			return WP_FLAMETHROWER;
		case TF_WP_MINIGUN:
			return WP_MINIGUN;
		case TF_WP_AUTORIFLE:
			return WP_ASSAULTRIFLE;
		case TF_WP_DARTGUN:
			return WP_DARTGUN;
		case TF_WP_PIPELAUNCHER:
			return WP_PIPELAUNCHER;
		case TF_WP_NAPALMCANNON:
			return WP_NAPALMCANNON;
		case TF_WP_DETPACK:
			return WP_DETPACK;
		case TF_WP_GRENADE1:
			return WP_GRENADE1;
		case TF_WP_GRENADE2:
			return WP_GRENADE2;
		default:
			return WP_NONE;
	}
}

int Bot_WeaponGameToBot(int weapon, int clsnum)
{
	switch (weapon)
	{
		case WP_AXE:
			switch (clsnum)
			{
				case Q3F_CLASS_PARAMEDIC:
					return TF_WP_MEDKIT;
				case Q3F_CLASS_AGENT:
					return TF_WP_KNIFE;
				case Q3F_CLASS_ENGINEER:
					return TF_WP_SPANNER;
				default:
					return TF_WP_AXE;
			}
		case WP_SHOTGUN:
			return TF_WP_SHOTGUN;
		case WP_SUPERSHOTGUN:
			return TF_WP_SUPERSHOTGUN;
		case WP_NAILGUN:
			return TF_WP_NAILGUN;
		case WP_SUPERNAILGUN:
			return TF_WP_SUPERNAILGUN;
		case WP_GRENADE_LAUNCHER:
			return TF_WP_GRENADE_LAUNCHER;
		case WP_ROCKET_LAUNCHER:
			return TF_WP_ROCKET_LAUNCHER;
		case WP_SNIPER_RIFLE:
			return TF_WP_SNIPER_RIFLE;
		case WP_RAILGUN:
			return TF_WP_RAILGUN;
		case WP_FLAMETHROWER:
			return TF_WP_FLAMETHROWER;
		case WP_MINIGUN:
			return TF_WP_MINIGUN;
		case WP_ASSAULTRIFLE:
			return TF_WP_AUTORIFLE;
		case WP_DARTGUN:
			return TF_WP_DARTGUN;
		case WP_PIPELAUNCHER:
			return TF_WP_PIPELAUNCHER;
		case WP_NAPALMCANNON:
			return TF_WP_NAPALMCANNON;
		default:
			return TF_WP_NONE;
	}
}

static int _grenadeBotToGame(int grenadeClass)
{
	switch (grenadeClass)
	{
		case TF_CLASSEX_DETPACK:
			return Q3F_GREN_CHARGE;
		//case OF_CLASSEX_PROXMINE:
		//	return Q3F_GREN_CLAYMORE;
		case TF_CLASSEX_CONC_GRENADE:
			return Q3F_GREN_CONCUSS;
		case TF_CLASSEX_CALTROP:
			return Q3F_GREN_FLASH;
		case TF_CLASSEX_MIRV_GRENADE:
			return Q3F_GREN_CLUSTER;
		case TF_CLASSEX_MIRVLET_GRENADE:
			return Q3F_GREN_CLUSTERSECTION;
		case TF_CLASSEX_EMP_GRENADE:
			return Q3F_GREN_EMP;
		case TF_CLASSEX_NAIL_GRENADE:
			return Q3F_GREN_NAIL;
		case TF_CLASSEX_NAPALM_GRENADE:
			return Q3F_GREN_NAPALM;
		case TF_CLASSEX_GAS_GRENADE:
			return Q3F_GREN_GAS;
		default:
			return Q3F_GREN_NONE;
	}
}

static int _grenadeTypeFromClass(int grentype, int playerClass)
{
	if(grentype == 1)
	{
		switch (playerClass)
		{
			case Q3F_CLASS_RECON:
				return Q3F_GREN_FLASH;
			case Q3F_CLASS_SNIPER:
				return Q3F_GREN_NORMAL;
			case Q3F_CLASS_SOLDIER:
				return Q3F_GREN_NORMAL;
			case Q3F_CLASS_GRENADIER:
				return Q3F_GREN_NORMAL;
			case Q3F_CLASS_PARAMEDIC:
				return Q3F_GREN_NORMAL;
			case Q3F_CLASS_MINIGUNNER:
				return Q3F_GREN_NORMAL;
			case Q3F_CLASS_FLAMETROOPER:
				return Q3F_GREN_NORMAL;
			case Q3F_CLASS_AGENT:
				return Q3F_GREN_NORMAL;
			case Q3F_CLASS_ENGINEER:
				return Q3F_GREN_NORMAL;
			case Q3F_CLASS_CIVILIAN:
			default:
				break;
		}
	}
	else if(grentype == 2)
	{
		switch (playerClass)
		{
			case Q3F_CLASS_RECON:
				return Q3F_GREN_CONCUSS;
			case Q3F_CLASS_SNIPER:
				return Q3F_GREN_FLASH;
			case Q3F_CLASS_SOLDIER:
				return Q3F_GREN_NAIL;
			case Q3F_CLASS_GRENADIER:
				return Q3F_GREN_CLUSTER;
			case Q3F_CLASS_PARAMEDIC:
				return Q3F_GREN_CONCUSS;
			case Q3F_CLASS_MINIGUNNER:
				return Q3F_GREN_CLUSTER;
			case Q3F_CLASS_FLAMETROOPER:
				return Q3F_GREN_NAPALM;
			case Q3F_CLASS_AGENT:
				return Q3F_GREN_GAS;
			case Q3F_CLASS_ENGINEER:
				return Q3F_GREN_EMP;
			case Q3F_CLASS_CIVILIAN:
			default:
				break;
		}
	}
	return Q3F_GREN_NONE;
}

#if 0
#define MAX_GAS_RADIUS 150.0
#define MAX_GAS_RADIUS_TIME 20000.0
#define UNAFFECTED_BY_GAS_DIST SQR(100)

gentity_t      *Bot_EntInvisibleByGasGren(vec3_t start, vec3_t end)
{
	gentity_t      *ent = NULL;
	vec3_t          gasCenter;
	float           gasRadius;

	// if the target is close enough, vision is not affected by gas gren
	if(DistanceSquared(start, end) < UNAFFECTED_BY_GAS_DIST)
	{
		return 0;
	}

	for(int i = 0; i < g_NumGasGrenades; ++i)
	{
		ent = g_GasGrenadeCache[i];

		if(ent->soundLoop != 1)
		{
			//pfnPrintMessage("smoke not up yet");
			// xkan, the smoke has not really started yet, see weapon_smokeBombExplode
			// and CG_RenderSmokeGrenadeSmoke
			continue;
		}
		// check the distance 
		VectorCopy(ent->s.pos.trBase, smokeCenter);
		// raise the center to better match the position of the smoke, see
		// CG_SpawnSmokeSprite().
		smokeCenter[2] += 32;
		// smoke sprite has a maximum radius of 640/2. and it takes a while for it to 
		// reach that size, so adjust the radius accordingly.
		smokeRadius = MAX_GAS_RADIUS * ((level.time - ent->grenadeExplodeTime) / MAX_GAS_RADIUS_TIME);
		if(smokeRadius > MAX_GAS_RADIUS)
			smokeRadius = MAX_GAS_RADIUS;
		// if distance from line is short enough, vision is blocked by smoke

		//float fColorRed[3] = { 1.0f, 0.0f, 0.0f };
		//float fColorGrn[3] = { 0.0f, 1.0f, 0.0f };

		if(DistanceFromLineSquared(smokeCenter, start, end) < (smokeRadius * smokeRadius))
		{
			//pfnPrintMessage("hid by smoke");          
			//pfnAddTempDisplayLine(smokeCenter, end, fColorGrn);
			return ent;
		}
		//pfnAddTempDisplayLine(smokeCenter, start, fColorRed);
	}

	return 0;
}
#endif

void Bot_Util_AddGoal(const char *_type, gentity_t *_ent, int _team, const char *_tag, const char *_extrakey = 0, obUserData *_extraval = 0)
{
	if(IsOmnibotLoaded())
	{
		MapGoalDef      goaldef;

		goaldef.Props.SetString("Type",_type);
		goaldef.Props.SetEntity("Entity",HandleFromEntity(_ent));
		goaldef.Props.SetInt("Team",_team);
		goaldef.Props.SetString("TagName",_tag);
		goaldef.Props.SetInt("InterfaceGoal",1);
		if(_extrakey && _extraval)
		{
			goaldef.Props.Set(_extrakey,*_extraval);
		}
		g_BotFunctions.pfnAddGoal(goaldef);
	}
}

int Bot_TeamGameBitsToBotBits(int teams)
{
	int             bitfield;

	bitfield = 0;
	for(int t = Q3F_TEAM_RED; t <= Q3F_TEAM_GREEN; ++t)
	{
		if((teams & (1 << t)))
		{
			bitfield |= (1 << Bot_TeamGameToBot(t));
		}
	}

	return (bitfield);
}

int _GetTeamsFromHoldingFlag(q3f_array_t * array)
{
	int             index, index2;
	q3f_keypair_t  *targkp;
	q3f_array_t    *targarray;
	q3f_data_t     *data, *data2;
	gentity_t      *ent;

	for(index = -1; data = G_Q3F_ArrayTraverse(array, &index);)
	{
		// Locate each target map in the main kparray
		if(!(targkp = G_Q3F_KeyPairArrayFind(level.targetnameArray, data->d.strdata)))
			return (qfalse);
		targarray = targkp->value.d.arraydata;
		// Check each ent in that hash
		for(index2 = -1, ent = NULL; data2 = G_Q3F_ArrayTraverse(targarray, &index2);)
		{
			ent = data2->d.entitydata;
			if(ent->mapdata && ent->mapdata->state != Q3F_STATE_DISABLED && ent->mapdata->state != Q3F_STATE_INVISIBLE)
				return Bot_TeamGameBitsToBotBits(ent->mapdata->team);
			ent = NULL;
		}
		if(!ent)
			return (0);	// Didn't find this entity
	}
	return (0);
}

bool _IsBackpack(gentity_t *_ent)
{
	switch(_ent->s.eType)
	{
		case ET_Q3F_GOAL:
		{
			if(!Q_stricmp( _ent->classname, "func_goalinfo" ) && _ent->mapdata && _ent->mapdata->give && !_ent->mapdata->holding)	/* func_goalinfo */
				return true;
			break;
		}
		case ET_ITEM:
		{
			if(_ent->item && _ent->item->giType != IT_BAD)
				return true;
			break;
		}
		default:
			break;
	}

	return false;
}

static bool _GetBackpackTeam(int playerTeam, gentity_t * _ent)
{
	// playerTeam is in bot format already
	// don't call unless _IsBackpack has been checked already
	int t = 0;
	
	if(_ent->mapdata)
		t = Bot_TeamGameBitsToBotBits(_ent->mapdata->team);
	if(t == 0)
		return true;
	if(t & (1 << playerTeam))
		return true;

	return false;
}

static int _GetEntityTeam(gentity_t * _ent)
{
	// hack, when the game joins clients again after warmup, they are temporarily ET_GENERAL entities(LAME)
	int             t = _ent->s.eType;

	if(_ent->client && (_ent - g_entities) < MAX_CLIENTS)
	{
		//t = ET_PLAYER;
		return Bot_TeamGameToBot(_ent->client->sess.sessionTeam);
	}

	switch (t)
	{
		case ET_PLAYER:
			return Bot_TeamGameToBot(_ent->client->sess.sessionTeam);
		case ET_Q3F_SENTRY:
		case ET_Q3F_SUPPLYSTATION:
			return _ent->parent &&
				_ent->parent->client ? Bot_TeamGameToBot(_ent->parent->client->sess.sessionTeam) : ETF_TEAM_NONE;
		case ET_MISSILE:
		{
			if(_ent->s.otherEntityNum >= 0 && _ent->s.otherEntityNum < ENTITYNUM_MAX_NORMAL)
				return &g_entities[_ent->s.otherEntityNum] &&
					g_entities[_ent->s.otherEntityNum].client ? Bot_TeamGameToBot(g_entities[_ent->s.otherEntityNum].client->sess.
																				  sessionTeam) : ETF_TEAM_NONE;
		}
			// Let this fall through
		default:
			//fixme
			return Bot_TeamGameToBot(_ent->s.otherEntityNum);
	}

	return 0;
}

static char *teamscoreptr;
static qboolean hasptr;

static int _GetEntityClass(gentity_t * _ent)
{
	// hack, when the game joins clients again after warmup, they are temporarily ET_GENERAL entities(LAME)
	int             t = _ent->s.eType;

	if(!hasptr)
	{
		G_Q3F_AddString(&teamscoreptr, "teamscore");
		hasptr = qtrue;
	}

	if(_ent->client && (_ent - g_entities) < MAX_CLIENTS)
		t = ET_PLAYER;

	// This entity isn't valid this gameindex
	if(_ent->mapdata && _ent->mapdata->gameindex && !(_ent->mapdata->gameindex & (1 << g_gameindex.integer)))
		return 0;

	if(_ent->classname)
	{
		if(!Q_stricmp(_ent->classname, "team_CTF_redspawn"))
			return ENT_CLASS_GENERIC_PLAYERSTART_TEAM1;
		else if(!Q_stricmp(_ent->classname, "team_CTF_bluespawn"))
			return ENT_CLASS_GENERIC_PLAYERSTART_TEAM2;
		else if(!Q_stricmp(_ent->classname, "info_player_deathmatch") || !Q_stricmp(_ent->classname, "info_player_start"))
		{
			if(_ent->mapdata && _ent->mapdata->team)
			{
				if(_ent->mapdata->team == (1 << Q3F_TEAM_RED))
					return ENT_CLASS_GENERIC_PLAYERSTART_TEAM1;
				else if(_ent->mapdata->team == (1 << Q3F_TEAM_BLUE))
					return ENT_CLASS_GENERIC_PLAYERSTART_TEAM2;
				else if(_ent->mapdata->team == (1 << Q3F_TEAM_YELLOW))
					return ENT_CLASS_GENERIC_PLAYERSTART_TEAM3;
				else if(_ent->mapdata->team == (1 << Q3F_TEAM_GREEN))
					return ENT_CLASS_GENERIC_PLAYERSTART_TEAM4;
				else
					return ENT_CLASS_GENERIC_PLAYERSTART;
			}
			else
				return ENT_CLASS_GENERIC_PLAYERSTART;
		}
	}

	switch (t)
	{
		case ET_GENERAL:
		{
			if(!Q_stricmp(_ent->classname, "func_button"))
				return ENT_CLASS_GENERIC_BUTTON;
			if(!Q_stricmp(_ent->classname, "trigger_multiple") && _ent->mapdata && (_ent->mapdata->give || _ent->mapdata->holding) && _ent->mapdata->other)	/* trigger_multiple */
			{
				q3f_keypair_t  *data = G_Q3F_KeyPairArrayFind(_ent->mapdata->other, teamscoreptr);

				if(data && data->value.d.intdata)
					return ENT_CLASS_GENERIC_FLAGCAPPOINT;
			}
			break;
		}
		case ET_INVISIBLE:
		{
			// This will likely not happen as above forces to ET_PLAYER
			if(_ent->client)
				return ENT_CLASS_GENERIC_SPECTATOR;
			break;
		}
		case ET_PLAYER:
		{
			if(!_ent->client ||
			   (_ent->client->sess.sessionTeam != Q3F_TEAM_RED &&
				_ent->client->sess.sessionTeam != Q3F_TEAM_BLUE &&
				_ent->client->sess.sessionTeam != Q3F_TEAM_YELLOW &&
				_ent->client->sess.sessionTeam != Q3F_TEAM_GREEN) || Q3F_IsSpectator(_ent->client))
				return ENT_CLASS_GENERIC_SPECTATOR;

			return Bot_PlayerClassGameToBot(_ent->client->ps.persistant[PERS_CURRCLASS]);
		}
/*	case ET_CORPSE:
		{
			return ET_CLASSEX_CORPSE;
		}*/
		case ET_Q3F_SENTRY:
		{
			return TF_CLASSEX_SENTRY;
		}
		case ET_Q3F_SUPPLYSTATION:
		{
			return TF_CLASSEX_DISPENSER;
		}
		case ET_MISSILE:
		{
			// Register certain weapons as threats to avoid or whatever.
			switch (_ent->s.weapon)
			{
				case WP_GRENADE_LAUNCHER:
					return TF_CLASSEX_GLGRENADE;
				case WP_ROCKET_LAUNCHER:
					return TF_CLASSEX_ROCKET;
				case WP_PIPELAUNCHER:
					return TF_CLASSEX_PIPE;
				case WP_NAPALMCANNON:
					return TF_CLASSEX_NAPALM;
				default:
					break;
			}
			break;
		}
		case ET_Q3F_GRENADE:
		{
			// Register certain grenades as threats to avoid or whatever.
			switch (_ent->s.weapon)
			{
				case Q3F_GREN_CHARGE:
					return TF_CLASSEX_DETPACK;
				//case Q3F_GREN_CLAYMORE:
				//	return OF_CLASSEX_PROXMINE;
				case Q3F_GREN_CONCUSS:
					return TF_CLASSEX_CONC_GRENADE;
				case Q3F_GREN_FLASH:
					return TF_CLASSEX_CALTROP;
				case Q3F_GREN_CLUSTER:
					return TF_CLASSEX_MIRV_GRENADE;
				case Q3F_GREN_CLUSTERSECTION:
					return TF_CLASSEX_MIRVLET_GRENADE;
				case Q3F_GREN_EMP:
					return TF_CLASSEX_EMP_GRENADE;
				case Q3F_GREN_NAIL:
					return TF_CLASSEX_NAIL_GRENADE;
				case Q3F_GREN_NAPALM:
					return TF_CLASSEX_NAPALM_GRENADE;
				case Q3F_GREN_GAS:
					return TF_CLASSEX_GAS_GRENADE;
				default:
					return TF_CLASSEX_GRENADE;
					break;
			}
			break;
		}
		case ET_Q3F_GOAL:
		{
			if(!Q_stricmp(_ent->classname, "func_goalinfo") && _ent->mapdata && _ent->mapdata->give)	/* func_goalinfo */
			{
				if(_ent->mapdata->give && _ent->mapdata->holding && _ent->mapdata->other)
				{
					q3f_keypair_t  *data = G_Q3F_KeyPairArrayFind(_ent->mapdata->other, teamscoreptr);

					if(data && data->value.d.intdata)
					{
						return ENT_CLASS_GENERIC_FLAGCAPPOINT;
					}
				}
				return TF_CLASSEX_BACKPACK;
			}
			if((!Q_stricmp(_ent->classname, "func_goalitem") || !Q_stricmp(_ent->classname, "func_flag")) && _ent->model && _ent->model[0] && _ent->mapdata && _ent->mapdata->groupname)	/* func_goalitem */ /* func_flag */
				return ENT_CLASS_GENERIC_FLAG;
			break;
		}
		case ET_ITEM:
		{
			if(_ent->item)
			{
				switch (_ent->item->giType)
				{
					case IT_Q3F_BACKPACK:
						return TF_CLASSEX_BACKPACK_AMMO;
//					case IT_Q3F_DEBRIS:
//						// technically only cells
//					case IT_Q3F_DEBRIS2:
//						// technically only cells
					case IT_Q3F_AMMOBOX:
						return TF_CLASSEX_BACKPACK_AMMO;
					case IT_HEALTH:
						return TF_CLASSEX_BACKPACK_HEALTH;
					case IT_ARMOR:
						return TF_CLASSEX_BACKPACK_ARMOR;
					default:
						break;
				}
			}
			break;
		}
		default:
			break;
	};
	return 0;
}

void Omnibot_Load_PrintMsg(const char *_msg)
{
	G_Printf("Omni-bot: %s%s\n", S_COLOR_GREEN, _msg);
}

void Omnibot_Load_PrintErr(const char *_msg)
{
	G_Printf("Omni-bot: %s%s\n", S_COLOR_RED, _msg);
}

int             g_LastScreenMessageTime = 0;

int obUtilBotContentsFromGameContents(int _contents)
{
	int             iBotContents = 0;

	if(_contents & CONTENTS_SOLID)
		iBotContents |= CONT_SOLID;
	if(_contents & CONTENTS_WATER)
		iBotContents |= CONT_WATER;
	if(_contents & CONTENTS_SLIME)
		iBotContents |= CONT_SLIME;
	if(_contents & CONTENTS_FOG)
		iBotContents |= CONT_FOG;
	if(_contents & CONTENTS_TELEPORTER)
		iBotContents |= CONT_TELEPORTER;
	if(_contents & CONTENTS_MOVER)
		iBotContents |= CONT_MOVER;
	if(_contents & CONTENTS_TRIGGER)
		iBotContents |= CONT_TRIGGER;
	if(_contents & CONTENTS_LAVA)
		iBotContents |= CONT_LAVA;
	if(_contents & CONTENTS_PLAYERCLIP)
		iBotContents |= CONT_PLYRCLIP;
	return iBotContents;
}

int obUtilBotSurfaceFromGameSurface(int _surfaceflags)
{
	int             iBotSurface = 0;

	if(_surfaceflags & SURF_SLICK)
		iBotSurface |= SURFACE_SLICK;
	if(_surfaceflags & SURF_LADDER)
		iBotSurface |= SURFACE_LADDER;
	return iBotSurface;
}

// helper stuff
extern          "C"
{
	qboolean InFieldOfVision(vec3_t viewangles, float fov, vec3_t angles)
	{
		int             i;
		float           diff, angle;

		for(i = 0; i < 2; i++)
		{
			angle = AngleNormalize360(viewangles[i]);
			angles[i] = AngleNormalize360(angles[i]);
			diff = angles[i] - angle;
			if(angles[i] > angle)
			{
				if(diff > 180.0)
					diff -= 360.0;
			}
			else
			{
				if(diff < -180.0)
					diff += 360.0;
			}
			if(diff > 0)
			{
				if(diff > fov * 0.5)
					return qfalse;
			}
			else
			{
				if(diff < -fov * 0.5)
					return qfalse;
			}
		}
		return qtrue;
	}
}

class           ETFInterface:public IEngineInterface
{
  public:
	int             AddBot(const MessageHelper & _data)
	{
		// wait until everything settles before adding bots on dedicated servers
		if ( g_dedicated.integer && ( level.time - level.startTime < 10000 ) )
			return -1;

		OB_GETMSG(Msg_Addbot);

		int num = trap_BotAllocateClient(0);

		if(num < 0)
		{
			PrintError("Could not add bot!");
			PrintError("No free slots!");
			return -1;
		}

		char userinfo[MAX_INFO_STRING] = { 0 };

		std::stringstream guid;
		guid << "OMNIBOT" << std::setw(2) << std::setfill('0') << num << std::right << std::setw(23) << "";

		gentity_t *bot = &g_entities[num];

		Info_SetValueForKey(userinfo, "name", pMsg->m_Name);
		Info_SetValueForKey(userinfo, "rate", "25000");
		Info_SetValueForKey(userinfo, "snaps", "20");
		Info_SetValueForKey(userinfo, "ip", "localhost");
		Info_SetValueForKey(userinfo, "cg_etfversion", FORTS_VERSION);
		Info_SetValueForKey(userinfo, "cl_guid", guid.str().c_str());

		trap_SetUserinfo(num, userinfo);

		const char     *s = 0;
		if((s = ClientConnect(num, qtrue, qtrue)) != 0)
		{
			PrintError(va("Could not connect bot: %s", s));
			num = -1;
		}

		//bot->client->ps.persistant[PERS_FLAGS] |= PF_AUTORELOAD;
		//bot->client->pers.autoReload = 2;

		// bad hack to prevent unhandled errors being returned as successful connections
		return bot && bot->inuse ? num : -1;
	}

	void            RemoveBot(const MessageHelper & _data)
	{
		OB_GETMSG(Msg_Kickbot);
		if(pMsg->m_GameId != Msg_Kickbot::InvalidGameId)
		{
			if(pMsg->m_GameId >= 0 && pMsg->m_GameId < MAX_CLIENTS)
			{
				gentity_t      *ent = &g_entities[pMsg->m_GameId];

				if(IsBot(ent))
					G_Q3F_DropClient(ent, "disconnected");
			}
		}
		else
		{
			char cleanNetName[MAX_NETNAME];
			char cleanName[MAX_NAME_LENGTH];

			Q_strncpyz(cleanName, pMsg->m_Name, MAX_NAME_LENGTH);
			Q_CleanStr(cleanName);

			for(gentity_t *ent = g_entities; ent < &g_entities[level.maxclients]; ent++)
			{
				if(!ent->inuse || !ent->client || ent->client->pers.connected == CON_DISCONNECTED || !IsBot(ent))
					continue;

				// clean stuff
				Q_strncpyz(cleanNetName, ent->client->pers.netname, MAX_NETNAME);
				Q_CleanStr(cleanNetName);

				if(!Q_stricmp(cleanNetName, cleanName))
				{
					G_Q3F_DropClient(ent, "disconnected");
				}
			}
		}
	}

	obResult        ChangeTeam(int _client, int _newteam, const MessageHelper * _data)
	{
		gentity_t      *bot;

		bot = &g_entities[_client];

		_newteam = _teamBotToGame(_newteam);

		// If a team was specified, try to choose it.
		if(_newteam >= Q3F_TEAM_RED && _newteam <= Q3F_TEAM_GREEN)
			if(SetTeam(bot, g_q3f_teamlist[_newteam].name) == qtrue)
				return Success;

		// If there wasn't or there was a problem choosing it, get an auto assigned one.
		_newteam = G_Q3F_GetAutoTeamNum(_client);
		return SetTeam(bot, g_q3f_teamlist[_newteam].name) == qtrue ? Success : InvalidEntity;
	}

	obResult        ChangeClass(int _client, int _newclass, const MessageHelper * _data)
	{
		gentity_t      *bot;

		bot = &g_entities[_client];

		_newclass = playerClassBotToGame(_newclass);

		// If the class is 0, or we have trouble picking the specified class,
		// pick a random class
		if(_newclass < Q3F_CLASS_MAX)
		{
			if(_newclass == -1 || _newclass == 0 || !G_Q3F_ChangeClassCommand(bot, g_q3f_classlist[_newclass]->s->commandstring))
				_newclass = G_Q3F_SelectRandomClass(bot->client->sess.sessionTeam, bot);
		}


		if(!Q3F_IsSpectator(bot->client) && bot->health > 0 && bot->client->ps.persistant[PERS_CURRCLASS] != _newclass)
		{
#if 0
			if(bot->client->respawnDelayTime > level.time && G_Q3F_RespawnAreaCheck(bot->r.absmin, bot->r.absmax, bot->client->sess.sessionTeam))
				;// Nothing
			else
#endif
				Cmd_Kill_f(bot);
		}

		return G_Q3F_ChangeClassCommand(bot, g_q3f_classlist[_newclass]->s->commandstring) == qtrue ? Success : InvalidEntity;
	}

	bool            DebugLine(const float _start[3], const float _end[3], const obColor & _color, float _time)
	{
		// for dedicated servers we tell the bot we can handle this function, so it doesn't open
		// an IPC channel. 
		if(g_dedicated.integer)
			return true;

		return false;
	}

	bool            DebugRadius(const float _pos[3], const float _radius, const obColor & _color, float _time)
	{
		// for dedicated servers we tell the bot we can handle this function, so it doesn't open
		// an IPC channel. 
		if(g_dedicated.integer)
			return true;

		return false;
	}

	bool            DebugPolygon(const obVec3 * _verts, const int _numverts, const obColor & _color, float _time, int _flags)
	{
		// for dedicated servers we tell the bot we can handle this function, so it doesn't open
		// an IPC channel. 
		if(g_dedicated.integer)
			return true;

		return false;
	}

#define PM_Paralyzed( x ) ( (x) == PM_DEAD || (x) == PM_FREEZE || (x) == PM_INTERMISSION )

	void            UpdateBotInput(int _client, const ClientInput & _input)
	{
		static usercmd_t cmd;
		gentity_t *bot = &g_entities[_client];

		// only causes problems
		bot->client->ps.pm_flags &= ~PMF_RESPAWNED;

		memset(&cmd, 0, sizeof(cmd));

		cmd.serverTime = level.time;

		// Set the weapon
		cmd.weapon = _weaponBotToGame(_input.m_CurrentWeapon);

		// Process the bot keypresses.
		if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_ATTACK1))
			cmd.buttons |= BUTTON_ATTACK;
		if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_WALK))
			cmd.buttons |= BUTTON_WALKING;
		if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_USE))
			cmd.buttons |= BUTTON_USE_HOLDABLE;
		if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_RESPAWN))
			cmd.buttons |= BUTTON_ATTACK;

		if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_GREN1) && !m_GrenadePrime[_client].m_Throw)
		{
			m_GrenadePrime[_client].m_PrimeTime = level.time;
			m_GrenadePrime[_client].m_Throw = true;
			m_GrenadePrime[_client].m_GrenType = 1;
			m_GrenadePrime[_client].m_GrenClass = _grenadeTypeFromClass(1, bot->client->ps.persistant[PERS_CURRCLASS]);
		}
		else if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_GREN2) && !m_GrenadePrime[_client].m_Throw)
		{
			m_GrenadePrime[_client].m_PrimeTime = level.time;
			m_GrenadePrime[_client].m_Throw = true;
			m_GrenadePrime[_client].m_GrenType = 2;
			m_GrenadePrime[_client].m_GrenClass = _grenadeTypeFromClass(2, bot->client->ps.persistant[PERS_CURRCLASS]);
		}
		else if(!_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_GREN1) && !_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_GREN2) &&
				m_GrenadePrime[_client].m_GrenType && m_GrenadePrime[_client].m_Throw)
		{
			trap_EA_Command(_client,
								va("tgren %d %d %d", m_GrenadePrime[_client].m_GrenClass, m_GrenadePrime[_client].m_PrimeTime,
								   level.time));
			m_GrenadePrime[_client].m_PrimeTime = 0;
			m_GrenadePrime[_client].m_Throw = false;
			m_GrenadePrime[_client].m_GrenType = 0;
			m_GrenadePrime[_client].m_GrenClass = Q3F_GREN_NONE;
		}

		if(m_GrenadePrime[_client].m_PrimeTime && m_GrenadePrime[_client].m_GrenType &&
		   ((m_GrenadePrime[_client].m_PrimeTime + Q3F_GRENADE_PRIME_TIME - 60) <= level.time ||
			bot->client->ps.stats[STAT_HEALTH] <= 0))
		{
			trap_EA_Command(_client,
								va("tgren %d %d %d", m_GrenadePrime[_client].m_GrenClass, m_GrenadePrime[_client].m_PrimeTime,
								   level.time));
			m_GrenadePrime[_client].m_PrimeTime = 0;
			m_GrenadePrime[_client].m_Throw = false;
			m_GrenadePrime[_client].m_GrenType = 0;
			m_GrenadePrime[_client].m_GrenClass = Q3F_GREN_NONE;
		}

		if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_DROPITEM))
			trap_EA_Command(_client, "dropflag");
		if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_DROPAMMO))
			trap_EA_Command(_client, "discard");

		if(!(bot->client->ps.stats[STAT_Q3F_FLAGS] & (1 << FL_Q3F_BUILDING)) &&
		   !(bot->client->ps.stats[STAT_Q3F_FLAGS] & (1 << FL_Q3F_LAYCHARGE)))
		{
			if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_BUILDSENTRY))
				trap_EA_Command(_client, "build autosentry");
			if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_BUILDDISPENSER))
				trap_EA_Command(_client, "build supplystation");
			if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_AIMSENTRY))
				trap_EA_Command(_client, "aimsentry");
			if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_BUILDDETPACK_5))
				trap_EA_Command(_client, "charge 5");
			if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_BUILDDETPACK_10))
				trap_EA_Command(_client, "charge 10");
			if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_BUILDDETPACK_20))
				trap_EA_Command(_client, "charge 20");
			if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_BUILDDETPACK_30))
				trap_EA_Command(_client, "charge 30");
		}
		else
		{
			if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_CANCELBUILD))
			{
				if(bot->client->ps.stats[STAT_Q3F_FLAGS] & (1 << FL_Q3F_BUILDING))
					trap_EA_Command(_client, "build cancel");
				else if(bot->client->ps.stats[STAT_Q3F_FLAGS] & (1 << FL_Q3F_LAYCHARGE))
					trap_EA_Command(_client, "charge cancel");
			}
		}

		if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_DETPIPES))
		{
			if ( !level.ceaseFire && !bot->client->ps.powerups[PW_Q3F_CEASEFIRE] )
				G_Q3F_DetPipe(bot, level.time);
		}
		if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_DETSENTRY))
			trap_EA_Command(_client, "destroy autosentry");
		if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_DETDISPENSER))
			trap_EA_Command(_client, "destroy supplystation");

		if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_CALLFORMEDIC))
			trap_EA_Command(_client, "saveme");
		//if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_CALLFORENGY))
		//  trap_BotSendCommand(_client, "armorme");
		if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_CLOAK))
			trap_EA_Command(_client, "invisible");
		if(_input.m_ButtonFlags.CheckFlag(TF_BOT_BUTTON_RADAR))
			G_Q3F_ToggleScanner(bot);
		if(bot->client->ps.weaponstate >= WEAPON_RRAISING && bot->client->ps.weaponstate <= WEAPON_RELOADING)
		{
			// keep the same weapon while reloading
			cmd.weapon = bot->client->ps.weapon;
		}
		else if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_RELOAD) && bot->client->ps.weaponstate == WEAPON_READY)
		{
			BG_Q3F_Request_Reload(&bot->client->ps);
		}

		// don't process view angles and moving stuff when dead
		if(PM_Paralyzed(bot->client->ps.pm_type) || Q3F_IsSpectator(bot->client))
		{
			// cant move in these states
			m_GrenadePrime[_client].m_PrimeTime = m_GrenadePrime[_client].m_GrenType = m_GrenadePrime[_client].m_GrenClass = 0;
			m_GrenadePrime[_client].m_Throw = false;
		}
		else
		{
			float fMaxSpeed = 127.f;
			vec3_t angles, bodyangles, forward, right;

			// Convert the bots vector to angles and set the view angle to the orientation
			vectoangles(_input.m_Facing, angles);
			SetClientViewAngle(bot, angles);

			if(cmd.buttons & BUTTON_WALKING)
				fMaxSpeed = 64.f;

			// Convert the move direction into forward and right moves to
			// take the bots orientation into account.

			// flatten the view angles so we get a proper fwd,right vector as relevant to movement.
			VectorCopy(angles,bodyangles);
			bodyangles[PITCH] = 0;

			AngleVectors(bodyangles, forward, right, NULL);
			const float fwd = DotProduct(forward, _input.m_MoveDir);
			const float rght = DotProduct(right, _input.m_MoveDir);

			cmd.forwardmove = (char)(fwd * fMaxSpeed);
			cmd.rightmove = (char)(rght * fMaxSpeed);

			if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_FWD) || _input.m_ButtonFlags.CheckFlag(BOT_BUTTON_MOVEUP))
				cmd.forwardmove = (char)fMaxSpeed;
			if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_BACK) || _input.m_ButtonFlags.CheckFlag(BOT_BUTTON_MOVEDN))
				cmd.forwardmove = (char)-fMaxSpeed;
			if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_RSTRAFE))
				cmd.rightmove = (char)fMaxSpeed;
			if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_LSTRAFE))
				cmd.rightmove = (char)-fMaxSpeed;
			if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_JUMP))
				cmd.upmove = (char)fMaxSpeed;
			if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_CROUCH))
				cmd.upmove = (char)-fMaxSpeed;
		}
		trap_BotUserCommand(_client, &cmd);
	}

	void            BotCommand(int _client, const char *_cmd)
	{
		trap_EA_Command(_client, (char *)_cmd);
	}

	obBool          IsInPVS(const float _pos[3], const float _target[3])
	{
		return trap_InPVS(_pos, _target) ? True : False;
	}

	obResult        TraceLine(obTraceResult & _result, const float _start[3], const float _end[3],
							  const AABB * _pBBox, int _mask, int _user, obBool _bUsePVS)
	{
		trace_t         tr;
		int             iMask = 0;
		qboolean        bInPVS = _bUsePVS ? trap_InPVS(_start, _end) : qtrue;

		if(bInPVS)
		{
			// Set up the collision masks
			if(_mask & TR_MASK_ALL)
				iMask = MASK_ALL;
			else
			{
				if(_mask & TR_MASK_SOLID)
					iMask |= MASK_SOLID;
				if(_mask & TR_MASK_PLAYER)
					iMask |= MASK_PLAYERSOLID;
				if(_mask & TR_MASK_SHOT)
					iMask |= MASK_SHOT;
				if(_mask & TR_MASK_OPAQUE)
					iMask |= MASK_OPAQUE;
				if(_mask & TR_MASK_WATER)
					iMask |= MASK_WATER;
				if(_mask & TR_MASK_PLAYERCLIP)
					iMask |= CONTENTS_PLAYERCLIP;
				if(_mask & (TR_MASK_FLOODFILL|TR_MASK_FLOODFILLENT))
					iMask |= CONTENTS_PLAYERCLIP | CONTENTS_SOLID;

				/*if(_mask & TR_MASK_SMOKEBOMB)
				   {
				   gentity_t *pSmokeBlocker = Bot_EntInvisibleBySmokeBomb((float*)_start, (float*)_end);
				   if(pSmokeBlocker)
				   {
				   _result.m_Fraction = 0.0f;
				   _result.m_HitEntity = HandleFromEntity(pSmokeBlocker);
				   return Success;
				   }
				   } */
			}

			if(_mask & TR_MASK_FLOODFILL)
			{
				trap_TraceNoEnts(&tr, _start,
					_pBBox ? _pBBox->m_Mins : NULL,
					_pBBox ? _pBBox->m_Maxs : NULL,
					_end, _user, iMask);
			}
			else
			{
				trap_Trace(&tr, _start,
					_pBBox ? _pBBox->m_Mins : NULL,
					_pBBox ? _pBBox->m_Maxs : NULL,
					_end, _user, iMask);
			}

			if((tr.entityNum != ENTITYNUM_WORLD) && (tr.entityNum != ENTITYNUM_NONE))
				_result.m_HitEntity = HandleFromEntity(&g_entities[tr.entityNum]);
			else
				_result.m_HitEntity.Reset();

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
			_result.m_Contents = obUtilBotContentsFromGameContents(tr.contents);
			_result.m_Surface = obUtilBotSurfaceFromGameSurface(tr.surfaceFlags);
		}
		else
		{
			// Not in PVS
			_result.m_Fraction = 0.0f;
			_result.m_HitEntity.Reset();
		}
		return bInPVS ? Success : OutOfPVS;
	}

	int             GetPointContents(const float _pos[3])
	{
		vec3_t          vpos = { _pos[0], _pos[1], _pos[2] };
		int             iContents = trap_PointContents(vpos, -1);

		return obUtilBotContentsFromGameContents(iContents);
	}

	GameEntity      GetLocalGameEntity()
	{
		for(gentity_t *e = g_entities; e < &g_entities[level.num_entities]; e++)
		{
			if(e->inuse && e->client && e->client->pers.connected == CON_CONNECTED && !IsBot(e) && e->client->pers.localClient)
				return HandleFromEntity(e);
		}

		return EntityFromID(0);
	}

	GameEntity      FindEntityInSphere(const float _pos[3], float _radius, GameEntity _pStart, int classId)
	{
		// square it to avoid the square root in the distance check.
		gentity_t      *pStartEnt = _pStart.IsValid()? EntityFromHandle(_pStart) : 0;

		const char     *pClassName = 0;
		int             iPlayerClass = 0;
		int             iGrenadeClass = 0;

		//int iSpawnFlags = 0;
		//bool            buildable = false;

		switch (classId)
		{
			case TF_CLASS_SCOUT:
			case TF_CLASS_SNIPER:
			case TF_CLASS_SOLDIER:
			case TF_CLASS_DEMOMAN:
			case TF_CLASS_MEDIC:
			case TF_CLASS_HWGUY:
			case TF_CLASS_PYRO:
			case TF_CLASS_SPY:
			case TF_CLASS_ENGINEER:
			case TF_CLASS_CIVILIAN:
			case TF_CLASS_ANY:
				iPlayerClass = classId != TF_CLASS_ANY ? classId : 0;
				pClassName = "player";
				break;
				//////////////////////////////////////////////////////////////////////////
			case TF_CLASSEX_SENTRY:
				pClassName = "autosentry";
				//buildable = true;
				break;
			case TF_CLASSEX_DISPENSER:
				pClassName = "supplystation";
				//buildable = true;
				break;
			case TF_CLASSEX_DETPACK:
				pClassName = "charge";
				//buildable = true;
				break;
			case TF_CLASSEX_GRENADE:
			case TF_CLASSEX_EMP_GRENADE:
			case TF_CLASSEX_NAIL_GRENADE:
			case TF_CLASSEX_MIRV_GRENADE:
			case TF_CLASSEX_MIRVLET_GRENADE:
			case TF_CLASSEX_NAPALM_GRENADE:
			case TF_CLASSEX_GAS_GRENADE:
			case TF_CLASSEX_CONC_GRENADE:
			case ETF_CLASSEX_FLASH_GRENADE:	//flashbang
				iGrenadeClass = _grenadeBotToGame(classId);
				pClassName = "handgrenade";
				break;
			case TF_CLASSEX_PIPE:
				pClassName = "pipe";
				break;
			case TF_CLASSEX_GLGRENADE:
				pClassName = "grenade";
				break;
			case TF_CLASSEX_ROCKET:
				pClassName = "rocket";
				break;
		}

		if(pClassName)
		{
			float           fSqRad = _radius * _radius;
			vec3_t          toent;

			while((pStartEnt = G_Find(pStartEnt, FOFS(classname), pClassName)) != NULL)
			{
				if(iPlayerClass && pStartEnt->client && pStartEnt->client->ps.persistant[PERS_CURRCLASS] != iPlayerClass)
					continue;

				//if(iSpawnFlags && !(pStartEnt->spawnflags & iSpawnFlags))
				//  continue;

				if(iGrenadeClass && pStartEnt->s.eType == ET_Q3F_GRENADE && pStartEnt->s.weapon != iGrenadeClass)
					continue;

				if(classId == TF_CLASSEX_SENTRY && pStartEnt->s.eType == ET_Q3F_SENTRY && !pStartEnt->s.legsAnim)
					continue;

				if(classId == TF_CLASSEX_DISPENSER && pStartEnt->s.eType == ET_Q3F_SUPPLYSTATION && !pStartEnt->s.legsAnim)
					continue;

				if(classId == TF_CLASSEX_DETPACK && pStartEnt->s.eType == ET_Q3F_GRENADE && pStartEnt->s.weapon == Q3F_GREN_CHARGE
				   && !pStartEnt->s.legsAnim)
					continue;

				VectorSubtract(_pos, pStartEnt->r.currentOrigin, toent);
				if(VectorLengthSquared(toent) < fSqRad)
					break;
			}
			return HandleFromEntity(pStartEnt);
		}
		return GameEntity();
	}

	int             GetEntityClass(const GameEntity _ent)
	{
		gentity_t      *pEnt = EntityFromHandle(_ent);

		return pEnt && pEnt->inuse ? _GetEntityClass(pEnt) : TF_CLASS_NONE;
	}

	/*int             GetMoverState(const GameEntity _ent)
	{
		gentity_t      *pEnt = EntityFromHandle(_ent);

		if(pEnt && pEnt->inuse && pEnt->s.eType == ET_MOVER && !Q_stricmpn(pEnt->classname, "func_", strlen("func_")))
		{
			switch(pEnt->moverState)
			{
			case MOVER_POS1:
			case MOVER_POS2:
				return MOVERSTATE_STATIONARY;
			case MOVER_1TO2:
			case MOVER_2TO1:
				return MOVERSTATE_MOVING;
			}
		}

		return MOVERSTATE_INVALID;

	}*/

	obResult        GetEntityCategory(const GameEntity _ent, BitFlag32 & _category)
	{
		obResult        res = Success;
		gentity_t      *pEnt = EntityFromHandle(_ent);

		if(!pEnt)
			return InvalidEntity;

		if(!hasptr)
		{
			G_Q3F_AddString(&teamscoreptr, "teamscore");
			hasptr = qtrue;
		}

		// hack, when the game joins clients again after warmup, they are temporarily ET_GENERAL entities(LAME)
		int             t = pEnt->s.eType;

		if(pEnt->client && (pEnt - g_entities) < MAX_CLIENTS)
			t = ET_PLAYER;

		switch (t)
		{
			case ET_GENERAL:
			{
				if(!Q_stricmp(pEnt->classname, "func_button"))	/* func_button */
				{
					_category.SetFlag(ENT_CAT_TRIGGER);
					_category.SetFlag(ENT_CAT_STATIC);
				}
				else if(!Q_stricmp(pEnt->classname, "trigger_multiple") && pEnt->mapdata && pEnt->mapdata->give && pEnt->mapdata->other)	/* trigger_multiple */
				{
					q3f_keypair_t  *data = G_Q3F_KeyPairArrayFind(pEnt->mapdata->other, teamscoreptr);

					if(data && data->value.d.intdata)
						_category.SetFlag(ENT_CAT_TRIGGER);
					else
						res = InvalidEntity;
				}
				// cs: waypoint tool, don't merge the spawns
				else if(!Q_stricmp(pEnt->classname, "info_player_start") || /* info_player_start */
					    !Q_stricmp(pEnt->classname, "info_player_deathmatch")) /* info_player_deathmatch */
				{
					// don't fill up the bots sensory mem at start with these
					_category.SetFlag(ENT_CAT_INTERNAL);
				}
				else
					res = InvalidEntity;
				break;
			}
			case ET_PLAYER:
			{
				if(!pEnt->client ||
				   (pEnt->client->sess.sessionTeam != Q3F_TEAM_RED &&
					pEnt->client->sess.sessionTeam != Q3F_TEAM_BLUE &&
					pEnt->client->sess.sessionTeam != Q3F_TEAM_YELLOW &&
					pEnt->client->sess.sessionTeam != Q3F_TEAM_GREEN) || Q3F_IsSpectator(pEnt->client))
				{
					res = InvalidEntity;
					break;
				}

				_category.SetFlag(ENT_CAT_SHOOTABLE);
				_category.SetFlag(ENT_CAT_PLAYER);
				if(pEnt->client->ps.eFlags & EF_Q3F_DISEASED)
				{
					_category.SetFlag(ENT_CAT_AVOID);
					_category.SetFlag(ENT_CAT_OBSTACLE);
				}
				break;
			}
			case ET_INVISIBLE:
			{
				//if(!Q_stricmp(pEnt->classname, "func_goalinfo") && (pEnt->mapdata && pEnt->mapdata->give))
				if((!Q_stricmp(pEnt->classname, "func_goalinfo") && (pEnt->mapdata && pEnt->mapdata->give)) || (!Q_stricmp(pEnt->classname, "func_goalitem") && pEnt->model && pEnt->model[0]))	/* func_goalinfo */ /* func_goalitem */
				{
					res = InvalidEntity;
					//_category.SetFlag(ENT_CAT_PICKUP);
					//_category.SetFlag(ENT_CAT_STATIC);
				}
				break;
			}
			case ET_Q3F_GOAL:
			{
				// Will be ET_INVISIBLE if hidden
				//if(!Q_stricmp(pEnt->classname, "func_goalinfo") && (pEnt->mapdata && pEnt->mapdata->give))
				if((!Q_stricmp(pEnt->classname, "func_goalinfo") && (pEnt->mapdata && pEnt->mapdata->give)) || (!Q_stricmp(pEnt->classname, "func_goalitem") && pEnt->model && pEnt->model[0]))	/* func_goalinfo */ /* func_goalitem */
				{
					if(!Q_stricmp(pEnt->classname, "func_goalinfo") && pEnt->mapdata && pEnt->mapdata->give && pEnt->mapdata->other)
					{
						q3f_keypair_t  *data = G_Q3F_KeyPairArrayFind(pEnt->mapdata->other, teamscoreptr);

						if(data && data->value.d.intdata)
						{
							_category.SetFlag(ENT_CAT_TRIGGER);
							break;
						}
					}

					_category.SetFlag(ENT_CAT_PICKUP);
					_category.SetFlag(ENT_CAT_STATIC);
					// Assume it's everything for now
					if(!Q_stricmp(pEnt->classname, "func_goalinfo") && (pEnt->mapdata && pEnt->mapdata->give))
					{
						_category.SetFlag(ENT_CAT_PICKUP_AMMO);
						_category.SetFlag(ENT_CAT_PICKUP_HEALTH);
						_category.SetFlag(ENT_CAT_PICKUP_ARMOR);
					}
					/*if(pEnt->mapdata && pEnt->mapdata->give && pEnt->mapdata->give->data->value.d.strdata)
					{
						if(Q_stristr(pEnt->mapdata->give->data->value.d.strdata, "ammo_"))
							_category.SetFlag(ENT_CAT_PICKUP_AMMO);
						if(Q_stristr(pEnt->mapdata->give->data->value.d.strdata, "gren1"))
							_category.SetFlag(ENT_CAT_PICKUP_AMMO);
						if(Q_stristr(pEnt->mapdata->give->data->value.d.strdata, "gren2"))
							_category.SetFlag(ENT_CAT_PICKUP_AMMO);
						if(Q_stristr(pEnt->mapdata->give->data->value.d.strdata, "health"))
							_category.SetFlag(ENT_CAT_PICKUP_HEALTH);
						if(Q_stristr(pEnt->mapdata->give->data->value.d.strdata, "armor"))
							_category.SetFlag(ENT_CAT_PICKUP_ARMOR);
					}*/
				}
				break;
			}
			case ET_ITEM:
			{
				if( !(pEnt->r.contents & CONTENTS_TRIGGER) )
				{
					res = InvalidEntity;
				}
				else if(pEnt->item && pEnt->item->giType == IT_HEALTH)
				{
					_category.SetFlag(ENT_CAT_PICKUP);
					_category.SetFlag(ENT_CAT_PICKUP_HEALTH);
				}
				else if(pEnt->item && pEnt->item->giType == IT_ARMOR)
				{
					_category.SetFlag(ENT_CAT_PICKUP);
					_category.SetFlag(ENT_CAT_PICKUP_ARMOR);
				}
				else if(pEnt->item && pEnt->item->giType == IT_Q3F_BACKPACK)
				{
					_category.SetFlag(ENT_CAT_PICKUP);
					_category.SetFlag(ENT_CAT_PICKUP_AMMO);
				}
				else if(pEnt->item && pEnt->item->giType == IT_Q3F_AMMOBOX)
				{
					_category.SetFlag(ENT_CAT_PICKUP);
					_category.SetFlag(ENT_CAT_PICKUP_AMMO);
				}
#if 0
				else if(pEnt->item && pEnt->item->giType == IT_Q3F_DEBRIS)
				{
					_category.SetFlag(ENT_CAT_PICKUP);
					_category.SetFlag(ENT_CAT_PICKUP_AMMO);
				}
				else if(pEnt->item && pEnt->item->giType == IT_Q3F_DEBRIS2)
				{
					_category.SetFlag(ENT_CAT_PICKUP);
					_category.SetFlag(ENT_CAT_PICKUP_AMMO);
				}
#endif
				else
				{
					res = InvalidEntity;
				}
				//_category.SetFlag(ENT_CAT_PICKUP);
				//_category.SetFlag(ENT_CAT_STATIC);
				break;
			}
				/*case ET_CORPSE:
				   {
				   _category.SetFlag(ENT_CAT_MISC);
				   break;
				   } */
			case ET_MISSILE:
			{
				// Register certain weapons as threats to avoid or whatever.
				switch (pEnt->s.weapon)
				{
					case WP_GRENADE_LAUNCHER:
					case WP_ROCKET_LAUNCHER:
					case WP_PIPELAUNCHER:
					case WP_NAPALMCANNON:
						_category.SetFlag(ENT_CAT_AVOID);
						_category.SetFlag(ENT_CAT_PROJECTILE);
						break;
					default:
						res = InvalidEntity;
				}
				break;
			}
			case ET_Q3F_GRENADE:
			{
				// Register certain grenades as threats to avoid or whatever.
				switch (pEnt->s.weapon)
				{
					//case Q3F_GREN_CHARGE:
					case Q3F_GREN_CONCUSS:
					case Q3F_GREN_FLASH:
					case Q3F_GREN_CLUSTER:
					case Q3F_GREN_CLUSTERSECTION:
					case Q3F_GREN_EMP:
					case Q3F_GREN_NAIL:
					case Q3F_GREN_NAPALM:
					case Q3F_GREN_NORMAL:
					case Q3F_GREN_GAS:
						_category.SetFlag(ENT_CAT_AVOID);
						_category.SetFlag(ENT_CAT_PROJECTILE);
						break;
					case Q3F_GREN_CHARGE:
						_category.SetFlag(ENT_CAT_AVOID);
						_category.SetFlag(ENT_CAT_PROJECTILE);
						_category.SetFlag(ENT_CAT_OBSTACLE);
						break;
				}
				break;
			}
			case ET_Q3F_SENTRY:
			{
				_category.SetFlag(ENT_CAT_SHOOTABLE);
				_category.SetFlag(TF_ENT_CAT_BUILDABLE);
				_category.SetFlag(ENT_CAT_AVOID);
				_category.SetFlag(ENT_CAT_OBSTACLE);
				break;
			}
			case ET_Q3F_SUPPLYSTATION:
			{
				_category.SetFlag(ENT_CAT_SHOOTABLE);
				_category.SetFlag(TF_ENT_CAT_BUILDABLE);
				_category.SetFlag(ENT_CAT_AVOID);
				_category.SetFlag(ENT_CAT_OBSTACLE);
				_category.SetFlag(ENT_CAT_STATIC);
			}
			default:
				res = InvalidEntity;
				break;			// ignore this type.
		};
		return res;
	}

	obResult        GetEntityFlags(const GameEntity _ent, BitFlag64 & _flags)
	{
		gentity_t      *pEnt = EntityFromHandle(_ent);

		if(pEnt && pEnt->inuse)
		{
			// hack, when the game joins clients again after warmup, they are temporarily ET_GENERAL entities(LAME)
			int             t = pEnt->s.eType;

			if(pEnt->client && (pEnt - g_entities) < MAX_CLIENTS)
				t = ET_PLAYER;

			switch (t)
			{
				case ET_PLAYER:
				{
					if(pEnt->client)
					{
						_flags.SetFlag(ENT_FLAG_VISTEST);
						if(!IsBot(pEnt))
							_flags.SetFlag(ENT_FLAG_HUMANCONTROLLED);
						if(pEnt->health <= 0 || (pEnt->client->ps.eFlags & EF_DEAD) ||
						   pEnt->client->ps.persistant[PERS_CURRCLASS] == Q3F_CLASS_NULL ||
						   pEnt->client->sess.sessionClass == Q3F_CLASS_NULL || (pEnt->flags & FL_NOTARGET) ||
						   pEnt->client->noclip)
							_flags.SetFlag(ENT_FLAG_DEAD);
						if(pEnt->client->ps.pm_flags & PMF_LADDER)
							_flags.SetFlag(ENT_FLAG_ONLADDER);
						if(pEnt->client->ps.pm_flags & PMF_DUCKED)
							_flags.SetFlag(ENT_FLAG_CROUCHED);
						if(pEnt->client->ps.groundEntityNum != ENTITYNUM_NONE)
							_flags.SetFlag(ENT_FLAG_ONGROUND);
						if(pEnt->client->ps.weaponstate == WEAPON_RELOADING)
							_flags.SetFlag(ENT_FLAG_RELOADING);
						if((pEnt->client->ps.eFlags & EF_Q3F_AIMING) && pEnt->client->ps.weapon == WP_SNIPER_RIFLE)
							_flags.SetFlag(ENT_FLAG_AIMING);
							//_flags.SetFlag(ENT_FLAG_IRONSIGHT);
						if((pEnt->client->ps.eFlags & EF_Q3F_AIMING) && pEnt->client->ps.weapon == WP_MINIGUN)
							_flags.SetFlag(ENT_FLAG_AIMING);
							//_flags.SetFlag(TF_ENT_FLAG_ASSAULTFIRING);

						if(pEnt->client->ps.eFlags & EF_Q3F_SAVEME)
							_flags.SetFlag(TF_ENT_FLAG_SAVEME);
						if(pEnt->client->ps.eFlags & EF_Q3F_ARMORME)
							_flags.SetFlag(TF_ENT_FLAG_ARMORME);
						if(pEnt->client->ps.extFlags & EXTF_BURNING)
							_flags.SetFlag(TF_ENT_FLAG_BURNING);
						if((pEnt->client->ps.extFlags & EXTF_TRANQED))
							_flags.SetFlag(TF_ENT_FLAG_TRANQED);
						if(pEnt->client->ps.powerups[PW_Q3F_FLASH])
							_flags.SetFlag(ETF_ENT_FLAG_BLIND);
						if(pEnt->client->ps.powerups[PW_Q3F_GAS])
							_flags.SetFlag(TF_ENT_FLAG_GASSED);
						if(pEnt->client->legwounds)
							_flags.SetFlag(TF_ENT_FLAG_LEGSHOT);
						if(G_Q3F_IsDisguised(pEnt))
							_flags.SetFlag(ETF_ENT_FLAG_DISGUISED);

						if(pEnt->client->ps.stats[STAT_Q3F_FLAGS] & (1 << FL_Q3F_BUILDING))
						{
							if(pEnt->client->sentry && !pEnt->client->sentry->s.legsAnim)
								_flags.SetFlag(TF_ENT_FLAG_BUILDING_SG);
							else if(pEnt->client->supplystation && !pEnt->client->supplystation->s.legsAnim)
								_flags.SetFlag(TF_ENT_FLAG_BUILDING_DISP);
						}
						if(pEnt->client->ps.stats[STAT_Q3F_FLAGS] & (1 << FL_Q3F_LAYCHARGE) && pEnt->client->chargeEntity &&
						   !pEnt->client->chargeEntity->s.legsAnim)
							_flags.SetFlag(TF_ENT_FLAG_BUILDING_DETP);
						/*if(pEnt->client->ps.stats[STAT_Q3F_FLAGS] & (1 << FL_Q3F_LAYCLAYMORE) && pEnt->client->claymoreEntity &&
						   !pEnt->client->claymoreEntity->s.legsAnim)
							_flags.SetFlag(OF_ENT_FLAG_BUILDING_PROXMINE);
						*/
					}
					break;
				}
				case ET_INVISIBLE:
				case ET_Q3F_GOAL:
				{
					if((!Q_stricmp(pEnt->classname, "func_goalinfo") && (pEnt->mapdata && pEnt->mapdata->give)) || (!Q_stricmp(pEnt->classname, "func_goalitem") && pEnt->model && pEnt->model[0]))	/* func_goalinfo */ /* func_goalitem */
					{
						_flags.SetFlag(ENT_FLAG_DISABLED);
					}
					break;
				}
				case ET_ITEM:
				{
					_flags.SetFlag(ENT_FLAG_VISTEST);
					if( !(pEnt->r.contents & CONTENTS_TRIGGER) )
					{
						_flags.SetFlag(ENT_FLAG_DISABLED);
					}
				}
				case ET_MISSILE:
				{
					// Register certain weapons as threats to avoid or whatever.
					switch (pEnt->s.weapon)
					{
						case WP_GRENADE_LAUNCHER:
						case WP_ROCKET_LAUNCHER:
						case WP_PIPELAUNCHER:
						case WP_NAPALMCANNON:
						default:
							_flags.SetFlag(ENT_FLAG_VISTEST);
					}
					break;
				}
				case ET_MOVER:
				{
					_flags.SetFlag(ENT_FLAG_VISTEST);
					break;
				}
				case ET_Q3F_GRENADE:
				{
					// Register certain grenades as threats to avoid or whatever.
					switch (pEnt->s.weapon)
					{
						case Q3F_GREN_CHARGE:
						{
							if(!pEnt->s.legsAnim)
								_flags.SetFlag(TF_ENT_FLAG_BUILDINPROGRESS);
							_flags.SetFlag(ENT_FLAG_VISTEST);
							break;
						}
						case Q3F_GREN_CONCUSS:
						case Q3F_GREN_FLASH:
						case Q3F_GREN_CLUSTER:
						case Q3F_GREN_CLUSTERSECTION:
						case Q3F_GREN_EMP:
						case Q3F_GREN_NAIL:
						case Q3F_GREN_NAPALM:
						case Q3F_GREN_GAS:
						default:
							_flags.SetFlag(ENT_FLAG_VISTEST);
							break;
					}
					break;
				}
				case ET_Q3F_SENTRY:
				case ET_Q3F_SUPPLYSTATION:
				{
					_flags.SetFlag(ENT_FLAG_VISTEST);
					if(!pEnt->s.legsAnim)
					{
						_flags.SetFlag(TF_ENT_FLAG_BUILDINPROGRESS);
						_flags.SetFlag(ENT_FLAG_DISABLED);
					}
					if(t == ET_Q3F_SENTRY && pEnt->s.legsAnim == 2)
						_flags.SetFlag(TF_ENT_FLAG_LEVEL2);
					else if(t == ET_Q3F_SENTRY && pEnt->s.legsAnim == 3)
						_flags.SetFlag(TF_ENT_FLAG_LEVEL3);
					break;
				}
				case ET_GENERAL:
				{
					break;
				}
			}

			if(pEnt->waterlevel >= 3)
				_flags.SetFlag(ENT_FLAG_UNDERWATER);
			else if(pEnt->waterlevel > 0)
				_flags.SetFlag(ENT_FLAG_INWATER);
			if(t != ET_PLAYER && pEnt->s.groundEntityNum != ENTITYNUM_NONE)
				_flags.SetFlag(ENT_FLAG_ONGROUND);
		}
		return Success;
	}

	obResult        GetEntityPowerups(const GameEntity _ent, BitFlag64 & _flags)
	{
		gentity_t      *pEnt = EntityFromHandle(_ent);

		if(pEnt && pEnt->inuse && pEnt->client)
		{
			if(pEnt->client->ps.persistant[PERS_CURRCLASS] == Q3F_CLASS_AGENT)
			{
				if(G_Q3F_IsDisguised(pEnt))
				{
					switch (pEnt->client->agentteam)
					{
						case 0:
							// No team set is same as disguised as current team
							switch (pEnt->client->sess.sessionTeam)
							{
								case Q3F_TEAM_RED:
									_flags.SetFlag(TF_PWR_DISGUISE_RED);
									break;
								case Q3F_TEAM_BLUE:
									_flags.SetFlag(TF_PWR_DISGUISE_BLUE);
									break;
								case Q3F_TEAM_YELLOW:
									_flags.SetFlag(TF_PWR_DISGUISE_YELLOW);
									break;
								case Q3F_TEAM_GREEN:
									_flags.SetFlag(TF_PWR_DISGUISE_GREEN);
									break;
							}
							break;
						case Q3F_TEAM_RED:
							_flags.SetFlag(TF_PWR_DISGUISE_RED);
							break;
						case Q3F_TEAM_BLUE:
							_flags.SetFlag(TF_PWR_DISGUISE_BLUE);
							break;
						case Q3F_TEAM_YELLOW:
							_flags.SetFlag(TF_PWR_DISGUISE_YELLOW);
							break;
						case Q3F_TEAM_GREEN:
							_flags.SetFlag(TF_PWR_DISGUISE_GREEN);
							break;
					}
					switch (pEnt->client->agentclass)
					{
						case Q3F_CLASS_RECON:
							_flags.SetFlag(TF_PWR_DISGUISE_SCOUT);
							break;
						case Q3F_CLASS_SNIPER:
							_flags.SetFlag(TF_PWR_DISGUISE_SNIPER);
							break;
						case Q3F_CLASS_SOLDIER:
							_flags.SetFlag(TF_PWR_DISGUISE_SOLDIER);
							break;
						case Q3F_CLASS_GRENADIER:
							_flags.SetFlag(TF_PWR_DISGUISE_DEMOMAN);
							break;
						case Q3F_CLASS_PARAMEDIC:
							_flags.SetFlag(TF_PWR_DISGUISE_MEDIC);
							break;
						case Q3F_CLASS_MINIGUNNER:
							_flags.SetFlag(TF_PWR_DISGUISE_HWGUY);
							break;
						case Q3F_CLASS_FLAMETROOPER:
							_flags.SetFlag(TF_PWR_DISGUISE_PYRO);
							break;
						case 0:
						case Q3F_CLASS_AGENT:
							// No class set is same as disguised as spy
							_flags.SetFlag(TF_PWR_DISGUISE_SPY);
							break;
						case Q3F_CLASS_ENGINEER:
							_flags.SetFlag(TF_PWR_DISGUISE_ENGINEER);
							break;
						case Q3F_CLASS_CIVILIAN:
							_flags.SetFlag(TF_PWR_DISGUISE_CIVILIAN);
							break;
					}
				}
				if(pEnt->client->ps.eFlags & EF_Q3F_INVISIBLE)
					_flags.SetFlag(TF_PWR_CLOAKED);
			}
		}
		return Success;
	}

	obResult        GetEntityEyePosition(const GameEntity _ent, float _pos[3])
	{
		if(GetEntityPosition(_ent, _pos) == Success)
		{
			gentity_t      *pEnt = EntityFromHandle(_ent);

			if(pEnt && pEnt->client)
			{
				if(Q3F_IsSpectator(pEnt->client) &&
					(pEnt->client->sess.spectatorState == SPECTATOR_FOLLOW || pEnt->client->sess.spectatorState == SPECTATOR_CHASE))
				{
					gentity_t      *pEntOther = INDEXENT(pEnt->client->sess.spectatorClient);

					_pos[2] += pEntOther->client->ps.viewheight;
				}
				else
				{
					_pos[2] += pEnt->client->ps.viewheight;
				}
			}
			return Success;
		}
		return InvalidEntity;
	}

	obResult        GetEntityBonePosition(const GameEntity _ent, int _boneid, float _pos[3])
	{
		// OF doesnt really support bones
		return GetEntityPosition(_ent, _pos);
	}

	obResult        GetEntityOrientation(const GameEntity _ent, float _fwd[3], float _right[3], float _up[3])
	{
		gentity_t      *pEnt = EntityFromHandle(_ent);

		if(pEnt && pEnt->inuse)
		{
			if(pEnt->client)
			{
				if(Q3F_IsSpectator(pEnt->client) &&
					(pEnt->client->sess.spectatorState == SPECTATOR_FOLLOW || pEnt->client->sess.spectatorState == SPECTATOR_CHASE))
				{
					gentity_t      *pEntOther = INDEXENT(pEnt->client->sess.spectatorClient);

					AngleVectors(pEntOther->client->ps.viewangles, _fwd, _right, _up);
				}
				else
				{
					AngleVectors(pEnt->client->ps.viewangles, _fwd, _right, _up);
				}
			}
			else
			{
				AngleVectors(pEnt->r.currentAngles, _fwd, _right, _up);
			}
			return Success;
		}
		return InvalidEntity;
	}

	obResult        GetEntityVelocity(const GameEntity _ent, float _velocity[3])
	{
		gentity_t      *pEnt = EntityFromHandle(_ent);

		if(pEnt && pEnt->inuse)
		{
			// hack, when the game joins clients again after warmup, they are temporarily ET_GENERAL entities(LAME)
			int             t = pEnt->s.eType;

			if(pEnt->client && (pEnt - g_entities) < MAX_CLIENTS)
				t = ET_PLAYER;

			if(t == ET_MOVER)
			{
				// Calculate the velocity ourselves. for some reason mover velocity
				// isn't in s.pos.trDelta
				const float     fDeltaTime = 50.0f;	// get this dynamically?

				_velocity[0] = (pEnt->r.currentOrigin[0] - pEnt->oldOrigin[0]) * fDeltaTime;
				_velocity[1] = (pEnt->r.currentOrigin[1] - pEnt->oldOrigin[1]) * fDeltaTime;
				_velocity[2] = (pEnt->r.currentOrigin[2] - pEnt->oldOrigin[2]) * fDeltaTime;
			}
			else
			{
				if(pEnt->client && pEnt->client->pers.connected == CON_CONNECTED && Q3F_IsSpectator(pEnt->client) &&
					(pEnt->client->sess.spectatorState == SPECTATOR_FOLLOW || pEnt->client->sess.spectatorState == SPECTATOR_CHASE))
				{
					gentity_t      *pEntOther = INDEXENT(pEnt->client->sess.spectatorClient);

					_velocity[0] = pEntOther->s.pos.trDelta[0];
					_velocity[1] = pEntOther->s.pos.trDelta[1];
					_velocity[2] = pEntOther->s.pos.trDelta[2];

					return Success;
				}

				_velocity[0] = pEnt->s.pos.trDelta[0];
				_velocity[1] = pEnt->s.pos.trDelta[1];
				_velocity[2] = pEnt->s.pos.trDelta[2];
			}
			return Success;
		}
		return InvalidEntity;
	}

	obResult        GetEntityPosition(const GameEntity _ent, float _pos[3])
	{
		gentity_t      *pEnt = EntityFromHandle(_ent);

		if(pEnt && pEnt->inuse)
		{
			if(pEnt->client && pEnt->client->pers.connected == CON_CONNECTED && Q3F_IsSpectator(pEnt->client) &&
				(pEnt->client->sess.spectatorState == SPECTATOR_FOLLOW || pEnt->client->sess.spectatorState == SPECTATOR_CHASE))
			{
				gentity_t      *pEntOther = INDEXENT(pEnt->client->sess.spectatorClient);

				_pos[0] = pEntOther->r.currentOrigin[0];
				_pos[1] = pEntOther->r.currentOrigin[1];
				_pos[2] = pEntOther->r.currentOrigin[2];

				return Success;
			}

			if(!pEnt->client)
			{
				vec3_t axis[3];
				//AngleVectors(pEnt->r.currentAngles, axis[0], axis[1], axis[2]);
				AnglesToAxis(pEnt->r.currentAngles, axis);

				vec3_t boxCenter;
				boxCenter[0] = ((pEnt->r.maxs[0] + pEnt->r.mins[0]) * 0.5f);
				boxCenter[1] = ((pEnt->r.maxs[1] + pEnt->r.mins[1]) * 0.5f);
				boxCenter[2] = ((pEnt->r.maxs[2] + pEnt->r.mins[2]) * 0.5f);

				vec3_t out;
				VectorCopy(pEnt->r.currentOrigin,out);
				for(int i = 0; i < 3; ++i) {
					vec3_t tmp;
					VectorScale(axis[i], boxCenter[i], tmp);
					VectorAdd(out, tmp, out);
				}
				VectorCopy(out, _pos);
				return Success;
			}

			// Clients and entities not caught above will return normal position.
			_pos[0] = pEnt->r.currentOrigin[0];
			_pos[1] = pEnt->r.currentOrigin[1];
			_pos[2] = pEnt->r.currentOrigin[2];

			return Success;
		}
		return InvalidEntity;
	}

	obResult GetEntityLocalAABB(const GameEntity _ent, AABB &_aabb)
	{
		gentity_t *pEnt = EntityFromHandle(_ent);
		if (pEnt && pEnt->inuse)
		{
			if(pEnt->client && pEnt->client->pers.connected == CON_CONNECTED && Q3F_IsSpectator(pEnt->client) &&
				(pEnt->client->sess.spectatorState == SPECTATOR_FOLLOW || pEnt->client->sess.spectatorState == SPECTATOR_CHASE))
			{
				gentity_t      *pEntOther = INDEXENT(pEnt->client->sess.spectatorClient);

				_aabb.m_Mins[0] = pEntOther->r.mins[0];
				_aabb.m_Mins[1] = pEntOther->r.mins[1];
				_aabb.m_Mins[2] = pEntOther->r.mins[2];
				_aabb.m_Maxs[0] = pEntOther->r.maxs[0];
				_aabb.m_Maxs[1] = pEntOther->r.maxs[1];
				_aabb.m_Maxs[2] = pEntOther->r.maxs[2];

				return Success;
			}

#if 0
			const unsigned int uiName = BG_Hash32(pEnt->classname);

			// Need to do special case stuff for certain entities.
			if(uiName == 0x677d95c8 ||	/* trigger_multiple */
			   uiName == 0xa38051b1 ||	/* trigger_always */
			   uiName == 0xf4a58870 ||	/* trigger_push */
			   uiName == 0xd0d4dc0f ||	/* trigger_teleport */
			   uiName == 0x4a9dd745 ||	/* trigger_hurt */
			   uiName == 0xc590d065 ||	/* func_forcefield */
			   uiName == 0x9ede7ffd ||	/* func_commandpoint */
			   uiName == 0xa813f2d7 ||	/* plat_trigger */
			   uiName == 0x6f0f3f82)	/* door_trigger */
				/*if (!Q_stricmp(pEnt->classname, "trigger_multiple") || 
				   !Q_stricmp(pEnt->classname, "func_commandpoint") ||
				   !Q_stricmp(pEnt->classname, "plat_trigger") ||
				   !Q_stricmp(pEnt->classname, "door_trigger")) */
			{
				// find the midpt
				vec3_t pos;
				pos[0] = pEnt->r.currentOrigin[0] + ((pEnt->r.maxs[0] + pEnt->r.mins[0]) * 0.5f);
				pos[1] = pEnt->r.currentOrigin[1] + ((pEnt->r.maxs[1] + pEnt->r.mins[1]) * 0.5f);
				pos[2] = pEnt->r.currentOrigin[2] + ((pEnt->r.maxs[2] + pEnt->r.mins[2]) * 0.5f);

				// figure out the local bounds from there
				_aabb.m_Mins[0] = pEnt->r.mins[0] - pos[0];
				_aabb.m_Mins[1] = pEnt->r.mins[1] - pos[1];
				_aabb.m_Mins[2] = pEnt->r.mins[2] - pos[2];
				_aabb.m_Maxs[0] = pEnt->r.maxs[0] - pos[0];
				_aabb.m_Maxs[1] = pEnt->r.maxs[1] - pos[1];
				_aabb.m_Maxs[2] = pEnt->r.maxs[2] - pos[2];
				return Success;
			}
#endif

			_aabb.m_Mins[0] = pEnt->r.mins[0];
			_aabb.m_Mins[1] = pEnt->r.mins[1];
			_aabb.m_Mins[2] = pEnt->r.mins[2];
			_aabb.m_Maxs[0] = pEnt->r.maxs[0];
			_aabb.m_Maxs[1] = pEnt->r.maxs[1];
			_aabb.m_Maxs[2] = pEnt->r.maxs[2];
			return Success;
		}
		return InvalidEntity;
	}
	obResult        GetEntityWorldAABB(const GameEntity _ent, AABB & _aabb)
	{
		gentity_t      *pEnt = EntityFromHandle(_ent);

		if(pEnt && pEnt->inuse)
		{
			if(pEnt->client && pEnt->client->pers.connected == CON_CONNECTED && Q3F_IsSpectator(pEnt->client) &&
				(pEnt->client->sess.spectatorState == SPECTATOR_FOLLOW || pEnt->client->sess.spectatorState == SPECTATOR_CHASE))
			{
				gentity_t      *pEntOther = INDEXENT(pEnt->client->sess.spectatorClient);

				_aabb.m_Mins[0] = pEntOther->r.absmin[0];
				_aabb.m_Mins[1] = pEntOther->r.absmin[1];
				_aabb.m_Mins[2] = pEntOther->r.absmin[2];
				_aabb.m_Maxs[0] = pEntOther->r.absmax[0];
				_aabb.m_Maxs[1] = pEntOther->r.absmax[1];
				_aabb.m_Maxs[2] = pEntOther->r.absmax[2];

				// raise player bounds slightly since it appears to be in the ground a bit
				if ( pEntOther->client )
				{
					_aabb.m_Mins[2] += 2.f;
					_aabb.m_Maxs[2] += 2.f;
				}

				return Success;
			}

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

			return Success;
		}
		return InvalidEntity;
	}

	obResult GetEntityWorldOBB(const GameEntity _ent, float *_center, float *_axis0, float *_axis1, float *_axis2, float *_extents )
	{
		gentity_t *pEnt = EntityFromHandle(_ent);
		if(pEnt)
		{
			vec3_t axis[3];
			AnglesToAxis( pEnt->r.currentAngles, axis );

			vec3_t boxCenter;
			boxCenter[0] = ((pEnt->r.maxs[0] + pEnt->r.mins[0]) * 0.5f);
			boxCenter[1] = ((pEnt->r.maxs[1] + pEnt->r.mins[1]) * 0.5f);
			boxCenter[2] = ((pEnt->r.maxs[2] + pEnt->r.mins[2]) * 0.5f);

			vec3_t out;
			VectorCopy(pEnt->r.currentOrigin,out);
			for( int i = 0; i < 3; ++i ) {
				vec3_t tmp;
				VectorScale( axis[i], boxCenter[i], tmp );
				VectorAdd( out, tmp, out );
			}
			VectorCopy( out, _center );
			VectorCopy( axis[0], _axis0 );
			VectorCopy( axis[1], _axis1 );
			VectorCopy( axis[2], _axis2 );
			_extents[0] = (pEnt->r.maxs[0] - pEnt->r.mins[0]) * 0.5f;
			_extents[1] = (pEnt->r.maxs[1] - pEnt->r.mins[1]) * 0.5f;
			_extents[2] = (pEnt->r.maxs[2] - pEnt->r.mins[2]) * 0.5f;

			return Success;
		}
		return InvalidEntity;
	}

	obResult        GetEntityGroundEntity(const GameEntity _ent, GameEntity & moveent)
	{
		gentity_t      *pEnt = EntityFromHandle(_ent);

		if(pEnt)
		{
			if(pEnt->s.groundEntityNum > 0 && pEnt->s.groundEntityNum < ENTITYNUM_MAX_NORMAL)
			{
				moveent = HandleFromEntity(&g_entities[pEnt->s.groundEntityNum]);
			}
			return Success;
		}
		return InvalidEntity;
	}

	GameEntity      GetEntityOwner(const GameEntity _ent)
	{
		GameEntity      owner;

		gentity_t      *pEnt = EntityFromHandle(_ent);

		if(pEnt && pEnt->inuse)
		{
			// hack, when the game joins clients again after warmup, they are temporarily ET_GENERAL entities(LAME)
			int             t = pEnt->s.eType;

			if(pEnt->client && (pEnt - g_entities) < MAX_CLIENTS)
				t = ET_PLAYER;

			switch (t)
			{
				case ET_Q3F_GOAL:
				{
					if(pEnt->mapdata && (pEnt->mapdata->flags & Q3F_FLAG_CARRYABLE) && pEnt->s.otherEntityNum != MAX_CLIENTS)
					{
						owner = HandleFromEntity(&g_entities[pEnt->s.otherEntityNum]);
					}
					break;
				}
				case ET_Q3F_SENTRY:
				{
					if(pEnt->parent && pEnt->parent->client && pEnt->parent->client->sentry == pEnt)
					{
						owner = HandleFromEntity(pEnt->parent);
					}
					break;
				}
				case ET_Q3F_SUPPLYSTATION:
				{
					if(pEnt->parent && pEnt->parent->client->supplystation == pEnt)
					{
						owner = HandleFromEntity(pEnt->parent);
					}
					break;
				}
				case ET_Q3F_GRENADE:
				{
					if(pEnt->s.weapon == Q3F_GREN_CHARGE && pEnt->activator && pEnt->activator->client &&
					   pEnt->activator->client->chargeEntity == pEnt)
					{
						owner = HandleFromEntity(pEnt->activator);
					}
					break;
				}
				default:
					// -1 means theres no owner.
					if(pEnt->s.otherEntityNum != MAX_CLIENTS)
						owner = HandleFromEntity(&g_entities[pEnt->s.otherEntityNum]);
					//if(pEnt->r.ownerNum < MAX_GENTITIES)
					//  owner = HandleFromEntity(&g_entities[pEnt->r.ownerNum]);
			}
		}
		return owner;
	}

	int             GetEntityTeam(const GameEntity _ent)
	{
		gentity_t      *pEnt = EntityFromHandle(_ent);

		return pEnt && pEnt->inuse ? _GetEntityTeam(pEnt) : ETF_TEAM_NONE;
	}

	const char     *GetEntityName(const GameEntity _ent)
	{
		gentity_t      *pEnt = EntityFromHandle(_ent);

		if(pEnt)
		{
			return _GetEntityName(pEnt);
		}
		return NULL;
	}

	obResult        GetCurrentWeaponClip(const GameEntity _ent, FireMode _mode, int &_curclip, int &_maxclip)
	{
		gentity_t      *bot = EntityFromHandle(_ent);

		if(bot && bot->inuse && bot->client)
		{
			int             iWeapon = bot->client->ps.weapon;

			if(iWeapon != -1)
			{
				bg_q3f_weapon_t *wp = BG_Q3F_GetWeapon(iWeapon);
				int         clip = Q3F_GetClipValue(iWeapon, &bot->client->ps);

				_curclip = clip >= 0 ? clip : 0;
				_maxclip = wp->clipsize;
				return Success;
			}
		}
		return InvalidEntity;
	}

	obResult        GetCurrentAmmo(const GameEntity _ent, int _weaponId, FireMode _mode, int &_cur, int &_max)
	{
		gentity_t      *bot = EntityFromHandle(_ent);

		if(bot && bot->inuse && bot->client)
		{
			int ammoIndex = 0;
			g_q3f_playerclass_t *cls = G_Q3F_GetClass(&bot->client->ps);

			_weaponId = _weaponBotToGame(_weaponId);
			ammoIndex = Q3F_GetAmmoTypeForWeapon(_weaponId);
			int maxclip = Q3F_GetClipSizeForWeapon(_weaponId);

			int clip = Q3F_GetClipValue(_weaponId, &bot->client->ps);

			if(clip == -1)
				clip = 0;

			_cur = 0;
			_max = 0;

			switch (_weaponId)
			{
				case WP_SHOTGUN:
				case WP_SUPERSHOTGUN:
				case WP_SNIPER_RIFLE:
				case WP_ASSAULTRIFLE:
				case WP_MINIGUN:
					_cur = clip + bot->client->ps.ammo[AMMO_SHELLS];
					_max = maxclip + cls->s->maxammo_shells;
					break;
				case WP_NAILGUN:
				case WP_SUPERNAILGUN:
				case WP_RAILGUN:
				case WP_DARTGUN:
					_cur = clip + bot->client->ps.ammo[AMMO_NAILS];
					_max = maxclip + cls->s->maxammo_nails;
					break;
				case WP_GRENADE_LAUNCHER:
				case WP_ROCKET_LAUNCHER:
				case WP_PIPELAUNCHER:
				case WP_NAPALMCANNON:
					_cur = clip + bot->client->ps.ammo[AMMO_ROCKETS];
					_max = maxclip + cls->s->maxammo_rockets;
					break;
				case WP_FLAMETHROWER:
					_cur = clip + bot->client->ps.ammo[AMMO_CELLS];
					_max = maxclip + cls->s->maxammo_cells;
					break;
				case WP_AXE:
					if(bot->client->ps.persistant[PERS_CURRCLASS] == Q3F_CLASS_ENGINEER)
					{
						_cur = clip + bot->client->ps.ammo[AMMO_CELLS];
						_max = maxclip + cls->s->maxammo_cells;
						break;
					}
					else if(bot->client->ps.persistant[PERS_CURRCLASS] == Q3F_CLASS_PARAMEDIC)
					{
						_cur = clip + bot->client->ps.ammo[AMMO_MEDIKIT];
						_max = maxclip + cls->s->maxammo_medikit;
						break;
					}
					break;
				case WP_DETPACK:
					_cur = bot->client->ps.ammo[AMMO_CHARGE];
					_max = cls->s->maxammo_charge;
					break;
				case WP_GRENADE1:
					_cur = (bot->client->ps.ammo[AMMO_GRENADES] & 0x00FF);
					_max = cls->s->gren1max;
					break;
				case WP_GRENADE2:
					_cur = (bot->client->ps.ammo[AMMO_GRENADES] >> 8);
					_max = cls->s->gren2max;
					break;
				default:
					return InvalidParameter;
			}
			return Success;
		}

		return InvalidEntity;
	}

	int             GetGameTime()
	{
		return level.time;
	}

	void            GetGoals()
	{
		const int       iAllTeams = (1 << ETF_TEAM_BLUE) | (1 << ETF_TEAM_RED) | (1 << ETF_TEAM_YELLOW) | (1 << ETF_TEAM_GREEN);
		int             iNumFlags = 0;
		int             iNumCaptures = 0;

		if(!hasptr)
		{
			G_Q3F_AddString(&teamscoreptr, "teamscore");
			hasptr = qtrue;
		}

		for (int i = MAX_CLIENTS; i < level.num_entities; i++) 
		{
			gentity_t *e = &g_entities[i];

			if (!e->inuse || !e->mapdata)
				continue;

			if(e->mapdata->gameindex && !(e->mapdata->gameindex & (1 << g_gameindex.integer)))
				continue;

			const char     *pGoalName = _GetEntityName(e);
			int             teams = e->mapdata->team;
				
			if(teams == 0)
				teams = iAllTeams;
			else
				teams = Bot_TeamGameBitsToBotBits(teams);
#if 0
			if(e->mapdata->botgoaltype)
			{
				switch(e->mapdata->botgoaltype)
				{
				case GIT_FLAG:
					Bot_Util_AddGoal("flag", e, teams, pGoalName);
					++iNumFlags;
					//Bot_Util_AddGoal(e, GOAL_CTF_FLAG, teams, pGoalName);
					break;
				case GIT_FLAGCAP:
					if(e->mapdata->team == 0)
						teams = _GetTeamsFromHoldingFlag(e->mapdata->holding);
					Bot_Util_AddGoal("cappoint", e, teams, pGoalName);
					++iNumCaptures;
					//Bot_Util_AddGoal(e, GOAL_CTF_FLAGCAP, teams, pGoalName);
					break;
				}
			}
			else
#endif
			{
				switch (e->s.eType)
				{
				case ET_Q3F_GOAL:
					if(!Q_stricmp(e->classname, "func_goalitem") || !Q_stricmp(e->classname, "func_flag"))
					{
						// This is going to be a flag.
						if(e->model && e->model[0] && e->mapdata->groupname)
						{
							Bot_Util_AddGoal("flag", e, teams, pGoalName);
							++iNumFlags;
							//Bot_Util_AddGoal(e, GOAL_CTF_FLAG, teams, pGoalName);
						}
					}
					if(!Q_stricmp(e->classname, "func_goalinfo"))
					{
						// This is going to be a flag-cap.
						if((e->mapdata->give || e->mapdata->holding) && e->mapdata->other)
						{
							q3f_keypair_t  *data = G_Q3F_KeyPairArrayFind(e->mapdata->other, teamscoreptr);

							if(data && data->value.d.intdata)
							{
								if(e->mapdata->team == 0)
									teams = _GetTeamsFromHoldingFlag(e->mapdata->holding);
								Bot_Util_AddGoal("cappoint", e, teams, pGoalName);
								++iNumCaptures;
								//Bot_Util_AddGoal(e, GOAL_CTF_FLAGCAP, teams, pGoalName);
							}
						}
					}
					break;
				default:
					if(!Q_stricmp(e->classname, "trigger_multiple"))
					{
						// This is going to be a flag-cap.
						if((e->mapdata->give || e->mapdata->holding) && e->mapdata->other)
						{
							q3f_keypair_t  *data = G_Q3F_KeyPairArrayFind(e->mapdata->other, teamscoreptr);

							if(data && data->value.d.intdata)
							{
								if(e->mapdata->team == 0)
									teams = _GetTeamsFromHoldingFlag(e->mapdata->holding);
								Bot_Util_AddGoal("cappoint", e, teams, pGoalName);
								++iNumCaptures;
								//Bot_Util_AddGoal(e, GOAL_CTF_FLAGCAP, teams, pGoalName);
							}
						}
					}
					break;
				}
			}
		}
//		G_Printf("%sflags: %i  captures: %i\n", S_COLOR_GREEN, iNumFlags, iNumCaptures);
	}

	void            GetPlayerInfo(obPlayerInfo & info)
	{
		for(int t = Q3F_TEAM_RED; t <= Q3F_TEAM_GREEN; ++t)
		{
			if((g_q3f_allowedteams & (1 << t)))
			{
				info.m_AvailableTeams |= (1 << t);
			}
		}

		info.m_MaxPlayers = level.maxclients;

		GameEntity      ge;
		int             i;

		for(gentity_t *ent = g_entities; ent < &g_entities[level.maxclients]; ent++)
		{
			if(!ent->inuse || !ent->client || ent->client->pers.connected != CON_CONNECTED)
				continue;

			ge = HandleFromEntity(ent);
			i  = ENTINDEX(ent);

			info.m_Players[i].m_Team = GetEntityTeam(ge);
			info.m_Players[i].m_Class = GetEntityClass(ge);
			info.m_Players[i].m_Controller = IsBot(ent) ? obPlayerInfo::Bot : obPlayerInfo::Human;
		}
	}

	obResult        InterfaceSendMessage(const MessageHelper & _data, const GameEntity _ent)
	{
		gentity_t      *pEnt = EntityFromHandle(_ent);
		bg_q3f_playerclass_t *cls = 0;

		switch (_data.GetMessageId())
		{
				// general messages
			case GEN_MSG_ISALIVE:
			{
				OB_GETMSG(Msg_IsAlive);
				if(pMsg)
				{
					if(pEnt && pEnt->inuse && pEnt->client && (pEnt->health > 0) && !Q3F_IsSpectator(pEnt->client))
					{
						pMsg->m_IsAlive = True;
					}
				}
				break;
			}
			case GEN_MSG_ISRELOADING:
			{
				OB_GETMSG(Msg_Reloading);
				if(pMsg)
				{
					if((pEnt && pEnt->inuse && pEnt->client && pEnt->client->ps.weaponstate >= WEAPON_RRAISING) &&
					   (pEnt->client->ps.weaponstate <= WEAPON_RELOADING))
					{
						pMsg->m_Reloading = True;
					}
				}
				break;
			}
			case GEN_MSG_ISREADYTOFIRE:
			{
				OB_GETMSG(Msg_ReadyToFire);
				if(pMsg)
				{
					if(pEnt && pEnt->inuse && pEnt->client)
					{
						if(pEnt->client->ps.weaponstate < WEAPON_RRAISING || pEnt->client->ps.weaponstate > WEAPON_RELOADING)
						{
							pMsg->m_Ready = True;
						}
					}
				}
				break;
			}
			case GEN_MSG_GETEQUIPPEDWEAPON:
			{
				OB_GETMSG(WeaponStatus);
				if(pMsg)
				{
					if(pEnt && pEnt->inuse && pEnt->client)
						pMsg->m_WeaponId =
							Bot_WeaponGameToBot(pEnt->client->ps.weapon, pEnt->client->ps.persistant[PERS_CURRCLASS]);
					else
						pMsg->m_WeaponId = 0;
				}
				break;
			}
			case GEN_MSG_GETMOUNTEDWEAPON:
			{
				OB_GETMSG(WeaponStatus);
				pMsg->m_WeaponId = TF_WP_NONE;
				break;
			}
			case GEN_MSG_GETWEAPONLIMITS:
			{
				OB_GETMSG(WeaponLimits);
				if(pMsg)
					pMsg->m_Limited = False;
				break;
			}
			case GEN_MSG_GETHEALTHARMOR:
			{
				OB_GETMSG(Msg_HealthArmor);
				if(pMsg)
				{
					if(pEnt)
					{
						if(pEnt->client)
						{
							cls = BG_Q3F_GetClass(&(pEnt->client->ps));
							pMsg->m_CurrentHealth = pEnt->client->ps.stats[STAT_HEALTH];
							pMsg->m_MaxHealth = cls->maxhealth;
							pMsg->m_CurrentArmor = pEnt->client->ps.stats[STAT_ARMOR];
							pMsg->m_MaxArmor = cls->maxarmour;
						}
						else
						{
							pMsg->m_CurrentHealth = pEnt->takedamage ? pEnt->health : 0;
							pMsg->m_MaxHealth = 0;
							pMsg->m_CurrentArmor = pMsg->m_MaxArmor = 0;
						}
					}
				}
				break;
			}
			case GEN_MSG_GETMAXSPEED:
			{
				OB_GETMSG(Msg_PlayerMaxSpeed);
				if(pMsg && pEnt)
				{
					if(pEnt->client)
					{
						if((cls = BG_Q3F_GetClass(&(pEnt->client->ps))) != NULL)
							pMsg->m_MaxSpeed = (float)cls->maxspeed;
						else
							pMsg->m_MaxSpeed = (float)g_speed.integer;
					}
					else
					{
						pMsg->m_MaxSpeed = (float)g_speed.integer;
					}
				}
				break;
			}
			case GEN_MSG_ISALLIED:
			{
				OB_GETMSG(Msg_IsAllied);
				if(pMsg)
				{
					gentity_t      *pEntOther = EntityFromHandle(pMsg->m_TargetEntity);

					if(pEntOther && pEnt)
					{
						if(ENTINDEX(pEntOther) == ENTITYNUM_WORLD)
							pMsg->m_IsAllied = True;
						else
						{
							if(pEnt->client && _IsBackpack(pEntOther))
							{
								pMsg->m_IsAllied = _GetBackpackTeam(_GetEntityTeam(pEnt), pEntOther) ? True : False;
							}
							else if(pEnt->client && (pEnt - g_entities) < MAX_CLIENTS && pEntOther->client && (pEntOther - g_entities) < MAX_CLIENTS)
							{
								pMsg->m_IsAllied = G_Q3F_IsAllied(pEnt, pEntOther) ? True : False;
							}
							else
							{
								pMsg->m_IsAllied = _GetEntityTeam(pEnt) == _GetEntityTeam(pEntOther) ? True : False;
							}
						}
					}
				}
				break;
			}
			case GEN_MSG_ISOUTSIDE:
			{
				OB_GETMSG(Msg_IsOutside);
				if(pMsg)
				{
					trace_t         tr;
					vec3_t          end;

					end[0] = pMsg->m_Position[0];
					end[1] = pMsg->m_Position[1];
					end[2] = pMsg->m_Position[2] + 4096;
					trap_Trace(&tr, pMsg->m_Position, NULL, NULL, end, -1, MASK_SOLID);

					if((tr.fraction < 1.0) && !(tr.surfaceFlags & SURF_NOIMPACT))
						pMsg->m_IsOutside = False;
					else
						pMsg->m_IsOutside = True;
				}
				break;
			}
			case GEN_MSG_CHANGENAME:
			{
				OB_GETMSG(Msg_ChangeName);
				if(pMsg && pEnt && pEnt->client)
				{
					char            userinfo[MAX_INFO_STRING];

					trap_GetUserinfo(pEnt->s.clientNum, userinfo, MAX_INFO_STRING);
					Info_SetValueForKey(userinfo, "name", pMsg->m_NewName);
					trap_SetUserinfo(pEnt->s.clientNum, userinfo);
					ClientUserinfoChanged(pEnt->s.clientNum, "name change");
				}
				break;
			}
			case GEN_MSG_ENTITYKILL:
			{
				OB_GETMSG(Msg_KillEntity);
				if(pMsg && pMsg->m_WhoToKill.IsValid() && g_cheats.integer)
				{
					gentity_t      *pWho = EntityFromHandle(pMsg->m_WhoToKill);

					if(pWho)
						G_Damage(pWho, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);
				}
				break;
			}
			case GEN_MSG_SERVERCOMMAND:
			{
				OB_GETMSG(Msg_ServerCommand);
				if(pMsg && pMsg->m_Command[0] /*&& g_cheats.integer*/)
				{
					trap_SendConsoleCommand(EXEC_NOW, pMsg->m_Command);
				}
				break;
			}
			case GEN_MSG_GETFLAGSTATE:
			{
				OB_GETMSG(Msg_FlagState);
				if(pMsg)
				{
					/* CS: flagreturn position is not updated while its carried. */
					/* Ensiform : Not relevant to ETF though, no returning flags */
					if(pEnt && pEnt->inuse && pEnt->s.eType == ET_Q3F_GOAL && pEnt->model && pEnt->model[0] && pEnt->mapdata &&
					   (pEnt->mapdata->flags & Q3F_FLAG_CARRYABLE))
					{
						switch (pEnt->mapdata->state)
						{
							case Q3F_STATE_INACTIVE:
								pMsg->m_FlagState = S_FLAG_AT_BASE;
								break;
							case Q3F_STATE_ACTIVE:
								pMsg->m_FlagState = S_FLAG_DROPPED;
								break;
							case Q3F_STATE_CARRIED:
								pMsg->m_FlagState = S_FLAG_CARRIED;
								pMsg->m_Owner = GetEntityOwner(_ent);
								break;
							default:
								pMsg->m_FlagState = S_FLAG_UNAVAILABLE;
								break;
						}
					}
				}
				break;
			}
			case GEN_MSG_GETCONTROLLINGTEAM:
			{
				ControllingTeam *pMsg = _data.Get < ControllingTeam > ();

				if(pMsg)
				{
					pMsg->m_ControllingTeam = 0;
				}
				break;
			}
			case GEN_MSG_GAMESTATE:
			{
				OB_GETMSG(Msg_GameState);
				if(pMsg)
				{
					/*if (level.framenum < 3)	// GAME_INIT_FRAMES for Q3 is 3 instead of 6 for ET
					{
						pMsg->m_GameState = GAME_STATE_INVALID;
						break;
					}*/

					int             iTimeLimit = (int)(g_timelimit.value * 60000);
					int             iMatchTime = level.time - level.startTime;

					pMsg->m_TimeLeft = (iTimeLimit - iMatchTime) / 1000.f;
					pMsg->m_GameState = GAME_STATE_PLAYING;
					/*switch(g_matchState.integer)
					   {
					   case MATCH_STATE_NORMAL:
					   case MATCH_STATE_PLAYING:
					   pMsg->m_GameState = GAME_STATE_PLAYING;
					   break;                   
					   case GS_WARMUP_COUNTDOWN:
					   pMsg->m_GameState = GAME_STATE_WARMUP_COUNTDOWN;
					   break;
					   case MATCH_STATE_PREPARE:
					   case MATCH_STATE_WARMUP:
					   pMsg->m_GameState = GAME_STATE_WARMUP;
					   break;
					   case GS_INTERMISSION:
					   pMsg->m_GameState = GAME_STATE_INTERMISSION;
					   break;
					   case MATCH_STATE_WAITING:
					   case MATCH_STATE_READYUP:
					   pMsg->m_GameState = GAME_STATE_WAITINGFORPLAYERS;
					   break;
					   default:
					   pMsg->m_GameState = GAME_STATE_INVALID;
					   break;
					   }*/
				}
				break;
			}
			case GEN_MSG_ENTITYSTAT:
			{
				OB_GETMSG(Msg_EntityStat);
				if(pMsg)
				{
					if(pEnt && pEnt->client)
					{
						if(!strcmp(pMsg->m_StatName, "kills"))
							pMsg->m_Result = obUserData(pEnt->client->sess.enemyKill + pEnt->client->sess.allyKill);
						else if(pEnt && pEnt->client && !strcmp(pMsg->m_StatName, "deaths"))
							pMsg->m_Result = obUserData(pEnt->client->ps.persistant[PERS_KILLED]);
						else if(pEnt && pEnt->client && !strcmp(pMsg->m_StatName, "score"))
							pMsg->m_Result = obUserData(pEnt->client->ps.persistant[PERS_SCORE]);
					}
				}
				break;
			}
			case GEN_MSG_TEAMSTAT:
			{
				OB_GETMSG(Msg_TeamStat);
				if(pMsg)
				{
					q3f_team_t iTeam = Q3F_TEAM_FREE;
					switch(pMsg->m_Team)
					{
					case ETF_TEAM_RED:
						iTeam = Q3F_TEAM_RED;
						break;
					case ETF_TEAM_BLUE:
						iTeam = Q3F_TEAM_BLUE;
						break;
					case ETF_TEAM_YELLOW:
						iTeam = Q3F_TEAM_YELLOW;
						break;
					case ETF_TEAM_GREEN:
						iTeam = Q3F_TEAM_GREEN;
						break;
					default:
						assert(0 && "Bad Team Selection!");
						return InvalidParameter;
					}

					if(!strcmp(pMsg->m_StatName, "score"))
					   pMsg->m_Result = obUserData(level.teamScores[iTeam]);
				}
				break;
			}
			case GEN_MSG_WPCHARGED:
			{
				OB_GETMSG(WeaponCharged);
				if(pMsg && pEnt && pEnt->inuse && pEnt->client)
				{
					if(pMsg->m_Weapon == TF_WP_MINIGUN)
					{
						pMsg->m_IsCharged = (pEnt->client->ps.ammo[AMMO_CELLS] >= 4) ? True : False;
					}
					else
					{
						pMsg->m_IsCharged = True;
					}
				}
				break;
			}
			case GEN_MSG_WPHEATLEVEL:
			{
				OB_GETMSG(WeaponHeatLevel);
				if(pMsg && pEnt && pEnt->inuse && pEnt->client)
				{
					pMsg->m_CurrentHeat = 0.0f;
					pMsg->m_MaxHeat = 0.0f;
				}
				break;
			}
			case GEN_MSG_MOVERAT:
			{
				OB_GETMSG(Msg_MoverAt);
				if(pMsg)
				{
					vec3_t          org, under;

					VectorCopy(pMsg->m_Position, org);
					VectorCopy(pMsg->m_Under, under);

					trace_t         tr;

					trap_Trace(&tr, org, NULL, NULL, under, -1, MASK_SOLID);

					//if(tr.fraction == 1.0f)
					//  G_Printf("^5Omni-bot::InterfaceSendMessage::GEN_MSG_MOVERAT -> tr.fraction == 1.0f\n");

					//if(tr.startsolid)
					//  G_Printf("^5Omni-bot::InterfaceSendMessage::GEN_MSG_MOVERAT -> tr.startsolid\n");

					//if(tr.entityNum == ENTITYNUM_WORLD)
					//  G_Printf("^5Omni-bot::InterfaceSendMessage::GEN_MSG_MOVERAT -> tr.entityNum == world\n");

					//if(tr.entityNum >= 0 && tr.entityNum < MAX_CLIENTS)
					//  G_Printf("^5Omni-bot::InterfaceSendMessage::GEN_MSG_MOVERAT -> tr.entityNum == client\n");

					if(tr.fraction == 1.0f || tr.startsolid || tr.entityNum == ENTITYNUM_WORLD ||
					   (tr.entityNum >= 0 && tr.entityNum < MAX_CLIENTS))
						break;

					gentity_t      *fent = &g_entities[tr.entityNum];

					//if(!fent)
					//  G_Printf("^5Omni-bot::InterfaceSendMessage::GEN_MSG_MOVERAT -> (ent:%i) == null\n", tr.entityNum);

					if(!fent)
						break;

					if(fent->s.eType == ET_MOVER && !Q_stricmpn(fent->classname, "func_", strlen("func_")))
					{
						//G_Printf("^5Omni-bot::InterfaceSendMessage::GEN_MSG_MOVERAT -> (ent:%i) == true\n", fent->s.number);
						pMsg->m_Entity = HandleFromEntity(fent);
					}
				}
				break;
			}
			case GEN_MSG_GETGAMETYPE:
				{
					OB_GETMSG(TF_GameMode);
					if(pMsg)
					{
						pMsg->m_GameMode = g_gameindex.integer;
					}
					break;
				}
			case GEN_MSG_SETCVAR:
				{
					OB_GETMSG(TF_CvarSet);
					if(pMsg)
					{
						trap_Cvar_Set(pMsg->m_Cvar, pMsg->m_Value);
					}
					break;
				}
			case GEN_MSG_GETCVAR:
				{
					OB_GETMSG(TF_CvarGet);
					if(pMsg)
					{
						pMsg->m_Value =
							trap_Cvar_VariableIntegerValue(pMsg->m_Cvar);
					}
					break;
				}
/*		case TF_MSG_GETBUILDABLES:
			{
				OB_GETMSG(TF_BuildInfo);
				if (pMsg && pEnt && pEnt->inuse && pEnt->client)
				{
					if (pEnt->client->ps.persistant[PERS_CURRCLASS] == Q3F_CLASS_ENGINEER)
					{
						pMsg->m_Sentry = (pEnt->client->sentry && pEnt->client->sentry->parent == pEnt) ? HandleFromEntity(pEnt->client->sentry) : GameEntity();
						pMsg->m_Dispenser = (pEnt->client->supplystation && pEnt->client->supplystation->parent == pEnt) ? HandleFromEntity(pEnt->client->supplystation) : GameEntity();
					}
					if (pEnt->client->ps.persistant[PERS_CURRCLASS] == Q3F_CLASS_GRENADIER)
					{
						pMsg->m_Detpack = (pEnt->client->chargeEntity && pEnt->client->chargeEntity->activator == pEnt) ? HandleFromEntity(pEnt->client->chargeEntity) : GameEntity();
					}
				}
				break;
			}*/
			case TF_MSG_PLAYERPIPECOUNT:
			{
				OB_GETMSG(TF_PlayerPipeCount);
				if(pMsg && pEnt && pEnt->inuse && pEnt->client)
				{
					pMsg->m_NumPipes = pEnt->client->pipecount;
					pMsg->m_MaxPipes = Q3F_MAX_PIPES_PER_GRENADIER;
				}
				break;
			}
			case TF_MSG_TEAMPIPEINFO:
			{
				OB_GETMSG(TF_TeamPipeInfo);
				if(pMsg && pEnt && pEnt->inuse && pEnt->client)
				{
					pMsg->m_NumTeamPipes = pEnt->client->ps.ammoclip[1];
					pMsg->m_NumTeamPipers = G_Q3F_ClassCount( pEnt->client->sess.sessionTeam, Q3F_CLASS_GRENADIER );
					pMsg->m_MaxPipesPerPiper = Q3F_MAX_PIPES_PER_GRENADIER;
				}
				break;
			}
			case TF_MSG_CANDISGUISE:
			{
				OB_GETMSG(TF_DisguiseOptions);
				if(pMsg)
				{
					const int       iCheckTeam = _teamBotToGame(pMsg->m_CheckTeam);

					for(int t = Q3F_TEAM_RED; t <= Q3F_TEAM_GREEN; ++t)
					{
						pMsg->m_Team[Bot_TeamGameToBot(t)] = (g_q3f_allowedteams & (1 << t)) ? True : False;

						if((g_q3f_allowedteams & (1 << t)) && (t == iCheckTeam))
						{
							for(int c = Q3F_CLASS_RECON; c <= Q3F_CLASS_CIVILIAN; ++c)
							{
								pMsg->m_Class[Bot_PlayerClassGameToBot(c)] = (g_q3f_teamlist[t].classmaximums[c]) ? True : False;
							}
						}
					}
				}
				break;
			}
			case TF_MSG_DISGUISE:
			{
				OB_GETMSG(TF_Disguise);
				if(pMsg)
				{
					int             iTeam = _teamBotToGame(pMsg->m_DisguiseTeam);
					int             iClass = playerClassBotToGame(pMsg->m_DisguiseClass);

					if(iTeam != Q3F_TEAM_FREE && iClass != Q3F_CLASS_NULL)
					{
						// FIXME can't do both at same time
						trap_EA_Command(ENTINDEX(pEnt), va("disguise %i %i", iTeam, iClass));
					}
					else
					{
						return InvalidParameter;
					}
				}
				break;
			}
			case TF_MSG_CLOAK:
			{
				OB_GETMSG(TF_FeignDeath);
				if(pMsg)
				{
					trap_EA_Command(ENTINDEX(pEnt), "invisible");
				}
				break;
			}
			case TF_MSG_LOCKPOSITION:
			{
				OB_GETMSG(TF_LockPosition);
				if(pMsg)
				{
					pEnt = EntityFromHandle(pMsg->m_TargetPlayer);
					if(pEnt && pEnt->inuse && pEnt->client &&
					   (pEnt->client->ps.pm_type == PM_FREEZE || pEnt->client->ps.pm_type == PM_NORMAL))
					{
						if(pMsg->m_Lock == True)
							pEnt->client->ps.pm_type = PM_FREEZE;
						else
							pEnt->client->ps.pm_type = PM_NORMAL;
						pMsg->m_Succeeded = True;
					}
				}
				break;
			}
			case TF_MSG_HUDTEXT:
			{
				OB_GETMSG(TF_HudText);
				pEnt = EntityFromHandle(pMsg->m_TargetPlayer);
				if(pMsg && pMsg->m_Message[0] && pEnt && pEnt->inuse && pEnt->client)
				{
					switch (pMsg->m_MessageType)
					{
						default:
						case TF_HudText::MsgConsole:
						{
							trap_SendServerCommand(ENTINDEX(pEnt), va("print \"%s\"", pMsg->m_Message));
							break;
						}
						case TF_HudText::MsgHudCenter:
						{
							trap_SendServerCommand(ENTINDEX(pEnt), va("cpb \"%s\"", pMsg->m_Message));
							break;
						}
					}
				}
				break;
			}
			//////////////////////////////////////////////////////////////////////////
			default:
			{
				return UnknownMessageType;
			}
		}
		return Success;
	}

	void            PrintError(const char *_error)
	{
		if(_error)
			G_Printf("%s%s\n", S_COLOR_RED, _error);
	}

	void            PrintMessage(const char *_msg)
	{
		if(_msg)
		{
			// q3 console doesn't support tabs, so convert to 4 spaces
			const int BufferSize = 1024;
			char tmpbuffer[BufferSize] = {};
			const char *src = _msg;
			char *dest = tmpbuffer;
			while(*src != 0)
			{
				if(*src == '\t')
				{
					for(int i = 0; i < 4; ++i)
						*dest++ = ' ';
					src++;
				}
				else
				{
					*dest++ = *src++;
				}
			}
			G_Printf("%s%s\n", S_COLOR_GREEN, tmpbuffer);
		}
	}

	bool           PrintScreenText(const float _pos[3], float _duration, const obColor & _color, const char *_msg)
	{
		// for dedicated servers we quit early because there can't possibly be a client 0 that is local client
		if(g_dedicated.integer)
			return qtrue;

		//if(_pos)
			return qfalse;

#if 0
		gentity_t      *pEnt = EntityFromHandle(GetLocalGameEntity());

		if(pEnt && pEnt->inuse && pEnt->client && pEnt->client->pers.connected == CON_CONNECTED && !IsBot(pEnt) && _msg && _msg[0])
		{
			if(0/*_pos*/)
			{
				trap_SendServerCommand(ENTINDEX(pEnt), va("wp \"%s\" %f %f %f %f", _msg, _pos[0], _pos[1], _pos[2], _duration));
			}
			else
			{
				if(g_LastScreenMessageTime != level.time)
				{
					trap_SendServerCommand(ENTINDEX(pEnt), va("cpb \"%s\"", _msg));
					g_LastScreenMessageTime = level.time;
				}
			}
		}
		return qtrue;
#endif
	}

	const char     *GetMapName()
	{
		static char     strMapName[MAX_QPATH] = { 0 };
		trap_Cvar_VariableStringBuffer("mapname", strMapName, sizeof(strMapName));
		COM_StripExtension(COM_SkipPath(strMapName), strMapName, sizeof(strMapName));
		return strMapName;
	}

	void            GetMapExtents(AABB & _aabb)
	{
		memset(&_aabb, 0, sizeof(AABB));
	}

	GameEntity      EntityByName(const char *_name)
	{
		for (int i = 0; i < level.num_entities; i++) 
		{
			gentity_t *e = &g_entities[i];

			if (!e->inuse)
				continue;

			const char     *pGoalName = _GetEntityName(e);

			/*if(e->client && !Q_stricmp(_name, pGoalName))
			{
				return HandleFromEntity(e);
			}
			else */if(e->mapdata && e->mapdata->groupname && e->mapdata->groupname->data->d.strdata && !Q_stricmp(_name, pGoalName))
			{
				return HandleFromEntity(e);
			}
			else if(e->targetname && !Q_stricmp(_name, pGoalName))
			{
				return HandleFromEntity(e);
			}
		}

		return GameEntity();
	}

	GameEntity      EntityFromID(const int _gameId)
	{
		gentity_t      *pEnt = INDEXENT(_gameId);

		return pEnt ? HandleFromEntity(pEnt) : GameEntity();
	}

	int             IDFromEntity(const GameEntity _ent)
	{
		gentity_t      *pEnt = EntityFromHandle(_ent);

		if(pEnt)
		{
			gentity_t      *pStart = g_entities;
			int             iIndex = pEnt - pStart;

			assert(iIndex >= 0);
			return (iIndex < MAX_GENTITIES) ? iIndex : -1;
		}
		return -1;
	}

	bool            DoesEntityStillExist(const GameEntity & _hndl)
	{
		return _hndl.IsValid()? EntityFromHandle(_hndl) != NULL : false;
	}

	int             GetAutoNavFeatures(AutoNavFeature * _feature, int _max)
	{
		int             iNumFeatures = 0;

		if(!hasptr)
		{
			G_Q3F_AddString(&teamscoreptr, "teamscore");
			hasptr = qtrue;
		}

		for(int i = MAX_CLIENTS; i < level.num_entities && iNumFeatures < _max; i++)
		{
			gentity_t      *e = &g_entities[i];

			if(!e->inuse)
				continue;

			////////////////////////////////////////////////////////////////////////
			_feature[iNumFeatures].m_Type = 0;
			_feature[iNumFeatures].m_TravelTime = 0;
			_feature[iNumFeatures].m_ObstacleEntity = false;
			for(int i = 0; i < 3; ++i)
			{
				_feature[iNumFeatures].m_Position[i] = e->r.currentOrigin[i];
				_feature[iNumFeatures].m_TargetPosition[i] = e->r.currentOrigin[i];
				_feature[iNumFeatures].m_Bounds.m_Mins[0] = 0.f;
				_feature[iNumFeatures].m_Bounds.m_Maxs[0] = 0.f;
				AngleVectors(e->s.angles, _feature[iNumFeatures].m_Facing, NULL, NULL);
			}

			_feature[iNumFeatures].m_Bounds.m_Mins[0] = e->r.absmin[0];
			_feature[iNumFeatures].m_Bounds.m_Mins[1] = e->r.absmin[1];
			_feature[iNumFeatures].m_Bounds.m_Mins[2] = e->r.absmin[2];
			_feature[iNumFeatures].m_Bounds.m_Maxs[0] = e->r.absmin[0];
			_feature[iNumFeatures].m_Bounds.m_Maxs[1] = e->r.absmin[1];
			_feature[iNumFeatures].m_Bounds.m_Maxs[2] = e->r.absmin[2];
			//////////////////////////////////////////////////////////////////////////
			if(e->classname)
			{
				if(!Q_stricmp(e->classname, "team_CTF_redspawn"))
					_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_PLAYERSTART_TEAM1;
				else if(!Q_stricmp(e->classname, "team_CTF_bluespawn"))
					_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_PLAYERSTART_TEAM2;
				else if(!Q_stricmp(e->classname, "info_player_deathmatch") || !Q_stricmp(e->classname, "info_player_start"))
				{
					if(e->mapdata && e->mapdata->team)
					{
						if(e->mapdata->team == (1 << Q3F_TEAM_RED))
							_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_PLAYERSTART_TEAM1;
						else if(e->mapdata->team == (1 << Q3F_TEAM_BLUE))
							_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_PLAYERSTART_TEAM2;
						else if(e->mapdata->team == (1 << Q3F_TEAM_YELLOW))
							_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_PLAYERSTART_TEAM3;
						else if(e->mapdata->team == (1 << Q3F_TEAM_GREEN))
							_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_PLAYERSTART_TEAM4;
						else
							_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_PLAYERSTART;
					}
					else
						_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_PLAYERSTART;
				}
				else if(!Q_stricmp(e->classname, "trigger_push"))
				{
					gentity_t      *pTarget = G_PickTarget(e->target);

					if(pTarget)
					{
						GetEntityCenter(e, _feature[iNumFeatures].m_Position);
						_feature[iNumFeatures].m_TargetPosition[0] = pTarget->r.currentOrigin[0];
						_feature[iNumFeatures].m_TargetPosition[1] = pTarget->r.currentOrigin[1];
						_feature[iNumFeatures].m_TargetPosition[2] = pTarget->r.currentOrigin[2];
						_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_JUMPPAD;
					}
				}
				else if(!Q_stricmp(e->classname, "trigger_teleport"))
				{
					gentity_t      *pTarget = G_PickTarget(e->target);

					if(pTarget)
					{
						GetEntityCenter(e, _feature[iNumFeatures].m_Position);
						_feature[iNumFeatures].m_TargetPosition[0] = pTarget->r.currentOrigin[0];
						_feature[iNumFeatures].m_TargetPosition[1] = pTarget->r.currentOrigin[1];
						_feature[iNumFeatures].m_TargetPosition[2] = pTarget->r.currentOrigin[2];
						_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_TELEPORTER;
					}
				}
				else if(!Q_stricmp(e->classname, "trigger_multiple"))
				{
					if(e->mapdata && (e->mapdata->give || e->mapdata->holding) && e->mapdata->other)
					{
						q3f_keypair_t  *data = G_Q3F_KeyPairArrayFind(e->mapdata->other, teamscoreptr);

						if(data && data->value.d.intdata)
						{
							GetEntityCenter(e, _feature[iNumFeatures].m_Position);
							_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_FLAGCAPPOINT;
						}
					}
					if(e->target)
					{
						gentity_t      *pTeleport = G_PickTarget(e->target);

						if(pTeleport && pTeleport->inuse && !Q_stricmp(pTeleport->classname, "target_teleporter"))
						{
							gentity_t      *pTarget = G_PickTarget(pTeleport->target);

							GetEntityCenter(e, _feature[iNumFeatures].m_Position);
							_feature[iNumFeatures].m_TargetPosition[0] = pTarget->r.currentOrigin[0];
							_feature[iNumFeatures].m_TargetPosition[1] = pTarget->r.currentOrigin[1];
							_feature[iNumFeatures].m_TargetPosition[2] = pTarget->r.currentOrigin[2];
							_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_TELEPORTER;
						}
					}
				}
				else if(!Q_stricmp(e->classname, "func_plat"))
				{
					GetEntityCenter(e, _feature[iNumFeatures].m_TargetPosition);
					//GetCenterTop(e, e->pos2, _feature[iNumFeatures].m_TargetPosition);
					for(int i = 0; i < 2; ++i)
					{
						_feature[iNumFeatures].m_Position[i] = _feature[iNumFeatures].m_TargetPosition[i];
					}
					// e->pos1[2] will always be negative
					_feature[iNumFeatures].m_Position[2] = e->pos1[2];//(_feature[iNumFeatures].m_TargetPosition[2] + e->pos1[2]);
					_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_LIFT;
				}
				else if(!Q_stricmp(e->classname, "func_goalitem"))
				{
					if(e->model && e->model[0] && e->mapdata && e->mapdata->groupname)
					{
						_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_FLAG;
					}
				}
				else if(!Q_stricmp(e->classname, "func_goalinfo"))
				{
					if(e->mapdata && e->mapdata->give && e->mapdata->holding && e->mapdata->other)
					{
						q3f_keypair_t  *data = G_Q3F_KeyPairArrayFind(e->mapdata->other, teamscoreptr);

						if(data && data->value.d.intdata)
						{
							_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_FLAGCAPPOINT;
						}
					}
				}
				else if(!Q_stricmp(e->classname, "team_CTF_redflag"))
				{
					_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_FLAG;
				}
				else if(!Q_stricmp(e->classname, "team_CTF_blueflag"))
				{
					_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_FLAG;
				}
			}

			if(_feature[iNumFeatures].m_Type != 0)
			{
				iNumFeatures++;
			}
		}
		return iNumFeatures;
	}

	const char     *GetGameName()
	{
		return GAME_VERSION;
	}

	const char     *GetModName()
	{
		return OMNIBOT_MODNAME;
	}

	const char     *GetModVers()
	{
		return OMNIBOT_MODVERSION;
	}

	const char     *GetBotPath()
	{
		return Omnibot_GetLibraryPath();
	}

	const char     *GetLogPath()
	{
		static char     logpath[512];

		trap_Cvar_VariableStringBuffer("fs_homepath", logpath, sizeof(logpath));
		return Omnibot_FixPath(logpath);
	}
};

void Bot_Interface_InitHandles()
{
	for(int i = 0; i < MAX_GENTITIES; ++i)
	{
		m_EntityHandles[i].m_HandleSerial = 1;
		m_EntityHandles[i].m_NewEntity = false;
		m_EntityHandles[i].m_Used = false;
	}

	for(int i = 0; i < MAX_CLIENTS; ++i)
	{
		m_GrenadePrime[i].m_PrimeTime = m_GrenadePrime[i].m_GrenType = m_GrenadePrime[i].m_GrenClass = 0;
		m_GrenadePrime[i].m_Throw = false;
	}
}

int Bot_Interface_Init()
{
	if(g_OmniBotEnable.integer == 0)
	{
		G_Printf("%s%s\n", S_COLOR_GREEN, "Omni-bot is currently disabled with \"omnibot_enable 0\"");
		return 1;
	}

	g_InterfaceFunctions = new ETFInterface;
	eomnibot_error  err = Omnibot_LoadLibrary(ETF_VERSION_LATEST, "omnibot_etf", Omnibot_FixPath(g_OmniBotPath.string));
	return err == BOT_ERROR_NONE;
}

void Bot_Interface_Shutdown()
{
	if(IsOmnibotLoaded())
	{
		g_BotFunctions.pfnShutdown();
	}
	Omnibot_FreeLibrary();
}

void Bot_Interface_Update()
{
	if(IsOmnibotLoaded())
	{
		char            buf[1024] = { 0 };

//#ifdef _DEBUG
//		trap_Cvar_Set("sv_cheats", "1");
//		trap_Cvar_Update(&g_cheats);
//#endif
		/*if (level.framenum == GAME_INIT_FRAMES) // GAME_INIT_FRAMES for Q3 is 3 instead of 6 for ET
		   Bot_Event_StartGame();*/

		//////////////////////////////////////////////////////////////////////////
		{
			static float    serverGravity = 0.0f;

			if(serverGravity != g_gravity.value)
			{
				Event_SystemGravity d = { -g_gravity.value };
				g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_GRAVITY, &d, sizeof(d)));
				serverGravity = g_gravity.value;
			}
			static int      cheatsEnabled = 0;

			if(g_cheats.integer != cheatsEnabled)
			{
				Event_SystemCheats d = { g_cheats.integer ? True : False };
				g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_CHEATS, &d, sizeof(d)));
				cheatsEnabled = g_cheats.integer;
			}
		}

		int             iNumBots = 0;

		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			if(!g_entities[i].inuse)
				continue;
			if(!g_entities[i].client)
				continue;
			if(g_entities[i].client->pers.connected != CON_CONNECTED)
				continue;

			// Send a spectated message to bots that are being spectated.
			if(Q3F_IsSpectator(g_entities[i].client) &&
				(g_entities[i].client->sess.spectatorState == SPECTATOR_FOLLOW || g_entities[i].client->sess.spectatorState == SPECTATOR_CHASE))
			{
				int             iDestination = g_entities[i].client->sess.spectatorClient;

				Bot_Event_Spectated(&g_entities[iDestination], i);
			}

			// fake handle server commands (to prevent server command overflow)
			if((g_entities[i].inuse == qtrue) && IsBot(&g_entities[i]))
			{
				++iNumBots;
				while(trap_BotGetServerCommand(i, buf, sizeof(buf)))
				{
				}
			}
		}

		if(!(g_OmniBotFlags.integer & OBF_DONT_SHOW_BOTCOUNT))
		{
			if(g_OmniBotPlaying.integer != iNumBots)
			{
				//g_OmniBotPlaying.integer = iNumBots;
				trap_Cvar_Set("omnibot_playing", va("%i", iNumBots));
				trap_Cvar_Update(&g_OmniBotPlaying);
				trap_Cvar_Set("sv_numbots", va("%i", iNumBots));
				trap_Cvar_Set("g_heavyWeaponRestriction", va("%i", iNumBots));
			}
		}
		else
		{
			if(g_OmniBotPlaying.integer != -1)
			{
				//g_OmniBotPlaying.integer = -1;
				trap_Cvar_Set("omnibot_playing", "-1");
				trap_Cvar_Update(&g_OmniBotPlaying);
				trap_Cvar_Set("sv_numbots", "0");
				trap_Cvar_Set("g_heavyWeaponRestriction", "0");
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// Register any pending entity updates.
		for(int i = 0; i < MAX_GENTITIES; ++i)
		{
			if(m_EntityHandles[i].m_NewEntity && g_entities[i].inuse)
			{
				m_EntityHandles[i].m_NewEntity = false;
				Bot_Event_EntityCreated(&g_entities[i]);
			}
		}
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Call the libraries update.
		g_BotFunctions.pfnUpdate();
		//////////////////////////////////////////////////////////////////////////
	}
}

//////////////////////////////////////////////////////////////////////////

int Bot_Interface_ConsoleCommand(int skiparg)
{
	enum { BuffSize=32 };
	char buffer[BuffSize] = {};
	trap_Argv(1,buffer,BuffSize);

	if(IsOmnibotLoaded())
	{
		if(!Q_stricmp( buffer, "unload" ))
		{
			Bot_Interface_Shutdown();
			return 1;
		}
		else if(!Q_stricmp( buffer, "reload" ))
		{
			Bot_Interface_Shutdown();
			Bot_Interface_InitHandles();
			Bot_Interface_Init();
			return 1;
		}

		Arguments       args;
		for(int i = 0; i < trap_Argc(); ++i)
		{
			trap_Argv(i, args.m_Args[args.m_NumArgs++], Arguments::MaxArgLength);
		}
		g_BotFunctions.pfnConsoleCommand(args);
	}
	else
	{
		if(!Q_stricmp( buffer, "load" ))
		{
			Bot_Interface_InitHandles();
			Bot_Interface_Init();
			return 1;
		}
		else
		{
			G_Printf("%s%s\n", S_COLOR_RED, "Omni-bot not loaded.");
		}
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////

void Bot_Event_ClientConnected(gentity_t * _player, qboolean _isbot)
{
	if(IsOmnibotLoaded())
	{
		if(_isbot)
		{
			// Reset grenade cache
			int             index = ENTINDEX(_player);
			m_GrenadePrime[index].m_PrimeTime = 0;
			m_GrenadePrime[index].m_Throw = false;
			m_GrenadePrime[index].m_GrenType = 0;
			m_GrenadePrime[index].m_GrenClass = Q3F_GREN_NONE;
		}

		Event_SystemClientConnected d;

		d.m_GameId = ENTINDEX(_player);
		d.m_IsBot = _isbot == qtrue ? True : False;
		g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_CLIENTCONNECTED, &d, sizeof(d)));
	}
}

void Bot_Event_ClientDisConnected(gentity_t * _player)
{
	if(IsOmnibotLoaded())
	{
		if(IsBot(_player))
		{
			// Reset grenade cache
			int             index = ENTINDEX(_player);
			m_GrenadePrime[index].m_PrimeTime = 0;
			m_GrenadePrime[index].m_Throw = false;
			m_GrenadePrime[index].m_GrenType = 0;
			m_GrenadePrime[index].m_GrenClass = Q3F_GREN_NONE;
		}

		Event_SystemClientDisConnected d = { ENTINDEX(_player) };

		g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_CLIENTDISCONNECTED, &d, sizeof(d)));
	}
}

void Bot_Event_ResetWeapons(gentity_t * _player)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(MESSAGE_RESETWEAPONS));
	}
}

void Bot_Event_AddWeapon(gentity_t * _player, int _weaponId)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_AddWeapon d = { _weaponId };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(MESSAGE_ADDWEAPON, &d, sizeof(d)));
	}
}

void Bot_Event_RemoveWeapon(gentity_t * _player, int _weaponId)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_RemoveWeapon d = { _weaponId };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(MESSAGE_REMOVEWEAPON, &d, sizeof(d)));
	}
}

void Bot_Event_TakeDamage(gentity_t * _player, gentity_t * _attacker)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_TakeDamage d = { HandleFromEntity(_attacker) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(PERCEPT_FEEL_PAIN, &d, sizeof(d)));
	}
}

void Bot_Event_Death(gentity_t * _victim, gentity_t * _killer, const char *_meansofdeath)
{
	if(IsOmnibotLoaded() && _victim && _victim->client && _victim->client->pers.connected == CON_CONNECTED && IsBot(_victim))
	{
		Event_Death     d;

		d.m_WhoKilledMe = HandleFromEntity(_killer);
		Q_strncpyz(d.m_MeansOfDeath, _meansofdeath ? _meansofdeath : "<unknown>", sizeof(d.m_MeansOfDeath));
		g_BotFunctions.pfnSendEvent(ENTINDEX(_victim), MessageHelper(MESSAGE_DEATH, &d, sizeof(d)));
	}
}

void Bot_Event_Healed(gentity_t * _player, gentity_t * _whodoneit)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_Healed    d = { HandleFromEntity(_whodoneit) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(MESSAGE_HEALED, &d, sizeof(d)));
	}
}

void Bot_Event_KilledSomeone(gentity_t * _attacker, gentity_t * _victim, const char *_meansofdeath)
{
	if(IsOmnibotLoaded() && _attacker && _attacker->client && _attacker->client->pers.connected == CON_CONNECTED && IsBot(_attacker))
	{
		Event_KilledSomeone d;

		d.m_WhoIKilled = HandleFromEntity(_victim);
		Q_strncpyz(d.m_MeansOfDeath,
				   _meansofdeath ? _meansofdeath : "<unknown>", sizeof(d.m_MeansOfDeath) / sizeof(d.m_MeansOfDeath[0]));
		g_BotFunctions.pfnSendEvent(ENTINDEX(_attacker), MessageHelper(MESSAGE_KILLEDSOMEONE, &d, sizeof(d)));
	}
}

void Bot_Event_Build_MustBeOnGround(gentity_t * _player, int _buildable)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_BUILD_MUSTBEONGROUND));
	}
}

void Bot_Event_Build_CantBuild(gentity_t * _player, int _buildable)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		int             iMsg = 0;

		switch (_buildable)
		{
			case Q3F_GREN_CHARGE:
				iMsg = TF_MSG_DETPACK_CANTBUILD;
				break;
			case ET_Q3F_SUPPLYSTATION:
				iMsg = TF_MSG_DISPENSER_CANTBUILD;
				break;
			case ET_Q3F_SENTRY:
				iMsg = TF_MSG_SENTRY_CANTBUILD;
				break;
		}
		if(iMsg != 0)
		{
			g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(iMsg));
		}
	}
}

void Bot_Event_Build_AlreadyBuilt(gentity_t * _player, int _buildable)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		int             iMsg = 0;

		switch (_buildable)
		{
			case Q3F_GREN_CHARGE:
				iMsg = TF_MSG_DETPACK_ALREADYBUILT;
				break;
			case ET_Q3F_SUPPLYSTATION:
				iMsg = TF_MSG_DISPENSER_ALREADYBUILT;
				break;
			case ET_Q3F_SENTRY:
				iMsg = TF_MSG_SENTRY_ALREADYBUILT;
				break;
		}
		if(iMsg != 0)
		{
			g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(iMsg));
		}
	}
}

void Bot_Event_Build_NotEnoughAmmo(gentity_t * _player, int _buildable)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		int             iMsg = 0;

		switch (_buildable)
		{
			case Q3F_GREN_CHARGE:
				iMsg = TF_MSG_DETPACK_NOTENOUGHAMMO;
				break;
			case ET_Q3F_SUPPLYSTATION:
				iMsg = TF_MSG_DISPENSER_NOTENOUGHAMMO;
				break;
			case ET_Q3F_SENTRY:
				iMsg = TF_MSG_SENTRY_NOTENOUGHAMMO;
				break;
		}
		if(iMsg != 0)
		{
			g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(iMsg));
		}
	}
}

void Bot_Event_BuildCancelled(gentity_t * _player, int _buildable)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		int             iMsg = 0;

		switch (_buildable)
		{
			case Q3F_GREN_CHARGE:
				iMsg = TF_MSG_DETPACK_BUILDCANCEL;
				break;
			case ET_Q3F_SUPPLYSTATION:
				iMsg = TF_MSG_DISPENSER_BUILDCANCEL;
				break;
			case ET_Q3F_SENTRY:
				iMsg = TF_MSG_SENTRY_BUILDCANCEL;
				break;
		}
		if(iMsg != 0)
		{
			g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(iMsg));
		}
	}
}

void Bot_Event_CantDisguiseAsTeam(gentity_t * _player, int _disguiseTeam)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_CantDisguiseTeam_TF d = { Bot_TeamGameToBot(_disguiseTeam) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_CANTDISGUISE_AS_TEAM, &d, sizeof(d)));
	}
}

void Bot_Event_CantDisguiseAsClass(gentity_t * _player, int _disguiseClass)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_CantDisguiseClass_TF d = { Bot_PlayerClassGameToBot(_disguiseClass) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_CANTDISGUISE_AS_CLASS, &d, sizeof(d)));
	}
}

void Bot_Event_Disguising(gentity_t * _player, int _disguiseTeam, int _disguiseClass)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_Disguise_TF d;

		d.m_ClassId = _disguiseClass;
		d.m_TeamId = _disguiseTeam;
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_DISGUISING, &d, sizeof(d)));
	}
}

void Bot_Event_Disguised(gentity_t * _player, int _disguiseTeam, int _disguiseClass)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_Disguise_TF d;

		d.m_ClassId = _disguiseClass;
		d.m_TeamId = _disguiseTeam;
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_DISGUISING, &d, sizeof(d)));
	}
}

void Bot_Event_DisguiseLost(gentity_t * _player)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_DISGUISE_LOST));
	}
}

void Bot_Event_UnCloaked(gentity_t * _player)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_UNCLOAKED));
	}
}

void Bot_Event_CantCloak(gentity_t * _player)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_CANT_CLOAK));
	}
}

void Bot_Event_Cloaked(gentity_t * _player)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_CLOAKED));
	}
}

void Bot_Event_RadarDetectedEnemy(gentity_t * _player, gentity_t * _ent)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_RadarUpdate_TF d = { HandleFromEntity(_ent) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_RADAR_DETECT_ENEMY, &d, sizeof(d)));
	}
}

void Bot_Event_BuildableDamaged(gentity_t * _player, int _type, gentity_t * _buildEnt)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		int             iMsg = 0;

		switch (_type)
		{
			case ET_Q3F_SUPPLYSTATION:
				iMsg = TF_MSG_DISPENSER_DAMAGED;
				break;
			case ET_Q3F_SENTRY:
				iMsg = TF_MSG_SENTRY_DAMAGED;
				break;
			default:
				return;
		}

		if(iMsg != 0)
		{
			Event_BuildableDamaged_TF d = { HandleFromEntity(_buildEnt) };
			g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(iMsg, &d, sizeof(d)));
		}
	}
}

void Bot_Event_DispenserBuilding(gentity_t * _player, gentity_t * _buildEnt)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_DispenserBuilding_TF d = { HandleFromEntity(_buildEnt) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_DISPENSER_BUILDING, &d, sizeof(d)));
	}
}

void Bot_Event_DispenserBuilt(gentity_t * _player, gentity_t * _buildEnt)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_DispenserBuilt_TF d = { HandleFromEntity(_buildEnt) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_DISPENSER_BUILT, &d, sizeof(d)));
	}
}

void Bot_Event_DispenserEnemyUsed(gentity_t * _player, gentity_t * _enemyUser)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_DispenserEnemyUsed_TF d = { HandleFromEntity(_enemyUser) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_DISPENSER_ENEMYUSED, &d, sizeof(d)));
	}
}

void Bot_Event_DispenserDestroyed(gentity_t * _player, gentity_t * _attacker)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_BuildableDestroyed_TF d = { HandleFromEntity(_attacker) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_DISPENSER_DESTROYED, &d, sizeof(d)));
	}
}

void Bot_Event_SentryUpgraded(gentity_t * _player, int _level)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_SentryUpgraded_TF d = { _level };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_SENTRY_UPGRADED, &d, sizeof(d)));
	}
}

void Bot_Event_SentryBuilding(gentity_t * _player, gentity_t * _buildEnt)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_SentryBuilding_TF d = { HandleFromEntity(_buildEnt) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_SENTRY_BUILDING, &d, sizeof(d)));
	}
}

void Bot_Event_SentryBuilt(gentity_t * _player, gentity_t * _buildEnt)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_SentryBuilt_TF d = { HandleFromEntity(_buildEnt) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_SENTRY_BUILT, &d, sizeof(d)));
	}
}

void Bot_Event_SentryDestroyed(gentity_t * _player, gentity_t * _attacker)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_BuildableDestroyed_TF d = { HandleFromEntity(_attacker) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_SENTRY_DESTROYED, &d, sizeof(d)));
	}
}

void Bot_Event_SentrySpottedEnemy(gentity_t * _player, gentity_t * _enemy)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_SentrySpotEnemy_TF d = { HandleFromEntity(_enemy) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_SENTRY_SPOTENEMY, &d, sizeof(d)));
	}
}

void Bot_Event_SentryAimed(gentity_t * _player, gentity_t * _buildEnt, vec3_t _dir)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_SentryAimed_TF d;

		d.m_Sentry = HandleFromEntity(_buildEnt);
		d.m_Direction[0] = _dir[0];
		d.m_Direction[1] = _dir[1];
		d.m_Direction[2] = _dir[2];
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_SENTRY_AIMED, &d, sizeof(d)));
	}
}

void Bot_Event_DetpackBuilding(gentity_t * _player, gentity_t * _buildEnt)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_DetpackBuilding_TF d = { HandleFromEntity(_buildEnt) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_DETPACK_BUILDING, &d, sizeof(d)));
	}
}

void Bot_Event_DetpackBuilt(gentity_t * _player, gentity_t * _buildEnt)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_DetpackBuilt_TF d = { HandleFromEntity(_buildEnt) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_DETPACK_BUILT, &d, sizeof(d)));
	}
}

void Bot_Event_DetpackDetonated(gentity_t * _player)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_DETPACK_DETONATED));
	}
}

void Bot_Event_DispenserDetonated(gentity_t * _player)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_DISPENSER_DETONATED));
	}
}

void Bot_Event_DispenserDismantled(gentity_t * _player)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_DISPENSER_DISMANTLED));
	}
}

void Bot_Event_SentryDetonated(gentity_t * _player)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_SENTRY_DETONATED));
	}
}

void Bot_Event_SentryDismantled(gentity_t * _player)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_SENTRY_DISMANTLED));
	}
}

void Bot_Event_FireWeapon(gentity_t * _player, int _weaponId, gentity_t * _projectile)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_WeaponFire d = {_weaponId, Primary, HandleFromEntity(_projectile)};
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(ACTION_WEAPON_FIRE, &d, sizeof(d)));
	}
}

void Bot_Event_Spectated(gentity_t * _player, int _who)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_Spectated d = { _who };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(MESSAGE_SPECTATED, &d, sizeof(d)));
	}
}

void Bot_Event_ChatMessage(gentity_t * _to, gentity_t * _source, int _type, const char *_message)
{
	if(IsOmnibotLoaded() && IsBot(_to))
	{
		int             iMsg = PERCEPT_HEAR_GLOBALCHATMSG;

		switch (_type)
		{
			case SAY_ALL:
				iMsg = PERCEPT_HEAR_GLOBALCHATMSG;
				break;
			case SAY_TEAM:
				iMsg = PERCEPT_HEAR_TEAMCHATMSG;
				break;
			default:
				return;
		}

		Event_ChatMessage d;

		d.m_WhoSaidIt = HandleFromEntity(_source);
		Q_strncpyz(d.m_Message, _message ? _message : "<unknown>", sizeof(d.m_Message) / sizeof(d.m_Message[0]));
		g_BotFunctions.pfnSendEvent(ENTINDEX(_to), MessageHelper(iMsg, &d, sizeof(d)));
	}
}

void Bot_Event_PlayerUsed(gentity_t * _player, gentity_t * _entityUsed)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		Event_PlayerUsed d = { HandleFromEntity(_entityUsed) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(PERCEPT_FEEL_PLAYER_USE, &d, sizeof(d)));
	}
}

void Bot_Event_GotSpannerArmor(gentity_t * _target, gentity_t * _engy, int _before, int _after)
{
	if(IsOmnibotLoaded() && IsBot(_target))
	{
		Event_GotEngyArmor d = { HandleFromEntity(_engy), _before, _after };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_target), MessageHelper(TF_MSG_GOT_ENGY_ARMOR, &d, sizeof(d)));
	}
}

void Bot_Event_GaveSpannerArmor(gentity_t * _engy, gentity_t * _target, int _before, int _after)
{
	if(IsOmnibotLoaded() && IsBot(_engy))
	{
		Event_GaveEngyArmor d = { HandleFromEntity(_target), _before, _after };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_engy), MessageHelper(TF_MSG_GAVE_ENGY_ARMOR, &d, sizeof(d)));
	}
}

void Bot_Event_GotMedicHealth(gentity_t * _target, gentity_t * _medic, int _before, int _after)
{
	if(IsOmnibotLoaded() && IsBot(_target))
	{
		Event_GotMedicHealth d = { HandleFromEntity(_medic), _before, _after };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_target), MessageHelper(TF_MSG_GOT_MEDIC_HEALTH, &d, sizeof(d)));
	}
}

void Bot_Event_GaveMedicHealth(gentity_t * _medic, gentity_t * _target, int _before, int _after)
{
	if(IsOmnibotLoaded() && IsBot(_medic))
	{
		Event_GaveMedicHealth d = { HandleFromEntity(_target), _before, _after };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_medic), MessageHelper(TF_MSG_GAVE_MEDIC_HEALTH, &d, sizeof(d)));
	}
}

void Bot_Event_Infected(gentity_t * _target, gentity_t * _infector)
{
	if(IsOmnibotLoaded() && IsBot(_target))
	{
		Event_Infected  d = { HandleFromEntity(_infector) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_target), MessageHelper(TF_MSG_INFECTED, &d, sizeof(d)));
	}
}

void Bot_Event_Cured(gentity_t * _curee, gentity_t * _curer)
{
	if(IsOmnibotLoaded() && IsBot(_curee))
	{
		Event_Cured     d = { HandleFromEntity(_curer) };
		g_BotFunctions.pfnSendEvent(ENTINDEX(_curee), MessageHelper(TF_MSG_CURED, &d, sizeof(d)));
	}
}

void Bot_Event_GotDispenserAmmo(gentity_t * _player)
{
	if(IsOmnibotLoaded() && IsBot(_player))
	{
		g_BotFunctions.pfnSendEvent(ENTINDEX(_player), MessageHelper(TF_MSG_GOT_DISPENSER_AMMO));
	}
}

void Bot_Event_MedicCall(gentity_t * _player)
{
	if(IsOmnibotLoaded())
	{
		Event_MedicCall d;

		d.m_ByWho = HandleFromEntity(_player);
		g_BotFunctions.pfnSendGlobalEvent(MessageHelper(TF_MSG_CALLFORMEDIC, &d, sizeof(d)));
	}
}

void Bot_Event_EngineerCall(gentity_t * _player)
{
	if(IsOmnibotLoaded())
	{
		Event_EngineerCall d;

		d.m_ByWho = HandleFromEntity(_player);
		g_BotFunctions.pfnSendGlobalEvent(MessageHelper(TF_MSG_CALLFORENGINEER, &d, sizeof(d)));
	}
}

void Bot_Event_Sound(gentity_t * _source, int _sndtype, const char *_name)
{
	if(IsOmnibotLoaded())
	{
		Event_Sound     d = { };
		d.m_Source = HandleFromEntity(_source);
		d.m_SoundType = _sndtype;
		g_InterfaceFunctions->GetEntityPosition(d.m_Source, d.m_Origin);
		Q_strncpyz(d.m_SoundName, _name ? _name : "<unknown>", sizeof(d.m_SoundName) / sizeof(d.m_SoundName[0]));
		g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_SOUND, &d, sizeof(d)));
	}
}

extern          "C"
{
	int G_Q3F_SentryMaxHealth( int level );
	int G_Q3F_SentryMaxShells( int level );
}
void Bot_Event_SendSentryStatsToBot(gentity_t * _sentry)
{
	if(IsOmnibotLoaded() && _sentry->parent && IsBot(_sentry->parent))
	{
		Event_SentryStatus_TF d;

		d.m_Entity = HandleFromEntity(_sentry);
		d.m_Health = _sentry->health;
		d.m_MaxHealth = G_Q3F_SentryMaxHealth(_sentry->s.legsAnim);
		d.m_Shells[0] = _sentry->s.otherEntityNum;
		d.m_Shells[1] = G_Q3F_SentryMaxShells(_sentry->s.legsAnim);
		d.m_Rockets[0] = _sentry->s.legsAnim >= 3 ? _sentry->s.otherEntityNum2 : 0;
		d.m_Rockets[1] = _sentry->s.legsAnim >= 3 ? 20 : 0;
		d.m_Level = _sentry->s.legsAnim;
		d.m_Position[0] = _sentry->r.currentOrigin[0];
		d.m_Position[1] = _sentry->r.currentOrigin[1];
		d.m_Position[2] = _sentry->r.currentOrigin[2];
		d.m_Facing[0] = _sentry->s.angles[0];
		d.m_Facing[1] = _sentry->s.angles[1];
		d.m_Facing[2] = _sentry->s.angles[2];
		g_BotFunctions.pfnSendEvent(ENTINDEX(_sentry->parent), MessageHelper(TF_MSG_SENTRY_STATS, &d, sizeof(d)));
	}
}

void Bot_Event_SendSupplyStatsToBot(gentity_t * _supplystation)
{
	if(IsOmnibotLoaded() && _supplystation->parent && IsBot(_supplystation->parent))
	{
		Event_DispenserStatus_TF d;

		d.m_Entity = HandleFromEntity(_supplystation);
		d.m_Health = _supplystation->health;
		d.m_Shells = _supplystation->s.origin2[0];
		d.m_Nails = _supplystation->s.origin2[1];
		d.m_Rockets = _supplystation->s.origin2[2];
		d.m_Cells = _supplystation->s.angles2[0];
		d.m_Armor = _supplystation->s.angles2[1];
		d.m_Position[0] = _supplystation->r.currentOrigin[0];
		d.m_Position[1] = _supplystation->r.currentOrigin[1];
		d.m_Position[2] = _supplystation->r.currentOrigin[2];
		d.m_Facing[0] = _supplystation->s.angles[0];
		d.m_Facing[1] = _supplystation->s.angles[1];
		d.m_Facing[2] = _supplystation->s.angles[2];
		g_BotFunctions.pfnSendEvent(ENTINDEX(_supplystation->parent), MessageHelper(TF_MSG_DISPENSER_STATS, &d, sizeof(d)));
	}
}

void            Bot_Event_EntityCreated(gentity_t * pEnt)
{
	if(pEnt && IsOmnibotLoaded())
	{
		// Get common properties.
		const int iEntNum = ENTINDEX(pEnt);
		GameEntity ent = HandleFromEntity(pEnt);
		int iClass = g_InterfaceFunctions->GetEntityClass(ent);

		if(iClass)
		{
			Event_EntityCreated d;
			d.m_Entity = GameEntity(iEntNum, m_EntityHandles[iEntNum].m_HandleSerial);

			d.m_EntityClass = iClass;
			g_InterfaceFunctions->GetEntityCategory(ent, d.m_EntityCategory);
			g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_ENTITYCREATED, &d, sizeof(d)));
			m_EntityHandles[iEntNum].m_Used = true;
		}
	}
}

extern          "C"
{
	void            Bot_TeleporterSource(gentity_t * pEnt)
	{
		if(IsOmnibotLoaded() && pEnt)
		{
			// teleporters involve dynamic pathfinding, so add the connection here.
			Event_EntityConnection c;

			c.m_Entity = HandleFromEntity(pEnt);
			c.m_ConnectionId = ENTINDEX(pEnt);
			c.m_ConnectionDir = CON_SOURCE;
			c.m_Teleport = true;
			c.m_Radius = RadiusFromBounds(pEnt->r.mins, pEnt->r.maxs);

			g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_ADD_ENTITY_CONNECTION, &c, sizeof(c)));
		}
	}

	void            Bot_TeleporterDest(gentity_t * pEnt)
	{
		if(IsOmnibotLoaded() && pEnt)
		{
			// teleporters involve dynamic pathfinding, so add the connection here.
			Event_EntityConnection c;

			c.m_Entity = HandleFromEntity(pEnt);
			c.m_ConnectionId = ENTINDEX(pEnt);
			c.m_ConnectionDir = CON_DEST;
			c.m_Teleport = true;
			c.m_Radius = 32.f;

			g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_ADD_ENTITY_CONNECTION, &c, sizeof(c)));
		}
	}

	void            Bot_Queue_EntityCreated(gentity_t * pEnt)
	{
		if(pEnt)
			m_EntityHandles[ENTINDEX(pEnt)].m_NewEntity = true;
	}
	void            Bot_Event_EntityDeleted(gentity_t * pEnt)
	{
		if(pEnt)
		{
			const int iEntNum = ENTINDEX(pEnt);

			if(IsOmnibotLoaded())
			{
				Event_EntityDeleted d = { GameEntity(iEntNum, m_EntityHandles[iEntNum].m_HandleSerial) };
				g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_ENTITYDELETED, &d, sizeof(d)));
			}
			m_EntityHandles[iEntNum].m_Used = false;
			m_EntityHandles[iEntNum].m_NewEntity = false;
			while(++m_EntityHandles[iEntNum].m_HandleSerial == 0)
			{
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////

	void            Bot_Util_SendTrigger(gentity_t * _ent, gentity_t * _activator, const char *_tagname, const char *_action)
	{
		if(IsOmnibotLoaded())
		{
			TriggerInfo     triggerInfo;

			triggerInfo.m_Entity = HandleFromEntity(_ent);
			Q_strncpyz(triggerInfo.m_TagName, _tagname, TriggerBufferSize);
			Q_strncpyz(triggerInfo.m_Action, _action, TriggerBufferSize);
			g_BotFunctions.pfnSendTrigger(triggerInfo);
		}
	}

	void Bot_AddDisarmGoal(gentity_t *_ent, int _team)
	{
		const int       iAllTeams = (1 << Q3F_TEAM_RED) | (1 << Q3F_TEAM_BLUE) | (1 << Q3F_TEAM_YELLOW) | (1 << Q3F_TEAM_GREEN);
		int             teams = iAllTeams;
		const char     *pGoalName = _GetEntityName(_ent);

		teams &= ~_team;

		teams = Bot_TeamGameBitsToBotBits(teams);

		Bot_Util_AddGoal("disarm",_ent,teams,pGoalName);
	}
};
