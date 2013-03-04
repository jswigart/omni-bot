//////////////////////////////////////////////////////////////////////////
// Bot-Related Includes

#include "../idlib/precompiled.h"
#pragma hdrstop

#include "../Projectile.h"

#include "BotExports.h"
#include "omnibot_interface.h"
#include "Q4_Config.h"
#include "Q4_Messages.h"

#pragma optimize("", off)

#define OMNIBOT_MODNAME "Quake 4"

idUserInterface *g_BotUI = 0;
bool g_DrawBotHud = false;

extern idCVar si_maxPlayers;
idCVar omnibot_path("omnibot_path", "", CVAR_SYSTEM | CVAR_ARCHIVE, "" );
idCVar omnibot_flags("omnibot_flags", "0", CVAR_SYSTEM | CVAR_ARCHIVE | CVAR_INTEGER | CVAR_SERVERINFO, "" );
idCVar omnibot_playing("omnibot_playing", "", CVAR_SYSTEM | CVAR_ROM | CVAR_INTEGER | CVAR_SERVERINFO, "" );

static idVec3 upVector(0.0f,0.0f,-1.0f);

//-----------------------------------------------------------------

static int	g_WeaponSlotCache[Q4_WP_MAX];
usercmd_t	g_BotUserCommands[MAX_CLIENTS];
bool		g_Bot[MAX_CLIENTS];
idDict		g_BotUserInfo[MAX_CLIENTS];

struct BotEntity
{
	obint16	m_HandleSerial;
	bool	m_NewEntity : 1;
	bool	m_Used : 1;
};

BotEntity		m_EntityHandles[MAX_GENTITIES];

//-----------------------------------------------------------------

const char *g_AllPlayerModels[] =
{
	"model_player_failed_transfer",
	"model_player_slimy_transfer",
	"model_player_corpse",
	"model_player_tactical_elite",
	"model_player_tactical_command",
	"model_player_marine",
	"model_player_marine_morris",
	"model_player_marine_voss",
	"model_player_marine_cortez",
	"model_player_marine_helmeted",
	"model_player_marine_helmeted_bright",
	"model_player_marine_medic",
	"model_player_marine_officer",
	"model_player_marine_fatigues",
	"model_player_marine_tech",
	"model_player_tactical_transfer",
	"model_player_tactical_transfer_bright",
	"model_player_kane_strogg",
};

const char *g_HumanPlayerModels[] =
{
	"model_player_failed_transfer",
	"model_player_slimy_transfer",
	"model_player_corpse",
	"model_player_tactical_elite",
	"model_player_tactical_command",
	"model_player_marine",
	"model_player_marine_morris",
	"model_player_marine_voss",
	"model_player_marine_cortez",
	"model_player_marine_helmeted",
	"model_player_marine_helmeted_bright",
	"model_player_marine_medic",
	"model_player_marine_officer",
	"model_player_marine_fatigues",
	"model_player_marine_tech",
	
};

const char *g_StroggPlayerModels[] =
{
	"model_player_tactical_transfer",
	"model_player_tactical_transfer_bright",
	"model_player_kane_strogg",
};

//-----------------------------------------------------------------

void idGameLocal::GetBotInput( int clientNum, usercmd_t &userCmd ) 
{
	userCmd.buttons = g_BotUserCommands[clientNum].buttons;
	userCmd.forwardmove = g_BotUserCommands[clientNum].forwardmove;
	userCmd.rightmove = g_BotUserCommands[clientNum].rightmove;
	userCmd.upmove = g_BotUserCommands[clientNum].upmove;
	userCmd.impulse = g_BotUserCommands[clientNum].impulse;
	userCmd.angles[0] = g_BotUserCommands[clientNum].angles[0];
	userCmd.angles[1] = g_BotUserCommands[clientNum].angles[1];
	userCmd.angles[2] = g_BotUserCommands[clientNum].angles[2];
};

//-----------------------------------------------------------------

bool AreCheatsEnabled()
{
	return cvarSystem->GetCVarBool( "net_allowCheats" );
}

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

//////////////////////////////////////////////////////////////////////////

void idBoundsToAABB(const idBounds &_idBounds, AABB &_aabb)
{
	_aabb.m_Mins[0] = _idBounds.b[0].x;
	_aabb.m_Mins[1] = _idBounds.b[0].y;
	_aabb.m_Mins[2] = _idBounds.b[0].z;
	_aabb.m_Maxs[0] = _idBounds.b[1].x;
	_aabb.m_Maxs[1] = _idBounds.b[1].y;
	_aabb.m_Maxs[2] = _idBounds.b[1].z;
}

void _GetFlagPosition(idEntity *pf, idVec3& origin, idMat3& axis)
{
	rvItemCTFFlag *pFlag = static_cast<rvItemCTFFlag*>(pf);
	if(gameLocal.IsFlagGameType())
	{
		rvCTFGameState *pCTFState = static_cast<rvCTFGameState*>(gameLocal.mpGame.GetGameState());
		if(pCTFState)
		{
			switch(pCTFState->GetFlagState(pFlag->GetTeam()))
			{
			case FS_AT_BASE:
				{
					// fall through
					break;
				}
			case FS_DROPPED:
				{
					// look for the dropped version
					for(idEntity* e = gameLocal.spawnedEntities.Next(); 
						e != NULL; 
						e = e->spawnNode.Next() ) 
					{
						if ( !e->IsType( rvItemCTFFlag::GetClassType() ) )
							continue;
						rvItemCTFFlag* pDroppedFlag = static_cast<rvItemCTFFlag*>(e);
						if(pDroppedFlag == pFlag)
							continue;
						if(pDroppedFlag->GetPowerup() != pFlag->GetPowerup())
							continue;

						pDroppedFlag->GetPosition(origin, axis);
						return;
					}
					break;
				}
			case FS_TAKEN:
			case FS_TAKEN_STROGG:
			case FS_TAKEN_MARINE:
				{
					int iFlagCarrierIndex = pCTFState->GetFlagCarrier(pFlag->GetTeam());
					idEntity *pFlagCarrier = gameLocal.entities[iFlagCarrierIndex];
					pFlagCarrier->GetPosition(origin, axis);
					return;
				}
			}
		}
	}
	pf->GetPosition(origin, axis);
}

void Omnibot_Load_PrintMsg(const char *_msg)
{
	gameLocal.Printf("Omni-bot: %s\n", _msg);
}

void Omnibot_Load_PrintErr(const char *_msg)
{
	gameLocal.Warning("Omni-bot: %s\n", _msg);
}

//-----------------------------------------------------------------

//static obResult obGetEntityCategory(const GameEntity _ent, BitFlag32 &_category)
//{
//	idEntity *pEnt = EntityFromHandle(_ent);
//	if(pEnt)
//	{
//		if(pEnt->IsType(idPlayer::GetClassType()))
//		{
//			_category.SetFlag(ENT_CAT_SHOOTABLE);
//			_category.SetFlag(ENT_CAT_PLAYER);
//		}
//	}
//	return Success;
//}

//-----------------------------------------------------------------

