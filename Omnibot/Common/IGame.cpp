////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "NameManager.h"
#include "ScriptManager.h"
#include "WeaponDatabase.h"
#include "MapGoalDatabase.h"
#include "Revision.h"
#include "IGame.h"
#include "BotBaseStates.h"
#include "BotGlobalStates.h"
#include "BotSteeringSystem.h"
#include "ScriptGoal.h"
#include "PathPlannerBase.h"
#include "IGameManager.h"
#include "gmCall.h"

#include <type_traits>

GameVars::GameVars()
	: mPlayerHeight( 100 )
	, mClientBase( 0 )
	, mGameAbbrev( "game" )
	, mGameName( "Unknown Game" )
	, mRendersToGame( false )
	, mVersionString( "0.9" )
{	
}

const std::string GameVars::GetDLLName() const
{
#ifdef WIN32
	return va( "omni-bot\\omnibot_%s.dll", mGameAbbrev.c_str() ).c_str();
#else
	return va( "omni-bot/omnibot_%s.so", mGameAbbrev.c_str() ).c_str();
#endif
}

const std::string GameVars::GetModSubFolder() const
{
#ifdef WIN32
	return va( "%s\\", mGameAbbrev.c_str() ).c_str();
#else
	return va( "%s", mGameAbbrev.c_str() ).c_str();
#endif
}

const std::string GameVars::GetNavSubfolder() const
{
#ifdef WIN32
	return va( "%s\\nav\\", mGameAbbrev.c_str() ).c_str();
#else
	return va( "%s/nav", mGameAbbrev.c_str() ).c_str();
#endif
}

const std::string GameVars::GetScriptSubfolder() const
{
#ifdef WIN32
	return va( "%s\\scripts\\", mGameAbbrev.c_str() ).c_str();
#else
	return va( "%s/scripts", mGameAbbrev.c_str() ).c_str();
#endif
}

NavParms::NavParms()
	: AgentHeightStand( 64.f )
	, AgentHeightCrouch( 32.f )
	, AgentRadius( 14.f )
	, AgentMaxClimb( 18.f )
	, WalkableSlopeAngle( 45.0f )
{
}

int32_t IGame::mGameMsec = 0;
int32_t IGame::mDeltaMsec = 0;
int32_t IGame::mStartTimeMsec = 0;
int32_t IGame::mGameFrame = 0;

float IGame::mGravity = 0;
bool IGame::mCheatsEnabled = false;
bool IGame::mBotJoining = false;

GameState IGame::mGameState = GAME_STATE_INVALID;
GameState IGame::mLastGameState = GAME_STATE_INVALID;

EntityInstance IGame::mGameEntities[ Constants::MAX_ENTITIES ];
int IGame::mMaxEntity = 0;

typedef std::shared_ptr<AiState::ScriptGoal> ScriptGoalPtr;
typedef std::list<ScriptGoalPtr> ScriptGoalList;

ScriptGoalList gScriptGoalList;

#include "google/protobuf/text_format.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"

IGame::IGame()
	: mStateRoot( 0 )
	, mNumDeletedThreads( 0 )
	, mbDrawBlockableTests( false )
{
}

IGame::~IGame()
{
}

const GameVars &IGame::GetGameVars() const
{
	return mGameVars;
}

bool IGame::CheckVersion( int version )
{
	return version == GetGameVars().mGameVersion;
}

GoalManager *IGame::AllocGoalManager()
{
	return new GoalManager;
}

bool IGame::Init( System & system )
{
	mStartTimeMsec = gEngineFuncs->GetGameTime();

	for ( int i = 0; i < MaxDeletedThreads; ++i )
	{
		mDeletedThreads[ i ] = GM_INVALID_THREAD;
	}

	for ( int i = 0; i < mMaxEntity; ++i )
	{
		mGameEntities[ i ].mEntity = GameEntity();
		mGameEntities[ i ].mEntInfo = EntityInfo();
		mGameEntities[ i ].mTimeStamp = 0;
	}

	InitCommands();
	InitScriptSupport();

	gWeaponDatabase.LoadWeaponDefinitions( true );
	gMapGoalDatabase.LoadMapGoalDefinitions( true );

	// Reset the global blackboard.
	gBlackboard.RemoveAllBBRecords( bbk_All );
	
	return true;
}

void IGame::Shutdown()
{
	mUpdateMap.clear();

	if ( GameStarted() )
	{
		EndGame();
		mLastGameState = mGameState = GAME_STATE_INVALID;
	}

	for ( int i = 0; i < Constants::MAX_PLAYERS; ++i )
	{
		if ( mClientList[ i ] )
		{
			mClientList[ i ]->Shutdown();
			mClientList[ i ].reset();
		}
	}

	gMapGoalDatabase.Unload();
	gScriptGoalList.clear();

	OB_DELETE( mStateRoot );
}

void IGame::InitScriptSupport()
{
	LOGFUNCBLOCK;
	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

	// Bind scripts
	LOG( "Initializing Game Bindings.." );
	InitScriptBinds( pMachine );
	LOG( "done." );

	// Register Script Constants.
	LOG( "Registering Script Constants.." );

	DisableGCInScope gcEn( pMachine );

	// Register Teams to Script.
	gmTableObject *pTeamTable = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "TEAM", gmVariable( pTeamTable ) );
	InitScriptTeams( pMachine, pTeamTable );

	// Register Weapons to Script.
	gmTableObject *pWeaponTable = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "WEAPON", gmVariable( pWeaponTable ) );
	InitScriptWeapons( pMachine, pWeaponTable );

	// Register Items to Script.
	gmTableObject *pItemTable = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "ITEM", gmVariable( pItemTable ) );
	InitScriptItems( pMachine, pItemTable );

	// Register Classes to Script
	gmTableObject *pGroupTable = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "GROUP", gmVariable( pGroupTable ) );
	InitScriptGroups( pMachine, pGroupTable );

	// Register Classes to Script
	gmTableObject *pClassTable = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "CLASS", gmVariable( pClassTable ) );
	InitScriptClasses( pMachine, pClassTable );

	// Register Roles to Script
	gmTableObject *pRoleTable = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "ROLE", gmVariable( pRoleTable ) );
	InitScriptRoles( pMachine, pRoleTable );

	// Register Skills to Script
	gmTableObject *pSkillTable = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "SKILL", gmVariable( pSkillTable ) );
	InitScriptSkills( pMachine, pSkillTable );

	// Register Events to Script.
	gmTableObject *pEventTable = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "EVENT", gmVariable( pEventTable ) );
	InitScriptEvents( pMachine, pEventTable );

	// Register the entity flags
	gmTableObject *pEntityFlags = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "ENTFLAG", gmVariable( pEntityFlags ) );
	InitScriptEntityFlags( pMachine, pEntityFlags );

	// Register the powerups.
	gmTableObject *pPowerUps = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "POWERUP", gmVariable( pPowerUps ) );
	InitScriptPowerups( pMachine, pPowerUps );

	// Register the entity categories
	gmTableObject *pCategoryTable = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "CAT", gmVariable( pCategoryTable ) );
	InitScriptCategories( pMachine, pCategoryTable );

	// Init the buttons table.
	gmTableObject *pButtonsTable = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "BTN", gmVariable( pButtonsTable ) );
	InitScriptBotButtons( pMachine, pButtonsTable );

	// Init the trace masks table.
	gmTableObject *pTraceMasks = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "TRACE", gmVariable( pTraceMasks ) );
	InitScriptTraceMasks( pMachine, pTraceMasks );

	// Register Voice queues.
	gmTableObject *pVoiceTable = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "VOICE", gmVariable( pVoiceTable ) );
	InitVoiceMacros( pMachine, pVoiceTable );

	// Register Debug Flags
	gmTableObject *pDebugTable = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "DEBUG", gmVariable( pDebugTable ) );
	InitDebugFlags( pMachine, pDebugTable );

	// Register contents flags
	gmTableObject *pContentTable = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "CONTENT", gmVariable( pContentTable ) );
	InitScriptContentFlags( pMachine, pContentTable );

	gmTableObject *pBlackboardTable = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "BB", gmVariable( pBlackboardTable ) );
	InitScriptBlackboardKeys( pMachine, pBlackboardTable );

	gmTableObject *pBoneTable = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "BONE", gmVariable( pBoneTable ) );
	InitBoneIds( pMachine, pBoneTable );

	gmTableObject *pBuyTable = pMachine->AllocTableObject();
	pMachine->GetGlobals()->Set( pMachine, "BUY", gmVariable( pBuyTable ) );
	InitScriptBuyMenu( pMachine, pBuyTable );

	// Register Profile Types
	gmTableObject *pProfileTable = pMachine->AllocTableObject();
	pProfileTable->Set( pMachine, "NONE", gmVariable( Client::PROFILE_NONE ) );
	pProfileTable->Set( pMachine, "CLASS", gmVariable( Client::PROFILE_CLASS ) );
	pProfileTable->Set( pMachine, "CUSTOM", gmVariable( Client::PROFILE_CUSTOM ) );
	pMachine->GetGlobals()->Set( pMachine, "PROFILE", gmVariable( pProfileTable ) );

	LOG( "done." );
}

bool IGame::FindTeamName( std::string& teamName, int teamId )
{
	int NumElements = 0;
	const IntEnum *Enum = 0;
	GetTeamEnumeration( Enum, NumElements );

	for ( int i = 0; i < NumElements; ++i )
	{
		if ( Enum[ i ].mValue == teamId )
		{
			teamName = Enum[ i ].mKey;
			return true;
		}
	}
	return false;
}


void IGame::InitScriptTeams( gmMachine *_machine, gmTableObject *_table )
{
	int NumElements = 0;
	const IntEnum *Enum = 0;
	GetTeamEnumeration( Enum, NumElements );

	for ( int i = 0; i < NumElements; ++i )
	{
		_table->Set( _machine, Enum[ i ].mKey, gmVariable( Enum[ i ].mValue ) );
	}
}

void IGame::InitScriptWeapons( gmMachine *_machine, gmTableObject *_table )
{
	int NumElements = 0;
	const IntEnum *Enum = 0;
	GetWeaponEnumeration( Enum, NumElements );

	for ( int i = 0; i < NumElements; ++i )
	{
		_table->Set( _machine, Enum[ i ].mKey, gmVariable( Enum[ i ].mValue ) );
	}
}

