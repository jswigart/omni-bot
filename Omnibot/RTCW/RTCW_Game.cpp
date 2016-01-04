////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "RTCW_Game.h"
#include "RTCW_Client.h"
#include "RTCW_VoiceMacros.h"

#include "gmCall.h"

#include "PathPlannerBase.h"
#include "BotPathing.h"
#include "NameManager.h"
#include "ScriptManager.h"
#include "IGameManager.h"
#include "RenderBuffer.h"

RTCW_Interface* gRTCWFuncs = 0;

IGame *CreateGameInstance()
{
	return new RTCW_Game;
}

RTCW_Game::RTCW_Game()
{
	mGameVars.mClientBase = 0;
	mGameVars.mGameVersion = RTCW_VERSION_LATEST;
	mGameVars.mGameAbbrev = "rtcw";
	mGameVars.mGameName = "Return to Castle Wolfenstein";
	mGameVars.mPlayerHeight = 64.f;
}

RTCW_Game::~RTCW_Game()
{
}

Client * RTCW_Game::CreateGameClient()
{
	return new RTCW_Client();
}

bool RTCW_Game::Init( System & system )
{
	AiState::SensoryMemory::SetEntityTraceOffsetCallback( RTCW_Game::RTCW_GetEntityClassTraceOffset );
	AiState::SensoryMemory::SetEntityAimOffsetCallback( RTCW_Game::RTCW_GetEntityClassAimOffset );

	if ( !IGame::Init( system ) )
		return false;

	return true;
}

static const IntEnum RTCW_TeamEnum [] =
{
	IntEnum( "SPECTATOR", OB_TEAM_SPECTATOR ),
	IntEnum( "AXIS", RTCW_TEAM_AXIS ),
	IntEnum( "ALLIES", RTCW_TEAM_ALLIES ),
};

void RTCW_Game::GetTeamEnumeration( const IntEnum *&_ptr, int &num )
{
	num = sizeof( RTCW_TeamEnum ) / sizeof( RTCW_TeamEnum[ 0 ] );
	_ptr = RTCW_TeamEnum;
}

static const IntEnum RTCW_WeaponEnum [] =
{
	IntEnum( "NONE", RTCW_WP_NONE ),
	IntEnum( "KNIFE", RTCW_WP_KNIFE ),
	IntEnum( "LUGER", RTCW_WP_LUGER ),
	IntEnum( "MP40", RTCW_WP_MP40 ),
	IntEnum( "MAUSER", RTCW_WP_MAUSER ),
	IntEnum( "AXIS_GRENADE", RTCW_WP_GREN_AXIS ),
	IntEnum( "PANZERFAUST", RTCW_WP_PANZERFAUST ),
	IntEnum( "VENOM", RTCW_WP_VENOM ),
	IntEnum( "FLAMETHROWER", RTCW_WP_FLAMETHROWER ),
	IntEnum( "COLT", RTCW_WP_COLT ),
	IntEnum( "THOMPSON", RTCW_WP_THOMPSON ),
	IntEnum( "GARAND", RTCW_WP_GARAND ),
	IntEnum( "ALLY_GRENADE", RTCW_WP_GREN_ALLIES ),
	IntEnum( "ROCKET", RTCW_WP_ROCKET_LAUNCHER ),
	IntEnum( "SNIPERRIFLE", RTCW_WP_SNIPERRIFLE ),
	IntEnum( "STEN", RTCW_WP_STEN ),
	IntEnum( "SYRINGE", RTCW_WP_SYRINGE ),
	IntEnum( "AMMO_PACK", RTCW_WP_AMMO_PACK ),
	IntEnum( "ARTY", RTCW_WP_ARTY ),
	IntEnum( "DYNAMITE", RTCW_WP_DYNAMITE ),
	IntEnum( "SNIPER", RTCW_WP_SNIPER ),
	IntEnum( "MEDKIT", RTCW_WP_MEDKIT ),
	IntEnum( "PLIERS", RTCW_WP_PLIERS ),
	IntEnum( "SMOKE_GRENADE", RTCW_WP_SMOKE_GRENADE ),
	IntEnum( "BINOCULARS", RTCW_WP_BINOCULARS ),
	IntEnum( "MOUNTABLE_MG42", RTCW_WP_MOUNTABLE_MG42 ),
	IntEnum( "FG42", RTCW_WP_FG42 ),
	IntEnum( "FG42_SCOPE", RTCW_WP_FG42SCOPE ),
	IntEnum( "SHOTGUN", RTCW_WP_SHOTGUN ),
};

