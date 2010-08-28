////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompET.h"
#include "gmETBinds.h"

#include "gmBotLibrary.h"
#include "gmConfig.h"
#include "gmThread.h"
#include "gmMachine.h"
#include "gmBot.h"

#define CHECK_THIS_BOT() \
	Client *native = gmBot::GetThisObject( a_thread ); \
	if(!native) \
	{ \
	GM_EXCEPTION_MSG("Script Function on NULL object"); \
	return GM_EXCEPTION; \
	}

// Title: ET Script Bindings

//////////////////////////////////////////////////////////////////////////

// function: ChangePrimaryWeapon
//		Sets the bots primary weapon to a new weapon to use upon respawn
//
// Parameters:
//
//		int - weapon id to choose for primary weapon
//
// Returns:
//		int - true if success, false if error
static int GM_CDECL gmfBotPickPrimaryWeapon(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(weaponId, 0);

	bool bSucess = InterfaceFuncs::SelectPrimaryWeapon(native, (ET_Weapon)weaponId);	
	a_thread->PushInt(bSucess ? 1 : 0);
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: ChangeSecondaryWeapon
//		Sets the bots secondary weapon to a new weapon to use upon respawn
//
// Parameters:
//
//		int - weapon id to choose for secondary weapon
//
// Returns:
//		int - true if success, false if error
static int GM_CDECL gmfBotPickSecondaryWeapon(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(weaponId, 0);

	bool bSucess = InterfaceFuncs::SelectSecondaryWeapon(native, (ET_Weapon)weaponId);	
	a_thread->PushInt(bSucess ? 1 : 0);
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetReinforceTime
//		Gets the current reinforcement time for this bots team
//
// Parameters:
//
//		none
//
// Returns:
//		int - reinforce timer
static int GM_CDECL gmfGetReinforceTime(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(0);
	
	a_thread->PushFloat(InterfaceFuncs::GetReinforceTime(native));
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetCursorHint
//		Gets the current hint and hint value for the client
//
// Parameters:
//
//		table - table to store results. function sets 'type' and 'value'
//
// Returns:
//		none
static int GM_CDECL gmfGetCurrentCursorHint(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_TABLE_PARAM(hint, 0);

	int iHintType = 0, iHintValue = 0;
	InterfaceFuncs::GetCurrentCursorHint(native, iHintType, iHintValue);

	hint->Set(a_thread->GetMachine(), "type", gmVariable(iHintType));
	hint->Set(a_thread->GetMachine(), "value", gmVariable(iHintValue));

	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: ChangeSpawnPoint
//		Changes the bots active spawn point
//
// Parameters:
//
//		int - Spawn point to change to
//
// Returns:
//		none
static int GM_CDECL gmfChangeSpawnPoint(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(spawnpoint, 0);

	InterfaceFuncs::ChangeSpawnPoint(native, spawnpoint);
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

static int GM_CDECL gmfFireteamCreate(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(0);
	InterfaceFuncs::FireTeamCreate(native);
	return GM_OK;
}

static int GM_CDECL gmfFireteamDisband(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(0);
	InterfaceFuncs::FireTeamDisband(native);
	return GM_OK;
}

static int GM_CDECL gmfFireTeamLeave(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(0);
	InterfaceFuncs::FireTeamLeave(native);
	return GM_OK;
}

static int GM_CDECL gmfFireTeamApply(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(fireteamnum,0);
	InterfaceFuncs::FireTeamApply(native,fireteamnum);
	return GM_OK;
}

static int GM_CDECL gmfFireTeamInvite(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(1);
	GameEntity ent;
	GM_CHECK_GAMEENTITY_FROM_PARAM(ent,0);
	InterfaceFuncs::FireTeamInvite(native,ent);
	return GM_OK;
}

static int GM_CDECL gmfFireTeamWarn(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(1);
	GameEntity ent;
	GM_CHECK_GAMEENTITY_FROM_PARAM(ent,0);
	InterfaceFuncs::FireTeamWarn(native,ent);
	return GM_OK;
}

static int GM_CDECL gmfFireTeamKick(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(1);
	GameEntity ent;
	GM_CHECK_GAMEENTITY_FROM_PARAM(ent,0);
	InterfaceFuncs::FireTeamKick(native,ent);
	return GM_OK;
}

static int GM_CDECL gmfFireTeamPropose(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(1);
	GameEntity ent;
	GM_CHECK_GAMEENTITY_FROM_PARAM(ent,0);
	InterfaceFuncs::FireTeamPropose(native,ent);
	return GM_OK;
}

static int GM_CDECL gmfFireTeamGetInfo(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(0);

	ET_FireTeamInfo ft;
	if(InterfaceFuncs::FireTeamGetInfo(native, ft) && ft.m_InFireTeam)
	{
		DisableGCInScope gcEn(a_thread->GetMachine());

		gmMachine *pM = a_thread->GetMachine();
		gmTableObject *tbl = pM->AllocTableObject();
		gmTableObject *mbrtbl = pM->AllocTableObject();
		tbl->Set(pM,"Members",gmVariable(mbrtbl));

		tbl->Set(pM,"FireTeamNum",gmVariable(ft.m_FireTeamNum));
		tbl->Set(pM,"Leader",gmVariable::EntityVar(ft.m_Leader.AsInt()));

		int m = 0;
		for(int i = 0; i < ET_FireTeamInfo::MaxMembers; ++i)
		{
			if(ft.m_Members[i].IsValid())
			{
				mbrtbl->Set(pM,m++,gmVariable::EntityVar(ft.m_Members[i].AsInt()));
			}
		}
		a_thread->PushTable(tbl);
	}
	else
		a_thread->PushNull();

	return GM_OK;
}

static int GM_CDECL gmfIsInFireTeam(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(0);

	ET_FireTeamInfo ft;
	if(InterfaceFuncs::FireTeamGetInfo(native, ft) && ft.m_InFireTeam)
		a_thread->PushInt(ft.m_InFireTeam?1:0);
	else
		a_thread->PushInt(0);

	return GM_OK;
}

static int GM_CDECL gmfVoteYes(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(0);
	native->GameCommand("vote yes");
	return GM_OK;
}

static int GM_CDECL gmfVoteNo(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(0);
	native->GameCommand("vote no");
	return GM_OK;
}

int gmfSayFireTeam(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	if(a_thread->GetNumParams() > 0)
	{
		const int bufferSize = 512;
		char buffer[bufferSize];

		int iMsgPos = 0;
		const int chatMsgSize = 2048;
		char chatMsg[chatMsgSize] = {0};

		// build the string
		for(int i = 0; i < a_thread->GetNumParams(); ++i)
		{
			const char *pAsString = a_thread->Param(i).AsString(a_thread->GetMachine(), buffer, bufferSize);
			if(pAsString)
			{
				int len = strlen(pAsString);
				if(chatMsgSize - iMsgPos > len)
				{
					Utils::StringCopy(&chatMsg[iMsgPos], pAsString, len);
					iMsgPos += len;
				}
			}
		}

		g_EngineFuncs->BotCommand(native->GetGameID(), Utils::VA("say_buddy \"%s\"", chatMsg));
		return GM_OK;
	}

	GM_EXCEPTION_MSG("Expected 1+ parameters");
	return GM_EXCEPTION;
}

//////////////////////////////////////////////////////////////////////////

// function: GetGameType
//		Gets the current game type
//
// Parameters:
//
//		none
//
// Returns:
//		int - game type
static int GM_CDECL gmfGetGameType(gmThread *a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	
	a_thread->PushInt(InterfaceFuncs::GetGameType());
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: SetCvar
//		This function will set a game cvar
//
// Parameters:
//	
//		char   - the cvar to set
//		char   - the value of the cvar to be set
//
//
// Returns:
//		None
static int GM_CDECL gmfSetCvar(gmThread *a_thread)
{
	if(a_thread->GetNumParams() > 1)
	{
		const int bufferSize = 512;
		char buffer[bufferSize];

		int iPos = 0;
		const int cvarSize = 2048;
		const int valueSize = 2048;
		char cvar[cvarSize] = {0};
		char value[valueSize] = {0};

		const char *pAsString = a_thread->Param(0).AsString(a_thread->GetMachine(), buffer, bufferSize);
		if(pAsString)
		{
			int len = strlen(pAsString);
			if(cvarSize - iPos > len)
			{
				Utils::StringCopy(&cvar[iPos], pAsString, len);
				iPos += len;
			}
		}

		iPos = 0;
		// and for the message...
		for(int i = 1; i < a_thread->GetNumParams(); ++i)
		{
			const char *pAsString = a_thread->Param(i).AsString(a_thread->GetMachine(), buffer, bufferSize);
			if(pAsString)
			{
				int len = strlen(pAsString);
				if(valueSize - iPos > len)
				{
					Utils::StringCopy(&value[iPos], pAsString, len);
					iPos += len;
				}
			}
		}

		bool bSucess = InterfaceFuncs::SetCvar(cvar, value);
		a_thread->PushInt(bSucess ? 1 : 0);
		return GM_OK;
	}

	GM_EXCEPTION_MSG("Expected 2+ parameters");
	return GM_EXCEPTION;
}

//////////////////////////////////////////////////////////////////////////

// function: GetCvar
//		This function will get the value of a game cvar
//
// Parameters:
//	
//		char   - the cvar to get
//
//
// Returns:
//		The value of the cvar
static int GM_CDECL gmfGetCvar(gmThread *a_thread)
{
	if(a_thread->GetNumParams() > 0)
	{
		const int bufferSize = 512;
		char buffer[bufferSize];

		int iPos = 0;
		const int cvarSize = 2048;
		char cvar[cvarSize] = {0};

		const char *pAsString = a_thread->Param(0).AsString(a_thread->GetMachine(), buffer, bufferSize);
		if(pAsString)
		{
			int len = strlen(pAsString);
			if(cvarSize - iPos > len)
			{
				Utils::StringCopy(&cvar[iPos], pAsString, len);
				iPos += len;
			}
		}

		a_thread->PushInt(InterfaceFuncs::GetCvar(cvar));
		return GM_OK;
	}

	GM_EXCEPTION_MSG("Expected 1 parameter");
	return GM_EXCEPTION;
}

//////////////////////////////////////////////////////////////////////////

// function: BotPush
//		Set the bots push flag.
//		
//
// Parameters:
//
//		int push - 0 or 1
//
// Returns:
//		none
static int GM_CDECL gmfDisableBotPush(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(botPush, 0);

	InterfaceFuncs::DisableBotPush(native, botPush);
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetExplosiveState
//		Return the state of the explosive; unarmed, armed, invalid.
//		
//
// Parameters:
//
//		GameEntity
//
// Returns:
//		explosive state
static int GM_CDECL gmfGetExplosiveState(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(1);
	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM(gameEnt, 0);
	OBASSERT(gameEnt.IsValid(), "Bad Entity");
	a_thread->PushInt(InterfaceFuncs::GetExplosiveState(native,gameEnt));
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetConstructableState
//		Return the state of the constructible; built, not built, invalid.
//		
//
// Parameters:
//
//		GameEntity
//
// Returns:
//		constructable state
static int GM_CDECL gmfGetConstructableState(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(1);
	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM(gameEnt, 0);
	OBASSERT(gameEnt.IsValid(), "Bad Entity");
	a_thread->PushInt(InterfaceFuncs::GetConstructableState(native,gameEnt));
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetDestroyableState
//		Return if the target is destroyable.
//		
//
// Parameters:
//
//		GameEntity
//
// Returns:
//		destroyable state
static int GM_CDECL gmfGetDestroyableState(gmThread *a_thread)
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS(1);
	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM(gameEnt, 0);
	OBASSERT(gameEnt.IsValid(), "Bad Entity");
	a_thread->PushInt(InterfaceFuncs::IsDestroyable(native,gameEnt));
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: IsWaitingForMedic
//		Returns true if entity is waiting for a medic
//		
//
// Parameters:
//
//		GameEntity
//
// Returns:
//		IsWaitingForMedic
static int GM_CDECL gmfIsWaitingForMedic(gmThread *a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM(gameEnt, 0);
	OBASSERT(gameEnt.IsValid(), "Bad Entity");

	if(InterfaceFuncs::IsWaitingForMedic(gameEnt))
		a_thread->PushInt(1);
	else
		a_thread->PushInt(0);

	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

static gmFunctionEntry s_ExtendedBotTypeLib[] =
{ 
	{"ChangePrimaryWeapon",		gmfBotPickPrimaryWeapon},
	{"ChangeSecondaryWeapon",	gmfBotPickSecondaryWeapon},	
	{"GetReinforceTime",		gmfGetReinforceTime},
	{"GetCursorHint",			gmfGetCurrentCursorHint},
	{"ChangeSpawnPoint",		gmfChangeSpawnPoint},
	
	{"IsInFireTeam",			gmfIsInFireTeam},
	{"FireteamCreate",			gmfFireteamCreate},
	{"FireteamDisband",			gmfFireteamDisband},
	{"FireTeamLeave",			gmfFireTeamLeave},
	{"FireTeamInvite",			gmfFireTeamInvite},
	{"FireTeamApply",			gmfFireTeamApply},
	{"FireTeamWarn",			gmfFireTeamWarn},	
	{"FireTeamKick",			gmfFireTeamKick},
	{"FireTeamPropose",			gmfFireTeamPropose},
		
	{"FireTeamGetInfo",			gmfFireTeamGetInfo},

	{"VoteYes",					gmfVoteYes},
	{"VoteNo",					gmfVoteNo},

	{"SayFireTeam",				gmfSayFireTeam},

	{"DisableBotPush",			gmfDisableBotPush},

	{"GetExplosiveState",		gmfGetExplosiveState},
	{"GetConstructableState",	gmfGetConstructableState},
	{"GetDestroyableState",		gmfGetDestroyableState},
};

static gmFunctionEntry s_ExtendedBotLib[] =
{
	{"GetGameType",				gmfGetGameType},
	{"SetCvar",					gmfSetCvar},
	{"GetCvar",					gmfGetCvar},
	{"IsWaitingForMedic",		gmfIsWaitingForMedic},
};

void gmBindETBotLibrary(gmMachine *_machine)
{
	// Register the bot functions.
	_machine->RegisterLibrary(s_ExtendedBotLib, sizeof(s_ExtendedBotLib) / sizeof(s_ExtendedBotLib[0]));
	_machine->RegisterTypeLibrary(gmBot::GetType(), s_ExtendedBotTypeLib, sizeof(s_ExtendedBotTypeLib) / sizeof(s_ExtendedBotTypeLib[0]));

	// Register additional bot properties

	// var: TargetBreakableDistance
	//		The distance the bot will target breakable entities. Targets beyond this range will be ignored.
	gmBot::RegisterAutoProperty("TargetBreakableDist", GM_FLOAT, offsetof(ET_Client, m_BreakableTargetDistance), 0);
}