void IGame::InitScriptRoles( gmMachine *_machine, gmTableObject *_table )
{
	int NumElements = 0;
	const IntEnum *Enum = 0;
	GetRoleEnumeration( Enum, NumElements );

	for ( int i = 0; i < NumElements; ++i )
	{
		_table->Set( _machine, Enum[ i ].mKey, gmVariable( Enum[ i ].mValue ) );
	}
}

static const IntEnum Generic_RoleEnum [] =
{
	IntEnum( "ATTACKER", 0 ),
	IntEnum( "DEFENDER", 1 ),
	IntEnum( "ROAMER", 2 ),
	IntEnum( "INFILTRATOR", 3 ),
	IntEnum( "SNIPER", 4 ),
	IntEnum( "AMBUSHER", 5 ),
	IntEnum( "TEAMCAPTAIN", 6 ),
	IntEnum( "FIRETEAMCAPTAIN", 7 ),
	IntEnum( "OFFENSECAPTAIN", 8 ),
	IntEnum( "DEFENSECAPTAIN", 9 ),
	IntEnum( "DEFENDER1", 10 ),
	IntEnum( "DEFENDER2", 11 ),
	IntEnum( "DEFENDER3", 12 ),
	IntEnum( "ATTACKER1", 13 ),
	IntEnum( "ATTACKER2", 14 ),
	IntEnum( "ATTACKER3", 15 ),
	IntEnum( "ESCORT", 16 ),
};

void IGame::GetRoleEnumeration( const IntEnum *&ptr, int &num )
{
	num = sizeof( Generic_RoleEnum ) / sizeof( Generic_RoleEnum[ 0 ] );
	ptr = Generic_RoleEnum;
}

// package: Common Script Events
//		Common Events for Every Game.
void IGame::InitScriptEvents( gmMachine *_machine, gmTableObject *_table )
{
	_table->Set( _machine, "DISCONNECTED", gmVariable( MSG_CLIENTDISCONNECTED ) );
	_table->Set( _machine, "START_GAME", gmVariable( MSG_STARTGAME ) );
	_table->Set( _machine, "END_GAME", gmVariable( MSG_ENDGAME ) );
	_table->Set( _machine, "NEW_ROUND", gmVariable( MSG_NEWROUND ) );
	_table->Set( _machine, "END_ROUND", gmVariable( MSG_ENDROUND ) );
	_table->Set( _machine, "GOAL_SUCCESS", gmVariable( MSG_GOAL_SUCCESS ) );
	_table->Set( _machine, "GOAL_FAILED", gmVariable( MSG_GOAL_FAILED ) );
	_table->Set( _machine, "GOAL_ABORTED", gmVariable( MSG_GOAL_ABORTED ) );
	_table->Set( _machine, "PATH_SUCCESS", gmVariable( MSG_PATH_SUCCESS ) );
	_table->Set( _machine, "PATH_FAILED", gmVariable( MSG_PATH_FAILED ) );
	_table->Set( _machine, "SCRIPTMSG", gmVariable( MSG_SCRIPTMSG ) );
	_table->Set( _machine, "SPAWNED", gmVariable( MSG_SPAWN ) );
	_table->Set( _machine, "CHANGETEAM", gmVariable( MSG_CHANGETEAM ) );
	_table->Set( _machine, "CHANGECLASS", gmVariable( MSG_CHANGECLASS ) );
	_table->Set( _machine, "INVALID_TEAM", gmVariable( MSG_INVALIDTEAM ) );
	_table->Set( _machine, "INVALID_CLASS", gmVariable( MSG_INVALIDCLASS ) );
	_table->Set( _machine, "DEATH", gmVariable( MSG_DEATH ) );
	_table->Set( _machine, "HEALED", gmVariable( MSG_HEALED ) );
	_table->Set( _machine, "REVIVED", gmVariable( MSG_REVIVED ) );
	_table->Set( _machine, "ADDWEAPON", gmVariable( MSG_ADDWEAPON ) );
	_table->Set( _machine, "REMOVEWEAPON", gmVariable( MSG_REMOVEWEAPON ) );
	_table->Set( _machine, "REFRESH_WEAPON", gmVariable( MSG_REFRESHWEAPON ) );
	_table->Set( _machine, "KILLEDSOMEONE", gmVariable( MSG_KILLEDSOMEONE ) );
	_table->Set( _machine, "PROXIMITY_TRIGGER", gmVariable( MSG_PROXIMITY_TRIGGER ) );
	_table->Set( _machine, "ENT_ENTER_RADIUS", gmVariable( MSG_ENT_ENTER_RADIUS ) );
	_table->Set( _machine, "ENT_LEAVE_RADIUS", gmVariable( MSG_ENT_LEAVE_RADIUS ) );
	_table->Set( _machine, "MG_ENTER_RADIUS", gmVariable( MSG_MG_ENTER_RADIUS ) );
	_table->Set( _machine, "MG_LEAVE_RADIUS", gmVariable( MSG_MG_LEAVE_RADIUS ) );
	_table->Set( _machine, "PLAYER_USE", gmVariable( MSG_FEEL_PLAYER_USE ) );
	_table->Set( _machine, "TOOK_DAMAGE", gmVariable( MSG_TOOK_DAMAGE ) );
	_table->Set( _machine, "SENSE_ENTITY", gmVariable( MSG_SENSE_ENTITY ) );
	_table->Set( _machine, "WEAPON_FIRE", gmVariable( MSG_WEAPON_FIRE ) );
	_table->Set( _machine, "WEAPON_CHANGE", gmVariable( MSG_WEAPON_CHANGE ) );
	_table->Set( _machine, "VOICE_GLOBAL", gmVariable( MSG_HEAR_VOICEMACRO_GLOBAL ) );
	_table->Set( _machine, "VOICE_TEAM", gmVariable( MSG_HEAR_VOICEMACRO_TEAM ) );
	_table->Set( _machine, "VOICE_PRIVATE", gmVariable( MSG_HEAR_VOICEMACRO_PRIVATE ) );
	_table->Set( _machine, "VOICE_GROUP", gmVariable( MSG_HEAR_VOICEMACRO_GROUP ) );
	_table->Set( _machine, "CHATMSG_GLOBAL", gmVariable( MSG_HEAR_CHATMSG_GLOBAL ) );
	_table->Set( _machine, "CHATMSG_TEAM", gmVariable( MSG_HEAR_CHATMSG_TEAM ) );
	_table->Set( _machine, "CHATMSG_PRIVATE", gmVariable( MSG_HEAR_CHATMSG_PRIVATE ) );
	_table->Set( _machine, "CHATMSG_GROUP", gmVariable( MSG_HEAR_CHATMSG_GROUP ) );
	_table->Set( _machine, "HEAR_SOUND", gmVariable( MSG_HEAR_SOUND ) );
	_table->Set( _machine, "MINE_ARMED", gmVariable( MSG_MINE_ARMED ) );
	_table->Set( _machine, "MINE_DETONATE", gmVariable( MSG_MINE_DETONATE ) );
	_table->Set( _machine, "MORTAR_IMPACT", gmVariable( MSG_MORTAR_IMPACT ) );
	_table->Set( _machine, "DROWNING", gmVariable( MSG_DROWNING ) );
}

void IGame::InitScriptCategories( gmMachine *_machine, gmTableObject *_table )
{
	_table->Set( _machine, "PLAYER", gmVariable( ENT_CAT_PLAYER ) );
	_table->Set( _machine, "VEHICLE", gmVariable( ENT_CAT_VEHICLE ) );
	_table->Set( _machine, "PROJECTILE", gmVariable( ENT_CAT_PROJECTILE ) );
	_table->Set( _machine, "SHOOTABLE", gmVariable( ENT_CAT_SHOOTABLE ) );
	
	_table->Set( _machine, "TRIGGER", gmVariable( ENT_CAT_TRIGGER ) );
	_table->Set( _machine, "MOVER", gmVariable( ENT_CAT_MOVER ) );
	_table->Set( _machine, "MOUNTED_WPN", gmVariable( ENT_CAT_MOUNTEDWEAPON ) );
	_table->Set( _machine, "MISC", gmVariable( ENT_CAT_MISC ) );
	_table->Set( _machine, "NOLOS", gmVariable( ENT_CAT_NOLOS ) );
	_table->Set( _machine, "PROP", gmVariable( ENT_CAT_PROP ) );
	_table->Set( _machine, "AUTODEFENSE", gmVariable( ENT_CAT_AUTODEFENSE ) );
	_table->Set( _machine, "OBSTACLE", gmVariable( ENT_CAT_OBSTACLE ) );
	_table->Set( _machine, "PROP_PUSHABLE", gmVariable( ENT_CAT_PROP_PUSHABLE ) );
	_table->Set( _machine, "BUILDABLE", gmVariable( ENT_CAT_BUILDABLE ) );	
}

static const IntEnum gGroupMapping [] =
{
	IntEnum( "SPECTATOR", ENT_GRP_SPECTATOR ),
	IntEnum( "PLAYER", ENT_GRP_PLAYER ),
	IntEnum( "PLAYERSTART", ENT_GRP_PLAYERSTART ),
	IntEnum( "BUTTON", ENT_GRP_BUTTON ),
	IntEnum( "RESUPPLY", ENT_GRP_RESUPPLY ),
	IntEnum( "LADDER", ENT_GRP_LADDER ),
	IntEnum( "TELEPORTER", ENT_GRP_TELEPORTER ),
	IntEnum( "LIFT", ENT_GRP_LIFT ),
	IntEnum( "MOVER", ENT_GRP_MOVER ),
	IntEnum( "JUMPPAD", ENT_GRP_JUMPPAD ),
	IntEnum( "JUMPPAD_TARGET", ENT_GRP_JUMPPAD_TARGET ),
	IntEnum( "GOAL", ENT_GRP_GOAL ),
	IntEnum( "WEAPON", ENT_GRP_WEAPON ),
	IntEnum( "FLAG", ENT_GRP_FLAG ),
	IntEnum( "CAPPOINT", ENT_GRP_FLAGCAPPOINT ),
	IntEnum( "CONTROLPOINT", ENT_GRP_CONTROLPOINT ),
	IntEnum( "PROP", ENT_GRP_PROP ),
	IntEnum( "PROP_STATIC", ENT_GRP_PROP_STATIC ),
	IntEnum( "PROP_EXPLODE", ENT_GRP_PROP_EXPLODE ),
	IntEnum( "PROP_BREAKABLE", ENT_GRP_PROP_BREAKABLE ),
	IntEnum( "MONSTER", ENT_GRP_MONSTER ),
	IntEnum( "PROJECTILE", ENT_GRP_PROJECTILE ),
	IntEnum( "POWERUP", ENT_GRP_POWERUP ),
};

