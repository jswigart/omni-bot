////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "gmConfig.h"
#include "gmBot.h"
#include "gmThread.h"
#include "gmMachine.h"
#include "gmGameEntity.h"
#include "gmBotLibrary.h"

#include "MC_Game.h"
#include "MC_Client.h"
#include "MC_InterfaceFuncs.h"

#define CHECK_THIS_BOT() \
	MC_Client *native = static_cast<MC_Client*>(gmBot::GetThisObject( a_thread )); \
	if(!native) \
		{ \
	GM_EXCEPTION_MSG("Script Function on NULL object"); \
	return GM_EXCEPTION; \
		}

#define VERIFY_MODULE_ID(id) if(id<0 || id >= MC_MODULE_MAX) { \
	GM_EXCEPTION_MSG("Invalid Module Id!"); \
	return GM_EXCEPTION; }

#define VERIFY_UPGRADE_ID(id) if(id<0 || id >= MC_UPGRADE_MAX) { \
	GM_EXCEPTION_MSG("Invalid Module Id!"); \
	return GM_EXCEPTION; }

// Title: MC Script Bindings

//////////////////////////////////////////////////////////////////////////

// Function: GetPlayerStats
//static int GM_CDECL gmfGetPlayerStats(gmThread *a_thread)
//{
//	CHECK_THIS_BOT();
//	GM_TABLE_PARAM(statTable,0,0);
//
//	MC_PlayerStats stats = {};
//	if(InterfaceFuncs::GetPlayerStats(native->GetGameEntity(),stats))
//	{
//		if(!statTable)
//			a_thread->GetMachine()->AllocTableObject();
//		statTable->Set(a_thread->GetMachine(),"TotalXp",gmVariable(stats.m_ExperienceTotal));
//		statTable->Set(a_thread->GetMachine(),"GameXp",gmVariable(stats.m_ExperienceGame));
//		statTable->Set(a_thread->GetMachine(),"ModulePoints",gmVariable(stats.m_ModulePoints));
//		statTable->Set(a_thread->GetMachine(),"WeaponPoints",gmVariable(stats.m_WeaponPoints));
//		statTable->Set(a_thread->GetMachine(),"Credits",gmVariable(stats.m_Credits));
//		statTable->Set(a_thread->GetMachine(),"Level",gmVariable(stats.m_Level));
//		statTable->Set(a_thread->GetMachine(),"Minions",gmVariable(stats.m_Minions));
//		statTable->Set(a_thread->GetMachine(),"MinionsMax",gmVariable(stats.m_MinionsMax));
//		statTable->Set(a_thread->GetMachine(),"AuxPower",gmVariable(stats.m_AuxPower));
//		statTable->Set(a_thread->GetMachine(),"AuxPowerMax",gmVariable(stats.m_AuxPowerMax));
//		statTable->Set(a_thread->GetMachine(),"AuxRegenRate",gmVariable(stats.m_AuxRegenRate));
//		a_thread->PushTable(statTable);
//		return GM_OK;
//	}
//	else
//	{
//		GM_EXCEPTION_MSG("Error Getting Stats!");
//		return GM_EXCEPTION;
//	}
//}

//////////////////////////////////////////////////////////////////////////

