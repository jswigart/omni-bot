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
static int GM_CDECL gmfGetPlayerStats( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_TABLE_PARAM( statTable, 0, 0 );

	MC_PlayerStats stats = {};
	if ( InterfaceFuncs::GetPlayerStats( native->GetGameEntity(), stats ) )
	{
		if ( !statTable )
			a_thread->GetMachine()->AllocTableObject();
		statTable->Set( a_thread->GetMachine(), "TotalXp", gmVariable( stats.mExperienceTotal ) );
		statTable->Set( a_thread->GetMachine(), "GameXp", gmVariable( stats.mExperienceGame ) );
		statTable->Set( a_thread->GetMachine(), "ModulePoints", gmVariable( stats.mModulePoints ) );
		statTable->Set( a_thread->GetMachine(), "WeaponPoints", gmVariable( stats.mWeaponPoints ) );
		statTable->Set( a_thread->GetMachine(), "Credits", gmVariable( stats.mCredits ) );
		statTable->Set( a_thread->GetMachine(), "Level", gmVariable( stats.mLevel ) );		
		statTable->Set( a_thread->GetMachine(), "AuxPower", gmVariable( stats.mAuxPower ) );
		statTable->Set( a_thread->GetMachine(), "AuxPowerMax", gmVariable( stats.mAuxPowerMax ) );
		statTable->Set( a_thread->GetMachine(), "AuxRegenRate", gmVariable( stats.mAuxRegenRate ) );

		statTable->Set( a_thread->GetMachine(), "Minions", gmVariable( stats.mCount[ MC_PlayerStats::Q_MINION ] ) );
		statTable->Set( a_thread->GetMachine(), "MinionsMax", gmVariable( stats.mCountMax[ MC_PlayerStats::Q_MINION ] ) );
		statTable->Set( a_thread->GetMachine(), "Lasers", gmVariable( stats.mCount[ MC_PlayerStats::Q_LASERS ] ) );
		statTable->Set( a_thread->GetMachine(), "LasersMax", gmVariable( stats.mCountMax[ MC_PlayerStats::Q_LASERS ] ) );
		statTable->Set( a_thread->GetMachine(), "Crows", gmVariable( stats.mCount[ MC_PlayerStats::Q_CROWS ] ) );
		statTable->Set( a_thread->GetMachine(), "CrowsMax", gmVariable( stats.mCountMax[ MC_PlayerStats::Q_CROWS ] ) );
		statTable->Set( a_thread->GetMachine(), "Magmine", gmVariable( stats.mCount[ MC_PlayerStats::Q_MAGMINE ] ) );
		statTable->Set( a_thread->GetMachine(), "MagmineMax", gmVariable( stats.mCountMax[ MC_PlayerStats::Q_MAGMINE ] ) );
		statTable->Set( a_thread->GetMachine(), "Turret", gmVariable( stats.mCount[ MC_PlayerStats::Q_TURRET ] ) );
		statTable->Set( a_thread->GetMachine(), "TurretMax", gmVariable( stats.mCountMax[ MC_PlayerStats::Q_TURRET ] ) );
		statTable->Set( a_thread->GetMachine(), "Manhack", gmVariable( stats.mCount[ MC_PlayerStats::Q_MANHACK ] ) );
		statTable->Set( a_thread->GetMachine(), "ManhackMax", gmVariable( stats.mCountMax[ MC_PlayerStats::Q_MANHACK ] ) );
		
		a_thread->PushTable( statTable );
		return GM_OK;
	}
	else
	{
		GM_EXCEPTION_MSG( "Error Getting Stats!" );
		return GM_EXCEPTION;
	}
}

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
	statTable->Set( a_thread->GetMachine(), "Level", gmVariable( modStats.mModule[ moduleType ].mLvl ) );
	statTable->Set( a_thread->GetMachine(), "MaxLevel", gmVariable( modStats.mModule[ moduleType ].mMaxLvl ) );
	statTable->Set( a_thread->GetMachine(), "UpgradeCost", gmVariable( modStats.mModule[ moduleType ].mUpgradeCost ) );
	statTable->Set( a_thread->GetMachine(), "AuxDrain", gmVariable( modStats.mModule[ moduleType ].mAuxDrain ) );
	statTable->Set( a_thread->GetMachine(), "Cooldown", gmVariable( modStats.mModule[ moduleType ].mCooldown ) );
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

		statTable->Set( a_thread->GetMachine(), "HeldObject", gmVariable::EntityVar( info.mHeldEntity.AsInt() ) );
		//statTable->Set(a_thread->GetMachine(),"PullObject",gmVariable::EntityVar(info.mPullingEntity.AsInt()));
		statTable->Set( a_thread->GetMachine(), "LaunchSpeed", gmVariable( info.mLaunchSpeed ) );
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

		statTable->Set( a_thread->GetMachine(), "Charge", gmVariable( info.mCurrentCharge ) );
		statTable->Set( a_thread->GetMachine(), "MaxCharge", gmVariable( info.mMaxCharge ) );
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
	{ "GetPlayerStats", gmfGetPlayerStats },
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
		a_operands[ 0 ] = gmVariable( stats.mExperienceTotal );
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
		a_operands[ 0 ] = gmVariable( stats.mExperienceGame );
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
		a_operands[ 0 ] = gmVariable( stats.mModulePoints );
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
		a_operands[ 0 ] = gmVariable( stats.mWeaponPoints );
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
		a_operands[ 0 ] = gmVariable( stats.mWeaponPoints );
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
		a_operands[ 0 ] = gmVariable( stats.mLevel );
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
		a_operands[ 0 ] = gmVariable( stats.mAuxPower );
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
		a_operands[ 0 ] = gmVariable( stats.mAuxPowerMax );
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
		a_operands[ 0 ] = gmVariable( stats.mAuxPowerMax );
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
		a_operands[ 0 ] = gmVariable( stats.mCount[ MC_PlayerStats::Q_MINION ] );
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
		a_operands[ 0 ] = gmVariable( stats.mCountMax[ MC_PlayerStats::Q_MINION ] );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getLasers( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.mCount[ MC_PlayerStats::Q_LASERS ] );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getLasersMax( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.mCountMax[ MC_PlayerStats::Q_LASERS ] );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getCrows( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.mCount[ MC_PlayerStats::Q_CROWS ] );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getCrowsMax( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.mCountMax[ MC_PlayerStats::Q_CROWS ] );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getMagmine( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.mCount[ MC_PlayerStats::Q_MAGMINE ] );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getMagmineMax( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.mCountMax[ MC_PlayerStats::Q_MAGMINE ] );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getTurret( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.mCount[ MC_PlayerStats::Q_TURRET ] );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getTurretMax( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.mCountMax[ MC_PlayerStats::Q_TURRET ] );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getManhack( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.mCount[ MC_PlayerStats::Q_MANHACK ] );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getManhackMax( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	if ( a_native )
	{
		const MC_PlayerStats stats = ( (MC_Client*)a_native )->GetPlayerStats();
		a_operands[ 0 ] = gmVariable( stats.mCountMax[ MC_PlayerStats::Q_MANHACK ] );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

void MC_Game::InitScriptBinds( gmMachine *_machine )
{
	LOG( "Binding MC Library.." );

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
	gmBot::registerProperty( "AuxPower", getAuxPower, NULL );
	gmBot::registerProperty( "AuxPowerMax", getAuxPowerMax, NULL );
	gmBot::registerProperty( "AuxRegenRate", getAuxPowerRegen, NULL );

	gmBot::registerProperty( "Minions", getMinions, NULL );
	gmBot::registerProperty( "MinionsMax", getMinionsMax, NULL );
	gmBot::registerProperty( "Lasers", getLasers, NULL );
	gmBot::registerProperty( "LasersMax", getLasersMax, NULL );
	gmBot::registerProperty( "Crows", getCrows, NULL );
	gmBot::registerProperty( "CrowsMax", getCrowsMax, NULL );
	gmBot::registerProperty( "Magmine", getMagmine, NULL );
	gmBot::registerProperty( "MagmineMax", getMagmineMax, NULL );
	gmBot::registerProperty( "Turrets", getTurret, NULL );
	gmBot::registerProperty( "TurretsMax", getTurretMax, NULL );
	gmBot::registerProperty( "Manhack", getManhack, NULL );
	gmBot::registerProperty( "ManhackMax", getManhackMax, NULL );
	
	{
		gmTableObject *pModuleTable = _machine->AllocTableObject();
		_machine->GetGlobals()->Set( _machine, "MODULE", gmVariable( pModuleTable ) );
		InitScriptModules( _machine, pModuleTable );
	}
}