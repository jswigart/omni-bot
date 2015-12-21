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

//////////////////////////////////////////////////////////////////////////

// Function: UpgradeModule
static int GM_CDECL gmfUpgradeModule( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( moduleType, 0 );

	VERIFY_MODULE_ID( moduleType );

	a_thread->PushInt( gModularCombatFuncs->ModuleUpgrade( native->GetGameEntity(), (MC_Module)moduleType ) ? 1 : 0 );
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
	
	a_thread->PushInt( gModularCombatFuncs->CanPhysPickup( native->GetGameEntity(), pickup ) ? 1 : 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// Function: GetPhysGunInfo
static int GM_CDECL gmfPhysGunInfo( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GM_TABLE_PARAM( statTable, 0, 0 );

	GameEntity heldEntity;
	float launchSpeed;
	if ( gModularCombatFuncs->GetPhysGunInfo( native->GetGameEntity(), heldEntity, launchSpeed ) )
	{
		if ( !statTable )
			a_thread->GetMachine()->AllocTableObject();

		statTable->Set( a_thread->GetMachine(), "HeldObject", gmVariable::EntityVar( heldEntity.AsInt() ) );
		//statTable->Set(a_thread->GetMachine(),"PullObject",gmVariable::EntityVar(info.mPullingEntity.AsInt()));
		statTable->Set( a_thread->GetMachine(), "LaunchSpeed", gmVariable( launchSpeed ) );
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

	/*float chargeCur = 0.0f, chargeMax = 0.0f;
	if ( gModularCombatFuncs->GetChargerStatus( unit, chargeCur, chargeMax ) )
	{
		if ( !statTable )
			statTable = a_thread->GetMachine()->AllocTableObject();

		statTable->Set( a_thread->GetMachine(), "Charge", gmVariable( chargeCur ) );
		statTable->Set( a_thread->GetMachine(), "MaxCharge", gmVariable( chargeMax ) );
		a_thread->PushTable( statTable );
	}*/
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// Function: GetModuleStats
static int GM_CDECL gmfGetModuleStats( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( moduleId, 0 );
	GM_TABLE_PARAM( statTable, 1, 0 );

	VERIFY_MODULE_ID( moduleId );

	if ( !statTable )
		a_thread->GetMachine()->AllocTableObject();
	
	int lvl, lvlMax, upgCost;
	float auxDrain, cooldown;
	if ( !gModularCombatFuncs->ModuleStats( native->GetGameEntity(), ( MC_Module )moduleId, lvl, lvlMax, upgCost, auxDrain, cooldown ) )
	{
		GM_EXCEPTION_MSG( "Failed to get module stats" );
		return GM_EXCEPTION;
	}

	statTable->Set( a_thread->GetMachine(), "Level", gmVariable( lvl ) );
	statTable->Set( a_thread->GetMachine(), "MaxLevel", gmVariable( lvlMax ) );
	statTable->Set( a_thread->GetMachine(), "UpgradeCost", gmVariable( upgCost ) );
	statTable->Set( a_thread->GetMachine(), "AuxDrain", gmVariable( auxDrain ) );
	statTable->Set( a_thread->GetMachine(), "Cooldown", gmVariable( cooldown ) );
	a_thread->PushTable( statTable );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////
// package: Modular Combat Bot Library Functions
static gmFunctionEntry sMCbotLib [] =
{
	{ "GetChargerStatus", gmfGetChargerStatus },
};

//////////////////////////////////////////////////////////////////////////
// package: Modular Combat Bot Script Functions
static gmFunctionEntry s_MCbotTypeLib [] =
{
	{ "UpgradeModule", gmfUpgradeModule },

	{ "CanPhysPickup", gmfCanPhysPickup },
	{ "GetPhysGunInfo", gmfPhysGunInfo },
};

//////////////////////////////////////////////////////////////////////////

static bool getTotalXp( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	int xpTotal, xpGame;
	if ( a_native && gModularCombatFuncs->GetPlayerExperience( a_native->GetGameEntity(), xpTotal, xpGame ) )
	{
		a_operands[ 0 ] = gmVariable( xpTotal );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getGameXp( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	int xpTotal, xpGame;
	if ( a_native && gModularCombatFuncs->GetPlayerExperience( a_native->GetGameEntity(), xpTotal, xpGame ) )
	{
		a_operands[ 0 ] = gmVariable( xpGame );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getModulePts( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	int modulePoints;
	if ( a_native && gModularCombatFuncs->GetPlayerModulePoints( a_native->GetGameEntity(), modulePoints ) )
	{	
		a_operands[ 0 ] = gmVariable( modulePoints );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getLevel( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	int level;
	if ( a_native && gModularCombatFuncs->GetPlayerLevel( a_native->GetGameEntity(), level ) )
	{
		a_operands[ 0 ] = gmVariable( level );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getAuxPower( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	float auxCur, auxMax, auxRegen;
	if ( a_native && gModularCombatFuncs->GetPlayerAux( a_native->GetGameEntity(), auxCur, auxMax, auxRegen ) )
	{
		a_operands[ 0 ] = gmVariable( auxCur );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getAuxPowerMax( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	float auxCur, auxMax, auxRegen;
	if ( a_native && gModularCombatFuncs->GetPlayerAux( a_native->GetGameEntity(), auxCur, auxMax, auxRegen ) )
	{
		a_operands[ 0 ] = gmVariable( auxMax );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getAuxPowerRegen( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	float auxCur, auxMax, auxRegen;
	if ( a_native && gModularCombatFuncs->GetPlayerAux( a_native->GetGameEntity(), auxCur, auxMax, auxRegen ) )
	{
		a_operands[ 0 ] = gmVariable( auxRegen );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getMinions( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	int cnt, cntMax;
	if ( a_native && gModularCombatFuncs->GetPlayerMinionCount( a_native->GetGameEntity(), cnt, cntMax ) )
	{
		a_operands[ 0 ] = gmVariable( cnt );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getMinionsMax( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	int cnt, cntMax;
	if ( a_native && gModularCombatFuncs->GetPlayerMinionCount( a_native->GetGameEntity(), cnt, cntMax ) )
	{
		a_operands[ 0 ] = gmVariable( cntMax );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getLasers( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	int cnt, cntMax;
	if ( a_native && gModularCombatFuncs->GetPlayerLaserCount( a_native->GetGameEntity(), cnt, cntMax ) )
	{
		a_operands[ 0 ] = gmVariable( cnt );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getLasersMax( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	int cnt, cntMax;
	if ( a_native && gModularCombatFuncs->GetPlayerLaserCount( a_native->GetGameEntity(), cnt, cntMax ) )
	{
		a_operands[ 0 ] = gmVariable( cntMax );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getCrows( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	int cnt, cntMax;
	if ( a_native && gModularCombatFuncs->GetPlayerCrowCount( a_native->GetGameEntity(), cnt, cntMax ) )
	{
		a_operands[ 0 ] = gmVariable( cnt );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getCrowsMax( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	int cnt, cntMax;
	if ( a_native && gModularCombatFuncs->GetPlayerCrowCount( a_native->GetGameEntity(), cnt, cntMax ) )
	{
		a_operands[ 0 ] = gmVariable( cntMax );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getMagmine( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	int cnt, cntMax;
	if ( a_native && gModularCombatFuncs->GetPlayerMagmineCount( a_native->GetGameEntity(), cnt, cntMax ) )
	{
		a_operands[ 0 ] = gmVariable( cnt );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getMagmineMax( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	int cnt, cntMax;
	if ( a_native && gModularCombatFuncs->GetPlayerMagmineCount( a_native->GetGameEntity(), cnt, cntMax ) )
	{
		a_operands[ 0 ] = gmVariable( cntMax );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getTurret( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	int cnt, cntMax;
	if ( a_native && gModularCombatFuncs->GetPlayerTurretCount( a_native->GetGameEntity(), cnt, cntMax ) )
	{
		a_operands[ 0 ] = gmVariable( cnt );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getTurretMax( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	int cnt, cntMax;
	if ( a_native && gModularCombatFuncs->GetPlayerTurretCount( a_native->GetGameEntity(), cnt, cntMax ) )
	{
		a_operands[ 0 ] = gmVariable( cntMax );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getManhack( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	int cnt, cntMax;
	if ( a_native && gModularCombatFuncs->GetPlayerManhackCount( a_native->GetGameEntity(), cnt, cntMax ) )
	{
		a_operands[ 0 ] = gmVariable( cnt );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

static bool getManhackMax( Client *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	int cnt, cntMax;
	if ( a_native && gModularCombatFuncs->GetPlayerManhackCount( a_native->GetGameEntity(), cnt, cntMax ) )
	{
		a_operands[ 0 ] = gmVariable( cntMax );
	}
	else
		a_operands[ 0 ].Nullify();
	return true;
}

void MC_Game::InitScriptBinds( gmMachine *_machine )
{
	LOG( "Binding MC Library.." );

	// Register the bot functions.
	_machine->RegisterLibrary( sMCbotLib, sizeof( sMCbotLib ) / sizeof( sMCbotLib[ 0 ] ) );
	//////////////////////////////////////////////////////////////////////////
	_machine->RegisterTypeLibrary( gmBot::GetType(), s_MCbotTypeLib, sizeof( s_MCbotTypeLib ) / sizeof( s_MCbotTypeLib[ 0 ] ) );

	gmBot::registerProperty( "TotalXp", getTotalXp, NULL );
	gmBot::registerProperty( "GameXp", getGameXp, NULL );
	gmBot::registerProperty( "ModulePoints", getModulePts, NULL );
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