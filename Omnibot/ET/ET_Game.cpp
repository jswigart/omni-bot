////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "ET_Game.h"
#include "ET_VoiceMacros.h"
#include "ET_Client.h"
#include "ET_Messages.h"
#include "System.h"
#include "RenderBuffer.h"

#include "PathPlannerBase.h"
#include "BotPathing.h"
#include "NameManager.h"
#include "ScriptManager.h"

EnemyTerritory_Interface* gEnemyTerritoryFuncs = 0;

int ET_Game::CLASSEXoffset;
bool ET_Game::IsETBlight, ET_Game::IsBastardmod;

IGame *CreateGameInstance()
{
	return new ET_Game;
}

ET_Game::ET_Game()
{
	mGameVars.mClientBase = 0;
	mGameVars.mGameVersion = ET_VERSION_LATEST;
	mGameVars.mGameAbbrev = "et";
	mGameVars.mGameName = "Enemy Territory";
	mGameVars.mPlayerHeight = 64.f;
}

ET_Game::~ET_Game()
{
}

Client *ET_Game::CreateGameClient()
{
	return new ET_Client;
}

bool ET_Game::GetAnalyticsKeys( GameAnalytics::Keys & keys )
{
	keys.mGameKey = "508391d546e97c34dca527038b9df10c";
	keys.mSecretKey = "c6eecafbb67e602b45f32ca2948ae520276c291e";
	keys.mDataApiKey = "19ea5d05b850d0b9379f1b9c381e2dfd7e636d13";
	keys.mVersionKey = va( "%s:v%s", GetGameVars().mGameName.c_str(), GetGameVars().mVersionString.c_str() ).c_str();
	return true;
}

bool ET_Game::Init( System & system )
{
	const char *modName = gEngineFuncs->GetModName();
	IsETBlight = !strcmp( modName, "etblight" );
	IsBastardmod = !strcmp( modName, "bastardmod" );
	CLASSEXoffset = IsETBlight ? 2 : 0;

	AiState::SensoryMemory::SetEntityTraceOffsetCallback( ET_Game::ET_GetEntityClassTraceOffset );
	AiState::SensoryMemory::SetEntityAimOffsetCallback( ET_Game::ET_GetEntityClassAimOffset );

	if ( !IGame::Init( system ) )
		return false;

	gEnemyTerritoryFuncs = dynamic_cast<EnemyTerritory_Interface*>( gEngineFuncs );

	return true;
}

static const IntEnum ET_TeamEnum [] =
{
	IntEnum( "SPECTATOR", OB_TEAM_SPECTATOR ),
	IntEnum( "AXIS", ET_TEAM_AXIS ),
	IntEnum( "ALLIES", ET_TEAM_ALLIES ),
};

void ET_Game::GetTeamEnumeration( const IntEnum *&_ptr, int &num )
{
	num = sizeof( ET_TeamEnum ) / sizeof( ET_TeamEnum[ 0 ] );
	_ptr = ET_TeamEnum;
}

