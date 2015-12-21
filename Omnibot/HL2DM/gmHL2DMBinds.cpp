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

#include "HL2DM_Game.h"
#include "HL2DM_Client.h"

#define CHECK_THIS_BOT() \
	HL2DM_Client *native = static_cast<HL2DM_Client*>(gmBot::GetThisObject( a_thread )); \
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

// Title: HL2DM Script Bindings

//////////////////////////////////////////////////////////////////////////

// Function: CanPhysPickup
static int GM_CDECL gmfCanPhysPickup( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GameEntity pickup;
	GM_CHECK_GAMEENTITY_FROM_PARAM( pickup, 0 );

	a_thread->PushInt( gHL2DMFuncs->CanPhysPickup( native->GetGameEntity(), pickup ) ? 1 : 0 );
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
	if ( gHL2DMFuncs->GetPhysGunInfo( native->GetGameEntity(), heldEntity, launchSpeed ) )
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
// package: Modular Combat Bot Library Functions
//static gmFunctionEntry s_HL2DMbotLib [] =
//{
//	//	{"LockPlayerPosition",		gmfLockPlayerPosition},
//	//	{"HudHint",					gmfHudHint},
//	//	{"HudMessage",				gmfHudTextBox},
//	//	{"HudAlert",				gmfHudAlert},
//	//	{"HudMenu",					gmfHudMenu},
//	//	{"HudTextMsg",				gmfHudTextMsg},
//
//	{ "GetChargerStatus", gmfGetChargerStatus },
//};

//////////////////////////////////////////////////////////////////////////
// package: Modular Combat Bot Script Functions
static gmFunctionEntry s_HL2DMbotTypeLib [] =
{
	{ "CanPhysPickup", gmfCanPhysPickup },
	{ "GetPhysGunInfo", gmfPhysGunInfo },
};

void HL2DM_Game::InitScriptBinds( gmMachine *_machine )
{
	LOG( "Binding HL2DM Library.." );

	// Register the bot functions.
	//_machine->RegisterLibrary( s_HL2DMbotLib, sizeof( s_HL2DMbotLib ) / sizeof( s_HL2DMbotLib[ 0 ] ) );
	//////////////////////////////////////////////////////////////////////////
	_machine->RegisterTypeLibrary( gmBot::GetType(), s_HL2DMbotTypeLib, sizeof( s_HL2DMbotTypeLib ) / sizeof( s_HL2DMbotTypeLib[ 0 ] ) );
}