static const IntEnum gClassMapping [] =
{
	IntEnum( "INVALID", ENT_CLASS_NONE ),
};

bool IGame::FindClassName( std::string& groupName, std::string& className, const EntityGroup group, int classId )
{
	const size_t iNumMappings = sizeof( gGroupMapping ) / sizeof( gGroupMapping[ 0 ] );
	for ( int i = 0; i < iNumMappings; ++i )
	{
		if ( gGroupMapping[ i ].mValue == group )
		{
			groupName = gGroupMapping[ i ].mKey;
			break;
		}
	}

	if ( group == ENT_GRP_WEAPON )
	{
		int numWpns = 0;
		const IntEnum *wpnEnum = 0;
		GetWeaponEnumeration( wpnEnum, numWpns );
		for ( int i = 0; i < numWpns; ++i )
		{
			if ( classId == wpnEnum[ i ].mValue )
			{
				className = wpnEnum[ i ].mKey;
				return true;
			}
		}
	}
	return false;
}

void IGame::InitScriptGroups( gmMachine *_machine, gmTableObject *_table )
{
	const size_t count = sizeof( gGroupMapping ) / sizeof( gGroupMapping[ 0 ] );
	for ( int i = 0; i < count; ++i )
	{
		_table->Set( _machine, gGroupMapping[ i ].mKey, gmVariable( gGroupMapping[ i ].mValue ) );
	}
}

void IGame::InitScriptClasses( gmMachine *_machine, gmTableObject *_table )
{
	// register base classes
	const size_t count = sizeof( gClassMapping ) / sizeof( gClassMapping[ 0 ] );
	for ( int i = 0; i < count; ++i )
	{
		_table->Set( _machine, gClassMapping[ i ].mKey, gmVariable( gClassMapping[ i ].mValue ) );
	}
}

void IGame::InitScriptWeaponClasses( gmMachine *_machine, gmTableObject *_table )
{
	int numWpns = 0;
	const IntEnum *wpnEnum = 0;
	GetWeaponEnumeration( wpnEnum, numWpns );
	for ( int i = 0; i < numWpns; ++i )
	{
		_table->Set( _machine, wpnEnum[ i ].mKey, gmVariable( wpnEnum[ i ].mValue ) );
	}
}

void IGame::InitScriptSkills( gmMachine *_machine, gmTableObject *_table )
{
}

void IGame::InitScriptEntityFlags( gmMachine *_machine, gmTableObject *_table )
{
	_table->Set( _machine, "TEAM1", gmVariable( ENT_FLAG_TEAM1 ) );
	_table->Set( _machine, "TEAM2", gmVariable( ENT_FLAG_TEAM2 ) );
	_table->Set( _machine, "TEAM3", gmVariable( ENT_FLAG_TEAM3 ) );
	_table->Set( _machine, "TEAM4", gmVariable( ENT_FLAG_TEAM4 ) );
	_table->Set( _machine, "VISTEST", gmVariable( ENT_FLAG_VISTEST ) );
	_table->Set( _machine, "DISABLED", gmVariable( ENT_FLAG_DISABLED ) );
	_table->Set( _machine, "PRONE", gmVariable( ENT_FLAG_PRONED ) );
	_table->Set( _machine, "CROUCHED", gmVariable( ENT_FLAG_CROUCHED ) );
	_table->Set( _machine, "CARRYABLE", gmVariable( ENT_FLAG_CARRYABLE ) );
	_table->Set( _machine, "DEAD", gmVariable( ENT_FLAG_DEAD ) );
	_table->Set( _machine, "INWATER", gmVariable( ENT_FLAG_INWATER ) );
	_table->Set( _machine, "UNDERWATER", gmVariable( ENT_FLAG_UNDERWATER ) );
	_table->Set( _machine, "ZOOMING", gmVariable( ENT_FLAG_ZOOMING ) );
	_table->Set( _machine, "ON_LADDER", gmVariable( ENT_FLAG_ONLADDER ) );
	_table->Set( _machine, "ON_GROUND", gmVariable( ENT_FLAG_ONGROUND ) );
	_table->Set( _machine, "RELOADING", gmVariable( ENT_FLAG_RELOADING ) );
	_table->Set( _machine, "HUMANCONTROLLED", gmVariable( ENT_FLAG_HUMANCONTROLLED ) );
	_table->Set( _machine, "IRONSIGHT", gmVariable( ENT_FLAG_IRONSIGHT ) );
	_table->Set( _machine, "ON_ICE", gmVariable( ENT_FLAG_ON_ICE ) );
	_table->Set( _machine, "IN_VEHICLE", gmVariable( ENT_FLAG_INVEHICLE ) );
	_table->Set( _machine, "FROZEN", gmVariable( ENT_FLAG_FROZEN ) );
	_table->Set( _machine, "TAUNTING", gmVariable( ENT_FLAG_TAUNTING ) );
	_table->Set( _machine, "AIMING", gmVariable( ENT_FLAG_AIMING ) );
}

void IGame::InitScriptPowerups( gmMachine *_machine, gmTableObject *_table )
{
	_table->Set( _machine, "INVINCIBLE", gmVariable( PWR_INVINCIBLE ) );
}

void IGame::InitScriptBotButtons( gmMachine *_machine, gmTableObject *_table )
{
	_table->Set( _machine, "ATTACK1", gmVariable( BOT_BUTTON_ATTACK1 ) );
	_table->Set( _machine, "ATTACK2", gmVariable( BOT_BUTTON_ATTACK2 ) );
	_table->Set( _machine, "JUMP", gmVariable( BOT_BUTTON_JUMP ) );
	_table->Set( _machine, "CROUCH", gmVariable( BOT_BUTTON_CROUCH ) );
	_table->Set( _machine, "PRONE", gmVariable( BOT_BUTTON_PRONE ) );
	_table->Set( _machine, "WALK", gmVariable( BOT_BUTTON_WALK ) );
	_table->Set( _machine, "USE", gmVariable( BOT_BUTTON_USE ) );
	_table->Set( _machine, "FORWARD", gmVariable( BOT_BUTTON_FWD ) );
	_table->Set( _machine, "BACKWARD", gmVariable( BOT_BUTTON_BACK ) );
	_table->Set( _machine, "STRAFE_R", gmVariable( BOT_BUTTON_RSTRAFE ) );
	_table->Set( _machine, "STRAFE_L", gmVariable( BOT_BUTTON_LSTRAFE ) );
	_table->Set( _machine, "RELOAD", gmVariable( BOT_BUTTON_RELOAD ) );
	_table->Set( _machine, "SPRINT", gmVariable( BOT_BUTTON_SPRINT ) );
	_table->Set( _machine, "DROP", gmVariable( BOT_BUTTON_DROP ) );
	_table->Set( _machine, "LEAN_L", gmVariable( BOT_BUTTON_LEANLEFT ) );
	_table->Set( _machine, "LEAN_R", gmVariable( BOT_BUTTON_LEANRIGHT ) );
	_table->Set( _machine, "AIM", gmVariable( BOT_BUTTON_AIM ) );
	_table->Set( _machine, "RESPAWN", gmVariable( BOT_BUTTON_RESPAWN ) );
	_table->Set( _machine, "TAUNT", gmVariable( BOT_BUTTON_TAUNT ) );
}

void IGame::InitScriptTraceMasks( gmMachine *_machine, gmTableObject *_table )
{
	_table->Set( _machine, "ALL", gmVariable( TR_MASK_ALL ) );
	_table->Set( _machine, "SOLID", gmVariable( TR_MASK_SOLID ) );
	_table->Set( _machine, "PLAYER", gmVariable( TR_MASK_PLAYER ) );
	_table->Set( _machine, "SHOT", gmVariable( TR_MASK_SHOT ) );
	_table->Set( _machine, "OPAQUE", gmVariable( TR_MASK_OPAQUE ) );
	_table->Set( _machine, "WATER", gmVariable( TR_MASK_WATER ) );
	_table->Set( _machine, "GRATE", gmVariable( TR_MASK_GRATE ) );
	_table->Set( _machine, "PLAYERCLIP", gmVariable( TR_MASK_PLAYERCLIP ) );
	_table->Set( _machine, "SMOKEBOMB", gmVariable( TR_MASK_SMOKEBOMB ) );
	_table->Set( _machine, "FLOODFILL", gmVariable( TR_MASK_FLOODFILL ) );
	_table->Set( _machine, "FLOODFILLENT", gmVariable( TR_MASK_FLOODFILLENT ) );
	_table->Set( _machine, "VISIBLE", gmVariable( TR_MASK_VISIBLE ) );
}

void IGame::InitScriptContentFlags( gmMachine *_machine, gmTableObject *_table )
{
	_table->Set( _machine, "SOLID", gmVariable( CONT_SOLID ) );
	_table->Set( _machine, "WATER", gmVariable( CONT_WATER ) );
	_table->Set( _machine, "SLIME", gmVariable( CONT_SLIME ) );
	_table->Set( _machine, "FOG", gmVariable( CONT_FOG ) );

	_table->Set( _machine, "TELEPORTER", gmVariable( CONT_TELEPORTER ) );
	_table->Set( _machine, "MOVER", gmVariable( CONT_MOVER ) );
	_table->Set( _machine, "TRIGGER", gmVariable( CONT_TRIGGER ) );
	_table->Set( _machine, "LAVA", gmVariable( CONT_LAVA ) );
}

