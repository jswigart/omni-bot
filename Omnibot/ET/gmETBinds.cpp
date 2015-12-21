////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "ET_Game.h"
#include "ET_Client.h"

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
static int GM_CDECL gmfBotPickPrimaryWeapon( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( weaponId, 0 );

	bool bSucess = gEnemyTerritoryFuncs->SelectWeapon( LoadOutPrimary, native->GetGameEntity(), (ET_Weapon)weaponId );
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

	bool bSucess = gEnemyTerritoryFuncs->SelectWeapon( LoadOutSecondary, native->GetGameEntity(), (ET_Weapon)weaponId );
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

	a_thread->PushFloat( gEnemyTerritoryFuncs->GetReinforceTime( native->GetGameEntity() ) / 1000.0f );
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

	int hintType = 0, hintValue = 0;
	gEnemyTerritoryFuncs->GetCurrentCursorHint( native->GetGameEntity(), hintType, hintValue );

	hint->Set( a_thread->GetMachine(), "type", gmVariable( hintType ) );
	hint->Set( a_thread->GetMachine(), "value", gmVariable( hintValue ) );

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

	gEnemyTerritoryFuncs->ChangeSpawnPoint( native->GetGameEntity(), spawnpoint );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

static int GM_CDECL gmfFireteamCreate( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 0 );
	gEnemyTerritoryFuncs->FireTeamCreate( native->GetGameEntity() );
	return GM_OK;
}

static int GM_CDECL gmfFireteamDisband( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 0 );
	gEnemyTerritoryFuncs->FireTeamDisband( native->GetGameEntity() );
	return GM_OK;
}

static int GM_CDECL gmfFireTeamLeave( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 0 );
	gEnemyTerritoryFuncs->FireTeamLeave( native->GetGameEntity() );
	return GM_OK;
}

static int GM_CDECL gmfFireTeamApply( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( fireteamnum, 0 );
	gEnemyTerritoryFuncs->FireTeamApply( native->GetGameEntity(), fireteamnum );
	return GM_OK;
}

static int GM_CDECL gmfFireTeamInvite( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GameEntity ent;
	GM_CHECK_GAMEENTITY_FROM_PARAM( ent, 0 );
	gEnemyTerritoryFuncs->FireTeamInvite( native->GetGameEntity(), ent );
	return GM_OK;
}

static int GM_CDECL gmfFireTeamWarn( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GameEntity ent;
	GM_CHECK_GAMEENTITY_FROM_PARAM( ent, 0 );
	gEnemyTerritoryFuncs->FireTeamWarn( native->GetGameEntity(), ent );
	return GM_OK;
}

static int GM_CDECL gmfFireTeamKick( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GameEntity ent;
	GM_CHECK_GAMEENTITY_FROM_PARAM( ent, 0 );
	gEnemyTerritoryFuncs->FireTeamKick( native->GetGameEntity(), ent );
	return GM_OK;
}

static int GM_CDECL gmfFireTeamPropose( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GameEntity ent;
	GM_CHECK_GAMEENTITY_FROM_PARAM( ent, 0 );
	gEnemyTerritoryFuncs->FireTeamPropose( native->GetGameEntity(), ent );
	return GM_OK;
}

static int GM_CDECL gmfFireTeamGetInfo( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 0 );

	int teamId = 0;
	GameEntity leader;
	GameEntity members[ 64 ];
	
	const int cnt = gEnemyTerritoryFuncs->FireTeamGetInfo( native->GetGameEntity(), teamId, leader, members );
	if ( cnt > 0 )
	{
		DisableGCInScope gcEn( a_thread->GetMachine() );

		gmMachine *pM = a_thread->GetMachine();
		gmTableObject *tbl = pM->AllocTableObject();
		gmTableObject *mbrtbl = pM->AllocTableObject();
		tbl->Set( pM, "Members", gmVariable( mbrtbl ) );

		tbl->Set( pM, "FireTeamNum", gmVariable( teamId ) );
		tbl->Set( pM, "Leader", gmVariable::EntityVar( leader.AsInt() ) );

		int m = 0;
		for ( int i = 0; i < cnt; ++i )
		{
			if ( members[ i ].IsValid() )
			{
				mbrtbl->Set( pM, m++, gmVariable::EntityVar( members[ i ].AsInt() ) );
			}
		}
		a_thread->PushTable( tbl );
	}
	else
		a_thread->PushNull();

	return GM_OK;
}