void RTCW_Game::GetWeaponEnumeration( const IntEnum *&_ptr, int &num )
{
	num = sizeof( RTCW_WeaponEnum ) / sizeof( RTCW_WeaponEnum[ 0 ] );
	_ptr = RTCW_WeaponEnum;
}

static const IntEnum g_RTCWClassMappings [] =
{
	IntEnum( "SOLDIER", RTCW_CLASS_SOLDIER ),
	IntEnum( "MEDIC", RTCW_CLASS_MEDIC ),
	IntEnum( "ENGINEER", RTCW_CLASS_ENGINEER ),
	IntEnum( "LIEUTENANT", RTCW_CLASS_LIEUTENANT ),
	IntEnum( "ANYPLAYER", RTCW_CLASS_ANY ),
	IntEnum( "MG42MOUNT", RTCW_CLASSEX_MG42MOUNT ),
	IntEnum( "DYNAMITE_ENT", RTCW_CLASSEX_DYNAMITE ),
	IntEnum( "VEHICLE", RTCW_CLASSEX_VEHICLE ),
	IntEnum( "BREAKABLE", RTCW_CLASSEX_BREAKABLE ),
	IntEnum( "CORPSE", RTCW_CLASSEX_CORPSE ),
	IntEnum( "INJUREDPLAYER", RTCW_CLASSEX_INJUREDPLAYER ),
	IntEnum( "TREASURE", RTCW_CLASSEX_TREASURE ),
	IntEnum( "ROCKET_ENT", RTCW_CLASSEX_ROCKET ),
	IntEnum( "FLAME", RTCW_CLASSEX_FLAMECHUNK ),
	IntEnum( "ARTY_ENT", RTCW_CLASSEX_ARTY ),
	IntEnum( "AIRSTRIKE", RTCW_CLASSEX_AIRSTRIKE ),
	IntEnum( "GRENADE", RTCW_CLASSEX_GRENADE ),
	IntEnum( "BROKENCHAIR", RTCW_CLASSEX_BROKENCHAIR ),
	IntEnum( "LANDMINE", RTCW_CLASSEX_MINE ),
};

void RTCW_Game::InitScriptClasses( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptClasses( _machine, _table );

	FilterSensory::ANYPLAYERCLASS = RTCW_CLASS_ANY;

	int32_t iNumMappings = sizeof( g_RTCWClassMappings ) / sizeof( g_RTCWClassMappings[ 0 ] );
	for ( int i = 0; i < iNumMappings; ++i )
	{
		_table->Set( _machine, g_RTCWClassMappings[ i ].mKey, gmVariable( g_RTCWClassMappings[ i ].mValue ) );
	}
}

