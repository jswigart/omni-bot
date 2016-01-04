////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "JA_Game.h"
#include "JA_VoiceMacros.h"
#include "JA_Client.h"

#include "RenderBuffer.h"

#include "System.h"
#include "PathPlannerBase.h"
#include "NameManager.h"
#include "ScriptManager.h"

#include "FilterSensory.h"
#include "BotSensoryMemory.h"

JediAcademy_Interface* gJediAcademyFuncs = 0;

IGame *CreateGameInstance()
{
	return new JA_Game;
}

JA_Game::JA_Game()
{
	mGameVars.mClientBase = 0;
	mGameVars.mGameVersion = JA_VERSION_LATEST;
	mGameVars.mGameAbbrev = "ja";
	mGameVars.mGameName = "Star Wars Jedi Knight: Jedi Academy";
	mGameVars.mPlayerHeight = 64.f;
}

JA_Game::~JA_Game()
{
}

Client *JA_Game::CreateGameClient()
{
	return new JA_Client;
}

bool JA_Game::Init( System & system )
{
	AiState::SensoryMemory::SetEntityTraceOffsetCallback( JA_Game::JA_GetEntityClassTraceOffset );
	AiState::SensoryMemory::SetEntityAimOffsetCallback( JA_Game::JA_GetEntityClassAimOffset );

	if ( !IGame::Init( system ) )
		return false;

	gJediAcademyFuncs = dynamic_cast<JediAcademy_Interface*>(gEngineFuncs);

	return true;
}

static const IntEnum ET_TeamEnum[] =
{
	IntEnum( "SPECTATOR", OB_TEAM_SPECTATOR ),
	IntEnum( "FREE", JA_TEAM_FREE ),
	IntEnum( "RED", JA_TEAM_RED ),
	IntEnum( "BLUE", JA_TEAM_BLUE ),
};

void JA_Game::GetTeamEnumeration( const IntEnum *&_ptr, int &num )
{
	num = sizeof( ET_TeamEnum ) / sizeof( ET_TeamEnum[ 0 ] );
	_ptr = ET_TeamEnum;
}

static const IntEnum JA_WeaponEnum[] =
{
	IntEnum( "NONE", JA_WP_NONE ),
	IntEnum( "STUN", JA_WP_STUN_BATON ),
	IntEnum( "MELEE", JA_WP_MELEE ),
	IntEnum( "SABER", JA_WP_SABER ),
	IntEnum( "PISTOL", JA_WP_BRYAR_PISTOL ),
	IntEnum( "BLASTER", JA_WP_BLASTER ),
	IntEnum( "DISRUPTOR", JA_WP_DISRUPTOR ),
	IntEnum( "BOWCASTER", JA_WP_BOWCASTER ),
	IntEnum( "REPEATER", JA_WP_REPEATER ),
	IntEnum( "DEMP2", JA_WP_DEMP2 ),
	IntEnum( "FLECHETTE", JA_WP_FLECHETTE ),
	IntEnum( "ROCKET_LAUNCHER", JA_WP_ROCKET_LAUNCHER ),
	IntEnum( "THERMAL", JA_WP_THERMAL ),
	IntEnum( "TRIP_MINE", JA_WP_TRIP_MINE ),
	IntEnum( "DET_PACK", JA_WP_DET_PACK ),
	IntEnum( "CONCUSSION", JA_WP_CONCUSSION ),
	IntEnum( "OLDPISTOL", JA_WP_BRYAR_OLD ),
};

void JA_Game::GetWeaponEnumeration( const IntEnum *&_ptr, int &num )
{
	num = sizeof( JA_WeaponEnum ) / sizeof( JA_WeaponEnum[ 0 ] );
	_ptr = JA_WeaponEnum;
}

