//////////////////////////////////////////////////////////////////////////
// Bot-Related Includes

#include "../idlib/precompiled.h"
#pragma hdrstop

//#include "../Projectile.h"
#ifdef _D3XP
#include "../../d3xp/Game_local.h"
#include "../../d3xp/Item.h"
#else
#include "../Game_local.h"
#include "../Item.h"
#endif

//#pragma optimize("", off)

extern idCVar si_maxPlayers;

idUserInterface *g_BotUI = 0;
bool g_DrawBotHud = false;

#define OB_snprintf idStr::snPrintf
#include "BotExports.h"
#include "omnibot_interface.h"
#include "D3_Config.h"
#include "D3_Messages.h"

idCVar omnibot_path("omnibot_path", "", CVAR_SYSTEM | CVAR_ARCHIVE, "" );
idCVar omnibot_flags("omnibot_flags", "0", CVAR_SYSTEM | CVAR_ARCHIVE | CVAR_INTEGER | CVAR_SERVERINFO, "" );
idCVar omnibot_playing("omnibot_playing", "", CVAR_SYSTEM | CVAR_ROM | CVAR_INTEGER | CVAR_SERVERINFO, "" );

static idVec3 upVector(0.0f,0.0f,-1.0f);

//-----------------------------------------------------------------

bool		g_Bot[MAX_CLIENTS];
idDict		g_BotUserInfo[MAX_CLIENTS];
int			g_WeaponSlotCache[D3_WP_MAX];
const int START_BOT_INDEX = 4;

struct BotEntity
{
	obint16	m_HandleSerial;
	bool	m_NewEntity : 1;
	bool	m_Used : 1;
};

BotEntity		m_EntityHandles[MAX_GENTITIES];

//-----------------------------------------------------------------

void idBoundsToAABB(const idBounds &_idBounds, AABB &_aabb)
{
	_aabb.m_Mins[0] = _idBounds.b[0].x;
	_aabb.m_Mins[1] = _idBounds.b[0].y;
	_aabb.m_Mins[2] = _idBounds.b[0].z;
	_aabb.m_Maxs[0] = _idBounds.b[1].x;
	_aabb.m_Maxs[1] = _idBounds.b[1].y;
	_aabb.m_Maxs[2] = _idBounds.b[1].z;
}

int CountTeamPlayers(int &_red, int &_blue, int _exclude = -1)
{
	_red = 0;
	_blue = 0;

	for(int i = 0; i < MAX_CLIENTS; ++i)
	{
		if(gameLocal.entities[i] && gameLocal.entities[i]->IsType(idPlayer::Type)) 
		{
			idPlayer *pPlr = static_cast<idPlayer*>(gameLocal.entities[i]);

			if(pPlr->spectating)
				continue;

			if(pPlr->entityNumber == _exclude)
				continue;

			if(pPlr->team == 0)
				_red++;
			else if(pPlr->team == 1)
				_blue++;
		}
	}
	return _red + _blue;
}

//-----------------------------------------------------------------

bool AreCheatsEnabled()
{
	return cvarSystem->GetCVarBool( "net_allowCheats" );
}

//-----------------------------------------------------------------

idEntity* INDEXENT(int iEdictNum)		
{
	return gameLocal.FindEntity(iEdictNum);
}

int ENTINDEX(const idEntity *pEdict)		
{ 
	return pEdict->entityNumber;
}

bool omnibot_interface::IsBot(int _clientnum)
{
	if((_clientnum >= 0) && (_clientnum < MAX_CLIENTS))
		return g_Bot[_clientnum];
	return false;
}

//-----------------------------------------------------------------

const char *g_Weapons[D3_WP_MAX] =
{
	"",
	"weapon_flashlight",
	"weapon_fists",
	"weapon_chainsaw",
	"weapon_pistol",
	"weapon_shotgun",
	"weapon_shotgun_double",
	"weapon_machinegun",
	"weapon_chaingun",
	"weapon_handgrenade",
	"weapon_plasmagun",
	"weapon_rocketlauncher",
	"weapon_bfg",
	"weapon_soulcube",
	"weapon_grabber",
};

int obUtilGetWeaponId(const char *_weaponName)
{
	if(_weaponName)
	{
		int iLen = idStr::Length(_weaponName);
		for(int i = 1; i < D3_WP_MAX; ++i)
		{
			if(!idStr::Cmpn(g_Weapons[i], _weaponName, iLen))
				return i;
		}
	}
	return D3_WP_NONE;
}

const char *obUtilGetStringFromWeaponId(int _weaponId)
{
	if(_weaponId > D3_WP_NONE && _weaponId < D3_WP_MAX)
		return g_Weapons[_weaponId];
	return 0;
}

const int obUtilGetBotTeamFromGameTeam(int _team)
{
#ifdef CTF
	switch(_team)
	{
	case 0:
		return D3_TEAM_RED;
	case 1:
		return D3_TEAM_BLUE;
	}
#endif
	return D3_TEAM_NONE;
}

const int obUtilGetGameTeamFromBotTeam(int _team)
{
#ifdef CTF
	switch(_team)
	{
	case D3_TEAM_RED:
		return 0;
	case D3_TEAM_BLUE:
		return 1;
	}
#endif
	return -1;
}

int obUtilBotContentsFromGameContents(int _contents)
{
	int iBotContents = 0;
	if(_contents & CONTENTS_SOLID)
		iBotContents |= CONT_SOLID;
	if(_contents & CONTENTS_WATER)
		iBotContents |= CONT_WATER;
	if(_contents & CONTENTS_TRIGGER)
		iBotContents |= CONT_TRIGGER;
	return iBotContents;
}

idEntity *EntityFromHandle(GameEntity _ent)
{
	obint16 index = _ent.GetIndex();
	if(m_EntityHandles[index].m_HandleSerial == _ent.GetSerial() && gameLocal.entities[index])
		return gameLocal.entities[index];
	return NULL;
}

GameEntity HandleFromEntity(idEntity *_ent)
{
	if(_ent)
		return GameEntity(_ent->entityNumber, m_EntityHandles[_ent->entityNumber].m_HandleSerial);
	else
		return GameEntity();
}

void Bot_Util_AddGoal(const char *_type,idEntity *_ent, int _team, const char *_tag)
{
	if(IsOmnibotLoaded())
	{
		MapGoalDef goaldef;

		goaldef.Props.SetString("Type",_type);
		goaldef.Props.SetEntity("Entity",HandleFromEntity(_ent));
		goaldef.Props.SetInt("Team",_team);
		goaldef.Props.SetString("TagName",_tag);
		goaldef.Props.SetInt("InterfaceGoal",1);

		g_BotFunctions.pfnAddGoal(goaldef);
	}
}

//////////////////////////////////////////////////////////////////////////

class D3Interface : public IEngineInterface
{
public:
	int AddBot(const MessageHelper &_data)
	{
		OB_GETMSG(Msg_Addbot);

		int iClientNum = -1;
		const char *guid = NULL;

		if ( !gameLocal.isMultiplayer ) 
		{
			gameLocal.Printf( "You may only add a bot to a multiplayer game\n" );
			return iClientNum;
		}

		if ( !gameLocal.isServer ) 
		{
			gameLocal.Printf( "Bots may only be added on server\n" );
			return iClientNum;
		}

		iClientNum = checkFreeSlot(true, -1);

		// Find a free slot.
		/*for(int i = START_BOT_INDEX; i < MAX_CLIENTS; ++i)
		{
			if(!gameLocal.entities[i])
			{
				iClientNum = i;
				break;
			}
		}*/

		if(iClientNum == -1)
		{
			g_InterfaceFunctions->PrintError("Unable to Add Bot!");
			return -1;
		}

		static idRandom rnd;

		//const int iNumPlayerModels = sizeof(g_AllPlayerModels) / sizeof(g_AllPlayerModels[0]);
		//const int iNumHumanModels = sizeof(g_HumanPlayerModels) / sizeof(g_HumanPlayerModels[0]);
		//const int iNumStroggModels = sizeof(g_StroggPlayerModels) / sizeof(g_StroggPlayerModels[0]);

		// Team
		int iRedPlayers = 0, iBluePlayers = 0;
		CountTeamPlayers(iRedPlayers, iBluePlayers, iClientNum);

		int iTeamChoice = 0;
		if(iRedPlayers < iBluePlayers)
			iTeamChoice = 0;
		else if(iRedPlayers > iBluePlayers)
			iTeamChoice = 1;
		else
			iTeamChoice = 0;

		g_BotUserInfo[iClientNum].Set( "ui_name", pMsg->m_Name );
		g_BotUserInfo[iClientNum].Set( "name", pMsg->m_Name );

		// Skin
		int iNumPlayerSkins = 0;
		while(ui_skinArgs[iNumPlayerSkins]) { iNumPlayerSkins++; }
		const char *pSkinName = ui_skinArgs[rnd.RandomInt(iNumPlayerSkins)];

		g_BotUserInfo[iClientNum].Set( "ui_skin", pSkinName );

		g_BotUserInfo[iClientNum].Set( "ui_hitscanTint", "120 0.6 1" );
		g_BotUserInfo[iClientNum].Set( "ui_autoSwitch", "0" );
		g_BotUserInfo[iClientNum].Set( "ui_autoReload", "0" );
		g_BotUserInfo[iClientNum].Set( "ui_showGun", "1" );

		if( gameLocal.IsTeamGame() )
		{
			g_BotUserInfo[iClientNum].Set("ui_team", iTeamChoice==0 ? "Red" : "Blue");
		}

#ifdef _DEBUG
		gameLocal.Printf( "%d Red, %d Blue: Bot joining team: %s\n", 
			iRedPlayers, iBluePlayers, iTeamChoice==0 ? "Red" : "Blue" );
#endif

		g_BotUserInfo[iClientNum].Set( "ui_ready", "Ready" );
		g_BotUserInfo[iClientNum].SetBool("bot", true);
		gameLocal.SetUserInfo( iClientNum, g_BotUserInfo[iClientNum], false, true );
		cmdSystem->BufferCommandText( CMD_EXEC_NOW, va( "updateUI %d\n", iClientNum ) );

		// Mark this slot as a bot.
		g_Bot[iClientNum] = true;

		// Start up client
		gameLocal.ServerClientConnect(iClientNum, guid);
		gameLocal.ServerClientBegin(iClientNum);

		// Success!, return its client num.
		return iClientNum;
	}