void IGame::InitScriptBlackboardKeys( gmMachine *_machine, gmTableObject *_table )
{
	_table->Set( _machine, "DELAY_GOAL", gmVariable( bbk_DelayGoal ) );
	_table->Set( _machine, "IS_TAKEN", gmVariable( bbk_IsTaken ) );
	_table->Set( _machine, "RUN_AWAY", gmVariable( bbk_RunAway ) );
}

void IGame::InitDebugFlags( gmMachine *_machine, gmTableObject *_table )
{
	_table->Set( _machine, "LOG", gmVariable( BOT_DEBUG_LOG ) );
	_table->Set( _machine, "MOVEMENT", gmVariable( BOT_DEBUG_MOVEVEC ) );
	_table->Set( _machine, "SCRIPT", gmVariable( BOT_DEBUG_SCRIPT ) );
	_table->Set( _machine, "PLANNER", gmVariable( BOT_DEBUG_PLANNER ) );
	_table->Set( _machine, "FAILED_PATHS", gmVariable( BOT_DEBUG_LOG_FAILED_PATHS ) );

	/*BOT_DEBUG_LOG = 0,
	BOT_DEBUG_MOVEVEC,
	BOT_DEBUG_SCRIPT,
	BOT_DEBUG_FPINFO,
	BOT_DEBUG_PLANNER,
	BOT_DEBUG_EVENTS,
	BOT_DEBUG_LOG_FAILED_PATHS,*/
}

void IGame::InitBoneIds( gmMachine *_machine, gmTableObject *_table )
{
	_table->Set( _machine, "TORSO", gmVariable( BONE_TORSO ) );
	_table->Set( _machine, "PELVIS", gmVariable( BONE_PELVIS ) );
	_table->Set( _machine, "HEAD", gmVariable( BONE_HEAD ) );
	_table->Set( _machine, "RIGHTARM", gmVariable( BONE_RIGHTARM ) );
	_table->Set( _machine, "LEFTARM", gmVariable( BONE_LEFTARM ) );
	_table->Set( _machine, "RIGHTHAND", gmVariable( BONE_RIGHTHAND ) );
	_table->Set( _machine, "LEFTHAND", gmVariable( BONE_LEFTHAND ) );
	_table->Set( _machine, "RIGHTLEG", gmVariable( BONE_RIGHTLEG ) );
	_table->Set( _machine, "LEFTLEG", gmVariable( BONE_LEFTLEG ) );
	_table->Set( _machine, "RIGHTFOOT", gmVariable( BONE_RIGHTFOOT ) );
	_table->Set( _machine, "LEFTFOOT", gmVariable( BONE_LEFTFOOT ) );
}

void IGame::UpdateGame( System & system )
{
	rmt_ScopedCPUSample( GameUpdate );

	CheckGameState();

	gEngineFuncs->GetGravity( mGravity );
	mCheatsEnabled = gEngineFuncs->AreCheatsEnabled();

	if ( mGameFrame > 0 )
	{
		if ( mSettingLimiter && mSettingLimiter->IsReady() )
			CheckServerSettings( true );
		else if ( mPlayersChanged )
			CheckServerSettings( false );
		mPlayersChanged = false;
	}

	// Check if we're dead or alive so we know what update function to call.
	if ( mStateRoot )
	{
		mStateRoot->RootUpdate();
	}

	// This is called often to update the state of the bots and perform their "thinking"
	{
		rmt_ScopedCPUSample( UpdateClients );

		for ( int i = 0; i < Constants::MAX_PLAYERS; ++i )
		{
			if ( mClientList[ i ] )
			{
				mClientList[ i ]->Update();
			}
		}
	}

	PropogateDeletedThreads();

	gBlackboard.PurgeExpiredRecords();

	UpdateProcesses();

	// Increment the game frame.
	++mGameFrame;
}

void IGame::UpdateProcesses()
{
	rmt_ScopedCPUSample( IGameUpdateProcesses );

	FunctorMap::iterator it = mUpdateMap.begin();
	for ( ; it != mUpdateMap.end(); )
	{
		if ( ( *( *it ).second )( ) == Function_Finished )
		{
			EngineFuncs::ConsoleMessage( va( "Finished Process: %s", ( *it ).first.c_str() ).c_str() );
			mUpdateMap.erase( it++ );
		}
		else
			++it;
	}
}

#ifdef ENABLE_REMOTE_DEBUGGING
void IGame::UpdateSync( RemoteLib::DebugConnection * connection, Remote::Game & cached, Remote::Game & update ) {
	SET_IF_DIFF( cached, update, RemoteConfigName(), name );

	// draw the entities registered with the system
	IGame::EntityIterator ent;
	while( IGame::IterateEntity( ent ) ) {
		// make sure there's enough room for the entity at this index
		if ( cached.entities().Capacity() <= ent.GetIndex() ) {
			cached.mutable_entities()->Reserve( ent.GetIndex() + 1 );
		}

		while ( cached.entities_size() <= ent.GetIndex() ) {
			cached.add_entities();
		}

		if ( update.entities().Capacity() <= ent.GetIndex() ) {
			update.mutable_entities()->Reserve( ent.GetIndex() + 1 );
		}

		while ( update.entities_size() <= ent.GetIndex() ) {
			update.add_entities();
		}

		Remote::Entity * cachedEntity = cached.mutable_entities( ent.GetIndex() );
		Remote::Entity * entityUpdate = update.mutable_entities( ent.GetIndex() );

		entityUpdate->Clear();

		SyncEntity( ent.GetEnt(), connection, *cachedEntity, *entityUpdate );

		using namespace google::protobuf;

		std::vector<const FieldDescriptor*> fieldList;
		entityUpdate->GetReflection()->ListFields( *entityUpdate, &fieldList );

		if ( fieldList.size() > 0 ) {
			entityUpdate->set_uid( ent.GetEnt().mEntity.AsInt() );

			const int bufferSize = 32;
			char buffer[ bufferSize ];
			io::ArrayOutputStream arrayStream( buffer, bufferSize );
			io::CodedOutputStream codedStream( &arrayStream );

			std::string serialized;

			const Remote::PayloadFormat msgformat = Remote::PAYLOAD_TEXT;
			if ( msgformat == Remote::PAYLOAD_TEXT ) {
				TextFormat::PrintToString( *entityUpdate, &serialized );
			} else {
				entityUpdate->SerializeToString( &serialized );
			}

			Remote::Packet packet;
			packet.set_packettype( Remote::PACKET_ENTITY );
			packet.set_payloadformat( msgformat );
			packet.set_payloadsize( serialized.size() );
			packet.set_payloaddata( serialized.c_str(), serialized.size() );

			std::string serializedPacket;
			packet.SerializeToString( &serializedPacket );

			codedStream.WriteVarint32( serializedPacket.size() );

			connection->getSendBuffer().beginWrite( RemoteLib::DataBuffer::WriteModeAllOrNone );
			connection->getSendBuffer().write( buffer, codedStream.ByteCount() );
			connection->getSendBuffer().write( serializedPacket.c_str(), serializedPacket.size() );

			if ( connection->getSendBuffer().endWrite() == 0 ) {
				// TEMP, this wont work right when repeated fields are in use
				cachedEntity->MergeFrom( *entityUpdate );
			}
		}
	}
}
void IGame::SyncEntity( const EntityInstance & ent, RemoteLib::DebugConnection * connection, Remote::Entity & cachedEntity, Remote::Entity & entityUpdate ) {
	//////////////////////////////////////////////////////////////////////////
	// check for values that have changed
	{
		Vector3f entPosition = Vector3f::ZERO,
			facingVector = Vector3f::ZERO,
			rightVector = Vector3f::ZERO,
			upVector = Vector3f::ZERO;

		Msg_HealthArmor hlth;
		EngineFuncs::EntityPosition( ent.mEntity, entPosition );
		const int entClass = gEngineFuncs->GetEntityClass( ent.mEntity );
		const int entTeam = gEngineFuncs->GetEntityTeam( ent.mEntity );
		const std::string entName = EngineFuncs::EntityName( ent.mEntity );

		EngineFuncs::EntityOrientation( ent.mEntity, facingVector, rightVector, upVector );

		SET_IF_DIFF( cachedEntity, entityUpdate, entName.c_str(), name );
		SET_IF_DIFF( cachedEntity.position(), *entityUpdate.mutable_position(), entPosition.X(), x );
		SET_IF_DIFF( cachedEntity.position(), *entityUpdate.mutable_position(), entPosition.Y(), y );
		SET_IF_DIFF( cachedEntity.position(), *entityUpdate.mutable_position(), entPosition.Z(), z );
		SET_IF_DIFF( cachedEntity.facing(), *entityUpdate.mutable_facing(), facingVector.X(), x );
		SET_IF_DIFF( cachedEntity.facing(), *entityUpdate.mutable_facing(), facingVector.Y(), y );
		SET_IF_DIFF( cachedEntity.facing(), *entityUpdate.mutable_facing(), facingVector.Z(), z );
		SET_IF_DIFF( cachedEntity, entityUpdate, entClass, classid );
		SET_IF_DIFF( cachedEntity, entityUpdate, entTeam, teamid );

		if ( gEngineFuncs->GetHealthAndArmor( ent.mEntity, hlth ) ) {
			SET_IF_DIFF( cachedEntity, entityUpdate, hlth.mCurrentHealth, health );
			SET_IF_DIFF( cachedEntity, entityUpdate, hlth.mMaxHealth, healthmax );
			SET_IF_DIFF( cachedEntity, entityUpdate, hlth.mCurrentArmor, armor );
			SET_IF_DIFF( cachedEntity, entityUpdate, hlth.mMaxArmor, armormax );
		}

		Box3f worldBounds;
		if ( EngineFuncs::EntityWorldOBB( ent.mEntity, worldBounds ) ) {
			SET_IF_DIFF( cachedEntity.size(), *entityUpdate.mutable_size(), worldBounds.Extent[0], x );
			SET_IF_DIFF( cachedEntity.size(), *entityUpdate.mutable_size(), worldBounds.Extent[1], y );
			SET_IF_DIFF( cachedEntity.size(), *entityUpdate.mutable_size(), worldBounds.Extent[2], z );
		}

		ClientPtr bot = GetClientByIndex( ent.mEntity.GetIndex() );
		if ( bot ) {
			bot->InternalSyncEntity( connection, cachedEntity, entityUpdate );
		}
	}
}
void IGame::InternalSyncEntity( const EntityInstance & ent, RemoteLib::DebugConnection * connection ) {
	// for derived classes
}
#endif