static const IntEnum JKJA_ClassEnum[] =
{
	IntEnum( "PLAYER", JA_CLASS_PLAYER ),
	IntEnum( "ASSAULT", JA_CLASS_ASSAULT ),
	IntEnum( "SCOUT", JA_CLASS_SCOUT ),
	IntEnum( "TECH", JA_CLASS_TECH ),
	IntEnum( "JEDI", JA_CLASS_JEDI ),
	IntEnum( "DEMO", JA_CLASS_DEMO ),
	IntEnum( "HW", JA_CLASS_HW ),
	IntEnum( "ANYPLAYER", JA_CLASS_ANY ),
	IntEnum( "ROCKET", JA_CLASSEX_ROCKET ),
	IntEnum( "MINE", JA_CLASSEX_MINE ),
	IntEnum( "DETPACK", JA_CLASSEX_DETPACK ),
	IntEnum( "THERMAL", JA_CLASSEX_THERMAL ),
	IntEnum( "CONCUSSION", JA_CLASSEX_CONCUSSION ),
	IntEnum( "BOWCASTER", JA_CLASSEX_BOWCASTER ),
	IntEnum( "BOWCASTER_ALT", JA_CLASSEX_BOWCASTER_ALT ),
	IntEnum( "REPEATER", JA_CLASSEX_REPEATER ),
	IntEnum( "REPEATER_BLOB", JA_CLASSEX_REPEATER_BLOB ),
	IntEnum( "FLECHETTE", JA_CLASSEX_FLECHETTE ),
	IntEnum( "FLECHETTE_ALT", JA_CLASSEX_FLECHETTE_ALT ),
	IntEnum( "BLASTER", JA_CLASSEX_BLASTER ),
	IntEnum( "PISTOL", JA_CLASSEX_PISTOL ),
	IntEnum( "DEMP2", JA_CLASSEX_DEMP2 ),
	IntEnum( "DEMP2_ALT", JA_CLASSEX_DEMP2_ALT ),
	IntEnum( "TURRET_MISSILE", JA_CLASSEX_TURRET_MISSILE ),
	IntEnum( "VEHMISSILE", JA_CLASSEX_VEHMISSILE ),
	IntEnum( "LIGHTSABER", JA_CLASSEX_LIGHTSABER ),
	IntEnum( "NPC", JA_CLASSEX_NPC ),
	IntEnum( "VEHICLE", JA_CLASSEX_VEHICLE ),
	IntEnum( "AUTOTURRET", JA_CLASSEX_AUTOTURRET ),
	IntEnum( "BREAKABLE", JA_CLASSEX_BREAKABLE ),
	IntEnum( "FORCEFIELD", JA_CLASSEX_FORCEFIELD ),
	IntEnum( "SIEGEITEM", JA_CLASSEX_SIEGEITEM ),
	IntEnum( "CORPSE", JA_CLASSEX_CORPSE ),
	IntEnum( "WEAPON", JA_CLASSEX_WEAPON ),
	IntEnum( "HOLDABLE", JA_CLASSEX_HOLDABLE ),
	IntEnum( "POWERUP", JA_CLASSEX_POWERUP ),
	IntEnum( "FLAGITEM", JA_CLASSEX_FLAGITEM ),
	IntEnum( "HOLOCRON", JA_CLASSEX_HOLOCRON ),
};

void JA_Game::InitScriptClasses( gmMachine *machine, gmTableObject *table )
{
	IGame::InitScriptClasses( machine, table );

	FilterSensory::ANYPLAYERCLASS = JA_CLASS_ANY;

	int32_t iNumMappings = sizeof( JKJA_ClassEnum ) / sizeof( JKJA_ClassEnum[ 0 ] );
	for ( int i = 0; i < iNumMappings; ++i )
	{
		table->Set( machine, JKJA_ClassEnum[ i ].mKey, gmVariable( JKJA_ClassEnum[ i ].mValue ) );
	}
}