	void RemoveBot(const MessageHelper &_data)
	{
		if(!gameLocal.isServer) 
			gameLocal.Printf( "Bots may only be removed on server, only it has the powah!\n" );

		OB_GETMSG(Msg_Kickbot);
		if(pMsg->m_GameId != Msg_Kickbot::InvalidGameId)
		{
			if(omnibot_interface::IsBot(pMsg->m_GameId))
				gameLocal.ServerClientDisconnect(pMsg->m_GameId);
		}
		else
		{
			idPlayer *pPlayer = gameLocal.GetClientByName(pMsg->m_Name);
			if(pPlayer && omnibot_interface::IsBot(pPlayer->entityNumber))
				gameLocal.ServerClientDisconnect(pPlayer->entityNumber);
		}
	}

	obResult ChangeTeam(int _client, int _newteam, const MessageHelper *_data)
	{
		idPlayer *pPlayer = static_cast<idPlayer*>(gameLocal.entities[_client]);

		if(pPlayer) 
		{
			if(gameLocal.IsTeamGame())
			{
				_newteam = obUtilGetGameTeamFromBotTeam(_newteam);

				// Simple random selection for now.
				if(_newteam == RANDOM_TEAM_IF_NO_TEAM || _newteam == RANDOM_TEAM)
				{
					int iRedPlayers = 0, iBluePlayers = 0;
					CountTeamPlayers(iRedPlayers, iBluePlayers, pPlayer->entityNumber);

					if(iRedPlayers < iBluePlayers)
						_newteam = 0;
					else if(iRedPlayers > iBluePlayers)
						_newteam = 1;
					else
						_newteam = 0;
				}
				int iTeam = _newteam;
				pPlayer->spawnArgs.SetInt("team", iTeam);
				g_BotUserInfo[_client].Set("ui_team", iTeam==0 ? "Red" : "Blue");
				gameLocal.SetUserInfo( _client, g_BotUserInfo[_client], true, true );

				pPlayer->Spectate( false );
				if(iTeam != pPlayer->team)
				{
					gameLocal.SwitchTeam(_client, iTeam);
				}
				cmdSystem->BufferCommandText( CMD_EXEC_NOW, va( "updateUI %d\n", _client ) );
			}
			else
			{
				pPlayer->Spectate( false );
				cmdSystem->BufferCommandText( CMD_EXEC_NOW, va( "updateUI %d\n", _client ) );
			}
			return Success;
		}
		return InvalidEntity;
	}

	obResult ChangeClass(int _client, int _newclass, const MessageHelper *_data)
	{
		return Success;
	}