// Function: GetModuleStats
static int GM_CDECL gmfGetModuleStats( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( moduleType, 0 );
	GM_TABLE_PARAM( statTable, 1, 0 );

	VERIFY_MODULE_ID( moduleType );

	if ( !statTable )
		a_thread->GetMachine()->AllocTableObject();
	const MC_ModuleStats &modStats = native->GetModuleStats();
	statTable->Set( a_thread->GetMachine(), "Lvl", gmVariable( modStats.m_Module[ moduleType ].m_Lvl ) );
	statTable->Set( a_thread->GetMachine(), "MaxLvl", gmVariable( modStats.m_Module[ moduleType ].m_MaxLvl ) );
	statTable->Set( a_thread->GetMachine(), "UpgradeCost", gmVariable( modStats.m_Module[ moduleType ].m_UpgradeCost ) );
	statTable->Set( a_thread->GetMachine(), "AuxDrain", gmVariable( modStats.m_Module[ moduleType ].m_AuxDrain ) );
	statTable->Set( a_thread->GetMachine(), "Cooldown", gmVariable( modStats.m_Module[ moduleType ].m_Cooldown ) );
	a_thread->PushTable( statTable );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// Function: UpgradeModule
static int GM_CDECL gmfUpgradeModule( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( moduleType, 0 );

	VERIFY_MODULE_ID( moduleType );

	a_thread->PushInt( InterfaceFuncs::UpgradeModule( native->GetGameEntity(), moduleType ) ? 1 : 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// Function: CanPhysPickup
static int GM_CDECL gmfCanPhysPickup( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GameEntity pickup;
	GM_CHECK_GAMEENTITY_FROM_PARAM( pickup, 0 );

	a_thread->PushInt( InterfaceFuncs::CanPhysPickup( native->GetGameEntity(), pickup ) ? 1 : 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// Function: GetPhysGunInfo
static int GM_CDECL gmfPhysGunInfo( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GM_TABLE_PARAM( statTable, 0, 0 );

	MC_PhysGunInfo info = {};
	if ( InterfaceFuncs::GetPhysGunInfo( native->GetGameEntity(), info ) )
	{
		if ( !statTable )
			a_thread->GetMachine()->AllocTableObject();

		statTable->Set( a_thread->GetMachine(), "HeldObject", gmVariable::EntityVar( info.m_HeldEntity.AsInt() ) );
		//statTable->Set(a_thread->GetMachine(),"PullObject",gmVariable::EntityVar(info.m_PullingEntity.AsInt()));
		statTable->Set( a_thread->GetMachine(), "LaunchSpeed", gmVariable( info.m_LaunchSpeed ) );
		a_thread->PushTable( statTable );
	}
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// Function: GetChargerStatus
static int GM_CDECL gmfGetChargerStatus( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GameEntity unit;
	GM_CHECK_GAMEENTITY_FROM_PARAM( unit, 0 );
	GM_TABLE_PARAM( statTable, 1, 0 );

	MC_ChargerStatus info = {};
	if ( InterfaceFuncs::GetChargerStatus( unit, info ) )
	{
		if ( !statTable )
			statTable = a_thread->GetMachine()->AllocTableObject();

		statTable->Set( a_thread->GetMachine(), "Charge", gmVariable( info.m_CurrentCharge ) );
		statTable->Set( a_thread->GetMachine(), "MaxCharge", gmVariable( info.m_MaxCharge ) );
		a_thread->PushTable( statTable );
	}
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////
// package: Modular Combat Bot Library Functions
static gmFunctionEntry s_MCbotLib [] =
{
	//	{"LockPlayerPosition",		gmfLockPlayerPosition},
	//	{"HudHint",					gmfHudHint},
	//	{"HudMessage",				gmfHudTextBox},
	//	{"HudAlert",				gmfHudAlert},
	//	{"HudMenu",					gmfHudMenu},
	//	{"HudTextMsg",				gmfHudTextMsg},

	{ "GetChargerStatus", gmfGetChargerStatus },
};

//////////////////////////////////////////////////////////////////////////
// package: Modular Combat Bot Script Functions
static gmFunctionEntry s_MCbotTypeLib [] =
{
	//{"GetPlayerStats",			gmfGetPlayerStats},
	{ "GetModuleStats", gmfGetModuleStats },
	{ "UpgradeModule", gmfUpgradeModule },

	{ "CanPhysPickup", gmfCanPhysPickup },
	{ "GetPhysGunInfo", gmfPhysGunInfo },
};

//////////////////////////////////////////////////////////////////////////

static bool getTotalXp( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.m_ExperienceTotal );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getGameXp( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.m_ExperienceGame );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getModulePts( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.m_ModulePoints );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getWeaponPts( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.m_WeaponPoints );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getCredits( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.m_WeaponPoints );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getLevel( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.m_Level );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getMinions( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.m_Minions );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getMinionsMax( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.m_MinionsMax );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getAuxPower( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.m_AuxPower );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getAuxPowerMax( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.m_AuxPowerMax );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getAuxPowerRegen( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.m_AuxPowerMax );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

void MC_Game::InitScriptBinds( gmMachine *_machine )
{
	LOG( "Binding MC Library..." );

	// Register the bot functions.
	_machine->RegisterLibrary( s_MCbotLib, sizeof( s_MCbotLib ) / sizeof( s_MCbotLib[ 0 ] ) );
	//////////////////////////////////////////////////////////////////////////
	_machine->RegisterTypeLibrary( gmBot::GetType(), s_MCbotTypeLib, sizeof( s_MCbotTypeLib ) / sizeof( s_MCbotTypeLib[ 0 ] ) );

	gmBot::registerProperty( "TotalXp", getTotalXp, NULL );
	gmBot::registerProperty( "GameXp", getGameXp, NULL );
	gmBot::registerProperty( "ModulePoints", getModulePts, NULL );
	gmBot::registerProperty( "WeaponPoints", getWeaponPts, NULL );
	gmBot::registerProperty( "Credits", getCredits, NULL );
	gmBot::registerProperty( "Level", getLevel, NULL );
	gmBot::registerProperty( "Minions", getMinions, NULL );
	gmBot::registerProperty( "MinionsMax", getMinionsMax, NULL );
	gmBot::registerProperty( "AuxPower", getAuxPower, NULL );
	gmBot::registerProperty( "AuxPowerMax", getAuxPowerMax, NULL );
	gmBot::registerProperty( "AuxRegenRate", getAuxPowerRegen, NULL );

	{
		gmTableObject *pModuleTable = _machine->AllocTableObject();
		_machine->GetGlobals()->Set( _machine, "MODULE", gmVariable( pModuleTable ) );
		InitScriptModules( _machine, pModuleTable );
	}
}