void RTCW_Game::InitVoiceMacros( gmMachine *_machine, gmTableObject *_table )
{
	_table->Set( _machine, "PATH_CLEARED", gmVariable( VCHAT_TEAM_PATHCLEARED ) );
	_table->Set( _machine, "ENEMY_WEAK", gmVariable( VCHAT_TEAM_ENEMYWEAK ) );
	_table->Set( _machine, "ALL_CLEAR", gmVariable( VCHAT_TEAM_ALLCLEAR ) );

	_table->Set( _machine, "INCOMING", gmVariable( VCHAT_TEAM_INCOMING ) );
	_table->Set( _machine, "FIRE_IN_THE_HOLE", gmVariable( VCHAT_TEAM_FIREINTHEHOLE ) );
	_table->Set( _machine, "ON_DEFENSE", gmVariable( VCHAT_TEAM_ONDEFENSE ) );
	_table->Set( _machine, "ON_OFFENSE", gmVariable( VCHAT_TEAM_ONOFFENSE ) );
	_table->Set( _machine, "TAKING_FIRE", gmVariable( VCHAT_TEAM_TAKINGFIRE ) );
	_table->Set( _machine, "MINES_CLEARED", gmVariable( VCHAT_TEAM_MINESCLEARED ) );
	_table->Set( _machine, "ENEMY_DISGUISED", gmVariable( VCHAT_TEAM_ENEMYDISGUISED ) );

	_table->Set( _machine, "NEED_MEDIC", gmVariable( VCHAT_TEAM_MEDIC ) );
	_table->Set( _machine, "NEED_AMMO", gmVariable( VCHAT_TEAM_NEEDAMMO ) );
	_table->Set( _machine, "NEED_BACKUP", gmVariable( VCHAT_TEAM_NEEDBACKUP ) );
	_table->Set( _machine, "NEED_ENGINEER", gmVariable( VCHAT_TEAM_NEEDENGINEER ) );
	_table->Set( _machine, "COVER_ME", gmVariable( VCHAT_TEAM_COVERME ) );
	_table->Set( _machine, "HOLD_FIRE", gmVariable( VCHAT_TEAM_HOLDFIRE ) );
	_table->Set( _machine, "WHERE_TO", gmVariable( VCHAT_TEAM_WHERETO ) );
	_table->Set( _machine, "NEED_OPS", gmVariable( VCHAT_TEAM_NEEDOPS ) );

	_table->Set( _machine, "FOLLOW_ME", gmVariable( VCHAT_TEAM_FOLLOWME ) );
	_table->Set( _machine, "LETS_GO", gmVariable( VCHAT_TEAM_LETGO ) );
	_table->Set( _machine, "MOVE", gmVariable( VCHAT_TEAM_MOVE ) );
	_table->Set( _machine, "CLEAR_PATH", gmVariable( VCHAT_TEAM_CLEARPATH ) );
	_table->Set( _machine, "DEFEND_OBJECTIVE", gmVariable( VCHAT_TEAM_DEFENDOBJECTIVE ) );
	_table->Set( _machine, "DISARM_DYNAMITE", gmVariable( VCHAT_TEAM_DISARMDYNAMITE ) );
	_table->Set( _machine, "CLEAR_MINES", gmVariable( VCHAT_TEAM_CLEARMINES ) );
	_table->Set( _machine, "REINFORCE_OFF", gmVariable( VCHAT_TEAM_REINFORCE_OFF ) );
	_table->Set( _machine, "REINFORCE_DEF", gmVariable( VCHAT_TEAM_REINFORCE_DEF ) );

	_table->Set( _machine, "AFFIRMATIVE", gmVariable( VCHAT_TEAM_AFFIRMATIVE ) );
	_table->Set( _machine, "NEGATIVE", gmVariable( VCHAT_TEAM_NEGATIVE ) );
	_table->Set( _machine, "THANKS", gmVariable( VCHAT_TEAM_THANKS ) );
	_table->Set( _machine, "WELCOME", gmVariable( VCHAT_TEAM_WELCOME ) );
	_table->Set( _machine, "SORRY", gmVariable( VCHAT_TEAM_SORRY ) );
	_table->Set( _machine, "OOPS", gmVariable( VCHAT_TEAM_OOPS ) );

	_table->Set( _machine, "COMMAND_ACK", gmVariable( VCHAT_TEAM_COMMANDACKNOWLEDGED ) );
	_table->Set( _machine, "COMMAND_DECLINED", gmVariable( VCHAT_TEAM_COMMANDDECLINED ) );
	_table->Set( _machine, "COMMAND_COMPLETED", gmVariable( VCHAT_TEAM_COMMANDCOMPLETED ) );
	_table->Set( _machine, "DESTROY_PRIMARY", gmVariable( VCHAT_TEAM_DESTROYPRIMARY ) );
	_table->Set( _machine, "DESTROY_SECONDARY", gmVariable( VCHAT_TEAM_DESTROYSECONDARY ) );
	_table->Set( _machine, "DESTROY_CONST", gmVariable( VCHAT_TEAM_DESTROYCONSTRUCTION ) );
	_table->Set( _machine, "CONST_COMMENCING", gmVariable( VCHAT_TEAM_CONSTRUCTIONCOMMENCING ) );
	_table->Set( _machine, "REPAIR_VEHICLE", gmVariable( VCHAT_TEAM_REPAIRVEHICLE ) );
	_table->Set( _machine, "DESTROY_VEHICLE", gmVariable( VCHAT_TEAM_DESTROYVEHICLE ) );
	_table->Set( _machine, "ESCORT_VEHICLE", gmVariable( VCHAT_TEAM_ESCORTVEHICLE ) );

	_table->Set( _machine, "IMA_SOLDIER", gmVariable( VCHAT_IMA_SOLDIER ) );
	_table->Set( _machine, "IMA_MEDIC", gmVariable( VCHAT_IMA_MEDIC ) );
	_table->Set( _machine, "IMA_ENGINEER", gmVariable( VCHAT_IMA_ENGINEER ) );
	_table->Set( _machine, "IMA_LIEUTENANT", gmVariable( VCHAT_IMA_LIEUTENANT ) );
	_table->Set( _machine, "IMA_COVERTOPS", gmVariable( VCHAT_IMA_COVERTOPS ) );

	_table->Set( _machine, "G_AFFIRMATIVE", gmVariable( VCHAT_GLOBAL_AFFIRMATIVE ) );
	_table->Set( _machine, "G_NEGATIVE", gmVariable( VCHAT_GLOBAL_NEGATIVE ) );
	_table->Set( _machine, "G_ENEMY_WEAK", gmVariable( VCHAT_GLOBAL_ENEMYWEAK ) );
	_table->Set( _machine, "G_HI", gmVariable( VCHAT_GLOBAL_HI ) );
	_table->Set( _machine, "G_BYE", gmVariable( VCHAT_GLOBAL_BYE ) );
	_table->Set( _machine, "G_GREATSHOT", gmVariable( VCHAT_GLOBAL_GREATSHOT ) );
	_table->Set( _machine, "G_CHEER", gmVariable( VCHAT_GLOBAL_CHEER ) );

	_table->Set( _machine, "G_THANKS", gmVariable( VCHAT_GLOBAL_THANKS ) );
	_table->Set( _machine, "G_WELCOME", gmVariable( VCHAT_GLOBAL_WELCOME ) );
	_table->Set( _machine, "G_OOPS", gmVariable( VCHAT_GLOBAL_OOPS ) );
	_table->Set( _machine, "G_SORRY", gmVariable( VCHAT_GLOBAL_SORRY ) );
	_table->Set( _machine, "G_HOLD_FIRE", gmVariable( VCHAT_GLOBAL_HOLDFIRE ) );
	_table->Set( _machine, "G_GOODGAME", gmVariable( VCHAT_GLOBAL_GOODGAME ) );
}