	void UpdateBotInput(int _client, const ClientInput &_input)
	{
		idPlayer *pPlayer = static_cast<idPlayer*>(gameLocal.entities[_client]);
		if(pPlayer)
		{
			usercmd_t &usercmd = gameLocal.usercmds[ _client ];
			memset(&usercmd, 0, sizeof( usercmd_t ) );

			// Process the bot keypresses.		
			if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_ATTACK1))
				usercmd.buttons |= BUTTON_ATTACK;
			if(!_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_WALK))
				usercmd.buttons |= BUTTON_RUN;
			if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_AIM))
				usercmd.buttons |= BUTTON_ZOOM;
			if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_RESPAWN))
				usercmd.buttons |= BUTTON_ATTACK;

			/*if(_input.m_ButtonFlags & BOT_BUTTON_USE)
			usercmd.buttons |= IN_USE;*/

			if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_RELOAD) &&
				pPlayer->weapon.GetEntity() &&
				pPlayer->weapon.GetEntity()->IsReady() &&
				!pPlayer->weapon.GetEntity()->IsReloading()) 
			{
				pPlayer->Reload();
			}

			// Set up the bot facing.
			idVec3 vFacing(_input.m_Facing[0], _input.m_Facing[1], _input.m_Facing[2]);
			idAngles viewAngles = vFacing.ToAngles();
			const idAngles &deltaViewAngles = pPlayer->GetDeltaViewAngles();
			usercmd.angles[0] = ANGLE2SHORT( viewAngles[0] - deltaViewAngles[0] );
			usercmd.angles[1] = ANGLE2SHORT( viewAngles[1] - deltaViewAngles[1] );
			usercmd.angles[2] = ANGLE2SHORT( viewAngles[2] - deltaViewAngles[2] );

			// Calculate the movement vector, taking into account the view direction.
			idVec3 vMoveDir(_input.m_MoveDir[0],_input.m_MoveDir[1],_input.m_MoveDir[2]);

			idVec3 vForward, vRight;//, vUp;
			viewAngles.ToVectors(&vForward, &vRight, NULL);

			float fDotForward	= DotProduct(vForward, vMoveDir);
			float fDotRight		= DotProduct(vRight, vMoveDir);
			//float fDotUp		= DotProduct(vUp, vMoveDir);
			usercmd.forwardmove = (signed char)idMath::ClampFloat(-127.0f, 127.0f, fDotForward * 127.0f);
			usercmd.rightmove = (signed char)idMath::ClampFloat(-127.0f, 127.0f, fDotRight * 127.0f);
			//usercmd.upmove = (signed char)idMath::ClampFloat(-127.0f, 127.0f, fDotUp * 127.0f);

			if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_FWD))
				usercmd.forwardmove = 127;
			if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_BACK))
				usercmd.forwardmove = -127;

			// Clear the vertical move if we aren't crouching or jumping
			if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_JUMP))
				usercmd.upmove = 127;		
			else if(_input.m_ButtonFlags.CheckFlag(BOT_BUTTON_CROUCH))
				usercmd.upmove = -127;

			const char *pNewWeapon = obUtilGetStringFromWeaponId(_input.m_CurrentWeapon);
			if(pNewWeapon)
				pPlayer->SelectWeapon(pNewWeapon);
		}
	}

	void BotCommand(int _client, const char *_cmd) 
	{
		// In Doom 3, commands are translated to events and stuff on the client side before
		// being sent to the server.
		idPlayer *pPlayer = static_cast<idPlayer*>(gameLocal.entities[_client]);
		if(pPlayer)
		{
			// Process chat commands.
			const char *pClientName = pPlayer->GetUserInfo()->GetString( "ui_name", "player" );
			if(!idStr::Cmpn(_cmd, "say_team", sizeof("say_team")-1)) // sizeof counts the null, so sub 1
			{
				idStr s = _cmd + sizeof("say_team");
				s.StripLeading('"');
				s.StripTrailing('"');
				gameLocal.mpGame.ProcessChatMessage( _client, true, pClientName, s.c_str(), NULL );
			}
			else if(!idStr::Cmpn(_cmd, "say", sizeof("say")-1)) // sizeof counts the null, so sub 1
			{
				idStr s = _cmd + sizeof("say");
				s.StripLeading('"');
				s.StripTrailing('"');
				gameLocal.mpGame.ProcessChatMessage( _client, false, pClientName, s.c_str(), NULL );
			}
		}	
	}

	obBool IsInPVS(const float _pos[3], const float _target[3])
	{
		idVec3 vSource(_pos[0], _pos[1], _pos[2]), vDest(_target[0], _target[1], _target[2]);
		pvsHandle_t pvs = gameLocal.pvs.SetupCurrentPVS(vSource);
		bool bInPvs = gameLocal.pvs.InCurrentPVS( pvs, vDest );
		gameLocal.pvs.FreeCurrentPVS( pvs );
		return bInPvs ? True : False;
	}

	obResult TraceLine(obTraceResult &_result, const float _start[3], const float _end[3], 
		const AABB *_pBBox , int _mask, int _user, obBool _bUsePVS)
	{
		idEntity *pIgnoreEnt = _user != -1 ? INDEXENT(_user) : 0;

		int iMask = 0;

		trace_t trace;
		idVec3 vStart(_start[0],_start[1],_start[2]);
		idVec3 vEnd(_end[0],_end[1],_end[2]);

		// TODO: implement PVS check.
		bool bEndPointInPVS = true;
		if(pIgnoreEnt)
		{
			pvsHandle_t pvs = gameLocal.pvs.SetupCurrentPVS( pIgnoreEnt->GetPVSAreas(), pIgnoreEnt->GetNumPVSAreas() );
			bEndPointInPVS = gameLocal.pvs.InCurrentPVS( pvs, vEnd );
			gameLocal.pvs.FreeCurrentPVS( pvs );
		}	

		if(!_bUsePVS || bEndPointInPVS)
		{
			// Set up the collision masks
			if(_mask & TR_MASK_ALL)
				iMask |= MASK_ALL;
			else
			{
				if(_mask & TR_MASK_SOLID)
					iMask |= MASK_SOLID;
				if(_mask & TR_MASK_PLAYER)
					iMask |= MASK_PLAYERSOLID;
				if(_mask & TR_MASK_SHOT)
					iMask |= MASK_SHOT_BOUNDINGBOX;
				if(_mask & TR_MASK_OPAQUE)
					iMask |= MASK_OPAQUE;
				if(_mask & TR_MASK_WATER)
					iMask |= MASK_WATER;
				if(_mask & TR_MASK_FLOODFILL)
					iMask |= MASK_SOLID;
			}

			// Initialize a ray with or without a bounds
			if(_pBBox)
			{
				idVec3 vMins(_pBBox->m_Mins[0],_pBBox->m_Mins[1],_pBBox->m_Mins[2]);
				idVec3 vMaxs(_pBBox->m_Maxs[0],_pBBox->m_Maxs[1],_pBBox->m_Maxs[2]);
				idBounds bounds(vMins, vMaxs);
				gameLocal.clip.TraceBounds(trace, vStart, vEnd, bounds, iMask, pIgnoreEnt);
			}
			else
			{
				gameLocal.clip.TracePoint(trace, vStart, vEnd, iMask, pIgnoreEnt);
			}

			if(trace.c.entityNum != ENTITYNUM_WORLD)
				_result.m_HitEntity = HandleFromEntity(gameLocal.entities[trace.c.entityNum]);
			else
				_result.m_HitEntity.Reset();

			// Fill in the bot trace struct.			
			_result.m_Fraction = trace.fraction;
			_result.m_StartSolid = 0;
			_result.m_Endpos[0] = trace.endpos.x;
			_result.m_Endpos[1] = trace.endpos.y;
			_result.m_Endpos[2] = trace.endpos.z;
			_result.m_Normal[0] = trace.c.normal.x;
			_result.m_Normal[1] = trace.c.normal.y;
			_result.m_Normal[2] = trace.c.normal.z;
			_result.m_Contents = obUtilBotContentsFromGameContents(trace.c.contents);
			return Success;
		}

		// No Hit or Not in PVS
		_result.m_Fraction = 0.0f;
		_result.m_HitEntity.Reset();

		return _bUsePVS && bEndPointInPVS ? OutOfPVS : Success;
	}

	GameEntity GetLocalGameEntity()
	{
		return EntityFromID(gameLocal.localClientNum);
	}

	int GetPointContents(const float _pos[3])
	{
		idVec3 vStart(_pos[0],_pos[1],_pos[2]);
		int iMask = CONTENTS_SOLID|CONTENTS_WATER|CONTENTS_TRIGGER;
		int c = gameLocal.clip.Contents( vStart, NULL, mat3_identity, iMask, NULL );
		return obUtilBotContentsFromGameContents(c);
	}

	GameEntity FindEntityInSphere(const float _pos[3], float _radius, GameEntity _pStart, int classId)
	{
		//const char *pClassName = 0;
		//switch(classId)
		//{
		//case D3_CLASS_PLAYER:
		//case D3_CLASS_ANY:	
		//	pClassName = "player";
		//	break;
		//	// Other values to identify the "class"
		//case D3_CLASSEX_GRENADE:
		//	break;
		//case D3_CLASSEX_POWERUP_QUADDAMAGE: 
		//	break;
		//case D3_CLASSEX_POWERUP_HASTE:
		//	break;
		//case D3_CLASSEX_POWERUP_REGENERATION:
		//	break;
		//case D3_CLASSEX_POWERUP_INVISIBILITY:
		//	break;
		//case D3_CLASSEX_POWERUP_CTF_MARINEFLAG:
		//	break;
		//case D3_CLASSEX_POWERUP_CTF_STROGGFLAG:
		//	break;
		//case D3_CLASSEX_POWERUP_CTF_ONEFLAG:
		//	break;
		//case D3_CLASSEX_POWERUP_AMMOREGEN:
		//	break;
		//case D3_CLASSEX_POWERUP_GUARD:
		//	break;
		//case D3_CLASSEX_POWERUP_DOUBLER:
		//	break;
		//case D3_CLASSEX_POWERUP_SCOUT:
		//	break;
		//case D3_CLASSEX_DEADZONEPOWERUP:
		//	break;
		//}
		return GameEntity();
	}

	int GetEntityClass(const GameEntity _ent)
	{
		int entType = 0;
		idEntity *pEnt = EntityFromHandle(_ent);
		if(pEnt)
		{
			if(pEnt->IsType(idPlayer::Type))
				return D3_CLASS_PLAYER;

			if(pEnt->IsType(idExplodingBarrel::Type))
			{
				// todo: handle team starts
				return ENT_CLASS_EXPLODING_BARREL;
			}

			//if(pEnt->IsType(idPlayerStart::Type))
			//{
			//	// todo: handle team starts
			//	return ENT_CLASS_GENERIC_PLAYERSTART;
			//}
			//////////////////////////////////////////////////////////////////////////

			// TinMan: TODO: an alternative to all this is to figure out classtype in 
			// identity init or spawn, and just match classname/classtype from a def.
			idStr entClassName = pEnt->spawnArgs.GetString("classname");
			entClassName.StripTrailing("_mp");

			// Check the cached ent type first.
			const char *pBotEntType = "botEntType";
			entType = pEnt->spawnArgs.GetInt(pBotEntType);
			if(!entType) 
			{
				entType = -1;

				if(pEnt->IsType(idProjectile::Type))
				{
					if(!idStr::Cmpn(entClassName.c_str(), "projectile_bfg", 14)) 
						entType = D3_CLASSEX_BFG;
					else if(!idStr::Cmpn(entClassName.c_str(), "projectile_grenade", 18)) 
						entType = D3_CLASSEX_GRENADE;
					else if(!idStr::Cmpn(entClassName.c_str(), "projectile_plasmablast", 22)) 
						entType = D3_CLASSEX_PLASMA;
					else if(!idStr::Cmpn(entClassName.c_str(), "projectile_rocket", 17)) 
						entType = D3_CLASSEX_ROCKET;
					else if(!idStr::Cmpn(entClassName.c_str(), "projectile_soulblast", 20)) 
						entType = D3_CLASSEX_SOULBLAST;
					else if(!idStr::Cmpn(entClassName.c_str(), "projectile_bullet", 17) ||
						!idStr::Cmpn(entClassName.c_str(), "projectile_chaingunbullet", 25)) // ignore bullets
						entType = -1;
					else
						assert(0 && "Unhandled Projectile Type!");
				}
				else if(pEnt->IsType(idMoveableItem::Type)) 
				{
					entClassName = entClassName.Right(entClassName.Length() - 14); // idStr("moveable_item_").Length()
					entClassName = "weapon_" + entClassName;

					int iWeaponId = obUtilGetWeaponId(entClassName.c_str());
					if(iWeaponId != D3_WP_NONE)
						entType = D3_CLASSEX_WEAPON+iWeaponId;
				}
				else if( pEnt->IsType(idItem::Type)) 
				{
					if(!idStr::Cmpn(entClassName.c_str(), "ammo", 4)) 
					{
						if(!idStr::Cmpn(entClassName.c_str(), "ammo_shell", 10)) 
							entType = D3_CLASSEX_AMMO_SHELLS;
						else if(!idStr::Cmpn(entClassName.c_str(), "ammo_rocket", 11)) 
							entType = D3_CLASSEX_AMMO_ROCKET;
						else if(!idStr::Cmpn(entClassName.c_str(), "ammo_clip", 9)) 
							entType = D3_CLASSEX_AMMO_CLIP;
						else if(!idStr::Cmpn(entClassName.c_str(), "ammo_bullets", 12)) 
							entType = D3_CLASSEX_AMMO_BULLETS;
						else if(!idStr::Cmpn(entClassName.c_str(), "ammo_belt", 9)) 
							entType = D3_CLASSEX_AMMO_BELT;
						else if(!idStr::Cmpn(entClassName.c_str(), "ammo_cells", 10)) 
							entType = D3_CLASSEX_AMMO_CELLS;
						else if(!idStr::Cmpn(entClassName.c_str(), "ammo_bfg", 8)) 
							entType = D3_CLASSEX_AMMO_BFG;
						else if(!idStr::Cmpn(entClassName.c_str(), "ammo_grenade", 12)) 
							entType = D3_CLASSEX_AMMO_GRENADE;
						else
							assert(0 && "Unhandled Ammo Type!");
					} 
					else if(!idStr::Cmpn(entClassName.c_str(), "weap", 4)) 
					{
						int iWeaponId = obUtilGetWeaponId(entClassName.c_str());
						if(iWeaponId != D3_WP_NONE)
							entType = D3_CLASSEX_WEAPON+iWeaponId;
						else
							assert(0 && "Unhandled Weapon Type!");
					} 
					else if(!idStr::Cmpn(entClassName.c_str(), "item_medkit", 11)) 
					{
						entType = ENT_CLASS_GENERIC_HEALTH;
					}
					else if(!idStr::Cmpn(entClassName.c_str(), "item_armor", 10)) 
					{
						entType = ENT_CLASS_GENERIC_ARMOR;
					}
				}
				else if(pEnt->IsType(idTrigger_Multi::Type)) 
				{
					const char *targetName;
					if(pEnt->spawnArgs.GetString("target", "", &targetName)) 
					{
						const idEntity *target = gameLocal.FindEntity(targetName);
						if(target && target->IsType(idPlayerStart::Type)) 
						{
							entType = ENT_CLASS_GENERIC_TELEPORTER;
						}
					}
				}
				// cache it in the entity.
				pEnt->spawnArgs.SetInt(pBotEntType, entType);
			}

			if ( entType == -1 )
				return 0;
		}
		return entType;
	}

	obResult GetEntityCategory(const GameEntity _ent, BitFlag32 &_category)
	{
		idEntity *pEnt = EntityFromHandle(_ent);
		if(pEnt)
		{
			if(pEnt->IsType(idPlayer::Type))
			{
				_category.SetFlag(ENT_CAT_SHOOTABLE);
				_category.SetFlag(ENT_CAT_PLAYER);
			}
			else if(pEnt->IsType(idExplodingBarrel::Type))
			{
				_category.SetFlag(ENT_CAT_MISC);
			}
			else if(pEnt->IsType(idItem::Type) || pEnt->IsType(idMoveableItem::Type))
			{
				idStr entClassName = pEnt->spawnArgs.GetString("classname");
				entClassName.StripTrailing("_mp");

				//weapons, ammo, health, armor
				_category.SetFlag(ENT_CAT_PICKUP);

				if(!idStr::Cmpn(entClassName.c_str(), "ammo", 4)) {
					_category.SetFlag(ENT_CAT_PICKUP_AMMO);
				}
				else if(!idStr::Cmpn(entClassName.c_str(), "item_armor", 10)) {
					_category.SetFlag(ENT_CAT_PICKUP_ARMOR);
				}
				else if(!idStr::Cmpn(entClassName.c_str(), "item_medkit", 11)) {
					_category.SetFlag(ENT_CAT_PICKUP_HEALTH);
				}
				else if(!idStr::Cmpn(entClassName.c_str(), "weap", 4)) {
					_category.SetFlag(ENT_CAT_PICKUP_WEAPON);
				}
			}
			else if(!pEnt->IsType(idProjectile::Type) && 
				!pEnt->IsType(idMoveableItem::Type))
			{
				_category.SetFlag(ENT_CAT_STATIC);
			}
		}
		return Success;
	}

	obResult GetEntityFlags(const GameEntity _ent, BitFlag64 &_flags)
	{
		idEntity *pEnt = EntityFromHandle(_ent);
		if(pEnt)
		{
			if(pEnt->fl.hidden)
				_flags.SetFlag(ENT_FLAG_DISABLED);

			//idActor *pActor = pEnt->IsType(idActor::Type) ? static_cast<idActor*>(pEnt) : 0;
			//if(pActor)
			//{
			//		if(pActor->IsInVehicle())
			//			_flags.SetFlag(D3_ENT_IN_VEHICLE);
			//}

			idPlayer *pPlayer = pEnt->IsType(idPlayer::Type) ? static_cast<idPlayer*>(pEnt) : 0;
			if(pPlayer)
			{
				_flags.SetFlag(ENT_FLAG_VISTEST);

				if(!omnibot_interface::IsBot(pPlayer->entityNumber))
					_flags.SetFlag(ENT_FLAG_HUMANCONTROLLED);

				if(pPlayer->AI_CROUCH)
					_flags.SetFlag(ENT_FLAG_CROUCHED);
				if(pPlayer->AI_DEAD || pPlayer->health <= 0)
					_flags.SetFlag(ENT_FLAG_DEAD);
				if(pPlayer->AI_ONLADDER)
					_flags.SetFlag(ENT_FLAG_ONLADDER);
				if(pPlayer->AI_ONGROUND)
					_flags.SetFlag(ENT_FLAG_ONGROUND);
				if(pPlayer->weapon.GetEntity() && pPlayer->weapon.GetEntity()->IsReloading())
					_flags.SetFlag(ENT_FLAG_RELOADING);

				//			if(pPlayer->IsFlashlightOn())
				//				_flags.SetFlag(D3_ENT_FLASHLIGHT_ON);

				/*if(pPlayer->isChatting)
				iFlags |= D3_ENT_CHATTING;*/
			}
			else if(pEnt->IsType(idExplodingBarrel::Type))
			{
				_flags.SetFlag(ENT_FLAG_VISTEST);
			}
			return Success;
		}

		return InvalidEntity;
	}

	obResult GetEntityPowerups(const GameEntity _ent, BitFlag64 &_flags)
	{
		idEntity *pEnt = EntityFromHandle(_ent);
		if(pEnt)
		{
			idPlayer *pPlayer = pEnt->IsType(idPlayer::Type) ? static_cast<idPlayer*>(pEnt) : 0;
			if(pPlayer)
			{
				if(pPlayer->PowerUpActive(BERSERK))
					_flags.SetFlag(D3_PWR_BERSERK);
				if(pPlayer->PowerUpActive(INVISIBILITY))
					_flags.SetFlag(D3_PWR_INVISIBILITY);
				if(pPlayer->PowerUpActive(MEGAHEALTH))
					_flags.SetFlag(D3_PWR_MEGAHEALTH);
				if(pPlayer->PowerUpActive(ADRENALINE))
					_flags.SetFlag(D3_PWR_ADRENALINE);

#ifdef CTF
				if(pPlayer->entityNumber == gameLocal.mpGame.GetFlagCarrier(0))
					_flags.SetFlag(D3_PWR_REDFLAG);
				if(pPlayer->entityNumber == gameLocal.mpGame.GetFlagCarrier(1))
					_flags.SetFlag(D3_PWR_BLUEFLAG);
#endif

			}		
			return Success;
		}
		return InvalidEntity;
	}

	obResult GetEntityEyePosition(const GameEntity _ent, float _pos[3])
	{
		idEntity *pEnt = EntityFromHandle(_ent);	
		if(pEnt)
		{
			idVec3 vEyePos = pEnt->GetEyePosition();
			_pos[0] = vEyePos.x;
			_pos[1] = vEyePos.y;
			_pos[2] = vEyePos.z;
			return Success;
		}
		return InvalidEntity;
	}

	obResult GetEntityBonePosition(const GameEntity _ent, int _boneid, float _pos[3])
	{
		// todo: get bone position based on bone id
		//idEntity *pEnt = EntityFromHandle(_ent);

		return GetEntityPosition(_ent, _pos);
	}

	obResult GetEntityOrientation(const GameEntity _ent, float _fwd[3], float _right[3], float _up[3])
	{
		idEntity *pEnt = EntityFromHandle(_ent);
		if(pEnt)
		{
			idAngles angles;

			if(pEnt->IsType(idPlayer::Type))
			{
				idPlayer *pPlayer = static_cast<idPlayer*>(pEnt);
				angles = pPlayer->viewAngles;
			}
			else
			{
				pEnt->GetLocalAngles(angles);
			}

			idVec3 vForward, vRight, vUp;
			angles.ToVectors(&vForward, _right ? &vRight : NULL, _up ? &vUp : NULL);
			_fwd[0] = vForward.x;
			_fwd[1] = vForward.y;
			_fwd[2] = vForward.z;

			if(_right)
			{
				_right[0] = vRight.x;
				_right[1] = vRight.y;
				_right[2] = vRight.z;
			}
			if(_up)
			{
				_up[0] = vUp.x;
				_up[1] = vUp.y;
				_up[2] = vUp.z;
			}
			return Success;
		}
		return InvalidEntity;
	}

	obResult GetEntityVelocity(const GameEntity _ent, float _velocity[3])
	{
		idEntity *pEnt = EntityFromHandle(_ent);
		if(pEnt)
		{
			idVec3 vLinear, vAngular;
			pEnt->GetWorldVelocities(vLinear, vAngular);
			_velocity[0] = vLinear.x;
			_velocity[1] = vLinear.y;
			_velocity[2] = vLinear.z;
			return Success;
		}

		_velocity[0] = 0.0f;
		_velocity[1] = 0.0f;
		_velocity[2] = 0.0f;

		return InvalidEntity;
	}

	obResult GetEntityPosition(const GameEntity _ent, float _pos[3])
	{	
		idEntity *pEnt = EntityFromHandle(_ent);
		if(pEnt)
		{
			idVec3 vPos;
			idMat3 mAxis;
			pEnt->GetPosition(vPos, mAxis);
			_pos[0] = vPos.x;
			_pos[1] = vPos.y;
			_pos[2] = vPos.z;
			return Success;
		}
		return InvalidEntity;
	}

	obResult GetEntityLocalAABB(const GameEntity _ent, AABB &_aabb)
	{
		idEntity *pEnt = EntityFromHandle(_ent);
		assert(pEnt && "Null Ent!");
		if(pEnt && pEnt->GetPhysics())
		{
			idBoundsToAABB(pEnt->GetPhysics()->GetBounds(), _aabb);
			return Success;
		}
		return InvalidEntity;
	}

	obResult GetEntityWorldAABB(const GameEntity _ent, AABB &_aabb)
	{
		idEntity *pEnt = EntityFromHandle(_ent);
		if(pEnt && pEnt->GetPhysics())
		{
			idBoundsToAABB(pEnt->GetPhysics()->GetAbsBounds(), _aabb);
			return Success;
		}
		return InvalidEntity;
	}

	obResult GetEntityWorldOBB(const GameEntity _ent, float *_center, float *_axis0, float *_axis1, float *_axis2, float *_extents )
	{
		idEntity *pEnt = EntityFromHandle(_ent);
		if(pEnt && pEnt->GetPhysics())
		{
			idAngles angles;
			if(pEnt->IsType(idPlayer::Type))
			{
				idPlayer *pPlayer = static_cast<idPlayer*>(pEnt);
				angles = pPlayer->viewAngles;
			}
			else
			{
				pEnt->GetLocalAngles(angles);
			}

			const idMat3 axis = angles.ToMat3();
			const idBounds bounds = pEnt->GetPhysics()->GetBounds();
			const idVec3 boundsCenter = bounds.GetCenter();
			const idVec3 boundsCenterWorld = axis * boundsCenter;
			VectorCopy( boundsCenterWorld.ToFloatPtr(), _center );
			_extents[0] = (bounds.b[1][0] - bounds.b[0][0]) * 0.5f;
			_extents[1] = (bounds.b[1][1] - bounds.b[0][1]) * 0.5f;
			_extents[2] = (bounds.b[1][2] - bounds.b[0][2]) * 0.5f;
			return Success;
		}
		return InvalidEntity;
	}

	obResult GetEntityGroundEntity(const GameEntity _ent, GameEntity &moveent)
	{
		idEntity *pEnt = EntityFromHandle(_ent);
		if(pEnt)
		{
			idPhysics *pPhysics = pEnt->GetPhysics();
			idPhysics_Actor *pPhysicsActor = 
				pEnt && pEnt->IsType(idPhysics_Actor::Type) ? 
				static_cast<idPhysics_Actor*>(pPhysics) : 0;

			if(pPhysicsActor && 
				pPhysicsActor->GetGroundEntity() &&  
				pPhysicsActor->GetGroundEntity()->entityNumber != ENTITYNUM_WORLD)
			{
				moveent = HandleFromEntity(pPhysicsActor->GetGroundEntity());
			}

			return Success;
		}
		else
		{
			moveent.Reset();
		}
		return InvalidEntity;
	}

	GameEntity GetEntityOwner(const GameEntity _ent)
	{
		return GameEntity();
	}

	int GetEntityTeam(const GameEntity _ent)
	{
		idEntity *pEnt = EntityFromHandle(_ent);
		idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::Type) ? 
			static_cast<idPlayer*>(pEnt) : 0;
		if(pPlayer)
		{
			return obUtilGetBotTeamFromGameTeam(pPlayer->team);
		}
		return D3_TEAM_NONE;
	}

	const char *GetEntityName(const GameEntity _ent)
	{
		static char strPlayerName[64] = {0};
		strPlayerName[0] = 0;

		idEntity *pEnt = EntityFromHandle(_ent);
		if(pEnt)
		{
			if(pEnt->entityNumber < MAX_CLIENTS && pEnt->entityNumber >= 0)
			{
				gameLocal.GetPlayerName(pEnt->entityNumber, strPlayerName);
			}
			else
			{
				const char *pName = pEnt->GetName();
				idStr::Copynz(strPlayerName, pName?pName:"<noname>",64);
			}
		}
		return strPlayerName;
	}

	obResult GetCurrentWeaponClip(const GameEntity _ent, FireMode _mode, int &_curclip, int &_maxclip)
	{
		idEntity *pEnt = EntityFromHandle(_ent);
		idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::Type) ? 
			static_cast<idPlayer*>(pEnt) : 0;
		if(pPlayer)
		{
			idWeapon *pWeapon = pPlayer->weapon.GetEntity();
			if(pWeapon)
			{
				_curclip = pWeapon->AmmoInClip();
				_maxclip = pWeapon->ClipSize();
			}
			else
			{
				_curclip = 0;
				_maxclip = 0;
			}		
			return Success;
		}
		return InvalidEntity;
	}

	obResult GetCurrentAmmo(const GameEntity _ent, int _weaponId, FireMode _mode, int &_cur, int &_max)
	{
		// NOTE: _ammotype is actually the weapon Id for q4 for simplicity.
		idEntity *pEnt = EntityFromHandle(_ent);
		idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::Type) ? 
			static_cast<idPlayer*>(pEnt) : 0;
		if(pPlayer)
		{
			const char *pWeaponName = obUtilGetStringFromWeaponId(_weaponId);
			int iAmmoRequired = 0, iAmmoIndex = pPlayer->inventory.AmmoIndexForWeaponClass(pWeaponName, &iAmmoRequired);		
			if(iAmmoRequired)
			{
				_cur = pPlayer->inventory.ammo[iAmmoIndex];

				// only add clip if clipped weapon. (-1 if not)
				if(g_WeaponSlotCache[_weaponId] == -1)
				{
					g_WeaponSlotCache[_weaponId] =  pPlayer->SlotForWeapon(pWeaponName);
				}
				int iWeaponSlot = g_WeaponSlotCache[_weaponId];//pPlayer->SlotForWeapon(pWeaponName);
				if(iWeaponSlot != -1 && pPlayer->inventory.clip[iWeaponSlot] != -1)
					_cur += pPlayer->inventory.clip[iWeaponSlot];
			}
			else
				_cur = 0;

			const char *pAmmoName = pWeaponName ? pPlayer->inventory.AmmoClassForWeaponClass(pWeaponName) : 0;
			if(pAmmoName)
			{
				_max = pPlayer->inventory.MaxAmmoForAmmoClass(pPlayer, pAmmoName);
			}
			else
			{
				_max = _cur;
			}
			return Success;
		}
		_cur = 0;
		_max = 0;
		return InvalidEntity;
	}

	int GetGameTime()
	{	
		return gameLocal.GetTime();
	}

	void GetGoals()
	{
		for(int iStartIndex = 0; iStartIndex < MAX_GENTITIES; ++iStartIndex)
		{
			idEntity *pEdict = INDEXENT(iStartIndex);
			if(!pEdict)
				continue;

#ifdef CTF
			const char *pName = pEdict->GetName();
			if(pEdict->IsType(idItemTeam::Type))
			{
				idItemTeam * flag = static_cast<idItemTeam*>(pEdict);
				if ( flag->team == 0 )
				{
					Bot_Util_AddGoal("flag", pEdict, (1<<D3_TEAM_BLUE), pName);
					continue;
				}
				else if ( flag->team == 1 )
				{
					Bot_Util_AddGoal("flag", pEdict, (1<<D3_TEAM_RED), pName);
					continue;
				}
			}

			if(pEdict->IsType(idTrigger_Flag::Type))
			{
				idTrigger_Flag *cap = static_cast<idTrigger_Flag*>(pEdict);
				if ( cap->GetTeam() == 0 )
				{
					Bot_Util_AddGoal("cappoint", pEdict, (1<<D3_TEAM_RED), pName);
					continue;
				}
				else if ( cap->GetTeam() == 1 )
				{
					Bot_Util_AddGoal("cappoint", pEdict, (1<<D3_TEAM_BLUE), pName);
					continue;
				}
			}
#endif
		}
	}

	void GetPlayerInfo(obPlayerInfo &info)
	{
		info.m_AvailableTeams |= (1<<D3_TEAM_BLUE);
		info.m_AvailableTeams |= (1<<D3_TEAM_RED);

		for(int i = 0; i < MAX_CLIENTS; ++i)
		{
			if(gameLocal.entities[i])
			{
				GameEntity ge = HandleFromEntity(gameLocal.entities[i]);				
				info.m_MaxPlayers = si_maxPlayers.GetInteger();
				info.m_Players[i].m_Team = GetEntityTeam(ge);
				info.m_Players[i].m_Class = GetEntityClass(ge);
				info.m_Players[i].m_Controller = omnibot_interface::IsBot(i)?obPlayerInfo::Bot:obPlayerInfo::Human;
			}
		}
	}

	obResult InterfaceSendMessage(const MessageHelper &_data, const GameEntity _ent)
	{
		idEntity *pEnt = EntityFromHandle(_ent);
		switch(_data.GetMessageId())
		{
			///////////////////////
			// General Messages. //
			///////////////////////
		case GEN_MSG_ISALIVE:
			{
				OB_GETMSG(Msg_IsAlive);
				if(pMsg)
				{
					if(pEnt)
					{
						if(pEnt->health > 0)
							pMsg->m_IsAlive = True;
						else
							pMsg->m_IsAlive = False;

						idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::Type) ? static_cast<idPlayer*>(pEnt) : 0;
						if(pPlayer && pPlayer->spectating)
							pMsg->m_IsAlive = False;
					}
				}
				break;
			}
		case GEN_MSG_ISRELOADING:
			{
				OB_GETMSG(Msg_Reloading);
				if(pMsg)
				{
					idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::Type) ? static_cast<idPlayer*>(pEnt) : 0;
					if(pPlayer && pPlayer->weapon.GetEntity())
					{
						pMsg->m_Reloading = pPlayer->weapon.GetEntity()->IsReloading() ? True : False;
					}
				}
				break;
			}
		case GEN_MSG_ISREADYTOFIRE:
			{
				OB_GETMSG(Msg_ReadyToFire);
				if(pMsg)
				{
					idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::Type) ? static_cast<idPlayer*>(pEnt) : 0;
					if(pPlayer && pPlayer->weapon.GetEntity())
					{
						pMsg->m_Ready = pPlayer->weapon.GetEntity()->IsReady() ? True : False;
					}
				}
				break;
			}
		case GEN_MSG_ISALLIED:
			{
				OB_GETMSG(Msg_IsAllied);
				if(pMsg)
				{
					idEntity *pEntOther = EntityFromHandle(pMsg->m_TargetEntity);
					idActor *pMeActor = pEnt && pEnt->IsType(idActor::Type) ? static_cast<idActor*>(pEnt) : 0;
					idActor *pTheyActor = pEntOther && pEntOther->IsType(idActor::Type) ? static_cast<idActor*>(pEntOther) : 0;

					if(pMeActor && pTheyActor)
					{
						switch(gameLocal.gameType)
						{
							// No Allies in DM an Tourney
						case GAME_DM:
						case GAME_TOURNEY:					
						case GAME_LASTMAN:
							pMsg->m_IsAllied = False;
							break;
#ifdef CTF
						case GAME_CTF:
#endif
						case GAME_TDM:
							pMsg->m_IsAllied = pMeActor->team == pTheyActor->team ? True : False;
							break;
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
					pMsg->m_IsOutside = False;
				}
				break;
			}
		case GEN_MSG_CHANGENAME:
			{
				OB_GETMSG(Msg_ChangeName);
				idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::Type) ? static_cast<idPlayer*>(pEnt) : 0;
				if(pMsg && pPlayer)
				{
					idDict *pDict = pPlayer->GetUserInfo();
					pDict->Set( "ui_name", pMsg->m_NewName );
					pDict->Set( "name", pMsg->m_NewName );
					gameLocal.SetUserInfo( pPlayer->entityNumber, *pDict, false, true ); 
				}
				break;
			}
		case GEN_MSG_ENTITYKILL:
			{
				OB_GETMSG(Msg_KillEntity);
				if(pMsg && pMsg->m_WhoToKill.IsValid() && AreCheatsEnabled())
				{
					/*if(gameLocal.CheatsOk())
					{
					idEntity *pWho = (idEntity *)pMsg->m_WhoToKill;
					if(pWho)
					G_Damage( pWho, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, MOD_TELEFRAG );
					}*/
				}
				break;
			}
		case GEN_MSG_SERVERCOMMAND:
			{
				OB_GETMSG(Msg_ServerCommand);
				if(pMsg && pMsg->m_Command[0] /*&& AreCheatsEnabled()*/)
				{
					cmdSystem->BufferCommandText( CMD_EXEC_NOW, pMsg->m_Command );
				}
				break;
			}
		case GEN_MSG_GETEQUIPPEDWEAPON:
			{
				OB_GETMSG(WeaponStatus);
				if(pMsg)
				{
					idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::Type) ? static_cast<idPlayer*>(pEnt) : 0;
					if(pPlayer)
						pMsg->m_WeaponId = obUtilGetWeaponId(pPlayer->GetCurrentWeapon().c_str());
					else
						pMsg->m_WeaponId = 0;
				}
				break;
			}
		case GEN_MSG_GETMOUNTEDWEAPON:
			{
				OB_GETMSG(WeaponStatus);
				pMsg->m_WeaponId = D3_WP_NONE;
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
					idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::Type) ? static_cast<idPlayer*>(pEnt) : 0;
					if(pPlayer)
					{
						pMsg->m_CurrentHealth = pPlayer->health;
						pMsg->m_MaxHealth = pPlayer->inventory.maxHealth;
						pMsg->m_CurrentArmor = pPlayer->inventory.armor;
						pMsg->m_MaxArmor = pPlayer->inventory.maxarmor;
					}
				}
				break;
			}
		case GEN_MSG_GETMAXSPEED:
			{
				OB_GETMSG(Msg_PlayerMaxSpeed);
				if(pMsg)
				{
					idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::Type) ? static_cast<idPlayer*>(pEnt) : 0;
					if(pPlayer)
					{
						idPhysics_Player *pPlayerPhysics = static_cast<idPhysics_Player*>(pPlayer->GetPlayerPhysics());
						pMsg->m_MaxSpeed = pPlayerPhysics->GetSpeed();
					}
				}
				break;
			}
		case GEN_MSG_GETFLAGSTATE:
			{
				OB_GETMSG(Msg_FlagState);
				if(pMsg)
				{
#ifdef CTF
					if(pEnt && gameLocal.mpGame.IsGametypeFlagBased())
					{
						if(pEnt->IsType(idItemTeam::Type))
						{
							idItemTeam *pFlag = static_cast<idItemTeam*>(pEnt);
							switch(gameLocal.mpGame.GetFlagStatus(pFlag->team))
							{
							case FLAGSTATUS_INBASE:
								pMsg->m_Owner.Reset();
								pMsg->m_FlagState = S_FLAG_AT_BASE;
								break;
							case FLAGSTATUS_TAKEN:
								pMsg->m_FlagState = S_FLAG_CARRIED;
								pMsg->m_Owner = HandleFromEntity(gameLocal.entities[gameLocal.mpGame.GetFlagCarrier(pFlag->GetTeam())]);
								break;
							case FLAGSTATUS_STRAY:
								pMsg->m_Owner.Reset();
								pMsg->m_FlagState = S_FLAG_DROPPED;
								break;
							case FLAGSTATUS_NONE:
								break;
							}
						}
					}
#endif
				}			
				break;
			}
		case GEN_MSG_GETCONTROLLINGTEAM:
			{
				OB_GETMSG(ControllingTeam);
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
					switch(gameLocal.mpGame.GetGameState())
					{
					case idMultiplayerGame::INACTIVE:
						pMsg->m_GameState = GAME_STATE_PAUSED;
						break;
					case idMultiplayerGame::WARMUP:
						pMsg->m_GameState = GAME_STATE_WARMUP;
						break;
					case idMultiplayerGame::COUNTDOWN:
						pMsg->m_GameState = GAME_STATE_WARMUP_COUNTDOWN;
						break;
					case idMultiplayerGame::GAMEON:
						pMsg->m_GameState = GAME_STATE_PLAYING;
						break;
					case idMultiplayerGame::SUDDENDEATH:
						pMsg->m_GameState = GAME_STATE_SUDDENDEATH;
						break;
					case idMultiplayerGame::GAMEREVIEW:
						pMsg->m_GameState = GAME_STATE_SCOREBOARD;
						break;
					}
				}
				break;
			}
		case GEN_MSG_ENTITYSTAT:
			{
				OB_GETMSG(Msg_EntityStat);
				if(pMsg && pEnt && (pEnt->entityNumber < MAX_CLIENTS || pEnt->entityNumber >= 0))
				{
					const mpPlayerState_t &ps = gameLocal.mpGame.GetPlayerState(pEnt->entityNumber);
					if(!strcmp(pMsg->m_StatName, "kills"))
						pMsg->m_Result = obUserData(ps.fragCount);
					else if(!strcmp(pMsg->m_StatName, "teamFrags"))
						pMsg->m_Result = obUserData(ps.teamFragCount);
					else if(!strcmp(pMsg->m_StatName, "wins"))
						pMsg->m_Result = obUserData(ps.wins);
				}
				break;
			}
		case GEN_MSG_TEAMSTAT:
			{
				OB_GETMSG(Msg_TeamStat);
				if(pMsg)
				{
					//int iTeam = TEAM_NONE;
					//switch(pMsg->m_Team)
					//{
					//case D3_TEAM_MARINE:
					//	iTeam = TEAM_MARINE;
					//	break;
					//case D3_TEAM_STROGG:
					//	iTeam = TEAM_STROGG;
					//	break;
					//default:
					//	assert(0 && "Bad Team Selection!");
					//	return InvalidParameter;
					//}

					//if(iTeam != TEAM_NONE)
					//{
					//	if(!strcmp(pMsg->m_StatName, "score"))
					//		pMsg->m_Result = obUserData(gameLocal.mpGame.GetTeamScore(iTeam));
					//}
				}
				break;
			}
		case GEN_MSG_WPCHARGED:
			{
				OB_GETMSG(WeaponCharged);
				if(pMsg)
				{
					pMsg->m_IsCharged = True;
				}
				break;
			}
		case GEN_MSG_MOVERAT:
			{
				OB_GETMSG(Msg_MoverAt);
				if(pMsg)
				{
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

					if(tr.DidHitNonWorldEntity() && !tr.m_pEnt->IsPlayer() && !startsolid)
					{
					pMsg->m_Entity = HandleFromEntity(tr.m_pEnt);
					}*/
				}
				break;
			}
			//////////////////////////////////
			// Game specific messages next. //
			//////////////////////////////////
		case D3_MSG_GETLOCATION:
			{
				OB_GETMSG(D3_Location);
				if(pMsg)
				{
					idVec3 vPos(pMsg->m_Position[0], pMsg->m_Position[1], pMsg->m_Position[2]);
					idLocationEntity *pLoc = gameLocal.LocationForPoint(vPos);
					if(pLoc && pLoc->GetName())
					{
						idStr::Copynz(pMsg->m_LocationName, pLoc->GetName(), sizeof(pMsg->m_LocationName));
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

	bool DebugLine(const float _start[3], const float _end[3], const obColor &_color, float _time)
	{
		idVec4 vColor(_color.rF(), _color.gF(), _color.bF(), _color.aF());
		idVec3 vStart(_start[0], _start[1], _start[2]);
		idVec3 vEnd(_end[0], _end[1], _end[2]);
		gameRenderWorld->DebugLine( vColor, vStart, vEnd, SEC2MS(_time), true );
		return true;
	}

	bool DebugArrow(const float _start[3], const float _end[3], const obColor &_color, float _time)
	{
		idVec4 vColor(_color.rF(), _color.gF(), _color.bF(), _color.aF());
		idVec3 vStart(_start[0], _start[1], _start[2]);
		idVec3 vEnd(_end[0], _end[1], _end[2]);
		gameRenderWorld->DebugArrow( vColor, vStart, vEnd, 2, SEC2MS(_time) );
		return true;
	}

	bool DebugRadius(const float _pos[3], const float _radius, const obColor &_color, float _time)
	{
		idVec3 vPos(_pos[0], _pos[1], _pos[2]);
		idVec4 vColor(_color.rF(), _color.gF(), _color.bF(), _color.aF());
		gameRenderWorld->DebugCircle( vColor, vPos, upVector, _radius, 8, SEC2MS(_time), false );
		return true;
	}

	bool DebugPolygon(const obVec3 *_verts, const int _numverts, const obColor &_color, float _time, int _flags)
	{
		idWinding winding;
		for(int i = 0; i < _numverts; ++i)
			winding.AddPoint(idVec3(_verts[i].x, _verts[i].y, _verts[i].z));
		idVec4 vColor(_color.rF(), _color.gF(), _color.bF(), _color.aF());
		gameRenderWorld->DebugPolygon(vColor, winding, SEC2MS(_time), !(_flags&IEngineInterface::DR_NODEPTHTEST));
		winding.ReverseSelf();
		gameRenderWorld->DebugPolygon(vColor, winding, SEC2MS(_time), !(_flags&IEngineInterface::DR_NODEPTHTEST));
		return true;
	}

	bool PrintScreenText(const float _pos[3], float _duration, const obColor &_color, const char *_msg)
	{	
		idPlayer *pPlayer = static_cast<idPlayer*>(gameLocal.entities[gameLocal.localClientNum]);
		if(pPlayer && _msg)
		{
			idVec4 vColor(_color.rF(), _color.gF(), _color.bF(), _color.aF());
			if(_pos)
			{
				idVec3 vPosition(_pos[0],_pos[1],_pos[2]);		
				idMat3 axis = pPlayer->viewAngles.ToMat3();
				gameRenderWorld->DrawText(_msg, vPosition, 0.2f, vColor, axis, 1, SEC2MS(_duration), true);
			}
			else
			{
				if(g_BotUI)
				{
					g_BotUI->SetStateString("hudtext", _msg);
					g_BotUI->StateChanged(gameLocal.time,true);
				}
			}
		}
		return true;
	}

	void PrintError(const char *_error)
	{	
		if(_error)
			gameLocal.Warning("%s\n", _error);
	}

	void PrintMessage(const char *_msg)
	{
		if(_msg)
			gameLocal.Printf("%s\n", _msg);
	}

	const char *GetMapName()
	{
		static char strMapName[64] = {0};
		idStr mapName = gameLocal.GetMapName();
		mapName.StripFileExtension();
		mapName.StripPath();
		idStr::Copynz(strMapName, mapName.c_str(), 64);	
		return strMapName;
	}

	void GetMapExtents(AABB &_aabb)
	{
		idBoundsToAABB(gameLocal.clip.GetWorldBounds(), _aabb);
	}

	GameEntity IEngineInterface::EntityByName(const char *_name)
	{
		return HandleFromEntity(gameLocal.GetClientByName(_name));
	}

	GameEntity EntityFromID(const int _gameId)
	{
		idEntity *pEnt = INDEXENT(_gameId);
		return pEnt ? HandleFromEntity(pEnt) : GameEntity();
	}

	int IDFromEntity(const GameEntity _ent)
	{
		idEntity *pEnt = EntityFromHandle(_ent);
		return pEnt ? pEnt->entityNumber : -1;
	}

	bool DoesEntityStillExist(const GameEntity &_hndl)
	{
		return _hndl.IsValid() ? EntityFromHandle(_hndl) != NULL : false;
	}

	int GetAutoNavFeatures(AutoNavFeature *_feature, int _max)
	{
		idVec3 vPos;
		idVec3 vForward, vRight, vUp;
		idAngles angles;

		int iNumFeatures = 0;
		for(int iStartIndex = 0; iStartIndex < MAX_GENTITIES; ++iStartIndex)
		{
			if(iNumFeatures >= _max)
				break;

			idEntity *pEdict = INDEXENT(iStartIndex);
			if(pEdict)
			{
				vPos = pEdict->GetPhysics()->GetOrigin();
				pEdict->GetLocalAngles(angles);
				angles.ToVectors(&vForward, &vRight, &vUp);

				//////////////////////////////////////////////////////////////////////////
				_feature[iNumFeatures].m_Type = 0;
				_feature[iNumFeatures].m_TravelTime = 0;
				for(int i = 0; i < 3; ++i)
				{
					_feature[iNumFeatures].m_Position[i] = vPos[i];
					_feature[iNumFeatures].m_TargetPosition[i] = vPos[i];
					_feature[iNumFeatures].m_Facing[i] = vForward[i];
					_feature[iNumFeatures].m_Bounds.m_Mins[i] = 0.f;
					_feature[iNumFeatures].m_Bounds.m_Maxs[i] = 0.f;
				}

				idBoundsToAABB(pEdict->GetPhysics()->GetAbsBounds(), _feature[iNumFeatures].m_Bounds);
				
				//////////////////////////////////////////////////////////////////////////

				if(pEdict->IsType(idPlayerStart::Type))
				{
					_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_PLAYERSTART;
				}
				/*else if(pEdict->IsType(rvJumpPad::Type))
				{
					rvJumpPad *pJumpPad = static_cast<rvJumpPad*>(pEdict);
					if(pJumpPad->targets.Num())
					{
						idVec3 vTarget = pJumpPad->targets[0]->GetPhysics()->GetOrigin();
						_feature[iNumFeatures].m_TargetPosition[0] = vTarget[0];
						_feature[iNumFeatures].m_TargetPosition[1] = vTarget[1];
						_feature[iNumFeatures].m_TargetPosition[2] = vTarget[2];
						_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_JUMPPAD;
					}
				}*/
				else if(pEdict->IsType(idTrigger_Multi::Type))
				{
					idTrigger_Multi *pTrigger = static_cast<idTrigger_Multi*>(pEdict);
					if(pTrigger->IsTeleporter())
					{
						idEntity *pTarget = pTrigger->targets[0].GetEntity();
						idBoundsToAABB(pTarget->GetPhysics()->GetAbsBounds(), _feature[iNumFeatures].m_TargetBounds);
						idVec3 vTarget = pTrigger->targets[0].GetEntity()->GetPhysics()->GetOrigin();
						_feature[iNumFeatures].m_TargetPosition[0] = vTarget[0];
						_feature[iNumFeatures].m_TargetPosition[1] = vTarget[1];
						_feature[iNumFeatures].m_TargetPosition[2] = vTarget[2];
						_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_TELEPORTER;
					}
				}

				if(_feature[iNumFeatures].m_Type != 0)
				{
					iNumFeatures++;
				}
			}
		}
		return iNumFeatures;
	}

	const char *GetGameName()
	{
		return "Doom 3";
	}

	const char *GetModName()
	{
		switch(gameLocal.gameType)
		{
		case GAME_SP:
			return "Single Player";
		case GAME_DM:
			return "Deathmatch";
		case GAME_TOURNEY:
			return "Tourney";
		case GAME_TDM:
			return "Team Deathmatch";
		case GAME_LASTMAN:
			return "Last Man Standing";
#ifdef CTF
		case GAME_CTF:
			return "CTF";
#endif
		}
		return "";
	}

	const char *GetModVers()
	{
		return ENGINE_VERSION;
	}

	const char *GetBotPath()
	{
		return Omnibot_GetLibraryPath();
	}

	const char *GetLogPath()
	{
		return ".";
	}
};

//////////////////////////////////////////////////////////////////////////
// Interface Functions

void omnibot_interface::InitOldBots()
{
	if(!IsOmnibotLoaded())
		return;

	for(int i = 0; i < MAX_CLIENTS; ++i)
	{
		if(IsBot(i))
		{
			gameLocal.SetUserInfo( i, g_BotUserInfo[i], false, true );
			cmdSystem->BufferCommandText( CMD_EXEC_NOW, va( "updateUI %d\n", i ) );

			// Mark this slot as a bot.
			g_Bot[i] = true;

			// Start up client
			const char *guid = NULL;
			gameLocal.ServerClientConnect(i, guid);
			gameLocal.ServerClientBegin(i);
		}
	}
}

bool omnibot_interface::InitBotInterface()
{
	if(!gameLocal.isServer)
		return false;

	gameLocal.Printf( "-------------- Omni-bot Init ----------------\n" );

	for(int i = 0; i < D3_WP_MAX; ++i)
		g_WeaponSlotCache[i] = -1;
	for(int i = 0; i < MAX_GENTITIES; ++i)
	{
		m_EntityHandles[i].m_HandleSerial = 1;
		m_EntityHandles[i].m_NewEntity = false;
		m_EntityHandles[i].m_Used = false;
	}

	g_InterfaceFunctions = new D3Interface;
	eomnibot_error err = Omnibot_LoadLibrary(D3_VERSION_LATEST, 
		"omnibot_d3", Omnibot_FixPath(omnibot_path.GetString()));
	gameLocal.Printf( "---------------------------------------------\n" );

	InitOldBots();

	if(err==BOT_ERROR_NONE)
	{
		g_BotUI = uiManager->FindGui("guis/omnibot/omnibot.gui", true, true, true);
	}
	return err == BOT_ERROR_NONE;
}

void omnibot_interface::ShutdownBotInterface()
{
	if(!gameLocal.isServer)
		return;

	gameLocal.Printf( "------------ Omni-bot Shutdown --------------\n" );

	if(g_BotFunctions.pfnShutdown)
	{
		gameLocal.Printf( "Omni-bot Shut Down Successfully\n" );
		g_BotFunctions.pfnShutdown();
		memset(&g_BotFunctions, 0, sizeof(g_BotFunctions));
	}
	else
	{
		gameLocal.Printf( "Omni-bot Not Loaded\n" );
	}

	Omnibot_FreeLibrary();
	gameLocal.Printf( "---------------------------------------------\n" );
}

void omnibot_interface::UpdateBotInterface()
{
	if(gameLocal.isServer && IsOmnibotLoaded())
	{		
		//////////////////////////////////////////////////////////////////////////
		{
			static float serverGravity = 0.0f;
			if(serverGravity != g_gravity.GetFloat())
			{
				Event_SystemGravity d = { -g_gravity.GetFloat() };
				g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_GRAVITY, &d, sizeof(d)));
				serverGravity = g_gravity.GetFloat();
			}
			static bool cheatsEnabled = false;
			if(AreCheatsEnabled() != cheatsEnabled)
			{
				Event_SystemCheats d = { AreCheatsEnabled()?True:False };
				g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_CHEATS, &d, sizeof(d)));
				cheatsEnabled = AreCheatsEnabled();
			}
		}

		//////////////////////////////////////////////////////////////////////////
		if(!(omnibot_flags.GetInteger() & OBF_DONT_SHOW_BOTCOUNT))
		{
			int iNumBots = 0;
			for(int i = 0; i < MAX_CLIENTS; ++i)
				if(g_Bot[i])
					++iNumBots;
			omnibot_playing.SetInteger(iNumBots);
		}
		else
			omnibot_playing.SetInteger(-1);

		//////////////////////////////////////////////////////////////////////////
		// Register any pending entity updates.
		for(int i = 0; i < MAX_GENTITIES; ++i)
		{
			if(m_EntityHandles[i].m_NewEntity && gameLocal.entities[i])
			{
				m_EntityHandles[i].m_NewEntity = false;
				Bot_Event_EntityCreated(gameLocal.entities[i]);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		idPlayer *localPlayer = gameLocal.GetLocalPlayer();
		if(localPlayer && localPlayer->spectator)
		{
			if(IsBot(localPlayer->spectator))
			{
				omnibot_interface::Bot_Event_Spectated(localPlayer->spectator,localPlayer->entityNumber);
				if(g_BotUI)
				{
					g_DrawBotHud = true;
					char playerName[128] = {};
					gameLocal.GetPlayerName(localPlayer->spectator, playerName);
					g_BotUI->SetStateString("botname",playerName);
					g_BotUI->StateChanged(gameLocal.time,true);
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		// Call the libraries update.
		g_BotFunctions.pfnUpdate();
	}
}

void omnibot_interface::DrawHud()
{
	if(g_BotUI && g_DrawBotHud)
	{
		g_BotUI->Redraw(gameLocal.time);
	}
}

void omnibot_interface::OmnibotCommand( const idCmdArgs &_args ) 
{
	if(IsOmnibotLoaded())
	{
		Arguments args;
		for(int i = 0; i < _args.Argc(); ++i)
		{
			idStr::Copynz(args.m_Args[args.m_NumArgs++], _args.Argv(i), Arguments::MaxArgLength);
		}
		g_BotFunctions.pfnConsoleCommand(args);
	}
	else
		gameLocal.Printf( "Omni-bot Not Loaded\n" );
}

void omnibot_interface::Bot_SendTrigger(const TriggerInfo &_triggerInfo)
{
	if(gameLocal.isServer && IsOmnibotLoaded())
	{
		g_BotFunctions.pfnSendTrigger(_triggerInfo);
	}
}

void Omnibot_Load_PrintMsg(const char *_msg)
{
	gameLocal.Printf("Omni-bot: %s\n", _msg);
}

void Omnibot_Load_PrintErr(const char *_msg)
{
	gameLocal.Warning("Omni-bot: %s\n", _msg);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void omnibot_interface::Bot_Event_ClientConnected(int _client, bool _isbot)
{
	if(IsOmnibotLoaded())
	{
		// apparently a necessary hack
		if(_isbot)
		{
			gameLocal.SetUserInfo( _client, g_BotUserInfo[_client], false, true  ); 
			cmdSystem->BufferCommandText( CMD_EXEC_NOW, va( "updateUI %d\n", _client ) );
		}

		Event_SystemClientConnected d;
		d.m_GameId = _client;
		d.m_IsBot = _isbot ? True : False;
		g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_CLIENTCONNECTED, &d, sizeof(d)));
	}
}

void omnibot_interface::Bot_Event_ClientDisConnected(int _client)
{
	if(IsOmnibotLoaded())
	{
		g_Bot[_client] = false;
		g_BotUserInfo[_client].Clear();

		Event_SystemClientDisConnected d = { _client };
		g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_CLIENTDISCONNECTED, &d, sizeof(d)));
	}
}