void IGame::ProcessEvent( const Message &message, CallbackParameters &cb )
{
	switch ( message.Id() )
	{
		CASE_MSG( EvGameStart )
		{
			StartGame();
			mLastGameState = mGameState = GAME_STATE_WAITINGFORPLAYERS;
			break;
		}
		CASE_MSG( EvGameEnd )
		{
			EndGame();
			break;
		}
		CASE_MSG( EvClientConnected )
		{
			ClientJoined( msg );
			mPlayersChanged = true;
			break;
		}
		CASE_MSG( EvClientDisconnected )
		{
			ClientLeft( msg );
			mPlayersChanged = true;
			break;
		}
		CASE_MSG( EvEntityCreated )
		{
			const EvEntityCreated* m = EvEntityCreated::Msg::Cast( message );
			if ( m )
			{
				const int index = m->mEntity.GetIndex();

				// special case don't replace existing entities with goal entities
				if ( mGameEntities[ index ].mEntity.IsValid() && m->mEntityInfo.mGroup == ENT_GRP_GOAL )
				{
					break;
				}
				
				if ( m->mEntityInfo.mCategory.CheckFlag( ENT_CAT_OBSTACLE ) )
				{
					//obstacleManager.AddObstacle( m->mEntity );
				}

				assert( m->mEntity.GetIndex() >= 0 && m->mEntity.GetIndex() < Constants::MAX_ENTITIES );
				mGameEntities[ index ].mEntity = m->mEntity;
				mGameEntities[ index ].mEntInfo = m->mEntityInfo;
				mGameEntities[ index ].mTimeStamp = IGame::GetTime();

				System::mInstance->mGoalManager->EntityCreated( mGameEntities[ index ] );
				System::mInstance->mTacticalManager->EntityCreated( mGameEntities[ index ] );
				System::mInstance->mNavigation->EntityCreated( mGameEntities[ index ] );

#ifdef _DEBUG
				std::string groupName, className;
				FindClassName( groupName, className, m->mEntityInfo.mGroup, m->mEntityInfo.mClassId );
				Utils::OutputDebug( kNormal, "Entity: %d created: (%s:%s)\n", index, groupName.c_str(), className.c_str() );
#endif

				// increase the upper limit if necessary.
				if ( mMaxEntity <= index )
					mMaxEntity = index + 1;
			}
			break;
		}
		CASE_MSG( EvEntityDeleted )
		{
			int index = msg->mEntity.GetIndex();
			if ( mGameEntities[ index ].mEntity.IsValid() )
			{
#ifdef _DEBUG
				std::string groupName, className;
				FindClassName( groupName, className, mGameEntities[ index ].mEntInfo.mGroup, mGameEntities[ index ].mEntInfo.mClassId );
				Utils::OutputDebug( kNormal, "Entity: %d deleted: (%s:%s)\n", index, groupName.c_str(), className.c_str() );
#endif
				System::mInstance->mGoalManager->EntityDeleted( mGameEntities[ index ] );
				System::mInstance->mTacticalManager->EntityDeleted( mGameEntities[ index ] );
				System::mInstance->mNavigation->EntityDeleted( mGameEntities[ index ] );

				mGameEntities[ index ].mEntity.Reset();
				mGameEntities[ index ].mEntInfo = EntityInfo();
				mGameEntities[ index ].mTimeStamp = 0;

				// decrease the upper limit if necessary.
				if ( mMaxEntity == index + 1 )
				{
					do
					{
						mMaxEntity--;
					}
					while ( mMaxEntity > 0 && !mGameEntities[ mMaxEntity - 1 ].mEntity.IsValid() );
				}
			}

			System::mInstance->mGoalManager->RemoveGoalByEntity( msg->mEntity );
			//////////////////////////////////////////////////////////////////////////
			if ( System::mInstance->mNavigation )
			{
				System::mInstance->mNavigation->RemoveEntityConnection( msg->mEntity );
			}
			break;
		}
		case MSG_NEWROUND:
		{
			NewRound();
			break;
		}
		case MSG_START_TRAINING:
		{
			StartTraining();
			break;
		}
		CASE_MSG( EvScriptSignal )
		{
			gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
			if ( msg->mSignalName[ 0 ] )
				pMachine->Signal( gmVariable( pMachine->AllocStringObject( msg->mSignalName ) ), GM_INVALID_THREAD, GM_INVALID_THREAD );
		}
		case MSG_SOUND:
		{
			/*const Event_Sound *m = message.Get<Event_Sound>();
			if(m)
			g_SoundDepot.Post(*m);*/
			break;
		}
		CASE_MSG( EvEntityConnection )
		{
			if ( System::mInstance->mNavigation )
			{
				System::mInstance->mNavigation->AddEntityConnection( *msg );
			}
			break;
		}
		CASE_MSG( EvAnalyticEvent )
		{
			/*if ( System::mInstance->mAnalytics )
			{
			if ( m->mHasPosition )
			System::mInstance->mAnalytics->AddGameEvent( m->mArea, m->mName, m->mPosition );
			else
			System::mInstance->mAnalytics->AddGameEvent( m->mArea, m->mName );
			}*/
			break;
		}
	}
}

void IGame::DispatchEvent( const Message &message, GameEntity dest )
{
	// some messages we don't want to forward to clients
	switch ( message.Id() )
	{
		CASE_MSG( EvScriptChanged )
		{
			gWeaponDatabase.ReloadScript( (LiveUpdateKey)msg->mScriptKey );
			return;
		}
		CASE_MSG( EvEntityCreated )
		{
			break;
		}
		CASE_MSG( EvEntityDeleted )
		{
			break;
		}
		CASE_MSG( EvWeaponFire )
		{	
			if ( System::mInstance->mAnalytics )
			{
				Vector3f pos;
				if ( EngineFuncs::EntityPosition( dest, pos ) )
				{
					Analytics::MessageUnion m;
					m.set_timestamp( IGame::GetTime() );

					Analytics::GameWeaponFired* event = m.mutable_gameweaponfired();
					FIELD_SET( event, weaponid, msg->mWeaponId );
					SMART_FIELD_SET( event, team, msg->mTeamNum );
					SMART_FIELD_SET( event, positionx, pos.X() );
					SMART_FIELD_SET( event, positiony, pos.Y() );
					SMART_FIELD_SET( event, positionz, pos.Z() );

					System::mInstance->mAnalytics->AddEvent( m );
				}
			}
			break;
		}
		default:
		{
			// Send this event to everyone.
			for ( int i = 0; i < Constants::MAX_PLAYERS; ++i )
			{
				if ( mClientList[ i ] )
					mClientList[ i ]->SendEvent( message );
			}
			break;
		}
	}

	// Allow the game to process it
	SendEvent( message );

	ClientPtr cp = GetClientByEntity( dest );
	if ( cp )
	{
		cp->SendEvent( message );
		return;
	}
}

void IGame::ClientJoined( const EvClientConnected *msg )
{
	Utils::OutputDebug( kInfo, "Client Joined Game, IsBot: %d, ClientNum: %d", msg->mIsBot, msg->mGameId );
	if ( msg->mIsBot && !mBotJoining )
	{
		CheckGameState();

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

			gEngineFuncs->ChangeTeam( msg->mGameId, cp->mDesiredTeam, NULL );
			gEngineFuncs->ChangeClass( msg->mGameId, cp->mDesiredClass, NULL );

			cp->CheckTeamEvent();
			cp->CheckClassEvent();
		}
	}
}

void IGame::ClientLeft( const EvClientDisconnected *msg )
{
	Utils::OutputDebug( kInfo, "Client Left Game, ClientNum: %d", msg->mGameId );

	ClientPtr &cp = GetClientFromCorrectedGameId( msg->mGameId );
	if ( cp )
	{
		cp->Shutdown();
		cp.reset();
	}
}

void IGame::CheckGameState()
{
	const GameState state = gEngineFuncs->GetGameState();
	switch ( state )
	{
		case GAME_STATE_WAITINGFORPLAYERS:
		case GAME_STATE_WARMUP:
		case GAME_STATE_WARMUP_COUNTDOWN:
		case GAME_STATE_PLAYING:
		case GAME_STATE_SUDDENDEATH:
			StartGame();
			break;
		case GAME_STATE_INVALID:
			EndGame();
			break;
		case GAME_STATE_INTERMISSION:
		case GAME_STATE_SCOREBOARD:
		case GAME_STATE_PAUSED:
			return;
	}
	mLastGameState = mGameState;
	mGameState = state;
}

void IGame::StartGame()
{
	if ( GameStarted() )
		return;

	//EngineFuncs::ConsoleMessage("<StartGame>");
	InitMapScript();
}

void IGame::EndGame()
{
	if ( !GameStarted() )
		return;

	EngineFuncs::ConsoleMessage( "<EndGame>" );
}

void IGame::NewRound()
{
	EngineFuncs::ConsoleMessage( "<NewRound>" );
}

void IGame::StartTraining()
{
	const char *pMapName = gEngineFuncs->GetMapName();
	if ( pMapName )
	{
		filePath script( "scripts/%s_train.gm", pMapName );

		int iThreadId;
		if ( !ScriptManager::GetInstance()->ExecuteFile( script, iThreadId ) )
		{
			EngineFuncs::ConsoleError( va( "Error Running Training Script: %s", script.c_str() ).c_str() );
		}
	}
}