void RTCW_Game::AddBot( ParamsAddbot& addbot, bool createnow )
{
	//////////////////////////////////////////////////////////////////////////
	if ( !addbot.mName[ 0 ] )
	{
		NamePtr nr = NameManager::GetInstance()->GetName();
		std::string name = nr ? nr->GetName() : Utils::FindOpenPlayerName();
		Utils::StringCopy( addbot.mName, name.c_str(), sizeof( addbot.mName ) );
	}
	//////////////////////////////////////////////////////////////////////////
	assert( GameStarted() );

	if ( createnow )
		mBotJoining = true;
	int iGameID = gEngineFuncs->AddBot( addbot );
	if ( createnow )
		mBotJoining = false;
	if ( iGameID != -1 && createnow )
	{
		ClientPtr &cp = GetClientFromCorrectedGameId( iGameID );

		if ( !cp )
		{
			// Initialize the appropriate slot in the list.
			cp.reset( CreateGameClient() );
			cp->Init( iGameID );
		}

		cp->mDesiredTeam = addbot.mTeam;
		cp->mDesiredClass = addbot.mClass;

		//////////////////////////////////////////////////////////////////////////
		// Script callbacks
		if ( cp->mDesiredTeam == -1 )
		{
			gmVariable vteam = ScriptManager::GetInstance()->ExecBotCallback(
				cp.get(),
				"SelectTeam" );
			cp->mDesiredTeam = vteam.IsInt() ? vteam.GetInt() : -1;
		}
		if ( cp->mDesiredClass == -1 )
		{
			gmVariable vclass = ScriptManager::GetInstance()->ExecBotCallback(
				cp.get(),
				"SelectClass" );
			cp->mDesiredClass = vclass.IsInt() ? vclass.GetInt() : -1;
		}
		//////////////////////////////////////////////////////////////////////////
		// Magik: As there's no instant team/class switching in ET, this is order dependent
		// always call pfnChangeClass() _before_ pfnChangeTeam()!
		// todo: send the weapon preferences as 3rd param
		gEngineFuncs->ChangeClass( iGameID, cp->mDesiredClass, NULL );
		gEngineFuncs->ChangeTeam( iGameID, cp->mDesiredTeam, NULL );

		cp->CheckTeamEvent();
		cp->CheckClassEvent();

		ScriptManager::GetInstance()->ExecBotCallback( mClientList[ iGameID ].get(), "SelectWeapons" );
	}
}