void omnibot_interface::Bot_Event_Spectated(int _client, int _who)
{
	if(IsOmnibotLoaded())
	{
		Event_Spectated d = { _who };
		g_BotFunctions.pfnSendEvent(_client, MessageHelper(MESSAGE_SPECTATED, &d, sizeof(d)));
	}
}

void omnibot_interface::Bot_Event_TakeDamage(int _client, idEntity *_ent)
{
	if(g_BotFunctions.pfnSendEvent)
	{
		Event_TakeDamage d = { HandleFromEntity(_ent) };
		g_BotFunctions.pfnSendEvent(_client, MessageHelper(PERCEPT_FEEL_PAIN, &d, sizeof(d)));
	}
}

void omnibot_interface::Bot_Event_Death(int _client, idEntity *_killer, const char *_meansofdeath)
{
	if(g_BotFunctions.pfnSendEvent)
	{
		Event_Death d;
		d.m_WhoKilledMe = HandleFromEntity(_killer);
		idStr::Copynz(d.m_MeansOfDeath, 
			_meansofdeath ? _meansofdeath : "<unknown>", 
			sizeof(d.m_MeansOfDeath) / sizeof(d.m_MeansOfDeath[0]));
		g_BotFunctions.pfnSendEvent(_client, MessageHelper(MESSAGE_DEATH, &d, sizeof(d)));
	}
}

