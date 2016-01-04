////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "RTCW_Game.h"
#include "RTCW_Client.h"

#include "gmConfig.h"
#include "gmThread.h"
#include "gmMachine.h"
#include "gmBot.h"
#include "gmBotLibrary.h" // crapshoot: for GM_CHECK_GAMEENTITY_FROM_PARAM in GetWeaponTag

#define CHECK_THIS_BOT() \
	Client *native = gmBot::GetThisObject( a_thread ); \
	if(!native) \
		{ \
	GM_EXCEPTION_MSG("Script Function on NULL object"); \
	return GM_EXCEPTION; \
		}

// Title: RTCW Script Bindings
//
//////////////////////////////////////////////////////////////////////////

// function: SendPrivateMessage
//		This function executes a private message for this bot.
//
// Parameters:
//
//		char   - partial name match for target client(s)
//		std::string - message to send
//
//
// Returns:
//		None
static int GM_CDECL gmfSendPrivateMessage( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	if ( a_thread->GetNumParams() > 1 )
	{
		const int bufferSize = 512;
		char buffer[ bufferSize ];

		int iMsgPos = 0;
		const int chatMsgSize = 2048;
		char chatMsg[ chatMsgSize ] = { 0 };
		char targName[ chatMsgSize ] = { 0 };

		const char *pAsString = a_thread->Param( 0 ).AsString( a_thread->GetMachine(), buffer, bufferSize );
		if ( pAsString )
		{
			int len = strlen( pAsString );
			if ( chatMsgSize - iMsgPos > len )
			{
				Utils::StringCopy( &targName[ iMsgPos ], pAsString, len );
				iMsgPos += len;
			}
		}

		iMsgPos = 0;
		// and for the message.
		for ( int i = 1; i < a_thread->GetNumParams(); ++i )
		{
			const char *pAsString = a_thread->Param( i ).AsString( a_thread->GetMachine(), buffer, bufferSize );
			if ( pAsString )
			{
				int len = strlen( pAsString );
				if ( chatMsgSize - iMsgPos > len )
				{
					Utils::StringCopy( &chatMsg[ iMsgPos ], pAsString, len );
					iMsgPos += len;
				}
			}
		}
		
		a_thread->PushInt( gRTCWFuncs->SendPrivateMessage( native->GetGameEntity(), targName, chatMsg ) ? 1 : 0 );
		return GM_OK;
	}

	GM_EXCEPTION_MSG( "Expected 2+ parameters" );
	return GM_EXCEPTION;
}

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
static int GM_CDECL gmfBotPickPrimaryWeapon( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( weaponId, 0 );

	bool bSucess = gRTCWFuncs->SelectPrimaryWeapon( native->GetGameEntity(), (RTCW_Weapon)weaponId );
	a_thread->PushInt( bSucess ? 1 : 0 );
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
static int GM_CDECL gmfBotPickSecondaryWeapon( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( weaponId, 0 );

	bool bSucess = gRTCWFuncs->SelectSecondaryWeapon( native->GetGameEntity(), (RTCW_Weapon)weaponId );
	a_thread->PushInt( bSucess ? 1 : 0 );
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
static int GM_CDECL gmfGetReinforceTime( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 0 );

	a_thread->PushFloat( gRTCWFuncs->GetReinforceTime( native->GetGameEntity() ) );
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
static int GM_CDECL gmfGetCurrentCursorHint( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_TABLE_PARAM( hint, 0 );

	int iHintType = 0, iHintValue = 0;
	gRTCWFuncs->GetCurrentCursorHint( native->GetGameEntity(), iHintType, iHintValue );

	hint->Set( a_thread->GetMachine(), "type", gmVariable( iHintType ) );
	hint->Set( a_thread->GetMachine(), "value", gmVariable( iHintValue ) );

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
static int GM_CDECL gmfChangeSpawnPoint( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( spawnpoint, 0 );

	gRTCWFuncs->ChangeSpawnPoint( native->GetGameEntity(), spawnpoint );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: CanSnipe
//		Checks the bot sniping capability. Playerclass is covert ops and has sniper weapon.
//
// Parameters:
//
//		void
//
// Returns:
//		none
static int GM_CDECL gmfCanSnipe( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 0 );

	a_thread->PushInt( gRTCWFuncs->CanSnipe( native->GetGameEntity() ) ? 1 : 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: Snipe
//		Gives the bot a snipe goal.
//
// Parameters:
//
//		<Vector3> - The <Vector3> position to snipe from
//		<Vector3> - The <Vector3> facing direction to look while sniping
//		<float> - OPTIONAL - Radius of the goal
//
// Returns:
//		none
static int GM_CDECL gmfSnipe( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 2 );
	/*	GM_CHECK_VECTOR_PARAM(x1,y1,z1,0);
	GM_CHECK_VECTOR_PARAM(x2,y2,z2,1);
	GM_FLOAT_PARAM(rad, 2, 32.f);

	Client *native = gmBot::GetThisObject( a_thread );
	if(!native)
	{
	GM_EXCEPTION_MSG("Script Function on NULL object");
	return GM_EXCEPTION;
	}

	RTCW_Goal_Snipe::GoalInfo goalInfo;

	MapGoalPtr mg(new ET_SniperGoal());
	mg->SetPosition(Vector3f(x1,y1,z1));
	mg->SetFacing(Vector3f(x2,y2,z2));
	mg->SetRadius(rad);

	native->GetBrain()->ResetSubgoals("Script Snipe");
	GoalPtr g(new ET_Goal_Snipe(native, RTCW_WP_NONE, mg, goalInfo));
	g->SignalStatus(true);
	native->GetBrain()->InsertSubgoal(g);*/

	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetSpawnPoint
//		Gets the bots current spawn point
//
// Parameters:
//
//		none
//
// Returns:
//		int - spawn point
static int GM_CDECL gmfGetSpawnPoint( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 0 );

	a_thread->PushInt( gRTCWFuncs->GetSpawnPoint( native->GetGameEntity() ) );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: Suicide
//		Set the bots suicide flag.
//
//
// Parameters:
//
//		int suicide - 0 or 1
//		int persist - 0 or 1
//
// Returns:
//		none
static int GM_CDECL gmfSetSuicide( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 2 );
	GM_CHECK_INT_PARAM( suicide, 0 );
	GM_CHECK_INT_PARAM( persist, 1 );

	gRTCWFuncs->SetSuicide( native->GetGameEntity(), suicide, persist );
	return GM_OK;
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
static int GM_CDECL gmfDisableBotPush( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( botPush, 0 );

	gRTCWFuncs->DisableBotPush( native->GetGameEntity(), botPush );
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
static int GM_CDECL gmfGetExplosiveState( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	if ( gameEnt.IsValid() )
		a_thread->PushInt( gRTCWFuncs->GetExplosiveState( native->GetGameEntity(), gameEnt ) );
	else
		a_thread->PushNull();
	return GM_OK;
}

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
static int GM_CDECL gmfGetDestroyableState( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	
	if ( gameEnt.IsValid() )
		a_thread->PushInt( gRTCWFuncs->IsDestroyable( native->GetGameEntity(), gameEnt ) );
	else
		a_thread->PushNull();
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetMG42Info
//		Returns currently mounted mg42 info for the bot
//
//
// Parameters:
//
//		GameEntity
//		Table
//
// Returns:
//		MG42 Info
static int gmfGetMG42Info( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );

	GM_CHECK_TABLE_PARAM( tbl, 0 );

	DisableGCInScope gcEn( a_thread->GetMachine() );

	if ( !tbl )
		tbl = a_thread->GetMachine()->AllocTableObject();

	ParamsMG42Info mg42Info;
	if ( tbl != NULL && gRTCWFuncs->GetMg42Properties( native->GetGameEntity(), mg42Info ) )
	{
		tbl->Set( a_thread->GetMachine(), "CenterFacing", gmVariable( mg42Info.mCenterFacing ) );
		tbl->Set( a_thread->GetMachine(), "MinHorizontal", gmVariable( mg42Info.mMinHorizontalArc ) );
		tbl->Set( a_thread->GetMachine(), "MaxHorizontal", gmVariable( mg42Info.mMaxHorizontalArc ) );
		tbl->Set( a_thread->GetMachine(), "MinVertical", gmVariable( mg42Info.mMinVerticalArc ) );
		tbl->Set( a_thread->GetMachine(), "MaxVertical", gmVariable( mg42Info.mMaxVerticalArc ) );
		a_thread->PushTable( tbl );
	}
	else
	{
		a_thread->PushNull();
	}
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: IsMG42Repairable
//		Returns whether or not the MG42 is repairable
//
//
// Parameters:
//
//		GameEntity
//
// Returns:
//		1 if the Mg42 is repairable
static int gmfIsMG42Repairable( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	
	a_thread->PushInt( gRTCWFuncs->IsMountableGunRepairable( native->GetGameEntity(), gameEnt ) ? 1 : 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: IsMedicNear
//		Returns whether or not a Medic is nearby
//
//
// Parameters:
//
//		None
//
// Returns:
//		1 if medic is near, 0 if not
static int gmfIsMedicNear( gmThread *a_thread )
{
	CHECK_THIS_BOT();

	int medicNear = gRTCWFuncs->IsMedicNear( native->GetGameEntity() ) ? 1 : 0;
	a_thread->PushInt( medicNear );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GotoLimbo
//		Instructs the bot to tap out
//
//
// Parameters:
//
//		None
//
// Returns:
//		1 if successful, 0 if not
static int gmfGoToLimbo( gmThread *a_thread )
{
	CHECK_THIS_BOT();

	int goLimbo = gRTCWFuncs->GoToLimbo( native->GetGameEntity() ) ? 1 : 0;
	a_thread->PushInt( goLimbo );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

static gmFunctionEntry s_ExtendedBotTypeLib [] =
{
	{ "ChangePrimaryWeapon", gmfBotPickPrimaryWeapon, NULL },
	{ "ChangeSecondaryWeapon", gmfBotPickSecondaryWeapon, NULL },
	{ "GetReinforceTime", gmfGetReinforceTime, NULL },
	{ "GetCursorHint", gmfGetCurrentCursorHint, NULL },
	{ "ChangeSpawnPoint", gmfChangeSpawnPoint, NULL },
	{ "CanSnipe", gmfCanSnipe, NULL },
	{ "Snipe", gmfSnipe, NULL },
	{ "SendPrivateMessage", gmfSendPrivateMessage, NULL },
	{ "GetSpawnPoint", gmfGetSpawnPoint, NULL },
	{ "Suicide", gmfSetSuicide, NULL },
	{ "DisableBotPush", gmfDisableBotPush, NULL },
	{ "GetExplosiveState", gmfGetExplosiveState, NULL },
	{ "GetDestroyableState", gmfGetDestroyableState, NULL },
	{ "GetMG42Info", gmfGetMG42Info, NULL },
	{ "IsMG42Repairable", gmfIsMG42Repairable, NULL },
	{ "IsMedicNear", gmfIsMedicNear, NULL },
	{ "GoToLimbo", gmfGoToLimbo, NULL },
};

//static gmFunctionEntry s_ExtendedBotLib[] =
//{
//};

void RTCW_Game::InitScriptBinds( gmMachine *_machine )
{
	// Register the bot functions.
	//_machine->RegisterLibrary(s_ExtendedBotLib, sizeof(s_ExtendedBotLib) / sizeof(s_ExtendedBotLib[0]));
	//////////////////////////////////////////////////////////////////////////
	_machine->RegisterTypeLibrary( gmBot::GetType(), s_ExtendedBotTypeLib, sizeof( s_ExtendedBotTypeLib ) / sizeof( s_ExtendedBotTypeLib[ 0 ] ) );

	// Register additional bot properties

	// var: TargetBreakableDistance
	//		The distance the bot will target breakable entities. Targets beyond this range will be ignored.
	gmBot::RegisterAutoProperty( "TargetBreakableDist", GM_FLOAT, offsetof( RTCW_Client, mBreakableTargetDistance ), 0 );
	gmBot::RegisterAutoProperty( "HealthEntityDist", GM_FLOAT, offsetof( RTCW_Client, mHealthEntityDistance ), 0 );
	gmBot::RegisterAutoProperty( "AmmoEntityDist", GM_FLOAT, offsetof( RTCW_Client, mAmmoEntityDistance ), 0 );
	gmBot::RegisterAutoProperty( "WeaponEntityDist", GM_FLOAT, offsetof( RTCW_Client, mWeaponEntityDistance ), 0 );
	gmBot::RegisterAutoProperty( "ProjectileEntityDist", GM_FLOAT, offsetof( RTCW_Client, mProjectileEntityDistance ), 0 );

	gmBot::RegisterAutoProperty( "StrafeJump", GM_INT, offsetof( RTCW_Client, mStrafeJump ), 0 );
}