void IGame::InitMapScript()
{
	// Get Goals from the game.
	System::mInstance->mGoalManager->Reset();

	ErrorObj err;
	System::mInstance->mGoalManager->Load( std::string( gEngineFuncs->GetMapName() ), err );
	err.PrintToConsole();

	System::mInstance->mGoalManager->InitGameGoals();

	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
	DisableGCInScope gcEn( pMachine );

	filePath script( "nav/%s.gm", gEngineFuncs->GetMapName() );

	// Load the script for the current map.
	int iThreadId = GM_INVALID_THREAD;
	if ( ScriptManager::GetInstance()->ExecuteFile( script, iThreadId ) )
	{
		{
			gmCall call;
			if ( call.BeginGlobalFunction( pMachine, "OnMapLoad", gmVariable::s_null, true ) )
			{
				call.End();
			}
		}

		{
			gmCall call;
			if ( call.BeginGlobalFunction( pMachine, "InitializeRoutes", gmVariable::s_null, true ) )
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

void IGame::InitTime()
{
	mGameMsec = gEngineFuncs->GetGameTime();
	mDeltaMsec = 0;
}

void IGame::UpdateTime()
{
	int iCurrentTime = gEngineFuncs->GetGameTime();
	mDeltaMsec = iCurrentTime - mGameMsec;
	mGameMsec = iCurrentTime;
}

void IGame::CheckServerSettings( bool managePlayers )
{
	rmt_ScopedCPUSample( IGameCheckServerSettings );

	if ( !System::mInstance->mNavigation->IsReady() )
		return;

	obPlayerInfo pi;
	gEngineFuncs->GetPlayerInfo( pi );

	//////////////////////////////////////////////////////////////////////////
	gmMachine *pM = ScriptManager::GetInstance()->GetMachine();
	DisableGCInScope gcEn( pM );

	gmVariable var = pM->GetGlobals()->Get( pM, "Server" );
	gmTableObject *pServerTbl = var.GetTableObjectSafe();
	if ( !pServerTbl )
	{
		pServerTbl = pM->AllocTableObject();
		pM->GetGlobals()->Set( pM, "Server", gmVariable( pServerTbl ) );
	}
	gmTableObject *pTeamsTbl = pServerTbl->Get( pM, "Team" ).GetTableObjectSafe();
	if ( !pTeamsTbl )
	{
		pTeamsTbl = pM->AllocTableObject();
		pServerTbl->Set( pM, "Team", gmVariable( pTeamsTbl ) );
	}

	pServerTbl->Set( pM, "NumPlayers", gmVariable( pi.GetNumPlayers() ) );
	pServerTbl->Set( pM, "NumPlayersNoSpec", gmVariable( pi.GetNumPlayers() - pi.GetNumPlayers( OB_TEAM_SPECTATOR ) ) );
	pServerTbl->Set( pM, "NumBots", gmVariable( pi.GetNumPlayers( OB_TEAM_ALL, obPlayerInfo::Bot ) ) );
	pServerTbl->Set( pM, "MaxPlayers", gmVariable( pi.GetMaxPlayers() ) );
	pServerTbl->Set( pM, "AvailableTeams", gmVariable( pi.GetAvailableTeams() ) );

	for ( int i = 0; i < obPlayerInfo::MaxTeams; ++i )
	{
		if ( pi.GetAvailableTeams() & ( 1 << i ) )
		{
			gmTableObject *pTeam = pTeamsTbl->Get( i ).GetTableObjectSafe();
			if ( !pTeam )
			{
				pTeam = pM->AllocTableObject();
				pTeamsTbl->Set( pM, i, gmVariable( pTeam ) );
			}

			pTeam->Set( pM, "NumPlayers", gmVariable( pi.GetNumPlayers( i, obPlayerInfo::Both ) ) );
			pTeam->Set( pM, "NumBots", gmVariable( pi.GetNumPlayers( i, obPlayerInfo::Bot ) ) );
			pTeam->Set( pM, "NumHumans", gmVariable( pi.GetNumPlayers( i, obPlayerInfo::Human ) ) );
		}
	}

	if ( managePlayers )
	{
		gmCall call;
		if ( call.BeginGlobalFunction( pM, "ManagePlayers" ) )
		{
			call.End();
		}
	}
}

void IGame::InitCommands()
{
	SetEx( "addbot", "Adds a bot to the game",
		this, &IGame::cmdAddbot );
	SetEx( "kickbot", "Removes a bot from the game",
		this, &IGame::cmdKickbot );
	SetEx( "kickall", "Kick all bots from the game",
		this, &IGame::cmdKickAll );
	SetEx( "debugbot", "Enables debugging output on a specific bot",
		this, &IGame::cmdDebugBot );
	SetEx( "drawblocktests", "Enables drawing of blockable line tests",
		this, &IGame::cmdDrawBlockableTests );
	SetEx( "dontshoot", "Enables/disables all bot shooting ability.",
		this, &IGame::cmdBotDontShoot );
	SetEx( "show_bb", "Shows the contents of the global blackboard.",
		this, &IGame::cmdDumpBlackboard );
	SetEx( "reload_weapons", "Reloads the weapon database from script files on disc.",
		this, &IGame::cmdReloadWeaponDatabase );
	SetEx( "show_bb", "Shows the contents of the global blackboard.",
		this, &IGame::cmdDumpBlackboard );
	SetEx( "revision", "Shows the revision the bot dll was built from.",
		this, &IGame::cmdRevision );
	SetEx( "print_filesystem", "Prints files from file system.",
		this, &IGame::cmdPrintFileSystem );
	SetEx( "stopprocess", "Stops a process by its name.",
		this, &IGame::cmdStopProcess );
	SetEx( "showprocesses", "Shows current proccesses.",
		this, &IGame::cmdShowProcesses );
}

bool IGame::UnhandledCommand( const StringVector & args )
{
	rmt_ScopedCPUSample( IGameUnhandledCommand );
	bool handled = false;
	for ( int i = 0; i < Constants::MAX_PLAYERS; ++i )
	{
		if ( mClientList[ i ] )
		{
			handled |= mClientList[ i ]->DistributeUnhandledCommand( args );
		}
	}
	return handled;
}

void IGame::cmdRevision( const StringVector & args )
{
	EngineFuncs::ConsoleMessage( va( "Omni-bot: Revision %s : %s",
		Revision::Number().c_str(),
		Revision::Date().c_str() ).c_str() );
}

void IGame::cmdBotDontShoot( const StringVector & args )
{
	if ( args.size() == 2 )
	{
		bool dontShoot;
		if ( Utils::StringToBool( args[ 1 ], dontShoot ) )
		{
			for ( int i = 0; i < Constants::MAX_PLAYERS; ++i )
			{
				if ( mClientList[ i ] )
				{
					mClientList[ i ]->SetUserFlag( Client::FL_SHOOTINGDISABLED, dontShoot );

					EngineFuncs::ConsoleMessage( va( "%s: shooting %s",
						mClientList[ i ]->GetName(),
						dontShoot ? "disabled" : "enabled" ).c_str() );
				}
			}
		}
		return;
	}

	EngineFuncs::ConsoleError( "Usage: bot dontshoot true/false/1/0/yes/no" );
}

void IGame::cmdDumpBlackboard( const StringVector & args )
{
	int iType = bbk_All;
	if ( args.size() > 1 )
	{
		if ( !Utils::ConvertString( args[ 1 ], iType ) )
			return;
	}
	gBlackboard.DumpBlackBoardContentsToGame( iType );
}

void IGame::cmdDebugBot( const StringVector & args )
{
	if ( args.size() < 3 )
	{
		EngineFuncs::ConsoleError( "debugbot syntax: bot debugbot botname debugtype" );
		EngineFuncs::ConsoleError( "types: log, move, aim, sensory, events, weapon, script, events, fpinfo" );
		return;
	}

	// What bot is this for?
	bool bAll = false;
	std::string botname = args[ 1 ];
	if ( botname == "all" )
		bAll = true;

	for ( int p = 0; p < Constants::MAX_PLAYERS; ++p )
	{
		if ( mClientList[ p ] )
		{
			ClientPtr bot = mClientList[ p ];
			if ( botname == bot->GetName() || bAll )
			{
				for ( uint32_t i = 2; i < args.size(); ++i )
				{
					using namespace AiState;

					std::string strDebugType = args[ i ];
					if ( strDebugType == "log" )
						bot->EnableDebug( BOT_DEBUG_LOG, !bot->IsDebugEnabled( BOT_DEBUG_LOG ) );
					else if ( strDebugType == "move" )
						bot->EnableDebug( BOT_DEBUG_MOVEVEC, !bot->IsDebugEnabled( BOT_DEBUG_MOVEVEC ) );
					else if ( strDebugType == "events" )
						bot->EnableDebug( BOT_DEBUG_EVENTS, !bot->IsDebugEnabled( BOT_DEBUG_EVENTS ) );
					else if ( strDebugType == "aim" )
					{
						FINDSTATEIF( Aimer, bot->GetStateRoot(), ToggleDebugDraw() );
					}
					else if ( strDebugType == "sensory" )
					{
						FINDSTATEIF( SensoryMemory, bot->GetStateRoot(), ToggleDebugDraw() );
					}
					else if ( strDebugType == "script" )
						bot->EnableDebug( BOT_DEBUG_SCRIPT, !bot->IsDebugEnabled( BOT_DEBUG_SCRIPT ) );
					else if ( strDebugType == "fpinfo" )
						bot->EnableDebug( BOT_DEBUG_FPINFO, !bot->IsDebugEnabled( BOT_DEBUG_FPINFO ) );
					else if ( strDebugType == "failedpaths" )
						bot->EnableDebug( BOT_DEBUG_LOG_FAILED_PATHS, !bot->IsDebugEnabled( BOT_DEBUG_LOG_FAILED_PATHS ) );
					else
					{
						if ( bot->GetStateRoot() )
						{
							State *pState = bot->GetStateRoot()->FindState( strDebugType.c_str() );
							if ( pState )
							{
								pState->DebugDraw( !pState->DebugDrawingEnabled() );
							}
						}
					}
				}
				if ( !bAll )
					return;
			}
		}
	}

	if ( !bAll )
	{
		EngineFuncs::ConsoleError( va( "no bot found named %s", botname.c_str() ).c_str() );
	}
}

void IGame::cmdKickAll( const StringVector & args )
{
	// Kick all bots from the game.
	for ( int i = 0; i < Constants::MAX_PLAYERS; ++i )
	{
		if ( mClientList[ i ] )
		{
			StringVector tl;
			tl.push_back( "kickbot" );
			tl.push_back( va( "%i", mClientList[ i ]->GetGameID() ).c_str() );
			CommandReciever::DispatchCommand( tl );
		}
	}
}

void IGame::cmdAddbot( const StringVector & args )
{
	if ( !System::mInstance->mNavigation->IsReady() )
	{
		EngineFuncs::ConsoleError( va( "No navigation file loaded, unable to add bots." ).c_str() );
		return;
	}

	int iTeam = -1; // -1 for autochoose in interface
	int iClass = -1; // -1 for autochoose in interface

	std::string profile;
	std::string nextName;

	// Parse the command arguments
	switch ( args.size() )
	{
		case 5:
			profile = args[ 4 ];
		case 4:
			nextName = args[ 3 ];
		case 3:
			iClass = atoi( args[ 2 ].c_str() );
		case 2:
			iTeam = atoi( args[ 1 ].c_str() );
		case 1:
			break;
		default:
			EngineFuncs::ConsoleError( "addbot team[1-2] class[1-5] name[std::string]" );
			return;
	};

	if ( nextName.empty() )
	{
		NamePtr nr = NameManager::GetInstance()->GetName();
		nextName = nr ? nr->GetName() : Utils::FindOpenPlayerName();
	}

	ParamsAddbot b;
	Utils::StringCopy( b.mName, nextName.c_str(), sizeof( b.mName ) );
	Utils::StringCopy( b.mProfile, profile.c_str(), sizeof( b.mProfile ) );
	b.mTeam = iTeam;
	b.mClass = iClass;
	AddBot( b, true );
}

void IGame::cmdKickbot( const StringVector & args )
{
	// Parse the command arguments
	bool bDidSomething = false;
	for ( int i = 1; i < (int)args.size(); ++i )
	{
		ParamsKickbot b;
		if ( !Utils::ConvertString( args[ i ], b.mGameId ) )
			Utils::StringCopy( b.mName, args[ i ].c_str(), ParamsKickbot::BufferSize );
		gEngineFuncs->RemoveBot( b );
		bDidSomething = true;
	}

	if ( !bDidSomething )
	{
		EngineFuncs::ConsoleError( "kickbot [std::string/gameid] .." );
	}
}

void IGame::cmdDrawBlockableTests( const StringVector & args )
{
	if ( args.size() >= 2 )
	{
		if ( !mbDrawBlockableTests && Utils::StringToTrue( args[ 1 ] ) )
		{
			EngineFuncs::ConsoleMessage( "Draw Blockable Tests on." );
			mbDrawBlockableTests = true;
		}
		else if ( mbDrawBlockableTests && Utils::StringToFalse( args[ 1 ] ) )
		{
			EngineFuncs::ConsoleMessage( "Draw Blockable Tests off." );
			mbDrawBlockableTests = false;
		}
		else
		{
			mbDrawBlockableTests = !mbDrawBlockableTests;
		}
	}
}

void IGame::cmdPrintFileSystem( const StringVector & args )
{
	std::string pth = "";
	std::string ex = ".*";

	DirectoryList dlist;
	FileSystem::FindAllFiles( pth, dlist, ex );

	EngineFuncs::ConsoleMessage( "------------------------------------" );
	EngineFuncs::ConsoleMessage( va( "%d Files %s, in %s", dlist.size(), ex.c_str(), pth.c_str() ).c_str() );
	for ( uint32_t i = 0; i < dlist.size(); ++i )
	{
		EngineFuncs::ConsoleMessage( dlist[ i ].string().c_str() );
	}
	EngineFuncs::ConsoleMessage( "------------------------------------" );
}

void IGame::cmdReloadWeaponDatabase( const StringVector & args )
{
	gWeaponDatabase.LoadWeaponDefinitions( true );
	DispatchEvent( EvRefreshAllWeapons::Msg() );
}

void IGame::cmdShowProcesses( const StringVector & args )
{
	EngineFuncs::ConsoleMessage( va( "# Processes: %d!", mUpdateMap.size() ).c_str() );
	FunctorMap::iterator it = mUpdateMap.begin(), itEnd = mUpdateMap.end();
	for ( ; it != itEnd; ++it )
	{
		EngineFuncs::ConsoleMessage( va( "Process: %s!", ( *it ).first.c_str() ).c_str() );
	}
}

void IGame::cmdStopProcess( const StringVector & args )
{
	if ( !args.empty() )
	{
		RemoveUpdateFunction( args[ 1 ] );
	}
}

bool IGame::AddUpdateFunction( const std::string &_name, FunctorPtr _func )
{
	if ( mUpdateMap.find( _name ) != mUpdateMap.end() )
	{
		EngineFuncs::ConsoleError( "That process is already running!" );
		return false;
	}
	EngineFuncs::ConsoleMessage( va( "Process %s has been started! ", _name.c_str() ).c_str() );
	mUpdateMap.insert( std::make_pair( _name, _func ) );
	return true;
}

bool IGame::RemoveUpdateFunction( const std::string &_name )
{
	FunctorMap::iterator it = mUpdateMap.find( _name );
	if ( it != mUpdateMap.end() )
	{
		EngineFuncs::ConsoleMessage( va( "Process %s has been stopped! ", _name.c_str() ).c_str() );
		mUpdateMap.erase( _name.c_str() );
		return true;
	}
	return false;
}

void IGame::AddBot( ParamsAddbot & parms, bool createnow )
{
	//////////////////////////////////////////////////////////////////////////
	if ( !parms.mName[ 0 ] )
	{
		NamePtr nr = NameManager::GetInstance()->GetName();
		std::string name = nr ? nr->GetName() : Utils::FindOpenPlayerName();
		Utils::StringCopy( parms.mName, name.c_str(), sizeof( parms.mName ) );
	}
	//////////////////////////////////////////////////////////////////////////
	if ( createnow )
		mBotJoining = true;
	int gameId = gEngineFuncs->AddBot( parms );
	if ( createnow )
		mBotJoining = false;
	if ( gameId != -1 && createnow )
	{
		ClientPtr &cp = GetClientFromCorrectedGameId( gameId );
		if ( !cp )
		{
			// Initialize the appropriate slot in the list.
			cp.reset( CreateGameClient() );
			cp->Init( gameId );
		}

		cp->mDesiredTeam = parms.mTeam;
		cp->mDesiredClass = parms.mClass;

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
		gEngineFuncs->ChangeTeam( gameId, cp->mDesiredTeam, NULL );
		gEngineFuncs->ChangeClass( gameId, cp->mDesiredClass, NULL );

		cp->CheckTeamEvent();
		cp->CheckClassEvent();
	}
}

ClientPtr IGame::GetClientByEntity( GameEntity ent )
{
	for ( int i = 0; i < Constants::MAX_PLAYERS; ++i )
	{
		if ( mClientList[ i ] && mClientList[ i ]->GetGameEntity() == ent )
			return mClientList[ i ];
	}
	return ClientPtr();
}

ClientPtr IGame::GetClientByIndex( int index )
{
	if ( InRangeT<int>( index, 0, Constants::MAX_PLAYERS ) )
		return mClientList[ index ];
	return ClientPtr();
}

ClientPtr &IGame::GetClientFromCorrectedGameId( int gameid )
{
	return mClientList[ GetGameVars().mClientBase + gameid ];
}

bool IGame::CreateCriteria( gmThread *_thread, CheckCriteria &_criteria, std::stringstream &err )
{
	_criteria = CheckCriteria();

	const int MaxArgs = 8;
	enum CriteriaArgType
	{
		kTypeNone,
		kTypeInt,
		kTypeFloat,
		kTypeNum,
		kTypeVector,
		kTypeEntity,
		kTypeMapGoal,

		kNumArgTypes
	};

	const char * ArgTypeNames [] =
	{
		"",
		"int",
		"float",
		"float or int",
		"vec3",
		"entity",
		"mapgoal",
	};
	static_assert( sizeof( ArgTypeNames ) / sizeof( ArgTypeNames[ 0 ] ) == kNumArgTypes, "Mismatched Arg Types" );

	//////////////////////////////////////////////////////////////////////////
	// Subject
	int NextArg = 0;
	if ( _thread->ParamType( NextArg ) == GM_ENTITY )
	{
		GameEntity e;
		e.FromInt( _thread->Param( NextArg ).GetEntity() );
		_criteria.mSubject.SetEntity( e );

		NextArg++;
	}
	//////////////////////////////////////////////////////////////////////////

	const char *pExpression = _thread->Param( NextArg ).GetCStringSafe( NULL );
	if ( pExpression )
	{
		int NumExpectedOperands = 0;
		CriteriaArgType ExpectedArgTypes[ MaxArgs ] = {};

		int iToken = CheckCriteria::TOK_CRITERIA;

		StringVector sv;
		Utils::Tokenize( pExpression, " ", sv );
		for ( uint32_t t = 0; t < sv.size(); ++t )
		{
			const uint32_t sHash = Utils::Hash32( sv[ t ].c_str() );

			if ( iToken == CheckCriteria::TOK_CRITERIA )
			{
				switch ( sHash )
				{
					case 0x29b19c8a /* not */:
					case 0x8fd958a1 /* doesnot */:
						_criteria.mNegated = true;
						break;
					case 0x84d1546a /* deleted */:
						_criteria.mCriteria = Criteria::ON_ENTITY_DELETED;
						_criteria.mOperator = Criteria::OP_EQUALS;
						_criteria.mOperand[ 0 ] = obUserData( 1 );
						NumExpectedOperands = 0;
						++iToken;
						break;
					case 0x587bc012 /* hasentflag */:
					case 0xabcfd8b3 /* hasentflags */:
					case 0x8770b3da /* haveentflag */:
					case 0xdf6ad30b /* haveentflags */:
						_criteria.mCriteria = Criteria::ON_ENTITY_FLAG;
						_criteria.mOperator = Criteria::OP_EQUALS;
						ExpectedArgTypes[ NumExpectedOperands++ ] = kTypeInt;
						++iToken;
						break;
					case 0x6b98ed8f /* health */:
						_criteria.mCriteria = Criteria::ON_ENTITY_HEALTH;
						NumExpectedOperands = 1;
						ExpectedArgTypes[ 0 ] = kTypeNum;
						++iToken;
						break;
					case 0x7962a87c /* weaponequipped */:
						_criteria.mCriteria = Criteria::ON_ENTITY_WEAPON;
						ExpectedArgTypes[ NumExpectedOperands++ ] = kTypeInt; // weaponid
						++iToken;
						break;
					case 0x32741c32 /* velocity */:
						_criteria.mCriteria = Criteria::ON_ENTITY_VELOCITY;
						ExpectedArgTypes[ NumExpectedOperands++ ] = kTypeVector; // velocity vector
						++iToken;
						break;
					case 0x3f44af2b /* weaponcharged */:
						_criteria.mCriteria = Criteria::ON_ENTITY_WEAPONCHARGED;
						ExpectedArgTypes[ NumExpectedOperands++ ] = kTypeInt; // weaponid
						++iToken;
						break;
					case 0x79e3594d /* weaponhasammo */:
						_criteria.mCriteria = Criteria::ON_ENTITY_WEAPONHASAMMO;
						ExpectedArgTypes[ NumExpectedOperands++ ] = kTypeInt; // weaponid
						ExpectedArgTypes[ NumExpectedOperands++ ] = kTypeInt; // ammo amount
						++iToken;
						break;
					case 0x8ad3b7b9 /* mapgoalavailable */:
						_criteria.mCriteria = Criteria::ON_MAPGOAL_AVAILABLE;
						ExpectedArgTypes[ NumExpectedOperands++ ] = kTypeMapGoal;
						++iToken;
						break;
					default:
						err << "invalid criteria " << sv[ t ].c_str() << std::endl;
						return false;
				}
			}
			else if ( iToken == CheckCriteria::TOK_OPERATOR )
			{
				_criteria.ParseOperator( sHash );
				break;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// Operand
		for ( int i = 0; i < NumExpectedOperands; ++i )
		{
			gmVariable o = _thread->Param( 2 + i );

			switch ( ExpectedArgTypes[ i ] )
			{
				case kTypeInt:
				{
					if ( !o.IsInt() )
						goto argError;

					_criteria.mOperand[ i ] = obUserData( o.GetInt() );
					break;
				}
				case kTypeFloat:
				{
					if ( !o.IsFloat() )
						goto argError;

					_criteria.mOperand[ i ] = obUserData( o.GetFloat() );
					break;
				}
				case kTypeNum:
				{
					if ( !o.IsNumber() )
						goto argError;

					_criteria.mOperand[ i ] = obUserData( o.GetFloatSafe() );
					break;
				}
				case kTypeVector:
				{
					if ( !o.IsVector() )
						goto argError;

					Vector3f v;
					o.GetVector( v );
					_criteria.mOperand[ i ] = obUserData( v.X(), v.Y(), v.Z() );
					break;
				}
				case kTypeEntity:
				{
					if ( !o.IsEntity() )
						goto argError;

					GameEntity e;
					e.FromInt( o.GetEntity() );
					_criteria.mOperand[ i ].SetEntity( e );
					break;
				}
				case kTypeMapGoal:
				{
					if ( o.IsString() )
					{
						const char * goalName = o.GetCStringSafe( 0 );
						MapGoalPtr mg = System::mInstance->mGoalManager->GetGoal( goalName );
						if ( mg )
						{
							_criteria.mOperand[ i ] = obUserData( mg->GetSerialNum() );
						}
						else
						{
							err << "unknown map goal '" << goalName << "'" << std::endl;
							return false;
						}
					}
					else
					{
						MapGoal *Mg = 0;
						if ( gmBind2::Class<MapGoal>::FromVar( _thread, o, Mg ) && Mg )
						{
							_criteria.mOperand[ i ] = Mg->GetSerialNum();
							return true;
						}
						else
							goto argError;
					}
					break;
				}
				default:
					break;
			}
			continue;
		argError:
			const char * gotType = _thread->GetMachine()->GetTypeName( o.m_type );
			err << "expected " << ArgTypeNames[ ExpectedArgTypes[ i ] ] << " got " << gotType << std::endl;
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

void IGame::LoadGoalScripts( bool _clearold )
{
	rmt_ScopedCPUSample( LoadGoalScripts );

	LOG( "Loading Script Goals" );
	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

	gmTableObject *pScriptGoalTable = pMachine->GetGlobals()->Get( pMachine, "ScriptGoals" ).GetTableObjectSafe();
	if ( _clearold || !pScriptGoalTable )
	{
		pScriptGoalTable = pMachine->AllocTableObject();
		pMachine->GetGlobals()->Set( pMachine, "ScriptGoals", gmVariable( pScriptGoalTable ) );
	}

	DirectoryList goalScripts;
	FileSystem::FindAllFiles( "global_scripts/goals", goalScripts, "goal_.*.gm" );
	FileSystem::FindAllFiles( "scripts/goals", goalScripts, "goal_.*.gm" );

	LOG( "Loading " << goalScripts.size() << " goal scripts from: scripts/goals" );
	DirectoryList::const_iterator cIt = goalScripts.begin(), cItEnd = goalScripts.end();
	for ( ; cIt != cItEnd; ++cIt )
	{
		ScriptGoalPtr ptr( new AiState::ScriptGoal( "" ) );

		int iThreadId;
		gmUserObject *userObj = ptr->GetScriptObject( pMachine );
		gmVariable varThis( userObj );

		filePath script( ( *cIt ).string().c_str() );
		LOG( "Loading Goal Definition: " << script );
		if ( ScriptManager::GetInstance()->ExecuteFile( script, iThreadId, &varThis ) && ptr->GetName()[ 0 ] )
		{
			ptr->SetSourceFile( script.c_str() );

			gScriptGoalList.push_back( ptr );

			// add to global table.
			pScriptGoalTable->Set( pMachine, ptr->GetName().c_str(), gmVariable( userObj ) );
		}
		else
		{
			LOG( "Error Running Goal Script: " << ( *cIt ).string() );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

IGame::EntityIterator::operator bool() const
{
	return mCurrent.mEntity.IsValid();
}

void IGame::EntityIterator::Clear()
{
	mCurrent.mEntity = GameEntity();
	mCurrent.mEntInfo = EntityInfo();
}

EntityInstance &IGame::EntityIterator::GetEnt()
{
	return mCurrent;
}
const EntityInstance &IGame::EntityIterator::GetEnt() const
{
	return mCurrent;
}
const int IGame::EntityIterator::GetIndex() const
{
	return mIndex;
}
IGame::EntityIterator::EntityIterator()
{
}
IGame::EntityIterator::EntityIterator( GameEntity initialEntity )
{
	if ( IGame::IsEntityValid( initialEntity ) )
	{
		mIndex = initialEntity.GetIndex();
		mCurrent = mGameEntities[ mIndex ];
	}
}

bool IGame::IsEntityValid( const GameEntity & hndl )
{
	if ( hndl.IsValid() )
	{
		const int index = hndl.GetIndex();
		if ( index >= 0 && index < Constants::MAX_ENTITIES )
		{
			EntityInstance &ei = mGameEntities[ index ];
			if ( !ei.mEntity.IsValid() )
				return false;

			UpdateEntity( ei );
			if ( ei.mEntInfo.mGroup != ENT_GRP_UNKNOWN && ei.mEntity.IsValid() )
				return true;
		}
	}
	return false;
}

GameEntity IGame::GetEntityByIndex( size_t index )
{
	if ( index >= 0 && index < Constants::MAX_ENTITIES )
	{
		return mGameEntities[ index ].mEntity;
	}
	return GameEntity();
}

bool IGame::GetEntityInfo( const GameEntity & hndl, EntityInfo& entInfo )
{
	if ( hndl.IsValid() )
	{
		const int index = hndl.GetIndex();
		if ( index >= 0 && index < Constants::MAX_ENTITIES )
		{
			EntityInstance &ei = mGameEntities[ index ];

			ei.mEntity = hndl;
			if ( !ei.mEntity.IsValid() )
				return false;

			UpdateEntity( ei );
			if ( ei.mEntInfo.mGroup != ENT_GRP_UNKNOWN && ei.mEntity.IsValid() )
			{
				entInfo = ei.mEntInfo;
				return true;
			}
		}
	}
	return false;
}

bool IGame::IterateEntity( IGame::EntityIterator &_it )
{
	int iStartIndex = 0;
	if ( _it )
		iStartIndex = _it.mCurrent.mEntity.GetIndex() + 1;

	for ( int i = iStartIndex; i < mMaxEntity; ++i )
	{
		if ( mGameEntities[ i ].mEntity.IsValid() )
		{
			UpdateEntity( mGameEntities[ i ] );
			_it.mCurrent = mGameEntities[ i ];
			_it.mIndex = i;
			return true;
		}
	}
	return false;
}

void IGame::UpdateEntity( EntityInstance& ent )
{
	if ( ent.mTimeStamp != IGame::GetTime() )
	{
		ent.mTimeStamp = IGame::GetTime();
		gEngineFuncs->GetEntityInfo( ent.mEntity, ent.mEntInfo );
	}
}

void IGame::AddDeletedThread( int threadId )
{
	// process the full buffer
	if ( mNumDeletedThreads == MaxDeletedThreads )
	{
		PropogateDeletedThreads();
	}

	// paranoid check
	if ( mNumDeletedThreads < MaxDeletedThreads )
	{
		mDeletedThreads[ mNumDeletedThreads++ ] = threadId;
	}
}

static bool _ThreadIdSort( int id1, int id2 )
{
	if ( id1 == GM_INVALID_THREAD )
		return false;
	if ( id2 == GM_INVALID_THREAD )
		return true;
	return id1 < id2;
}

void IGame::PropogateDeletedThreads()
{
	rmt_ScopedCPUSample( IGamePropogateDeletedThreads );

	if ( mNumDeletedThreads > 0 )
	{
		std::sort( mDeletedThreads, mDeletedThreads + mNumDeletedThreads, _ThreadIdSort );
		for ( int i = 0; i < Constants::MAX_PLAYERS; ++i )
		{
			if ( mClientList[ i ] )
			{
				mClientList[ i ]->PropogateDeletedThreads( mDeletedThreads, mNumDeletedThreads );
			}
		}
		mNumDeletedThreads = 0;
	}
}

//////////////////////////////////////////////////////////////////////////

void IGame::GetNavParms( NavParms & navParms ) const
{
	navParms = NavParms();
}