void omnibot_interface::Bot_Event_KilledSomeone(int _client, idEntity *_victim, const char *_meansofdeath)
{
	if(g_BotFunctions.pfnSendEvent)
	{
		Event_KilledSomeone d;
		d.m_WhoIKilled = HandleFromEntity(_victim);
		idStr::Copynz(d.m_MeansOfDeath, 
			_meansofdeath ? _meansofdeath : "<unknown>", 
			sizeof(d.m_MeansOfDeath) / sizeof(d.m_MeansOfDeath[0]));
		g_BotFunctions.pfnSendEvent(_client, MessageHelper(MESSAGE_DEATH, &d, sizeof(d)));
	}
}

void omnibot_interface::Bot_Event_ResetWeapons(int _client)
{
	if(g_BotFunctions.pfnSendEvent)
	{
		g_BotFunctions.pfnSendEvent(_client, MessageHelper(MESSAGE_RESETWEAPONS));
	}
}

void omnibot_interface::Bot_Event_AddWeapon(int _client, int _weaponId)
{
	if(g_BotFunctions.pfnSendEvent)
	{
		Event_AddWeapon d = { _weaponId };
		g_BotFunctions.pfnSendEvent(_client, MessageHelper(MESSAGE_ADDWEAPON, &d, sizeof(d)));
	}
}