static int GM_CDECL gmfIsInFireTeam( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 0 );

	int teamId = 0;
	GameEntity leader;
	GameEntity members[ 64 ];

	const int cnt = gEnemyTerritoryFuncs->FireTeamGetInfo( native->GetGameEntity(), teamId, leader, members );
	a_thread->PushInt( cnt > 0 ? 1 : 0 );

	return GM_OK;
}

static int GM_CDECL gmfVoteYes( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 0 );
	native->GameCommand( "vote yes" );
	return GM_OK;
}

static int GM_CDECL gmfVoteNo( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 0 );
	native->GameCommand( "vote no" );
	return GM_OK;
}

int gmfSayFireTeam( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	if ( a_thread->GetNumParams() > 0 )
	{
		const int bufferSize = 512;
		char buffer[ bufferSize ];

		int iMsgPos = 0;
		const int chatMsgSize = 2048;
		char chatMsg[ chatMsgSize ] = { 0 };

		// build the string
		for ( int i = 0; i < a_thread->GetNumParams(); ++i )
		{
			const char *pAsString = a_thread->Param( i ).AsString( a_thread->GetMachine(), buffer, bufferSize );
			if ( pAsString )
			{
				int len = (int)strlen( pAsString );
				if ( chatMsgSize - iMsgPos > len )
				{
					Utils::StringCopy( &chatMsg[ iMsgPos ], pAsString, len );
					iMsgPos += len;
				}
			}
		}

		gEngineFuncs->BotCommand( native->GetGameID(), va( "say_buddy \"%s\"", chatMsg ).c_str() );
		return GM_OK;
	}

	GM_EXCEPTION_MSG( "Expected 1+ parameters" );
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
static int GM_CDECL gmfDisableBotPush( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( botPush, 0 );

	gEnemyTerritoryFuncs->DisableBotPush( native->GetGameEntity(), botPush!=0 );
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
		a_thread->PushInt( gEnemyTerritoryFuncs->GetExplosiveState( gameEnt ) );
	else
		a_thread->PushNull();
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
static int GM_CDECL gmfGetConstructableState( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	
	if ( gameEnt.IsValid() )
		a_thread->PushInt( gEnemyTerritoryFuncs->GetConstructableState( native->GetGameEntity(), gameEnt ) );
	else
		a_thread->PushNull();
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetDestroyableState
//		Return the state of the destroyable; exploded, not exploded, invalid.
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
		a_thread->PushInt( gEnemyTerritoryFuncs->IsDestroyable( native->GetGameEntity(), gameEnt ) );
	else
		a_thread->PushNull();
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
static int GM_CDECL gmfIsWaitingForMedic( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	
	a_thread->PushInt( gEnemyTerritoryFuncs->IsWaitingForMedic( gameEnt ) ? 1 : 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetMG42Info
//		Returns currently mounted mg42 info for the bot
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
	if ( tbl != NULL && gEnemyTerritoryFuncs->GetMg42Properties( native->GetGameEntity(), mg42Info ) )
	{
		tbl->Set( a_thread->GetMachine(), "CenterFacing", gmVariable( mg42Info.mCenterFacing ) );
		tbl->Set( a_thread->GetMachine(), "MinHorizontal", gmVariable( mg42Info.mMinHorizontalArc ) );
		tbl->Set( a_thread->GetMachine(), "MaxHorizontal", gmVariable( mg42Info.mMaxHorizontalArc ) );
		tbl->Set( a_thread->GetMachine(), "MinVertical", gmVariable( mg42Info.mMinVerticalArc ) );
		tbl->Set( a_thread->GetMachine(), "MaxVertical", gmVariable( mg42Info.mMaxVerticalArc ) );
		a_thread->PushInt( 1 );
	}
	else
	{
		a_thread->PushNull();
	}
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetMountedPlayerOnMG42
//		Returns entity currently mounted on the given mg42 entity
//
//
// Parameters:
//
//		GameEntity
//
// Returns:
//		Entity of the owner
static int gmfGetMountedPlayerOnMG42( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	GM_CHECK_NUM_PARAMS( 1 );
	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	
	GameEntity owner = gEnemyTerritoryFuncs->GetMountedPlayerOnMG42( gameEnt );
	if ( owner.IsValid() )
	{
		gmVariable v;
		v.SetEntity( owner.AsInt() );
		a_thread->Push( v );
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
	
	a_thread->PushInt( gEnemyTerritoryFuncs->IsMountableGunRepairable( native->GetGameEntity(), gameEnt ) ? 1 : 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: TeamLandminesAvailable
//		Returns number of landmines available to be planted
//
//
// Parameters:
//
//		None
//
// Returns:
//		Number of landmines available to be planted
static int gmfTeamLandminesAvailable( gmThread *a_thread )
{
	CHECK_THIS_BOT();
	int currentMines, maxMines;
	gEnemyTerritoryFuncs->TeamMineCount( native->GetGameEntity(), currentMines, maxMines );
	a_thread->PushInt( maxMines - currentMines );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetCabinetData
//		Returns table of data for the cabinet
//
//
// Parameters:
//
//		GameEntity
//		Table
//
// Returns:
//		Table of Cabinet Data
static int gmfGetCabinetData( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );
	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	GM_CHECK_TABLE_PARAM( tbl, 1 );

	DisableGCInScope gcEn( a_thread->GetMachine() );

	if ( !tbl )
		tbl = a_thread->GetMachine()->AllocTableObject();

	ParamsCabinetData data;
	if ( tbl != NULL && gEnemyTerritoryFuncs->GetCabinetData( gameEnt, data ) )
	{
		tbl->Set( a_thread->GetMachine(), "CurrentAmount", gmVariable( data.mCurrentAmount ) );
		tbl->Set( a_thread->GetMachine(), "MaxAmount", gmVariable( data.mMaxAmount ) );
		tbl->Set( a_thread->GetMachine(), "Rate", gmVariable( data.mRate ) );
		a_thread->PushInt( 1 );
	}
	else
	{
		a_thread->PushNull();
	}
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

	{ "IsInFireTeam", gmfIsInFireTeam, NULL },
	{ "FireteamCreate", gmfFireteamCreate, NULL },
	{ "FireteamDisband", gmfFireteamDisband, NULL },
	{ "FireTeamLeave", gmfFireTeamLeave, NULL },
	{ "FireTeamInvite", gmfFireTeamInvite, NULL },
	{ "FireTeamApply", gmfFireTeamApply, NULL },
	{ "FireTeamWarn", gmfFireTeamWarn, NULL },
	{ "FireTeamKick", gmfFireTeamKick, NULL },
	{ "FireTeamPropose", gmfFireTeamPropose, NULL },

	{ "FireTeamGetInfo", gmfFireTeamGetInfo, NULL },

	{ "VoteYes", gmfVoteYes, NULL },
	{ "VoteNo", gmfVoteNo, NULL },

	{ "SayFireTeam", gmfSayFireTeam, NULL },

	{ "DisableBotPush", gmfDisableBotPush, NULL },

	{ "GetExplosiveState", gmfGetExplosiveState, NULL },
	{ "GetConstructableState", gmfGetConstructableState, NULL },
	{ "GetDestroyableState", gmfGetDestroyableState, NULL },

	// TODO: add owner to MG42Info table when breaking mod compat doesn't matter?
	{ "GetMG42Info", gmfGetMG42Info, NULL },
	{ "GetMountedPlayerOnMG42", gmfGetMountedPlayerOnMG42, NULL },

	{ "IsMG42Repairable", gmfIsMG42Repairable, NULL },
	{ "TeamLandminesAvailable", gmfTeamLandminesAvailable, NULL },
};

static gmFunctionEntry s_ExtendedBotLib [] =
{
	{ "IsWaitingForMedic", gmfIsWaitingForMedic, NULL },
	{ "GetCabinetData", gmfGetCabinetData, NULL },
};

void ET_Game::InitScriptBinds( gmMachine *_machine )
{
	// Register the bot functions.
	_machine->RegisterLibrary( s_ExtendedBotLib, sizeof( s_ExtendedBotLib ) / sizeof( s_ExtendedBotLib[ 0 ] ) );
	_machine->RegisterTypeLibrary( gmBot::GetType(), s_ExtendedBotTypeLib, sizeof( s_ExtendedBotTypeLib ) / sizeof( s_ExtendedBotTypeLib[ 0 ] ) );

	// Register additional bot properties

	// var: TargetBreakableDistance
	//		The distance the bot will target breakable entities. Targets beyond this range will be ignored.
	gmBot::RegisterAutoProperty( "TargetBreakableDist", GM_FLOAT, offsetof( ET_Client, mBreakableTargetDistance ), 0 );
}