static const IntEnum ET_WeaponEnum[ 128 ] =
{
	IntEnum( "NONE", ET_WP_NONE ),
	IntEnum( "KNIFE", ET_WP_KNIFE ),
	IntEnum( "BINOCULARS", ET_WP_BINOCULARS ),
	IntEnum( "AXIS_GRENADE", ET_WP_GREN_AXIS ),
	IntEnum( "ALLY_GRENADE", ET_WP_GREN_ALLIES ),
	IntEnum( "LUGER", ET_WP_LUGER ),
	IntEnum( "LUGER_AKIMBO", ET_WP_AKIMBO_LUGER ),
	IntEnum( "LUGER_SILENCED", ET_WP_SILENCED_LUGER ),
	IntEnum( "LUGER_AKIMBO_SILENCED", ET_WP_AKIMBO_SILENCED_LUGER ),
	IntEnum( "MP40", ET_WP_MP40 ),
	IntEnum( "COLT", ET_WP_COLT ),
	IntEnum( "COLT_AKIMBO", ET_WP_AKIMBO_COLT ),
	IntEnum( "COLT_SILENCED", ET_WP_SILENCED_COLT ),
	IntEnum( "COLT_AKIMBO_SILENCED", ET_WP_AKIMBO_SILENCED_COLT ),
	IntEnum( "THOMPSON", ET_WP_THOMPSON ),
	IntEnum( "PANZERFAUST", ET_WP_PANZERFAUST ),
	IntEnum( "FLAMETHROWER", ET_WP_FLAMETHROWER ),
	IntEnum( "MORTAR", ET_WP_MORTAR ),
	IntEnum( "MORTAR_SET", ET_WP_MORTAR_SET ),
	IntEnum( "MOBILE_MG42", ET_WP_MOBILE_MG42 ),
	IntEnum( "MOBILE_MG42_SET", ET_WP_MOBILE_MG42_SET ),
	IntEnum( "SYRINGE", ET_WP_SYRINGE ),
	IntEnum( "MEDKIT", ET_WP_MEDKIT ),
	IntEnum( "ADRENALINE", ET_WP_ADRENALINE ),
	IntEnum( "STEN", ET_WP_STEN ),
	IntEnum( "FG42", ET_WP_FG42 ),
	IntEnum( "FG42_SCOPE", ET_WP_FG42_SCOPE ),
	IntEnum( "SATCHEL", ET_WP_SATCHEL ),
	IntEnum( "SATCHEL_DET", ET_WP_SATCHEL_DET ),
	IntEnum( "K43", ET_WP_K43 ),
	IntEnum( "K43_SCOPE", ET_WP_K43_SCOPE ),
	IntEnum( "GARAND", ET_WP_GARAND ),
	IntEnum( "GARAND_SCOPE", ET_WP_GARAND_SCOPE ),
	IntEnum( "AMMO_PACK", ET_WP_AMMO_PACK ),
	IntEnum( "SMOKE_GRENADE", ET_WP_SMOKE_GRENADE ),
	IntEnum( "SMOKE_MARKER", ET_WP_SMOKE_MARKER ),
	IntEnum( "DYNAMITE", ET_WP_DYNAMITE ),
	IntEnum( "PLIERS", ET_WP_PLIERS ),
	IntEnum( "LANDMINE", ET_WP_LANDMINE ),
	IntEnum( "KAR98", ET_WP_KAR98 ),
	IntEnum( "GPG40", ET_WP_GPG40 ),
	IntEnum( "CARBINE", ET_WP_CARBINE ),
	IntEnum( "M7", ET_WP_M7 ),
	IntEnum( "MOUNTABLE_MG42", ET_WP_MOUNTABLE_MG42 ),
};

void ET_Game::GetWeaponEnumeration( const IntEnum *&_ptr, int &num )
{
	const int arraySize = sizeof( ET_WeaponEnum ) / sizeof( ET_WeaponEnum[ 0 ] );
	int n = 0;
	for ( int i = 0; i < arraySize; ++i )
	{
		if ( ET_WeaponEnum[ i ].mKey )
		{
			++n;
		}
	}
	num = n;
	_ptr = ET_WeaponEnum;
}

bool ET_Game::AddWeaponId( const char * weaponName, int weaponId )
{
	/*const char * wpnName = mExtraWeaponNames.AddUniqueString( weaponName );

	const int arraySize = sizeof( ET_WeaponEnum ) / sizeof( ET_WeaponEnum[ 0 ] );
	for ( int i = 0; i < arraySize; ++i )
	{
		if ( !ET_WeaponEnum[ i ].mKey )
		{
			ET_WeaponEnum[ i ].mKey = wpnName;
			ET_WeaponEnum[ i ].mValue = weaponId;
			return true;
		}
	}*/
	return false;
}