void omnibot_interface::Bot_Event_RemoveWeapon(int _client, int _weaponId)
{
	if(g_BotFunctions.pfnSendEvent)
	{
		Event_RemoveWeapon d = { _weaponId };
		g_BotFunctions.pfnSendEvent(_client, MessageHelper(MESSAGE_REMOVEWEAPON, &d, sizeof(d)));
	}
}

void omnibot_interface::Bot_Event_FireWeapon(int _client, int _weaponId, idEntity *_projectile)
{
	if(g_BotFunctions.pfnSendEvent)
	{
		Event_WeaponFire d = { };
		d.m_WeaponId = _weaponId;
		d.m_Projectile = HandleFromEntity(_projectile);
		d.m_FireMode = Primary;
		g_BotFunctions.pfnSendEvent(_client, MessageHelper(ACTION_WEAPON_FIRE, &d, sizeof(d)));
	}
}

void omnibot_interface::Bot_Event_Sound(idEntity *_source, int _sndtype, const char *_name)
{
	if (IsOmnibotLoaded())
	{
		Event_Sound d = {};
		d.m_Source = HandleFromEntity(_source);
		d.m_SoundType = _sndtype;
		idVec3 v;
		idMat3 m;
		_source->GetPosition(v,m);
		d.m_Origin[0] = v[0];
		d.m_Origin[1] = v[1];
		d.m_Origin[2] = v[2];
		idStr::Copynz(d.m_SoundName, _name ? _name : "<unknown>", sizeof(d.m_SoundName) / sizeof(d.m_SoundName[0]));
		g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_SOUND, &d, sizeof(d)));
	}
}