//static obResult obGetEntityFlags(const GameEntity _ent, BitFlag64 &_flags)
//{
//	idEntity *pEnt = EntityFromHandle(_ent);	
//	if(pEnt)
//	{
//		if(pEnt->fl.hidden)
//			_flags.SetFlag(ENT_FLAG_DISABLED);
//
//		idActor *pActor = pEnt->IsType(idActor::GetClassType()) ? static_cast<idActor*>(pEnt) : 0;
//		if(pActor)
//		{
//			if(pActor->IsInVehicle())
//				_flags.SetFlag(Q4_ENT_IN_VEHICLE);
//		}
//
//		idPlayer *pPlayer = pEnt->IsType(idPlayer::GetClassType()) ? static_cast<idPlayer*>(pEnt) : 0;
//		if(pPlayer)
//		{
//			if(!omnibot_interface::IsBot(pPlayer->entityNumber))
//				_flags.SetFlag(ENT_FLAG_HUMANCONTROLLED);
//
//			if(pPlayer->IsCrouching())
//				_flags.SetFlag(ENT_FLAG_CROUCHED);
//			if(pPlayer->pfl.dead || pPlayer->health <= 0)
//				_flags.SetFlag(ENT_FLAG_DEAD);
//			if(pPlayer->pfl.onLadder)
//				_flags.SetFlag(ENT_FLAG_ONLADDER);
//			if(pPlayer->pfl.onGround)
//				_flags.SetFlag(ENT_FLAG_ONGROUND);
//			if(pPlayer->weapon && pPlayer->weapon->IsReloading())
//				_flags.SetFlag(ENT_FLAG_RELOADING);
//
//			if(pPlayer->IsFlashlightOn())
//				_flags.SetFlag(Q4_ENT_FLASHLIGHT_ON);
//
//			if(pPlayer->inBuyZone)
//				_flags.SetFlag(Q4_ENT_IN_BUY_ZONE);
//
//			/*if(pPlayer->isChatting)
//				iFlags |= Q4_ENT_CHATTING;*/
//		}
//		return Success;
//	}
//
//	return InvalidEntity;
//}

//-----------------------------------------------------------------

//static obResult obGetEntityPowerups(const GameEntity _ent, BitFlag64 &_flags)
//{
//	idEntity *pEnt = EntityFromHandle(_ent);
//
//	if(pEnt)
//	{
//		idPlayer *pPlayer = pEnt->IsType(idPlayer::GetClassType()) ? static_cast<idPlayer*>(pEnt) : 0;
//		if(pPlayer)
//		{
//			if(pPlayer->PowerUpActive(POWERUP_QUADDAMAGE))
//				_flags.SetFlag(Q4_PWR_QUAD);
//			if(pPlayer->PowerUpActive(POWERUP_HASTE))
//				_flags.SetFlag(Q4_PWR_HASTE);
//			if(pPlayer->PowerUpActive(POWERUP_REGENERATION))
//				_flags.SetFlag(Q4_PWR_REGEN);
//			if(pPlayer->PowerUpActive(POWERUP_INVISIBILITY))
//				_flags.SetFlag(Q4_PWR_INVIS);
//			if(pPlayer->PowerUpActive(POWERUP_CTF_MARINEFLAG))
//				_flags.SetFlag(Q4_PWR_MARINEFLAG);
//			if(pPlayer->PowerUpActive(POWERUP_CTF_STROGGFLAG))
//				_flags.SetFlag(Q4_PWR_STROGGFLAG);
//			if(pPlayer->PowerUpActive(POWERUP_CTF_ONEFLAG))
//				_flags.SetFlag(Q4_PWR_ONEFLAG);
//			if(pPlayer->PowerUpActive(POWERUP_AMMOREGEN))
//				_flags.SetFlag(Q4_PWR_AMMOREGEN);
//			if(pPlayer->PowerUpActive(POWERUP_GUARD))
//				_flags.SetFlag(Q4_PWR_GUARD);
//			if(pPlayer->PowerUpActive(POWERUP_DOUBLER))
//				_flags.SetFlag(Q4_PWR_DOUBLER);
//			if(pPlayer->PowerUpActive(POWERUP_SCOUT))
//				_flags.SetFlag(Q4_PWR_SCOUT);
//			if(pPlayer->PowerUpActive(POWERUP_DEADZONE))
//				_flags.SetFlag(Q4_PWR_DEADZONE);			
//			if(pPlayer->PowerUpActive(POWERUP_TEAM_AMMO_REGEN))
//				_flags.SetFlag(Q4_PWR_TEAM_AMMO_REGEN);
//			if(pPlayer->PowerUpActive(POWERUP_TEAM_HEALTH_REGEN))
//				_flags.SetFlag(Q4_PWR_TEAM_HEALTH_REGEN);
//			if(pPlayer->PowerUpActive(POWERUP_TEAM_DAMAGE_MOD))
//				_flags.SetFlag(Q4_PWR_TEAM_DAMAGE_MOD);
//		}		
//		return Success;
//	}
//	return InvalidEntity;
//}
void Bot_Util_AddGoal(const char *_type, idEntity *_ent, int _team, const char *_tag)
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

//-----------------------------------------------------------------

const char *g_Weapons[Q4_WP_MAX] =
{
	"",
	"weapon_blaster",
	"weapon_machinegun",
	"weapon_shotgun",
	"weapon_nailgun",
	"weapon_hyperblaster",
	"weapon_rocketlauncher",
	"weapon_lightninggun",
	"weapon_grenadelauncher",
	"weapon_gauntlet",
	"weapon_dmg",
	"weapon_railgun",
	"weapon_napalmgun"
};

int obUtilGetWeaponId(const char *_weaponName)
{
	if(_weaponName)
	{
		for(int i = 1; i < Q4_WP_MAX; ++i)
		{
			if(!idStr::Cmpn(g_Weapons[i], _weaponName, idStr::Length(g_Weapons[i])))
				return i;
		}
	}
	return Q4_WP_NONE;
}

const char *obUtilGetStringFromWeaponId(int _weaponId)
{
	if(_weaponId > Q4_WP_NONE && _weaponId < Q4_WP_MAX)
		return g_Weapons[_weaponId];
	return 0;
}