static const IntEnum gClassMapping [] =
{
	IntEnum( "SOLDIER", ET_CLASS_SOLDIER ),
	IntEnum( "MEDIC", ET_CLASS_MEDIC ),
	IntEnum( "ENGINEER", ET_CLASS_ENGINEER ),
	IntEnum( "FIELDOPS", ET_CLASS_FIELDOPS ),
	IntEnum( "COVERTOPS", ET_CLASS_COVERTOPS ),
	IntEnum( "ANYPLAYER", ET_CLASS_ANY ),
	IntEnum( "MG42MOUNT", ET_CLASSEX_MG42MOUNT ),
	IntEnum( "DYNAMITE_ENT", ET_CLASSEX_DYNAMITE ),
	IntEnum( "LANDMINE_ENT", ET_CLASSEX_MINE ),
	IntEnum( "SATCHEL_ENT", ET_CLASSEX_SATCHEL ),
	IntEnum( "SMOKEBOMB", ET_CLASSEX_SMOKEBOMB ),
	IntEnum( "SMOKEMARKER", ET_CLASSEX_SMOKEMARKER ),
	IntEnum( "VEHICLE", ET_CLASSEX_VEHICLE ),
	IntEnum( "VEHICLE_HVY", ET_CLASSEX_VEHICLE_HVY ),
	IntEnum( "VEHICLE_NODAMAGE", ET_CLASSEX_VEHICLE_NODAMAGE ),
	IntEnum( "BREAKABLE", ET_CLASSEX_BREAKABLE ),
	IntEnum( "CORPSE", ET_CLASSEX_CORPSE ),
	//IntEnum("INJUREDPLAYER",	ET_CLASSEX_INJUREDPLAYER),
	IntEnum( "TRIPMINE", ET_CLASSEX_TREASURE ), // cs: to avoid breaking mod compat
	IntEnum( "ROCKET", ET_CLASSEX_ROCKET ),
	IntEnum( "MORTAR_ENT", ET_CLASSEX_MORTAR ),
	IntEnum( "FLAME", ET_CLASSEX_FLAMECHUNK ),
	IntEnum( "ARTY", ET_CLASSEX_ARTY ),
	IntEnum( "AIRSTRIKE", ET_CLASSEX_AIRSTRIKE ),
	IntEnum( "HEALTHCABINET", ET_CLASSEX_HEALTHCABINET ),
	IntEnum( "AMMOCABINET", ET_CLASSEX_AMMOCABINET ),
	IntEnum( "GRENADE", ET_CLASSEX_GRENADE ),
	IntEnum( "M7_GRENADE", ET_CLASSEX_M7_GRENADE ),
	IntEnum( "GPG40_GRENADE", ET_CLASSEX_GPG40_GRENADE ),
	IntEnum( "BROKENCHAIR", ET_CLASSEX_BROKENCHAIR ),
};

void ET_Game::InitScriptClasses( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptClasses( _machine, _table );

	FilterSensory::ANYPLAYERCLASS = ET_CLASS_ANY + ET_Game::CLASSEXoffset;

	const size_t iNumMappings = sizeof( gClassMapping ) / sizeof( gClassMapping[ 0 ] );
	for ( int i = 0; i < iNumMappings; ++i )
	{
		_table->Set( _machine, gClassMapping[ i ].mKey, gmVariable( gClassMapping[ i ].mValue ) );
	}

	//if ( ET_Game::CLASSEXoffset == 2 ) //ETBlight
	//{
	//	_table->Set( _machine, "SCIENTIST", gmVariable( 6 ) );
	//	_table->Set( _machine, "SUPER_SOLDIER", gmVariable( 7 ) );
	//}
}

void ET_Game::InitScriptSkills( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptSkills( _machine, _table );

	_table->Set( _machine, "BATTLE_SENSE", gmVariable( ET_SKILL_BATTLE_SENSE ) );
	_table->Set( _machine, "ENGINEERING", gmVariable( ET_SKILL_ENGINEERING ) );
	_table->Set( _machine, "FIRST_AID", gmVariable( ET_SKILL_FIRST_AID ) );
	_table->Set( _machine, "SIGNALS", gmVariable( ET_SKILL_SIGNALS ) );
	_table->Set( _machine, "LIGHT_WEAPONS", gmVariable( ET_SKILL_LIGHT_WEAPONS ) );
	_table->Set( _machine, "HEAVY_WEAPONS", gmVariable( ET_SKILL_HEAVY_WEAPONS ) );
	_table->Set( _machine, "COVERTOPS", gmVariable( ET_SKILL_COVERTOPS ) );
}