void JA_Game::InitVoiceMacros( gmMachine *machine, gmTableObject *table )
{
	table->Set( machine, "ATTACK_POSITION", gmVariable( VCHAT_ATTACK_POSITION ) );
	table->Set( machine, "ATTACK_PRIMARY", gmVariable( VCHAT_ATTACK_PRIMARY ) );
	table->Set( machine, "ATTACK_SECONDARY", gmVariable( VCHAT_ATTACK_SECONDARY ) );

	table->Set( machine, "DEFEND_GUNS", gmVariable( VCHAT_DEFEND_GUNS ) );
	table->Set( machine, "DEFEND_POSITION", gmVariable( VCHAT_DEFEND_POSITION ) );
	table->Set( machine, "DEFEND_PRIMARY", gmVariable( VCHAT_DEFEND_PRIMARY ) );
	table->Set( machine, "DEFEND_SECONDARY", gmVariable( VCHAT_DEFEND_SECONDARY ) );

	table->Set( machine, "REPLY_COMING", gmVariable( VCHAT_REPLY_COMING ) );
	table->Set( machine, "REPLY_GO", gmVariable( VCHAT_REPLY_GO ) );
	table->Set( machine, "REPLY_NO", gmVariable( VCHAT_REPLY_NO ) );
	table->Set( machine, "REPLY_STAY", gmVariable( VCHAT_REPLY_STAY ) );
	table->Set( machine, "REPLY_YES", gmVariable( VCHAT_REPLY_YES ) );

	table->Set( machine, "REQUEST_ASSISTANCE", gmVariable( VCHAT_REQUEST_ASSISTANCE ) );
	table->Set( machine, "REQUEST_DEMO", gmVariable( VCHAT_REQUEST_DEMO ) );
	table->Set( machine, "REQUEST_HEAVYWEAPS", gmVariable( VCHAT_REQUEST_HEAVYWEAPS ) );
	table->Set( machine, "REQUEST_MEDIC", gmVariable( VCHAT_REQUEST_MEDIC ) );
	table->Set( machine, "REQUEST_SUPPLIES", gmVariable( VCHAT_REQUEST_SUPPLIES ) );
	table->Set( machine, "REQUEST_TECH", gmVariable( VCHAT_REQUEST_TECH ) );

	table->Set( machine, "SPOT_AIR", gmVariable( VCHAT_SPOT_AIR ) );
	table->Set( machine, "SPOT_DEFENSES", gmVariable( VCHAT_SPOT_DEFENSES ) );
	table->Set( machine, "SPOT_EMPLACED", gmVariable( VCHAT_SPOT_EMPLACED ) );
	table->Set( machine, "SPOT_SNIPER", gmVariable( VCHAT_SPOT_SNIPER ) );
	table->Set( machine, "SPOT_TROOPS", gmVariable( VCHAT_SPOT_TROOPS ) );

	table->Set( machine, "TACTICS_COVER", gmVariable( VCHAT_TACTICS_COVER ) );
	table->Set( machine, "TACTICS_FALLBACK", gmVariable( VCHAT_TACTICS_FALLBACK ) );
	table->Set( machine, "TACTICS_FOLLOW", gmVariable( VCHAT_TACTICS_FOLLOW ) );
	table->Set( machine, "TACTICS_HOLD", gmVariable( VCHAT_TACTICS_HOLD ) );
	table->Set( machine, "TACTICS_SPLITUP", gmVariable( VCHAT_TACTICS_SPLITUP ) );
	table->Set( machine, "TACTICS_TOGETHER", gmVariable( VCHAT_TACTICS_TOGETHER ) );
}

void JA_Game::AddBot( ParamsAddbot & parms, bool createnow )
{
	//////////////////////////////////////////////////////////////////////////
	if ( !parms.mName[ 0 ] )
	{
		NamePtr nr = NameManager::GetInstance()->GetName();
		std::string name = nr ? nr->GetName() : Utils::FindOpenPlayerName();
		Utils::StringCopy( parms.mName, name.c_str(), sizeof( parms.mName ) );
	}
	//////////////////////////////////////////////////////////////////////////
	assert( GameStarted() );

	if ( createnow )
		mBotJoining = true;
	int iGameID = gEngineFuncs->AddBot( parms );
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
		// Magik: As there's no instant team/class switching in JA, this is order dependent
		// always call pfnChangeClass() _before_ pfnChangeTeam()!
		// todo: send the weapon preferences as 3rd param
		gEngineFuncs->ChangeClass( iGameID, cp->mDesiredClass, NULL );
		gEngineFuncs->ChangeTeam( iGameID, cp->mDesiredTeam, NULL );

		cp->CheckTeamEvent();
		cp->CheckClassEvent();

		/*ScriptManager::GetInstance()->ExecBotCallback(cp.get(), "SelectWeapons");*/
	}
}

void JA_Game::InitScriptEvents( gmMachine *machine, gmTableObject *table )
{
	IGame::InitScriptEvents( machine, table );
}