const int obUtilGetBotTeamFromGameTeam(int _team)
{
	switch(_team)
	{
	case TEAM_MARINE:
		return Q4_TEAM_MARINE;
	case TEAM_STROGG:
		return Q4_TEAM_STROGG;
	}
	return Q4_TEAM_NONE;
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

//-----------------------------------------------------------------

void omnibot_interface::Bot_SendTrigger(const TriggerInfo &_triggerInfo)
{
	if( gameLocal.isServer && IsOmnibotLoaded() )
	{
		g_BotFunctions.pfnSendTrigger(_triggerInfo);
	}
}

//-----------------------------------------------------------------

class Q4Interface : public IEngineInterface
{
public:
	int AddBot(const MessageHelper &_data)
	{
		OB_GETMSG(Msg_Addbot);
		if ( !gameLocal.isMultiplayer ) 
		{
			gameLocal.Printf( "You may only add a bot to a multiplayer game\n" );
			return -1;
		}

		if ( !gameLocal.isServer )
		{
			g_InterfaceFunctions->PrintMessage( "Bots may only be added on server\n" );
			return -1;
		}

		// Find a free slot.
		int iClientNum = networkSystem->ServerConnectBot();
		if(iClientNum == -1)
		{
			g_InterfaceFunctions->PrintError("Unable to Add Bot!");
			return -1;
		}

		static idRandom rnd;
		const int iNumPlayerModels = sizeof(g_AllPlayerModels) / sizeof(g_AllPlayerModels[0]);
		const int iNumHumanModels = sizeof(g_HumanPlayerModels) / sizeof(g_HumanPlayerModels[0]);
		const int iNumStroggModels = sizeof(g_StroggPlayerModels) / sizeof(g_StroggPlayerModels[0]);

		// Team
		const char *pTeamName = 0;
		const char *pModelName = 0;
		const char *pMarineModelName = 0;
		const char *pStroggModelName = 0;
		const char *pSkinName = "base";
		if(pMsg->m_Team == Q4_TEAM_MARINE)
			pTeamName = "Marine";
		else if(pMsg->m_Team == Q4_TEAM_STROGG)
			pTeamName = "Strogg";
		else
			pTeamName = (rnd.RandomInt(2)-1) ? "Strogg" : "Marine";

		// Model
		if(pMsg->m_Model[0])
			pModelName = pMsg->m_Model;//g_AllPlayerModels[pMsg->m_ModelId % iNumPlayerModels];
		else
			pModelName = g_AllPlayerModels[rnd.RandomInt(iNumPlayerModels-1)];

		// Marine model
		if(pMsg->m_Model[0])
			pMarineModelName = pMsg->m_Model;//g_HumanPlayerModels[pMsg->m_ModelId % iNumHumanModels];
		else
			pMarineModelName = g_HumanPlayerModels[rnd.RandomInt(iNumHumanModels-1)];

		// Strogg model
		if(pMsg->m_Model[0])
			pStroggModelName = pMsg->m_Model;//g_HumanPlayerModels[pMsg->m_ModelId % iNumStroggModels];
		else
			pStroggModelName = g_StroggPlayerModels[rnd.RandomInt(iNumStroggModels-1)];

		// Skin
		g_BotUserInfo[iClientNum].Set( "ui_name", pMsg->m_Name );
		g_BotUserInfo[iClientNum].Set( "name", pMsg->m_Name );

		g_BotUserInfo[iClientNum].Set( "ui_model", pModelName );
		g_BotUserInfo[iClientNum].Set( "ui_skin", pSkinName );

		g_BotUserInfo[iClientNum].Set( "ui_model_marine", pMarineModelName );
		g_BotUserInfo[iClientNum].Set( "ui_model_strogg", pStroggModelName );
		g_BotUserInfo[iClientNum].Set( "ui_hitscanTint", "120 0.6 1" );
		g_BotUserInfo[iClientNum].Set( "ui_autoSwitch", "0" );
		g_BotUserInfo[iClientNum].Set( "ui_clan", "^4[BOT]" );
		if( gameLocal.IsTeamGame() )
			g_BotUserInfo[iClientNum].Set( "ui_team", pTeamName );

		g_BotUserInfo[iClientNum].Set( "ui_ready", "Ready" );
		g_BotUserInfo[iClientNum].SetBool("bot", true);
		gameLocal.SetUserInfo( iClientNum, g_BotUserInfo[iClientNum], false );
		cmdSystem->BufferCommandText( CMD_EXEC_NOW, va( "updateUI %d\n", iClientNum ) );

		// Mark this slot as a bot.
		g_Bot[iClientNum] = true;

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
				cmdSystem->BufferCommandText( CMD_EXEC_NOW, va( "kick %d", pMsg->m_GameId ) );
		}
		else
		{
			int iClient = gameLocal.GetClientNumByName(pMsg->m_Name);
			if(omnibot_interface::IsBot(iClient))
				cmdSystem->BufferCommandText( CMD_EXEC_NOW, va( "kick %d", iClient ) );
		}
	}

	int RemoveBot(const char *_name)
	{
		if ( !gameLocal.isServer ) 
		{
			gameLocal.Printf( "Bots may only be removed on server, only it has the powah!\n" );
			return 0;
		}

		int iClient = gameLocal.GetClientNumByName(_name);
		cmdSystem->BufferCommandText( CMD_EXEC_NOW, va( "kick %d", iClient ) );
		return 0;
	}

	obResult ChangeTeam(int _client, int _newteam, const MessageHelper *_data)
	{
		idPlayer *pPlayer = static_cast<idPlayer*>(gameLocal.entities[_client]);
		if(pPlayer && gameLocal.IsTeamGame()) 
		{
			// Simple random selection for now.
			if(_newteam == RANDOM_TEAM_IF_NO_TEAM || _newteam == RANDOM_TEAM)
			{
				int iPlayerCount[2] = { 0, 0 };
				for(int i = 0; i < MAX_CLIENTS; ++i)
				{
					if(gameLocal.entities[i] && gameLocal.entities[i]->IsType(idPlayer::GetClassType())) 
					{
						idPlayer *pPlr = static_cast<idPlayer*>(gameLocal.entities[i]);
						if(pPlayer != pPlr)
							iPlayerCount[pPlr->team]++;
					}
				}

				if(iPlayerCount[0] < iPlayerCount[1])
					_newteam = 0;
				else if(iPlayerCount[0] > iPlayerCount[1])
					_newteam = 1;
				else
					_newteam = gameLocal.random.RandomInt(2) == 0 ? Q4_TEAM_MARINE : Q4_TEAM_STROGG;
			}
			int iTeam = _newteam;
			pPlayer->spawnArgs.SetInt("team", iTeam);
			pPlayer->GetUserInfo()->Set("ui_team", iTeam ? "Strogg" : "Marine");
			//pPlayer->ServerSpectate( false );
			//gameLocal.SwitchTeam(_client, _newteam);
			if(iTeam != pPlayer->team)
			{
				gameLocal.SwitchTeam(_client, iTeam);
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
		assert(pPlayer && "Null Ent!");

		if(pPlayer)
		{
			usercmd_t &usercmd = g_BotUserCommands[ _client ];
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
				pPlayer->weapon &&
				pPlayer->weapon->IsReady() &&
				!pPlayer->weapon->IsReloading()) 
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

			if(g_WeaponSlotCache[_input.m_CurrentWeapon] == -1)
			{
				const char *pNewWeapon = obUtilGetStringFromWeaponId(_input.m_CurrentWeapon);
				if(pNewWeapon)
					g_WeaponSlotCache[_input.m_CurrentWeapon] = pPlayer->SlotForWeapon(pNewWeapon);
			}

			if(g_WeaponSlotCache[_input.m_CurrentWeapon] != -1)
				pPlayer->SelectWeapon(g_WeaponSlotCache[_input.m_CurrentWeapon], false);
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
			/*else
			{
				idStr s = _cmd;
				s.StripLeading('"');
				s.StripTrailing('"');
			
				cmdSystem->BufferCommandText( CMD_EXEC_NOW, s.c_str() );
			}*/
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
				gameLocal.TraceBounds(NULL, trace, vStart, vEnd, bounds, iMask, pIgnoreEnt);
			}
			else
			{
				gameLocal.TracePoint(NULL, trace, vStart, vEnd, iMask, pIgnoreEnt);
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
		trace_t trace;
		idVec3 vStart(_pos[0],_pos[1],_pos[2]);
		int iMask = CONTENTS_SOLID|CONTENTS_WATER|CONTENTS_TRIGGER;
		gameLocal.TracePoint(NULL, trace, vStart, vStart, iMask, NULL);
		return obUtilBotContentsFromGameContents(trace.c.contents);
	}

	GameEntity FindEntityInSphere(const float _pos[3], float _radius, GameEntity _pStart, int classId)
	{
		const char *pClassName = 0;
		switch(classId)
		{
		case Q4_CLASS_PLAYER:
		case Q4_CLASS_ANY:	
			pClassName = "player";
			break;
			// Other values to identify the "class"
		case Q4_CLASSEX_GRENADE:
			break;
		case Q4_CLASSEX_POWERUP_QUADDAMAGE: 
			break;
		case Q4_CLASSEX_POWERUP_HASTE:
			break;
		case Q4_CLASSEX_POWERUP_REGENERATION:
			break;
		case Q4_CLASSEX_POWERUP_INVISIBILITY:
			break;
		case Q4_CLASSEX_POWERUP_CTF_MARINEFLAG:
			break;
		case Q4_CLASSEX_POWERUP_CTF_STROGGFLAG:
			break;
		case Q4_CLASSEX_POWERUP_CTF_ONEFLAG:
			break;
		case Q4_CLASSEX_POWERUP_AMMOREGEN:
			break;
		case Q4_CLASSEX_POWERUP_GUARD:
			break;
		case Q4_CLASSEX_POWERUP_DOUBLER:
			break;
		case Q4_CLASSEX_POWERUP_SCOUT:
			break;
		case Q4_CLASSEX_DEADZONEPOWERUP:
			break;
		}
		return GameEntity();
	}

	int GetEntityClass(const GameEntity _ent)
	{
		int entType = 0;
		idEntity *pEnt = EntityFromHandle(_ent);
		if(pEnt)
		{
			if(pEnt->IsType(idPlayer::GetClassType()))
				return Q4_CLASS_PLAYER;

			//////////////////////////////////////////////////////////////////////////

			idStr entClassName = pEnt->spawnArgs.GetString("classname");
			entClassName.StripTrailing("_mp");

			// Check the cached ent type first.
			const char *pBotEntType = "botEntType";
			entType = pEnt->spawnArgs.GetInt(pBotEntType);
			if(!entType) 
			{
				entType = -1;

				if(pEnt->IsType(idProjectile::GetClassType()))
				{
					// projectiles					
					if(!idStr::Cmpn(entClassName.c_str(), "projectile_grenade", 18)) 
						entType = Q4_CLASSEX_GRENADE;
					else if(!idStr::Cmpn(entClassName.c_str(), "projectile_rocket", 17)) 
						entType = Q4_CLASSEX_ROCKET;
					else if(!idStr::Cmpn(entClassName.c_str(), "projectile_nail", 15)) 
						entType = Q4_CLASSEX_NAIL;					
					else if(!idStr::Cmpn(entClassName.c_str(), "projectile_hyperblaster", 23)) 
						entType = Q4_CLASSEX_HYPERBLASTERSHOT;
					else if(!idStr::Cmpn(entClassName.c_str(), "projectile_napalm", 17)) 
						entType = Q4_CLASSEX_NAPALMSHOT;
					else if(!idStr::Cmpn(entClassName.c_str(), "impact_napalm_projectile", 24)) 
						entType = Q4_CLASSEX_NAPALMSHOT;
					else if(!idStr::Cmpn(entClassName.c_str(), "projectile_dmg", 14)) 
						entType = Q4_CLASSEX_DARKMATTERSHOT;
					else
						assert(0 && "Unhandled Projectile Type!");
				}
				else if(pEnt->IsType(idMoveableItem::GetClassType())) 
				{
					entClassName = entClassName.Right(entClassName.Length() - 14); // idStr("moveable_item_").Length()
					entClassName = "weapon_" + entClassName;

					int iWeaponId = obUtilGetWeaponId(entClassName.c_str());
					if(iWeaponId != Q4_WP_NONE)
						entType = Q4_CLASSEX_WEAPON+iWeaponId;
				}
				else if( pEnt->IsType(idItem::GetClassType())) 
				{
					if(!idStr::Cmpn(entClassName.c_str(), "ammo", 4)) 
					{
						if(!idStr::Cmpn(entClassName.c_str(), "ammo_machinegun", 15)) 
							entType = Q4_CLASSEX_AMMO + Q4_WP_MACHINEGUN;
						else if(!idStr::Cmpn(entClassName.c_str(), "ammo_nailgun", 12)) 
							entType = Q4_CLASSEX_AMMO + Q4_WP_NAILGUN;
						else if(!idStr::Cmpn(entClassName.c_str(), "ammo_railgun", 12)) 
							entType = Q4_CLASSEX_AMMO + Q4_WP_RAILGUN;
						else if(!idStr::Cmpn(entClassName.c_str(), "ammo_shotgun", 12)) 
							entType = Q4_CLASSEX_AMMO + Q4_WP_SHOTGUN;
						else if(!idStr::Cmpn(entClassName.c_str(), "ammo_hyperblaster", 17)) 
							entType = Q4_CLASSEX_AMMO + Q4_WP_HYPERBLASTER;
						else if(!idStr::Cmpn(entClassName.c_str(), "ammo_rocketlauncher", 19)) 
							entType = Q4_CLASSEX_AMMO + Q4_WP_ROCKETLAUNCHER;
						else if(!idStr::Cmpn(entClassName.c_str(), "ammo_grenadelauncher", 20)) 
							entType = Q4_CLASSEX_AMMO + Q4_WP_GRENADELAUNCHER;
						else if(!idStr::Cmpn(entClassName.c_str(), "ammo_lightninggun", 17)) 
							entType = Q4_CLASSEX_AMMO + Q4_WP_LIGHTNINGGUN;
						else if(!idStr::Cmpn(entClassName.c_str(), "ammo_napalmgun", 14)) 
							entType = Q4_CLASSEX_AMMO + Q4_WP_NAPALMGUN;
						else if(!idStr::Cmpn(entClassName.c_str(), "ammo_dmg", 8)) 
							entType = Q4_CLASSEX_AMMO + Q4_WP_DMG;
						else
							assert(0 && "Unhandled Ammo Type!");
					} 
					else if(!idStr::Cmpn(entClassName.c_str(), "weap", 4)) 
					{
						int iWeaponId = obUtilGetWeaponId(entClassName.c_str());
						if(iWeaponId != Q4_WP_NONE)
							entType = Q4_CLASSEX_WEAPON+iWeaponId;
						else
							assert(0 && "Unhandled Weapon Type!");
					} 
					else if(!idStr::Cmpn(entClassName.c_str(), "item_health", 11)) 
					{
						if(!idStr::Cmpn(entClassName.c_str(), "item_health_mega", 16))
							entType = Q4_CLASSEX_POWERUP_MEGAHEALTH;
						else
							entType = ENT_CLASS_GENERIC_HEALTH;
					}
					else if(!idStr::Cmpn(entClassName.c_str(), "item_armor", 10)) 
					{
						entType = ENT_CLASS_GENERIC_ARMOR;
					}
				}
				/*else if(pEnt->IsType(idTrigger_Multi::GetClassType())) 
				{
					const char *targetName;
					if(pEnt->spawnArgs.GetString("target", "", &targetName)) 
					{
						const idEntity *target = gameLocal.FindEntity(targetName);
						if(target && target->IsType(idPlayerStart::GetClassType())) 
						{
							entType = ENT_CLASS_GENERIC_TELEPORTER;
						}
					}
				}*/
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
			if(pEnt->IsType(idPlayer::GetClassType()))
			{
				_category.SetFlag(ENT_CAT_SHOOTABLE);
				_category.SetFlag(ENT_CAT_PLAYER);
			}
			else if(pEnt->IsType(idItem::GetClassType()) || pEnt->IsType(idMoveableItem::GetClassType()))
			{
				//weapons, ammo, health, armor
				_category.SetFlag(ENT_CAT_PICKUP);

				idStr entClassName = pEnt->spawnArgs.GetString("classname");
				entClassName.StripTrailing("_mp");

				if(!idStr::Cmpn(entClassName.c_str(), "ammo", 4)) {
					_category.SetFlag(ENT_CAT_PICKUP_AMMO);
				}
				else if(!idStr::Cmpn(entClassName.c_str(), "item_armor", 10)) {
					_category.SetFlag(ENT_CAT_PICKUP_ARMOR);
				}
				else if(!idStr::Cmpn(entClassName.c_str(), "item_health", 11)) {
					_category.SetFlag(ENT_CAT_PICKUP_HEALTH);
				}
				else if(!idStr::Cmpn(entClassName.c_str(), "weap", 4)) {
					_category.SetFlag(ENT_CAT_PICKUP_WEAPON);
				}
			}
			else if(!pEnt->IsType(idProjectile::GetClassType()) && 
				!pEnt->IsType(idMoveableItem::GetClassType()))
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

			idActor *pActor = pEnt->IsType(idActor::GetClassType()) ? static_cast<idActor*>(pEnt) : 0;
			if(pActor)
			{
				if(pActor->IsInVehicle())
					_flags.SetFlag(Q4_ENT_IN_VEHICLE);
			}

			idPlayer *pPlayer = pEnt->IsType(idPlayer::GetClassType()) ? static_cast<idPlayer*>(pEnt) : 0;
			if(pPlayer)
			{
				_flags.SetFlag(ENT_FLAG_VISTEST);

				if(!omnibot_interface::IsBot(pPlayer->entityNumber))
					_flags.SetFlag(ENT_FLAG_HUMANCONTROLLED);

				if(pPlayer->IsCrouching())
					_flags.SetFlag(ENT_FLAG_CROUCHED);
				if(pPlayer->pfl.dead || pPlayer->health <= 0)
					_flags.SetFlag(ENT_FLAG_DEAD);
				if(pPlayer->pfl.onLadder)
					_flags.SetFlag(ENT_FLAG_ONLADDER);
				if(pPlayer->pfl.onGround)
					_flags.SetFlag(ENT_FLAG_ONGROUND);
				if(pPlayer->weapon && pPlayer->weapon->IsReloading())
					_flags.SetFlag(ENT_FLAG_RELOADING);

				if(pPlayer->IsFlashlightOn())
					_flags.SetFlag(Q4_ENT_FLASHLIGHT_ON);

				if(pPlayer->inBuyZone)
					_flags.SetFlag(Q4_ENT_IN_BUY_ZONE);

				/*if(pPlayer->isChatting)
				iFlags |= Q4_ENT_CHATTING;*/
			}
			else if(pEnt->IsType(idProjectile::GetClassType()))
			{
				_flags.SetFlag(ENT_FLAG_VISTEST);
			}
			else if(pEnt->IsType(idMoveableItem::GetClassType())) 
			{
				_flags.SetFlag(ENT_FLAG_VISTEST);
			}
			else if(pEnt->IsType(idItem::GetClassType()))
			{
				idItem *pItem = static_cast<idItem*>(pEnt);
				if (pItem && pItem->pickedUp)
					_flags.SetFlag(ENT_FLAG_DISABLED);
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
			idPlayer *pPlayer = pEnt->IsType(idPlayer::GetClassType()) ? static_cast<idPlayer*>(pEnt) : 0;
			if(pPlayer)
			{
				if(pPlayer->PowerUpActive(POWERUP_QUADDAMAGE))
					_flags.SetFlag(Q4_PWR_QUAD);
				if(pPlayer->PowerUpActive(POWERUP_HASTE))
					_flags.SetFlag(Q4_PWR_HASTE);
				if(pPlayer->PowerUpActive(POWERUP_REGENERATION))
					_flags.SetFlag(Q4_PWR_REGEN);
				if(pPlayer->PowerUpActive(POWERUP_INVISIBILITY))
					_flags.SetFlag(Q4_PWR_INVIS);
				if(pPlayer->PowerUpActive(POWERUP_CTF_MARINEFLAG))
					_flags.SetFlag(Q4_PWR_MARINEFLAG);
				if(pPlayer->PowerUpActive(POWERUP_CTF_STROGGFLAG))
					_flags.SetFlag(Q4_PWR_STROGGFLAG);
				if(pPlayer->PowerUpActive(POWERUP_CTF_ONEFLAG))
					_flags.SetFlag(Q4_PWR_ONEFLAG);
				if(pPlayer->PowerUpActive(POWERUP_AMMOREGEN))
					_flags.SetFlag(Q4_PWR_AMMOREGEN);
				if(pPlayer->PowerUpActive(POWERUP_GUARD))
					_flags.SetFlag(Q4_PWR_GUARD);
				if(pPlayer->PowerUpActive(POWERUP_DOUBLER))
					_flags.SetFlag(Q4_PWR_DOUBLER);
				if(pPlayer->PowerUpActive(POWERUP_SCOUT))
					_flags.SetFlag(Q4_PWR_SCOUT);
				if(pPlayer->PowerUpActive(POWERUP_DEADZONE))
					_flags.SetFlag(Q4_PWR_DEADZONE);			
				if(pPlayer->PowerUpActive(POWERUP_TEAM_AMMO_REGEN))
					_flags.SetFlag(Q4_PWR_TEAM_AMMO_REGEN);
				if(pPlayer->PowerUpActive(POWERUP_TEAM_HEALTH_REGEN))
					_flags.SetFlag(Q4_PWR_TEAM_HEALTH_REGEN);
				if(pPlayer->PowerUpActive(POWERUP_TEAM_DAMAGE_MOD))
					_flags.SetFlag(Q4_PWR_TEAM_DAMAGE_MOD);
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

			if(pEnt->IsType(idPlayer::GetClassType()))
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

			if(pEnt->IsType(rvItemCTFFlag::GetClassType()))
			{
				_GetFlagPosition(pEnt, vPos, mAxis);
				_pos[0] = vPos.x;
				_pos[1] = vPos.y;
				_pos[2] = vPos.z;
				return Success;
			}		
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
		assert(pEnt && "Null Ent!");
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
			if(pEnt->IsType(idPlayer::GetClassType()))
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
			idVec3 boundsCenterWorld = axis * boundsCenter;
			//VectorCopy( boundsCenterWorld.ToFloatPtr(), _center );
			_center = boundsCenterWorld.ToFloatPtr();
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
				pEnt && pEnt->IsType(idPhysics_Actor::GetClassType()) ? 
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
		idEntity *pEdict = EntityFromHandle(_ent);
		if(pEdict && pEdict->IsType(idPlayer::GetClassType()))
		{
			idPlayer *pPlayer = static_cast<idPlayer*>(pEdict);
			return obUtilGetBotTeamFromGameTeam(pPlayer->team);
		}	
		return Q4_TEAM_NONE;
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
		idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::GetClassType()) ? 
			static_cast<idPlayer*>(pEnt) : 0;
		if(pPlayer)
		{
			if(pPlayer->weapon)
			{
				_curclip = pPlayer->weapon->AmmoInClip();
				_maxclip = pPlayer->weapon->ClipSize();
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
		idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::GetClassType()) ? 
			static_cast<idPlayer*>(pEnt) : 0;
		if(pPlayer)
		{
			const char *pWeaponName = obUtilGetStringFromWeaponId(_weaponId);
			int iAmmoRequired = 0, iAmmoIndex = pPlayer->inventory.AmmoIndexForWeaponClass(pWeaponName, &iAmmoRequired);		
			if(iAmmoRequired)
				_cur = pPlayer->inventory.ammo[iAmmoIndex];
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
			if(pEdict)
			{
				if(pEdict->IsType(rvItemCTFFlag::GetClassType()))
				{
					rvItemCTFFlag *pFlag = static_cast<rvItemCTFFlag*>(pEdict);

					const char *pName = pEdict->GetName();
					int iTeam = obUtilGetBotTeamFromGameTeam(pFlag->GetTeam());
					switch(iTeam)
					{
					case Q4_TEAM_NONE:
						{
							int iGoalTeam = (1<<Q4_TEAM_MARINE) | (1<<Q4_TEAM_STROGG);
							Bot_Util_AddGoal("flag", pEdict, iGoalTeam, pName);
							// TODO: get the cap point.
							break;
						}
					case Q4_TEAM_MARINE:
						{
							Bot_Util_AddGoal("flag", pEdict, 1<<Q4_TEAM_STROGG, pName);
							Bot_Util_AddGoal("cappoint", pEdict, 1<<Q4_TEAM_MARINE, pName);
							//g_BotFunctions.pfnAddGoal(pEdict, GOAL_CTF_RETURN_FLAG, 1<<Q4_TEAM_STROGG, pName, NULL);
							break;
						}
					case Q4_TEAM_STROGG:
						{
							Bot_Util_AddGoal("flag", pEdict, 1<<Q4_TEAM_MARINE, pName);
							Bot_Util_AddGoal("cappoint", pEdict, 1<<Q4_TEAM_STROGG, pName);
							//g_BotFunctions.pfnAddGoal(pEdict, GOAL_CTF_RETURN_FLAG, 1<<Q4_TEAM_MARINE, pName, NULL);
							break;
						}
					}
				}
				else if(pEdict->IsType(riDeadZonePowerup::GetClassType()))
				{
					const char *pName = pEdict->GetName();
					int iGoalTeam = (1<<Q4_TEAM_MARINE) | (1<<Q4_TEAM_STROGG);
					Bot_Util_AddGoal("flag", pEdict, iGoalTeam, pName);
				}
				else if(pEdict->IsType(idTrigger::GetClassType()))
				{
					// Deadzone regions
					if(pEdict->spawnArgs.GetBool("requiresDeadZonePowerup", "1"))
					{
						const char *pName = pEdict->GetName();
						int iGoalTeam = (1<<Q4_TEAM_MARINE) | (1<<Q4_TEAM_STROGG);
						Bot_Util_AddGoal("deadzone", pEdict, iGoalTeam, pName);
					}
				}
				/*else if(pEdict->IsType(idItemPowerup::GetClassType()))
				{
					const char *pName = pEdict->GetName();
					int iGoalTeam = (1<<Q4_TEAM_MARINE) | (1<<Q4_TEAM_STROGG);
					Bot_Util_AddGoal("POWERUP", pEdict, iGoalTeam, pName);
				}
				else if(pEdict->IsType(idItem::GetClassType()))
				{
					const char *pName = pEdict->GetName();
					int iGoalTeam = (1<<Q4_TEAM_MARINE) | (1<<Q4_TEAM_STROGG);

					//TODO: clean up the names. AMMO_ammo_nailgun should be AMMO_nailgun, etc
					if(idStr::Cmpn(pName, "ammo_", 5) == 0)
						Bot_Util_AddGoal(pEdict, GOAL_AMMO, iGoalTeam, pName, NULL);
					else if (idStr::Cmpn(pName, "item_armor_", 11) == 0)
						Bot_Util_AddGoal(pEdict, GOAL_ARMOR, iGoalTeam, pName, NULL);
					else if (idStr::Cmpn(pName, "item_health_", 12) == 0)
						Bot_Util_AddGoal(pEdict, GOAL_HEALTH, iGoalTeam, pName, NULL);
					else if (idStr::Cmpn(pName, "weapon_", 7) == 0)
						Bot_Util_AddGoal(pEdict, Q4_GOAL_WEAPON, iGoalTeam, pName, NULL);
				}*/
			}
		}
	}

	void GetPlayerInfo(obPlayerInfo &info)
	{
		info.m_AvailableTeams |= (1<<Q4_TEAM_STROGG);
		info.m_AvailableTeams |= (1<<Q4_TEAM_MARINE);

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
					if(pEnt->health > 0)
						pMsg->m_IsAlive = True;
					else
						pMsg->m_IsAlive = False;

					idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::GetClassType()) ? static_cast<idPlayer*>(pEnt) : 0;
					if(pPlayer && pPlayer->spectating)
						pMsg->m_IsAlive = False;
				}
				break;
			}
		case GEN_MSG_ISRELOADING:
			{
				OB_GETMSG(Msg_Reloading);
				if(pMsg)
				{
					idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::GetClassType()) ? static_cast<idPlayer*>(pEnt) : 0;
					if(pPlayer && pPlayer->weapon)
					{
						pMsg->m_Reloading = pPlayer->weapon->IsReloading() ? True : False;
					}
				}
				break;
			}
		case GEN_MSG_ISREADYTOFIRE:
			{
				OB_GETMSG(Msg_ReadyToFire);
				if(pMsg)
				{
					idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::GetClassType()) ? static_cast<idPlayer*>(pEnt) : 0;
					if(pPlayer && pPlayer->weapon)
					{
						pMsg->m_Ready = pPlayer->weapon->IsReady() ? True : False;
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

					idActor *pMeActor = pEnt && pEnt->IsType(idActor::GetClassType()) ? static_cast<idActor*>(pEnt) : 0;
					idActor *pTheyActor = pEntOther && pEntOther->IsType(idActor::GetClassType()) ? static_cast<idActor*>(pEntOther) : 0;

					if(pMeActor && pTheyActor)
					{
						switch(gameLocal.gameType)
						{
							// No Allies in DM an Tourney
						case GAME_DM:
						case GAME_TOURNEY:
							pMsg->m_IsAllied = False;
							break;
						case GAME_TDM:
						case GAME_CTF:
						case GAME_1F_CTF:
						case GAME_ARENA_CTF:
						case GAME_ARENA_1F_CTF:
						case GAME_DEADZONE:
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
				idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::GetClassType()) ? static_cast<idPlayer*>(pEnt) : 0;
				if(pMsg && pPlayer)
				{
					idDict *pDict = pPlayer->GetUserInfo();
					pDict->Set( "ui_name", pMsg->m_NewName );
					pDict->Set( "name", pMsg->m_NewName );
					gameLocal.SetUserInfo( pPlayer->entityNumber, *pDict, false ); 
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
					idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::GetClassType()) ? static_cast<idPlayer*>(pEnt) : 0;
					if(pPlayer && pPlayer->weapon && pPlayer->weaponDef)
						pMsg->m_WeaponId = obUtilGetWeaponId(pPlayer->weaponDef->GetName());
					else
						pMsg->m_WeaponId = 0;
				}
				break;
			}
		case GEN_MSG_GETMOUNTEDWEAPON:
			{
				OB_GETMSG(WeaponStatus);
				pMsg->m_WeaponId = Q4_WP_NONE;
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
					idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::GetClassType()) ? static_cast<idPlayer*>(pEnt) : 0;
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
					idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::GetClassType()) ? static_cast<idPlayer*>(pEnt) : 0;
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
					if(pEnt && gameLocal.IsFlagGameType())
					{
						rvItemCTFFlag *pFlag = static_cast<rvItemCTFFlag*>(pEnt);
						rvCTFGameState *pCTFState = static_cast<rvCTFGameState*>(gameLocal.mpGame.GetGameState());
						if(pCTFState)
						{
							switch(pCTFState->GetFlagState(pFlag->GetTeam()))
							{
							case FS_AT_BASE:
								pMsg->m_Owner.Reset();
								pMsg->m_FlagState = S_FLAG_AT_BASE;							
								break;
							case FS_DROPPED:
								pMsg->m_Owner.Reset();
								pMsg->m_FlagState = S_FLAG_DROPPED;
								break;
							case FS_TAKEN:
							case FS_TAKEN_STROGG:
							case FS_TAKEN_MARINE:
								pMsg->m_FlagState = S_FLAG_CARRIED;
								pMsg->m_Owner = HandleFromEntity(gameLocal.entities[pCTFState->GetFlagCarrier(pFlag->GetTeam())]);
								break;
							}
						}
					}
				}			
				break;
			}
		case GEN_MSG_GETCONTROLLINGTEAM:
			{
				OB_GETMSG(ControllingTeam);
				if(pMsg && pEnt)
				{
					// Deadzone
					if(pEnt->IsType(idTrigger::GetClassType()) && pEnt->spawnArgs.GetBool("requiresDeadZonePowerup", "1"))
					{
						
					}
					else if(pEnt->IsType(riDeadZonePowerup::GetClassType()))
					{
						
					}
				}
				break;
			}
		case GEN_MSG_GAMESTATE:
			{
				OB_GETMSG(Msg_GameState);
				if(pMsg)
				{
					switch(gameLocal.mpGame.GetGameState()->GetMPGameState())
					{
					case INACTIVE:
						pMsg->m_GameState = GAME_STATE_PAUSED;
						break;
					case WARMUP:
						pMsg->m_GameState = GAME_STATE_WARMUP;
						break;
					case COUNTDOWN:
						pMsg->m_GameState = GAME_STATE_WARMUP_COUNTDOWN;
						break;
					case GAMEON:
						pMsg->m_GameState = GAME_STATE_PLAYING;
						break;
					case SUDDENDEATH:
						pMsg->m_GameState = GAME_STATE_SUDDENDEATH;
						break;
					case GAMEREVIEW:
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
					idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::GetClassType()) ? static_cast<idPlayer*>(pEnt) : 0;
					rvPlayerStat* stat = statManager->GetPlayerStat( pEnt->entityNumber );
					if(!strcmp(pMsg->m_StatName, "kills"))
						pMsg->m_Result = obUserData(stat->kills);
					else if(!strcmp(pMsg->m_StatName, "deaths"))
						pMsg->m_Result = obUserData(stat->deaths);
					else if(!strcmp(pMsg->m_StatName, "suicides"))
						pMsg->m_Result = obUserData(stat->suicides);
					else if(pPlayer && !strcmp(pMsg->m_StatName, "deadzonescore"))
						pMsg->m_Result = obUserData(gameLocal.mpGame.GetPlayerDeadZoneScore(pPlayer));
				}
				break;
			}
		case GEN_MSG_TEAMSTAT:
			{
				OB_GETMSG(Msg_TeamStat);
				if(pMsg)
				{
					int iTeam = TEAM_NONE;
					switch(pMsg->m_Team)
					{
					case Q4_TEAM_MARINE:
						iTeam = TEAM_MARINE;
						break;
					case Q4_TEAM_STROGG:
						iTeam = TEAM_STROGG;
						break;
					default:
						assert(0 && "Bad Team Selection!");
						return InvalidParameter;
					}

					if(iTeam != TEAM_NONE)
					{
						if(!strcmp(pMsg->m_StatName, "score"))
							pMsg->m_Result = obUserData(gameLocal.mpGame.GetTeamScore(iTeam));
					}
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
		case Q4_MSG_GETLOCATION:
			{
				OB_GETMSG(Q4_Location);
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
		case Q4_MSG_GETPLAYERCASH:
			{
				OB_GETMSG(Q4_PlayerCash);
				if(pMsg)
				{
					idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::GetClassType()) ? static_cast<idPlayer*>(pEnt) : 0;
					if(pPlayer)
					{
						pMsg->m_Cash = pPlayer->buyMenuCash;
					}
				}
				break;
			}
		case Q4_MSG_ISBUYINGALLOWED:
			{
				OB_GETMSG(Q4_IsBuyingAllowed);
				if(pMsg)
				{
					pMsg->m_BuyingAllowed = gameLocal.mpGame.IsBuyingAllowedInTheCurrentGameMode() ? True : False;
				}
				break;
			}
		case Q4_MSG_BUYSOMETHING:
			{
				OB_GETMSG(Q4_ItemBuy);
				if(pMsg)
				{
					idPlayer *pPlayer = pEnt && pEnt->IsType(idPlayer::GetClassType()) ? static_cast<idPlayer*>(pEnt) : 0;
					if(pPlayer)
					{
						const char *pItemString = 0;
						switch(pMsg->m_Item)
						{
						case Q4_BUY_MACHINEGUN:
							pItemString = "weapon_machinegun";
							break;
						case Q4_BUY_SHOTGUN:
							pItemString = "weapon_shotgun";
							break;
						case Q4_BUY_NAILGUN:
							pItemString = "weapon_nailgun";
							break;
						case Q4_BUY_HYPERBLASTER:
							pItemString = "weapon_hyperblaster";
							break;
						case Q4_BUY_ROCKETLAUNCHER:
							pItemString = "weapon_rocketlauncher";
							break;
						case Q4_BUY_LIGHTNINGGUN:
							pItemString = "weapon_lightninggun";
							break;
						case Q4_BUY_GRENADELAUNCHER:
							pItemString = "weapon_grenadelauncher";
							break;
						case Q4_BUY_RAILGUN:
							pItemString = "weapon_railgun";
							break;
						case Q4_BUY_NAPALMGUN:
							pItemString = "weapon_napalmgun";
							break;
						case Q4_BUY_ARMOR_SM:
							pItemString = "item_armor_small";
							break;
						case Q4_BUY_ARMOR_LG:
							pItemString = "item_armor_large";
							break;
						case Q4_BUY_AMMO_REFILL:
							pItemString = "ammorefill";
							break;
						case Q4_BUY_AMMO_REGEN:
							pItemString = "ammo_regen";
							break;
						case Q4_BUY_HEALTH_REGEN:
							pItemString = "health_regen";
							break;
						case Q4_BUY_DAMAGE_BOOST:
							pItemString = "damage_boost";
							break;
						default:
							return InvalidParameter;
						}
						if(pItemString)
						{
							pMsg->m_Success = pPlayer->AttemptToBuyItem(pItemString) ? True : False;
						}
					}
				}
				break;
			}
		default:
			return UnknownMessageType;
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

	bool DebugRadius(const float _pos[3], const float _radius, const obColor &_color, float _time)
	{
		idVec3 vPos(_pos[0], _pos[1], _pos[2]);
		idVec4 vColor(_color.rF(), _color.gF(), _color.bF(), _color.aF());
		gameRenderWorld->DebugCircle( vColor, vPos, upVector, _radius, 8, SEC2MS(_time), false );
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
				gameRenderWorld->DrawText(_msg, vPosition, 0.3f, vColor, axis, 1, SEC2MS(_duration), true);
			}
			else
			{
				if(g_BotUI)
				{
					g_BotUI->SetStateString("hudtext", _msg);
					g_BotUI->StateChanged(gameLocal.time);
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
		const idBounds &worldBounds = gameLocal.GetWorldBounds(gameLocal.entities[gameLocal.world->entityNumber]);
		idBoundsToAABB(worldBounds, _aabb);
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
				
				if(pEdict->IsType(idPlayerStart::GetClassType()))
				{
					_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_PLAYERSTART;
				}
				else if(pEdict->IsType(rvJumpPad::GetClassType()))
				{
					rvJumpPad *pJumpPad = static_cast<rvJumpPad*>(pEdict);
					if(pJumpPad->targets.Num())
					{
						idEntity *pTarget = pJumpPad->targets[0];
						idBoundsToAABB(pTarget->GetPhysics()->GetAbsBounds(), _feature[iNumFeatures].m_TargetBounds);
						idVec3 vTarget = pTarget->GetPhysics()->GetOrigin();
						_feature[iNumFeatures].m_TargetPosition[0] = vTarget[0];
						_feature[iNumFeatures].m_TargetPosition[1] = vTarget[1];
						_feature[iNumFeatures].m_TargetPosition[2] = vTarget[2];
						_feature[iNumFeatures].m_Type = ENT_CLASS_GENERIC_JUMPPAD;
					}
				}
				else if(pEdict->IsType(idTrigger_Multi::GetClassType()))
				{
					idTrigger_Multi *pTrigger = static_cast<idTrigger_Multi*>(pEdict);
					if(pTrigger->IsTeleporter())
					{
						idEntity *pTarget = pTrigger->targets[0];
						idBoundsToAABB(pTarget->GetPhysics()->GetAbsBounds(), _feature[iNumFeatures].m_TargetBounds);
						idVec3 vTarget = pTarget->GetPhysics()->GetOrigin();
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
		return "Quake 4";
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
		case GAME_CTF:
			return "CTF";
		case GAME_1F_CTF:
			return "1 Flag CTF";
		case GAME_ARENA_CTF:
			return "Arena CTF";
		case GAME_ARENA_1F_CTF:
			return "Arena 1 Flag CTF";
		case GAME_DEADZONE:
			return "DeadZone";
		}
		return "";
	}

	const char *GetModVers()
	{
		return "Quake 4 1.4.1";
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
idDict g_RejoinBots[MAX_CLIENTS];
void omnibot_interface::InitOldBots()
{
	if(!IsOmnibotLoaded() || !gameLocal.isServer)
		return;

	for(int i = 0; i < MAX_CLIENTS; ++i)
	{
		if(g_RejoinBots[i].GetNumKeyVals())
		{
			int iClientNum = networkSystem->ServerConnectBot();
			if(iClientNum != -1)
			{
				g_BotUserInfo[iClientNum] = g_RejoinBots[i];
				gameLocal.SetUserInfo( iClientNum, g_BotUserInfo[iClientNum], false );
				cmdSystem->BufferCommandText( CMD_EXEC_NOW, va( "updateUI %d\n", iClientNum ) );
				g_Bot[iClientNum] = true;
			}
			g_RejoinBots[i].Clear();
		}
	}
}

void omnibot_interface::MapRestart()
{
	for(int i = 0; i < MAX_CLIENTS; ++i)
	{
		if(IsBot(i))
		{
			g_RejoinBots[i] = g_BotUserInfo[i];
		}
	}
}

bool omnibot_interface::InitBotInterface()
{
	if( !gameLocal.isServer )
		return false;

	gameLocal.Printf( "-------------- Omni-bot Init ----------------\n" );

	for(int i = 0; i < Q4_WP_MAX; ++i)
		g_WeaponSlotCache[i] = -1;
	for(int i = 0; i < MAX_GENTITIES; ++i)
	{
		m_EntityHandles[i].m_HandleSerial = 1;
		m_EntityHandles[i].m_NewEntity = false;
		m_EntityHandles[i].m_Used = false;
	}

	memset(&g_BotUserCommands, 0, sizeof(g_BotUserCommands));

	g_InterfaceFunctions = new Q4Interface;
	eomnibot_error err = Omnibot_LoadLibrary(Q4_VERSION_LATEST, "omnibot_q4", omnibot_path.GetString());
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
	if( !gameLocal.isServer )
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
	if( gameLocal.isServer && IsOmnibotLoaded() )
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
			if(omnibot_playing.GetInteger() != iNumBots)
				omnibot_playing.SetInteger(iNumBots);
		}
		else
		{
			if(omnibot_playing.GetInteger() != -1)
				omnibot_playing.SetInteger(-1);
		}

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
		g_DrawBotHud = false;
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
					g_BotUI->StateChanged(gameLocal.time);
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
		g_BotUI->ClearState();
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
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void omnibot_interface::Bot_Event_ClientConnected(int _client, bool _isbot)
{
	if(IsOmnibotLoaded())
	{
		// apparently a necessary hack
		if(_isbot)
		{
			gameLocal.SetUserInfo( _client, g_BotUserInfo[_client], false ); 
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
	if(IsOmnibotLoaded() && IsBot(_client))
	{
		Event_TakeDamage d = { HandleFromEntity(_ent) };
		g_BotFunctions.pfnSendEvent(_client, MessageHelper(PERCEPT_FEEL_PAIN, &d, sizeof(d)));
	}
}

void omnibot_interface::Bot_Event_Death(int _client, idEntity *_killer, const char *_meansofdeath)
{
	if(IsOmnibotLoaded() && IsBot(_client))
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
	if(IsOmnibotLoaded() && IsBot(_client))
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
	if(IsOmnibotLoaded() && IsBot(_client))
	{
		g_BotFunctions.pfnSendEvent(_client, MessageHelper(MESSAGE_RESETWEAPONS));
	}
}

void omnibot_interface::Bot_Event_AddWeapon(int _client, int _weaponId)
{
	if(IsOmnibotLoaded() && IsBot(_client))
	{
		Event_AddWeapon d = { _weaponId };
		g_BotFunctions.pfnSendEvent(_client, MessageHelper(MESSAGE_ADDWEAPON, &d, sizeof(d)));
	}
}

void omnibot_interface::Bot_Event_FireWeapon(int _client, int _weaponId, idEntity *_projectile)
{
	if(IsOmnibotLoaded() && IsBot(_client))
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
		if ( !ent || !ent->IsType( idPlayer::GetClassType() ) )
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