void ET_Game::InitVoiceMacros( gmMachine *_machine, gmTableObject *_table )
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
	_table->Set( _machine, "IMA_FIELDOPS", gmVariable( VCHAT_IMA_FIELDOPS ) );
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

	_table->Set( _machine, "P_DEPLOYMORTAR", gmVariable( VCHAT_PRIVATE_DEPLOYMORTAR ) );
	_table->Set( _machine, "P_HEALSQUAD", gmVariable( VCHAT_PRIVATE_HEALSQUAD ) );
	_table->Set( _machine, "P_HEALME", gmVariable( VCHAT_PRIVATE_HEALME ) );
	_table->Set( _machine, "P_REVIVETEAMMATE", gmVariable( VCHAT_PRIVATE_REVIVETEAMMATE ) );
	_table->Set( _machine, "P_REVIVEME", gmVariable( VCHAT_PRIVATE_REVIVEME ) );
	_table->Set( _machine, "P_DESTROYOBJECTIVE", gmVariable( VCHAT_PRIVATE_DESTROYOBJECTIVE ) );
	_table->Set( _machine, "P_REPAIROBJECTIVE", gmVariable( VCHAT_PRIVATE_REPAIROBJECTIVE ) );
	_table->Set( _machine, "P_CONSTRUCTOBJECTIVE", gmVariable( VCHAT_PRIVATE_CONSTRUCTOBJECTIVE ) );
	_table->Set( _machine, "P_DEPLOYLANDMINES", gmVariable( VCHAT_PRIVATE_DEPLOYLANDMINES ) );
	_table->Set( _machine, "P_DISARMLANDMINES", gmVariable( VCHAT_PRIVATE_DISARMLANDMINES ) );
	_table->Set( _machine, "P_CALLAIRSTRIKE", gmVariable( VCHAT_PRIVATE_CALLAIRSTRIKE ) );
	_table->Set( _machine, "P_CALLARTILLERY", gmVariable( VCHAT_PRIVATE_CALLARTILLERY ) );
	_table->Set( _machine, "P_RESUPPLYSQUAD", gmVariable( VCHAT_PRIVATE_RESUPPLYSQUAD ) );
	_table->Set( _machine, "P_RESUPPLYME", gmVariable( VCHAT_PRIVATE_RESUPPLYME ) );
	_table->Set( _machine, "P_EXPLOREAREA", gmVariable( VCHAT_PRIVATE_EXPLOREAREA ) );
	_table->Set( _machine, "P_CHECKFORLANDMINES", gmVariable( VCHAT_PRIVATE_CHECKFORLANDMINES ) );
	_table->Set( _machine, "P_SATCHELOBJECTIVE", gmVariable( VCHAT_PRIVATE_SATCHELOBJECTIVE ) );
	_table->Set( _machine, "P_INFILTRATE", gmVariable( VCHAT_PRIVATE_INFILTRATE ) );
	_table->Set( _machine, "P_GOUNDERCOVER", gmVariable( VCHAT_PRIVATE_GOUNDERCOVER ) );
	_table->Set( _machine, "P_PROVIDESNIPERCOVER", gmVariable( VCHAT_PRIVATE_PROVIDESNIPERCOVER ) );
	_table->Set( _machine, "P_ATTACK", gmVariable( VCHAT_PRIVATE_ATTACK ) );
	_table->Set( _machine, "P_FALLBACK", gmVariable( VCHAT_PRIVATE_FALLBACK ) );
}

void ET_Game::AddBot( ParamsAddbot & parms, bool createnow )
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
		// Magik: As there's no instant team/class switching in ET, this is order dependent
		// always call pfnChangeClass() _before_ pfnChangeTeam()!
		// todo: send the weapon preferences as 3rd param
		gEngineFuncs->ChangeTeam( iGameID, cp->mDesiredTeam, NULL );
		gEngineFuncs->ChangeClass( iGameID, cp->mDesiredClass, NULL );

		cp->CheckTeamEvent();
		cp->CheckClassEvent();

		ScriptManager::GetInstance()->ExecBotCallback( cp.get(), "SelectWeapons" );
	}
}