void omnibot_interface::Bot_RestartGame()
{
	if (IsOmnibotLoaded())
	{
		g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_STARTGAME));
	}
}

void omnibot_interface::Notify_ChatMsg(idPlayer *_player, const char *_msg)
{
	if(!IsOmnibotLoaded())
		return;

	Event_ChatMessage d;
	d.m_WhoSaidIt = HandleFromEntity(_player);
	idStr::Copynz(d.m_Message, _msg ? _msg : "<unknown>",
		sizeof(d.m_Message) / sizeof(d.m_Message[0]));
	g_BotFunctions.pfnSendGlobalEvent(MessageHelper(PERCEPT_HEAR_GLOBALCHATMSG, &d, sizeof(d)));
}

void omnibot_interface::Notify_TeamChatMsg(idPlayer *_player, const char *_msg)
{
	if(!IsOmnibotLoaded())
		return;

	Event_ChatMessage d;
	d.m_WhoSaidIt = HandleFromEntity(_player);
	idStr::Copynz(d.m_Message, _msg ? _msg : "<unknown>",
		sizeof(d.m_Message) / sizeof(d.m_Message[0]));

	for ( int i = 0; i < gameLocal.numClients; i++ )
	{
		idEntity *ent = gameLocal.entities[ i ]; 
		if ( !ent || !ent->IsType( idPlayer::Type ) )
			continue;

		idPlayer *pPlayer = static_cast<idPlayer*>(ent);

		// Check player classes on this player's team
		if (pPlayer && pPlayer->team == _player->team)
		{
			g_BotFunctions.pfnSendEvent(pPlayer->entityNumber, 
				MessageHelper(PERCEPT_HEAR_TEAMCHATMSG, &d, sizeof(d)));
		}
	}
}