// package: RTCW Script Events
//		Custom Events for Return to Castle Wolfenstein. Also see <Common Script Events>
void RTCW_Game::InitScriptEvents( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptEvents( _machine, _table );
}

void RTCW_Game::InitScriptEntityFlags( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptEntityFlags( _machine, _table );

	_table->Set( _machine, "MNT_MG42", gmVariable( RTCW_ENT_FLAG_MNT_MG42 ) );
	_table->Set( _machine, "CARRYINGGOAL", gmVariable( RTCW_ENT_FLAG_CARRYINGGOAL ) );
	_table->Set( _machine, "LIMBO", gmVariable( RTCW_ENT_FLAG_INLIMBO ) );
	_table->Set( _machine, "MOUNTABLE", gmVariable( RTCW_ENT_FLAG_ISMOUNTABLE ) );
	_table->Set( _machine, "POISONED", gmVariable( RTCW_ENT_FLAG_POISONED ) );
	_table->Set( _machine, "DISGUISED", gmVariable( RTCW_ENT_FLAG_DISGUISED ) );
	_table->Set( _machine, "MOUNTED", gmVariable( RTCW_ENT_FLAG_MOUNTED ) );
	_table->Set( _machine, "INJURED", gmVariable( RTCW_ENT_FLAG_INJURED ) );
}

void RTCW_Game::InitScriptPowerups( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptPowerups( _machine, _table );

	_table->Set( _machine, "FIRE", gmVariable( RTCW_PWR_FIRE ) );
	_table->Set( _machine, "ELECTRIC", gmVariable( RTCW_PWR_ELECTRIC ) );
	_table->Set( _machine, "BREATHER", gmVariable( RTCW_PWR_BREATHER ) );
	_table->Set( _machine, "NOFATIGUE", gmVariable( RTCW_PWR_NOFATIGUE ) );
	_table->Set( _machine, "REDFLAG", gmVariable( RTCW_PWR_REDFLAG ) );
	_table->Set( _machine, "BLUEFLAG", gmVariable( RTCW_PWR_BLUEFLAG ) );
	_table->Set( _machine, "BALL", gmVariable( RTCW_PWR_BALL ) );
}

void RTCW_Game::InitCommands()
{
	IGame::InitCommands();
}