// package: ET Script Events
//		Custom Events for Enemy Territory. Also see <Common Script Events>
void ET_Game::InitScriptEvents( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptEvents( _machine, _table );

	_table->Set( _machine, "FIRETEAM_CHAT_MSG", gmVariable( MSG_HEAR_CHATMSG_GROUP ) );

	_table->Set( _machine, "FIRETEAM_CREATED", gmVariable( ET_MSG_FIRETEAM_CREATED ) );
	_table->Set( _machine, "FIRETEAM_DISBANDED", gmVariable( ET_MSG_FIRETEAM_DISBANDED ) );
	_table->Set( _machine, "FIRETEAM_JOINED", gmVariable( ET_MSG_FIRETEAM_JOINED ) );
	_table->Set( _machine, "FIRETEAM_LEFT", gmVariable( ET_MSG_FIRETEAM_LEFT ) );
	_table->Set( _machine, "FIRETEAM_INVITED", gmVariable( ET_MSG_FIRETEAM_INVITED ) );
	_table->Set( _machine, "FIRETEAM_PROPOSAL", gmVariable( ET_MSG_FIRETEAM_PROPOSAL ) );
	_table->Set( _machine, "FIRETEAM_WARNED", gmVariable( ET_MSG_FIRETEAM_WARNED ) );
	_table->Set( _machine, "AMMO_RECIEVED", gmVariable( ET_MSG_RECIEVEDAMMO ) );
}

void ET_Game::InitScriptEntityFlags( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptEntityFlags( _machine, _table );

	_table->Set( _machine, "DISGUISED", gmVariable( ET_ENT_FLAG_DISGUISED ) );
	_table->Set( _machine, "MOUNTED", gmVariable( ET_ENT_FLAG_MOUNTED ) );
	_table->Set( _machine, "MNT_MG42", gmVariable( ET_ENT_FLAG_MNT_MG42 ) );
	_table->Set( _machine, "MNT_TANK", gmVariable( ET_ENT_FLAG_MNT_TANK ) );
	_table->Set( _machine, "MNT_AAGUN", gmVariable( ET_ENT_FLAG_MNT_AAGUN ) );
	_table->Set( _machine, "CARRYINGGOAL", gmVariable( ET_ENT_FLAG_CARRYINGGOAL ) );
	_table->Set( _machine, "LIMBO", gmVariable( ET_ENT_FLAG_INLIMBO ) );
	_table->Set( _machine, "MOUNTABLE", gmVariable( ET_ENT_FLAG_ISMOUNTABLE ) );
	_table->Set( _machine, "POISONED", gmVariable( ET_ENT_FLAG_POISONED ) );
}

void ET_Game::InitScriptPowerups( gmMachine *_machine, gmTableObject *_table )
{
}

void ET_Game::InitCommands()
{
	IGame::InitCommands();
}

/*
bounding boxes for et
standing	(-18, -18, -24) x (18, 18, 48)
crouched	(-18, -18, -24) x (18, 18, 24)
proned		(-18, -18, -24) x (18, 18, 16)
*/
const float ET_Game::ET_GetEntityClassTraceOffset( const TargetInfo &_target )
{
	if ( _target.mEntInfo.mGroup == ENT_GRP_PLAYER )
	{
		if ( _target.mEntInfo.mClassId > ET_CLASS_NULL && _target.mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS )
		{
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_PRONED ) )
				return 16.0f;
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_CROUCHED ) )
				return 24.0f;
			return 48.0f;
		}
	}

	/*switch ( _classs - ET_Game::CLASSEXoffset )
	{
	case ET_CLASSEX_DYNAMITE:
	case ET_CLASSEX_MINE:
	case ET_CLASSEX_SATCHEL:
	case ET_CLASSEX_SMOKEBOMB:
	case ET_CLASSEX_CORPSE:
	return 2.0f;
	}*/

	return 0.0f;
}

/*
bounding boxes for et
standing	(-18, -18, -24) x (18, 18, 48)
crouched	(-18, -18, -24) x (18, 18, 24)
proned		(-18, -18, -24) x (18, 18, 16)
*/
const float ET_Game::ET_GetEntityClassAimOffset( const TargetInfo &_target )
{
	if ( _target.mEntInfo.mGroup == ENT_GRP_PLAYER )
	{
		if ( _target.mEntInfo.mClassId > ET_CLASS_NULL && _target.mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS )
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

void ET_Game::ClientJoined( const EvClientConnected * msg )
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