/*
=====================
BotAI::WriteUserCmdsToSnapshot
=====================
*/
void omnibot_interface::WriteUserCmdsToSnapshot( idBitMsg &msg )
{
	// Count the bots.
	int iNumBots = 0;
	for(int i = START_BOT_INDEX; i < MAX_CLIENTS; ++i)
	{
		if(g_Bot[i])
			++iNumBots;
	}

	// send the number of bots over the wire
	msg.WriteBits( iNumBots, 5 );
	if( iNumBots > 0 ) 
	{
		for ( int clientNum = START_BOT_INDEX; clientNum < MAX_CLIENTS; ++clientNum ) 
		{ 
			// write the bot number over the wire
			if ( IsBot( clientNum ) ) 
			{
				msg.WriteBits( clientNum , 5 );
				usercmd_t &cmd = gameLocal.usercmds[clientNum]; 
				msg.WriteShort( cmd.buttons ); 
				msg.WriteShort( cmd.mx ); 
				msg.WriteShort( cmd.my ); 
				msg.WriteChar( cmd.forwardmove ); 
				msg.WriteChar( cmd.rightmove ); 
				msg.WriteChar( cmd.upmove ); 
				msg.WriteShort( cmd.angles[0] ); 
				msg.WriteShort( cmd.angles[1] ); 
				msg.WriteShort( cmd.angles[2] ); 
				msg.WriteLong( cmd.gameFrame );
				msg.WriteLong( cmd.gameTime );
			}
		}
	}
}

/*
=====================
BotAI::ReadUserCmdsFromSnapshot
=====================
*/
void omnibot_interface::ReadUserCmdsFromSnapshot( const idBitMsg &msg ) 
{
	int iNumBots = msg.ReadBits( 5 ); 
	if( iNumBots > 0 ) {
		for (int i = 0; i < iNumBots; i++ ) 
		{ 
			int clientNum = msg.ReadBits( 5 );
			usercmd_t &cmd = gameLocal.usercmds[clientNum]; 
			cmd.buttons     = msg.ReadShort(); 
			cmd.mx          = msg.ReadShort(); 
			cmd.my          = msg.ReadShort(); 
			cmd.forwardmove = msg.ReadChar(); 
			cmd.rightmove   = msg.ReadChar(); 
			cmd.upmove      = msg.ReadChar(); 
			cmd.angles[0]   = msg.ReadShort(); 
			cmd.angles[1]   = msg.ReadShort(); 
			cmd.angles[2]   = msg.ReadShort(); 
			cmd.gameFrame = msg.ReadLong();
			cmd.gameTime = msg.ReadLong();
		} 
	}
} 

void omnibot_interface::Bot_Queue_EntityCreated(idEntity *pEnt)
{
	if(pEnt)
		m_EntityHandles[pEnt->entityNumber].m_NewEntity = true;
}

void omnibot_interface::Bot_Event_EntityCreated(idEntity *pEnt)
{
	if(pEnt && IsOmnibotLoaded())
	{
		// Get common properties.
		GameEntity ent = HandleFromEntity(pEnt);
		int iClass = g_InterfaceFunctions->GetEntityClass(ent);
		if(iClass)
		{
			Event_EntityCreated d;
			d.m_Entity = GameEntity(pEnt->entityNumber, m_EntityHandles[pEnt->entityNumber].m_HandleSerial);

			d.m_EntityClass = iClass;
			g_InterfaceFunctions->GetEntityCategory(ent, d.m_EntityCategory);
			g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_ENTITYCREATED, &d, sizeof(d)));
			m_EntityHandles[pEnt->entityNumber].m_Used = true;
		}
	}
}

void omnibot_interface::Bot_Event_EntityDeleted(idEntity *pEnt)
{
	if(pEnt)
	{
		if(IsOmnibotLoaded())
		{
			Event_EntityDeleted d;
			d.m_Entity = GameEntity(pEnt->entityNumber, m_EntityHandles[pEnt->entityNumber].m_HandleSerial);
			g_BotFunctions.pfnSendGlobalEvent(MessageHelper(GAME_ENTITYDELETED, &d, sizeof(d)));
		}		
		m_EntityHandles[pEnt->entityNumber].m_Used = false;
		m_EntityHandles[pEnt->entityNumber].m_NewEntity = false;
		while(++m_EntityHandles[pEnt->entityNumber].m_HandleSerial==0) {}
	}
}