void JA_Game::InitScriptEntityFlags( gmMachine *machine, gmTableObject *table )
{
	IGame::InitScriptEntityFlags( machine, table );

	table->Set( machine, "NPC", gmVariable( JA_ENT_FLAG_NPC ) );
	table->Set( machine, "VEH", gmVariable( JA_ENT_FLAG_VEHICLE ) );
	table->Set( machine, "VEH_PILOTED", gmVariable( JA_ENT_FLAG_VEHICLE_PILOTED ) );
	table->Set( machine, "JETPACK", gmVariable( JA_ENT_FLAG_JETPACK ) );
	table->Set( machine, "CLOAKED", gmVariable( JA_ENT_FLAG_CLOAKED ) );
	table->Set( machine, "CARRYINGFLAG", gmVariable( JA_ENT_FLAG_CARRYINGFLAG ) );
	table->Set( machine, "SIEGEDEAD", gmVariable( JA_ENT_FLAG_SIEGEDEAD ) );
	table->Set( machine, "YSALAMIRI", gmVariable( JA_ENT_FLAG_YSALAMIRI ) );
}

void JA_Game::InitScriptPowerups( gmMachine *machine, gmTableObject *table )
{
	table->Set( machine, "REDFLAG", gmVariable( JA_PWR_REDFLAG ) );
	table->Set( machine, "BLUEFLAG", gmVariable( JA_PWR_BLUEFLAG ) );
	table->Set( machine, "ONEFLAG", gmVariable( JA_PWR_ONEFLAG ) );
	table->Set( machine, "FORCE_ENLIGHTENMENT", gmVariable( JA_PWR_FORCE_ENLIGHTENED_LIGHT ) );
	table->Set( machine, "FORCE_ENDARKENMENT", gmVariable( JA_PWR_FORCE_ENLIGHTENED_DARK ) );
	table->Set( machine, "FORCE_BOON", gmVariable( JA_PWR_FORCE_BOON ) );
	table->Set( machine, "YSALAMIRI", gmVariable( JA_PWR_YSALAMIRI ) );
}

void JA_Game::InitScriptContentFlags( gmMachine *machine, gmTableObject *table )
{
	IGame::InitScriptContentFlags( machine, table );
	table->Set( machine, "LIGHTSABER", gmVariable( CONT_LIGHTSABER ) );
}

void JA_Game::InitScriptBotButtons( gmMachine *machine, gmTableObject *table )
{
	IGame::InitScriptBotButtons( machine, table );

	table->Set( machine, "FORCEPOWER", gmVariable( BOT_BUTTON_FORCEPOWER ) );
	table->Set( machine, "FORCEGRIP", gmVariable( BOT_BUTTON_FORCEGRIP ) );
	table->Set( machine, "FORCELIGHTNING", gmVariable( BOT_BUTTON_FORCELIGHTNING ) );
	table->Set( machine, "FORCEDRAIN", gmVariable( BOT_BUTTON_FORCEDRAIN ) );
}

void JA_Game::InitCommands()
{
	IGame::InitCommands();
}

/*
bounding boxes for ja
standing	(-15, -15, -24) x (15, 15, 40)
crouched	(-15, -15, -24) x (15, 15, 16)
*/
const float JA_Game::JA_GetEntityClassTraceOffset( const TargetInfo &target )
{
	if ( target.mEntInfo.mGroup == ENT_GRP_PLAYER )
	{
		if ( target.mEntInfo.mClassId > JA_CLASS_NULL && target.mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS )
		{
			if ( target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_PRONED ) )
				return 16.0f;
			if ( target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_CROUCHED ) )
				return 24.0f;
			return 48.0f;
		}
	}
	return 0.0f;
}

/*
bounding boxes for ja
standing	(-15, -15, -24) x (15, 15, 40)
crouched	(-15, -15, -24) x (15, 15, 16)
*/
const float JA_Game::JA_GetEntityClassAimOffset( const TargetInfo &target )
{
	if ( target.mEntInfo.mGroup == ENT_GRP_PLAYER )
	{
		if ( target.mEntInfo.mClassId > JA_CLASS_NULL && target.mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS )
		{
			if ( target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_PRONED ) )
				return 16.0f;
			if ( target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_CROUCHED ) )
				return 24.0f;
			return 48.0f;
		}
	}
	return 0.0f;
}

void JA_Game::ClientJoined( const EvClientConnected *msg )
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