const float RTCW_Game::RTCW_GetEntityClassTraceOffset( const TargetInfo &_target )
{
	if ( _target.mEntInfo.mGroup == ENT_GRP_PLAYER )
	{
		if ( _target.mEntInfo.mClassId > RTCW_CLASS_NULL && _target.mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS )
		{
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_PRONED ) )
				return 16.0f;
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_CROUCHED ) )
				return 24.0f;
			return 48.0f;
		}
	}
	return 0.0f;
}

const float RTCW_Game::RTCW_GetEntityClassAimOffset( const TargetInfo &_target )
{
	if ( _target.mEntInfo.mGroup == ENT_GRP_PLAYER )
	{
		if ( _target.mEntInfo.mClassId > RTCW_CLASS_NULL && _target.mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS )
		{
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_PRONED ) )
				return 16.0f;
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_CROUCHED ) )
				return 24.0f;
			return 48.0f;
		}
	}
	return 0.0f;
}

void RTCW_Game::ClientJoined( const EvClientConnected *msg )
{
	Utils::OutputDebug( kInfo, "Client Joined Game, IsBot: %d, ClientNum: %d", msg->mIsBot, msg->mGameId );
	if ( msg->mIsBot && !mBotJoining )
	{
		CheckGameState();
		assert( GameStarted() );
		assert( msg->mGameId < Constants::MAX_PLAYERS && msg->mGameId >= 0 );

		// If a bot isn't created by now, it has probably been a map change,
		// and the game has re-added the clients itself.
		ClientPtr &cp = GetClientFromCorrectedGameId( msg->mGameId );
		if ( !cp )
		{
			// Initialize the appropriate slot in the list.
			cp.reset( CreateGameClient() );
			cp->Init( msg->mGameId );

			cp->mDesiredTeam = msg->mDesiredTeam;
			cp->mDesiredClass = msg->mDesiredClass;

			gEngineFuncs->ChangeClass( msg->mGameId, cp->mDesiredClass, NULL );
			gEngineFuncs->ChangeTeam( msg->mGameId, cp->mDesiredTeam, NULL );

			cp->CheckTeamEvent();
			cp->CheckClassEvent();
		}
	}
}

void RTCW_Game::StartGame()
{
	if ( GameStarted() )
		return;

	//EngineFuncs::ConsoleMessage("<StartGame>");

	// Get Goals from the game.
	System::mInstance->mGoalManager->Reset();

	ErrorObj err;
	System::mInstance->mGoalManager->Load( std::string( gEngineFuncs->GetMapName() ), err );
	err.PrintToConsole();
	
	System::mInstance->mGoalManager->InitGameGoals();

	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
	DisableGCInScope gcEn( pMachine );

	// Load the script for the current map.
	int iThreadId = GM_INVALID_THREAD;

	const char * mapName = gEngineFuncs->GetMapName();
	filePath script( "nav/%s.gm", mapName );

	obStringBuffer str;
	gEngineFuncs->ServerGetVariable( "g_deathmatch", str.mBuffer, obStringBuffer::BUFFER_LENGTH );

	if ( std::string( "1" ) == str.mBuffer )
		script = filePath( "nav/%s_dm.gm", mapName );
	else if ( gEngineFuncs->GetGameType() == 7 )
		script = filePath( "nav/%s_cp.gm", mapName );

	if ( ScriptManager::GetInstance()->ExecuteFile( script, iThreadId ) )
	{
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
		{
			gmCall call;
			if ( call.BeginGlobalFunction( pMachine, "OnMapLoad", gmVariable::s_null, true ) )
			{
				call.End();
			}
		}
	}

	// cs: moved this out so it does not depend on a map script. some autoexec scripts define it.
	{
		gmCall call;
		if ( call.BeginGlobalFunction( pMachine, "PostMapLoad", gmVariable::s_null, true ) )
		{
			call.End();
		}
	}

	// Other initialization.
	mSettingLimiter.reset( new Regulator( 2000 ) );
}
