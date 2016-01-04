////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "TF_Game.h"
#include "TF_Messages.h"
#include "System.h"

#include "NameManager.h"

#include "PathPlannerBase.h"
#include "BotSensoryMemory.h"
#include "BotSteeringSystem.h"
#include "FilterSensory.h"

TeamFortress_Interface* gTeamFortressFuncs = 0;

TF_Game::TF_Game()
{
}

TF_Game::~TF_Game()
{
}

bool TF_Game::Init( System & system )
{
	AiState::SensoryMemory::SetEntityTraceOffsetCallback( TF_Game::TF_GetEntityClassTraceOffset );
	AiState::SensoryMemory::SetEntityAimOffsetCallback( TF_Game::TF_GetEntityClassAimOffset );

	gTeamFortressFuncs = dynamic_cast<TeamFortress_Interface*>(gEngineFuncs);

	return IGame::Init( system );
}

void TF_Game::Shutdown()
{
	IGame::Shutdown();
}

static const IntEnum TF_WeaponEnum[] =
{
	IntEnum( "NONE", TF_WP_NONE ),
	IntEnum( "AXE", TF_WP_AXE ),
	IntEnum( "CROWBAR", TF_WP_CROWBAR ),
	IntEnum( "KNIFE", TF_WP_KNIFE ),
	IntEnum( "MEDKIT", TF_WP_MEDKIT ),
	IntEnum( "SPANNER", TF_WP_SPANNER ),
	IntEnum( "SHOTGUN", TF_WP_SHOTGUN ),
	IntEnum( "SUPERSHOTGUN", TF_WP_SUPERSHOTGUN ),
	IntEnum( "NAILGUN", TF_WP_NAILGUN ),
	IntEnum( "SUPERNAILGUN", TF_WP_SUPERNAILGUN ),
	IntEnum( "GRENADELAUNCHER", TF_WP_GRENADE_LAUNCHER ),
	IntEnum( "RPG", TF_WP_ROCKET_LAUNCHER ),
	IntEnum( "SNIPERRIFLE", TF_WP_SNIPER_RIFLE ),
	IntEnum( "RAILGUN", TF_WP_RAILGUN ),
	IntEnum( "FLAMETHROWER", TF_WP_FLAMETHROWER ),
	IntEnum( "ASSAULTCANNON", TF_WP_MINIGUN ),
	IntEnum( "AUTORIFLE", TF_WP_AUTORIFLE ),
	IntEnum( "TRANQGUN", TF_WP_DARTGUN ),
	IntEnum( "PIPELAUNCHER", TF_WP_PIPELAUNCHER ),
	IntEnum( "NAPALMCANNON", TF_WP_NAPALMCANNON ),
	IntEnum( "UMBRELLA", TF_WP_UMBRELLA ),
	IntEnum( "GRENADE1", TF_WP_GRENADE1 ),
	IntEnum( "GRENADE2", TF_WP_GRENADE2 ),
	IntEnum( "GRENADE", TF_WP_GRENADE ),
	IntEnum( "GRENADE_CONC", TF_WP_GRENADE_CONC ),
	IntEnum( "GRENADE_EMP", TF_WP_GRENADE_EMP ),
	IntEnum( "GRENADE_NAIL", TF_WP_GRENADE_NAIL ),
	IntEnum( "GRENADE_MIRV", TF_WP_GRENADE_MIRV ),
	IntEnum( "GRENADE_GAS", TF_WP_GRENADE_GAS ),
	IntEnum( "GRENADE_CALTROPS", TF_WP_GRENADE_CALTROPS ),
	IntEnum( "GRENADE_NAPALM", TF_WP_GRENADE_NAPALM ),
	IntEnum( "DEPLOY_SENTRY", TF_WP_DEPLOY_SG ),
	IntEnum( "DEPLOY_DISPENSER", TF_WP_DEPLOY_DISP ),
	IntEnum( "DEPLOY_DETPACK", TF_WP_DEPLOY_DETP ),
	IntEnum( "DEPLOY_JUMPPAD", TF_WP_DEPLOY_JUMPPAD ),
};

void TF_Game::GetWeaponEnumeration( const IntEnum *&_ptr, int &num )
{
	num = sizeof( TF_WeaponEnum ) / sizeof( TF_WeaponEnum[ 0 ] );
	_ptr = TF_WeaponEnum;
}

static const IntEnum gClassMapping[] =
{
	IntEnum( "NONE", TF_CLASS_NONE ),
	IntEnum( "SCOUT", TF_CLASS_SCOUT ),
	IntEnum( "SNIPER", TF_CLASS_SNIPER ),
	IntEnum( "SOLDIER", TF_CLASS_SOLDIER ),
	IntEnum( "DEMOMAN", TF_CLASS_DEMOMAN ),
	IntEnum( "MEDIC", TF_CLASS_MEDIC ),
	IntEnum( "HWGUY", TF_CLASS_HWGUY ),
	IntEnum( "PYRO", TF_CLASS_PYRO ),
	IntEnum( "SPY", TF_CLASS_SPY ),
	IntEnum( "ENGINEER", TF_CLASS_ENGINEER ),
	IntEnum( "CIVILIAN", TF_CLASS_CIVILIAN ),
	IntEnum( "ANYPLAYER", TF_CLASS_ANY ),
	IntEnum( "SENTRY", TF_CLASSEX_SENTRY ),
	IntEnum( "DISPENSER", TF_CLASSEX_DISPENSER ),
	IntEnum( "TELEPORTER_ENTRANCE", TF_CLASSEX_TELEPORTER_ENTRANCE ),
	IntEnum( "TELEPORTER_EXIT", TF_CLASSEX_TELEPORTER_EXIT ),
	IntEnum( "DETPACK", TF_CLASSEX_DETPACK ),
	IntEnum( "PIPEBOMB", TF_CLASSEX_PIPE ),
	IntEnum( "GLGREN", TF_CLASSEX_GLGRENADE ),
	IntEnum( "ROCKET", TF_CLASSEX_ROCKET ),
	IntEnum( "NAPALM_ROCKET", TF_CLASSEX_NAPALM ),
	IntEnum( "SYRINGE", TF_CLASSEX_SYRINGE ),
	IntEnum( "GRENADE", TF_CLASSEX_GRENADE ),
	IntEnum( "EMP_GRENADE", TF_CLASSEX_EMP_GRENADE ),
	IntEnum( "NAIL_GRENADE", TF_CLASSEX_NAIL_GRENADE ),
	IntEnum( "MIRV_GRENADE", TF_CLASSEX_MIRV_GRENADE ),
	IntEnum( "MIRVLET_GRENADE", TF_CLASSEX_MIRVLET_GRENADE ),
	IntEnum( "NAPALM_GRENADE", TF_CLASSEX_NAPALM_GRENADE ),
	IntEnum( "GAS_GRENADE", TF_CLASSEX_GAS_GRENADE ),
	IntEnum( "CONC_GRENADE", TF_CLASSEX_CONC_GRENADE ),
	IntEnum( "CALTROP", TF_CLASSEX_CALTROP ),
	IntEnum( "TURRET", TF_CLASSEX_TURRET ),
	IntEnum( "VEHICLE", TF_CLASSEX_VEHICLE ),
	IntEnum( "VEHICLE_NODAMAGE", TF_CLASSEX_VEHICLE_NODAMAGE ),
};

void TF_Game::InitScriptClasses( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptClasses( _machine, _table );

	FilterSensory::ANYPLAYERCLASS = TF_CLASS_ANY;

	int32_t iNumMappings = sizeof( gClassMapping ) / sizeof( gClassMapping[ 0 ] );
	for ( int i = 0; i < iNumMappings; ++i )
	{
		_table->Set( _machine, gClassMapping[ i ].mKey, gmVariable( gClassMapping[ i ].mValue ) );
	}
}

void TF_Game::InitScriptEntityFlags( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptEntityFlags( _machine, _table );

	_table->Set( _machine, "NEED_HEALTH", gmVariable( TF_ENT_FLAG_SAVEME ) );
	_table->Set( _machine, "NEED_ARMOR", gmVariable( TF_ENT_FLAG_ARMORME ) );
	_table->Set( _machine, "BURNING", gmVariable( TF_ENT_FLAG_BURNING ) );
	_table->Set( _machine, "TRANQUED", gmVariable( TF_ENT_FLAG_TRANQED ) );
	_table->Set( _machine, "INFECTED", gmVariable( TF_ENT_FLAG_INFECTED ) );
	_table->Set( _machine, "GASSED", gmVariable( TF_ENT_FLAG_GASSED ) );
	_table->Set( _machine, "SNIPE_AIMING", gmVariable( ENT_FLAG_IRONSIGHT ) );
	_table->Set( _machine, "AC_FIRING", gmVariable( TF_ENT_FLAG_ASSAULTFIRING ) );
	_table->Set( _machine, "LEGSHOT", gmVariable( TF_ENT_FLAG_LEGSHOT ) );
	_table->Set( _machine, "CALTROP", gmVariable( TF_ENT_FLAG_CALTROP ) );
	_table->Set( _machine, "RADIOTAGGED", gmVariable( TF_ENT_FLAG_RADIOTAGGED ) );
	_table->Set( _machine, "CAN_SABOTAGE", gmVariable( TF_ENT_FLAG_CAN_SABOTAGE ) );
	_table->Set( _machine, "SABOTAGED", gmVariable( TF_ENT_FLAG_SABOTAGED ) );
	_table->Set( _machine, "SABOTAGED2", gmVariable( TF_ENT_FLAG_SABOTAGED2 ) );
	_table->Set( _machine, "SABOTAGING", gmVariable( TF_ENT_FLAG_SABOTAGING ) );
	_table->Set( _machine, "MALFUNCTION", gmVariable( TF_ENT_FLAG_MALFUNCTION ) );
	_table->Set( _machine, "BUILDING_SG", gmVariable( TF_ENT_FLAG_BUILDING_SG ) );
	_table->Set( _machine, "BUILDING_DISP", gmVariable( TF_ENT_FLAG_BUILDING_DISP ) );
	_table->Set( _machine, "BUILDING_DETP", gmVariable( TF_ENT_FLAG_BUILDING_DETP ) );

	_table->Set( _machine, "BUILDING_ENTRANCE", gmVariable( TF_ENT_FLAG_BUILDING_ENTRANCE ) );
	_table->Set( _machine, "BUILDING_EXIT", gmVariable( TF_ENT_FLAG_BUILDING_EXIT ) );
	_table->Set( _machine, "BUILDINPROGRESS", gmVariable( TF_ENT_FLAG_BUILDINPROGRESS ) );
}

void TF_Game::InitScriptPowerups( gmMachine *_machine, gmTableObject *_table )
{
	_table->Set( _machine, "DISGUISE_BLUE", gmVariable( TF_PWR_DISGUISE_BLUE ) );
	_table->Set( _machine, "DISGUISE_RED", gmVariable( TF_PWR_DISGUISE_RED ) );
	_table->Set( _machine, "DISGUISE_YELLOW", gmVariable( TF_PWR_DISGUISE_YELLOW ) );
	_table->Set( _machine, "DISGUISE_GREEN", gmVariable( TF_PWR_DISGUISE_GREEN ) );
	_table->Set( _machine, "DISGUISE_SCOUT", gmVariable( TF_PWR_DISGUISE_SCOUT ) );
	_table->Set( _machine, "DISGUISE_SNIPER", gmVariable( TF_PWR_DISGUISE_SNIPER ) );
	_table->Set( _machine, "DISGUISE_SOLDIER", gmVariable( TF_PWR_DISGUISE_SOLDIER ) );
	_table->Set( _machine, "DISGUISE_DEMOMAN", gmVariable( TF_PWR_DISGUISE_DEMOMAN ) );
	_table->Set( _machine, "DISGUISE_MEDIC", gmVariable( TF_PWR_DISGUISE_MEDIC ) );
	_table->Set( _machine, "DISGUISE_HWGUY", gmVariable( TF_PWR_DISGUISE_HWGUY ) );
	_table->Set( _machine, "DISGUISE_PYRO", gmVariable( TF_PWR_DISGUISE_PYRO ) );
	_table->Set( _machine, "DISGUISE_ENGINEER", gmVariable( TF_PWR_DISGUISE_ENGINEER ) );
	_table->Set( _machine, "DISGUISE_SPY", gmVariable( TF_PWR_DISGUISE_SPY ) );
	_table->Set( _machine, "DISGUISE_CIVILIAN", gmVariable( TF_PWR_DISGUISE_CIVILIAN ) );
}

// package: TF Script Events
//		Custom Events for Team Fortress Games. Also see <Common Script Events>
void TF_Game::InitScriptEvents( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptEvents( _machine, _table );

	_table->Set( _machine, "GOT_ENGY_ARMOR", gmVariable( TF_MSG_GOT_ENGY_ARMOR ) );
	_table->Set( _machine, "GAVE_ENGY_ARMOR", gmVariable( TF_MSG_GAVE_ENGY_ARMOR ) );
	_table->Set( _machine, "GOT_MEDIC_HEALTH", gmVariable( TF_MSG_GOT_MEDIC_HEALTH ) );
	_table->Set( _machine, "GAVE_MEDIC_HEALTH", gmVariable( TF_MSG_GAVE_MEDIC_HEALTH ) );
	_table->Set( _machine, "GOT_DISPENSER_AMMO", gmVariable( TF_MSG_GOT_DISPENSER_AMMO ) );
	_table->Set( _machine, "INFECTED", gmVariable( TF_MSG_INFECTED ) );
	_table->Set( _machine, "CURED", gmVariable( TF_MSG_CURED ) );
	_table->Set( _machine, "BURNED", gmVariable( TF_MSG_BURNLEVEL ) );

	_table->Set( _machine, "BUILD_NOTENOUGHAMMO", gmVariable( TF_MSG_BUILD_NOTENOUGHAMMO ) );
	_table->Set( _machine, "BUILD_ALREADYBUILT", gmVariable( TF_MSG_BUILD_ALREADYBUILT ) );
	_table->Set( _machine, "BUILD_CANTBUILDHERE", gmVariable( TF_MSG_BUILD_CANTBUILDHERE ) );
	_table->Set( _machine, "BUILD_BUILDING", gmVariable( TF_MSG_BUILD_BUILDING ) );
	_table->Set( _machine, "BUILD_BUILT", gmVariable( TF_MSG_BUILD_BUILT ) );
	_table->Set( _machine, "BUILD_BUILDCANCEL", gmVariable( TF_MSG_BUILD_BUILDCANCEL ) );
	_table->Set( _machine, "BUILD_DESTROYED", gmVariable( TF_MSG_BUILD_DESTROYED ) );
	_table->Set( _machine, "BUILD_SPOTENEMY", gmVariable( TF_MSG_BUILD_SPOTENEMY ) );
	_table->Set( _machine, "BUILD_AIMED", gmVariable( TF_MSG_BUILD_AIMED ) );
	_table->Set( _machine, "BUILD_DAMAGED", gmVariable( TF_MSG_BUILD_DAMAGED ) );
	_table->Set( _machine, "BUILD_UPGRADED", gmVariable( TF_MSG_BUILD_UPGRADED ) );
	_table->Set( _machine, "BUILD_DETONATED", gmVariable( TF_MSG_BUILD_DETONATED ) );
	_table->Set( _machine, "BUILD_DISMANTLED", gmVariable( TF_MSG_BUILD_DISMANTLED ) );
	_table->Set( _machine, "BUILD_SABOTAGED", gmVariable( TF_MSG_BUILD_SABOTAGED ) );
	_table->Set( _machine, "BUILD_ENEMYUSED", gmVariable( TF_MSG_BUILD_ENEMYUSED ) );
	_table->Set( _machine, "BUILD_DETONATE", gmVariable( TF_MSG_BUILD_DETONATE ) );
		
	// General Events
	_table->Set( _machine, "CLASS_DISABLE", gmVariable( TF_MSG_CLASS_DISABLED ) );
	_table->Set( _machine, "CLASS_NOTAVAILABLE", gmVariable( TF_MSG_CLASS_NOTAVAILABLE ) );
	_table->Set( _machine, "CLASS_CHANGELATER", gmVariable( TF_MSG_CLASS_CHANGELATER ) );

	// event: BUILD_NOTONGROUND
	//		The bot must be on ground to build(currently isn't).
	_table->Set( _machine, "BUILD_NOTONGROUND", gmVariable( TF_MSG_BUILD_MUSTBEONGROUND ) );
	// event: RADAR_DETECTENEMY
	//		Radar detected enemy entity.
	//
	// Parameters:
	//		detected - entity who was detected.
	_table->Set( _machine, "RADAR_DETECTENEMY", gmVariable( TF_MSG_RADAR_DETECT_ENEMY ) );
	// event: RADIOTAG_UPDATE
	//		Radio tag detected enemy entity.
	//
	// Parameters:
	//		detected - entity who was detected.
	_table->Set( _machine, "RADIOTAG_UPDATE", gmVariable( TF_MSG_RADIOTAG_UPDATE ) );

	// Demo-man
	_table->Set( _machine, "PIPE_PROXIMITY", gmVariable( TF_MSG_PIPE_PROXIMITY ) );

	//TF_MSG_DETPIPES,		// The bot has detected the desire to det pipes.
	//TF_MSG_DETPIPESNOW,		// Configurable delayed message for the actual detting.
	//TF_MSG_DEMOMAN_END,

	// Medic
	_table->Set( _machine, "MEDIC_CALLED", gmVariable( TF_MSG_CALLEDFORMEDIC ) );

	// HW-Guy

	// Pyro

	// Spy
	_table->Set( _machine, "INVALID_DISGUISE_TEAM", gmVariable( TF_MSG_CANTDISGUISE_AS_TEAM ) );
	_table->Set( _machine, "INVALID_DISGUISE_CLASS", gmVariable( TF_MSG_CANTDISGUISE_AS_CLASS ) );
	_table->Set( _machine, "DISGUISING", gmVariable( TF_MSG_DISGUISING ) );
	_table->Set( _machine, "DISGUISED", gmVariable( TF_MSG_DISGUISED ) );
	_table->Set( _machine, "DISGUISE_LOST", gmVariable( TF_MSG_DISGUISE_LOST ) );
	_table->Set( _machine, "CANT_CLOAK", gmVariable( TF_MSG_CANT_CLOAK ) );
	_table->Set( _machine, "CLOAKED", gmVariable( TF_MSG_CLOAKED ) );
	_table->Set( _machine, "UNCLOAKED", gmVariable( TF_MSG_UNCLOAKED ) );
	_table->Set( _machine, "SABOTAGED_SENTRY", gmVariable( TF_MSG_SABOTAGED_SENTRY ) );
	_table->Set( _machine, "SABOTAGED_DISPENSER", gmVariable( TF_MSG_SABOTAGED_DISPENSER ) );

	// Engineer
	_table->Set( _machine, "ENGINEER_CALLED", gmVariable( TF_MSG_CALLEDFORENGINEER ) );
	
	// Civilian
}

void TF_Game::InitScriptBotButtons( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptBotButtons( _machine, _table );

	_table->Set( _machine, "GREN1", gmVariable( TF_BOT_BUTTON_GREN1 ) );
	_table->Set( _machine, "GREN2", gmVariable( TF_BOT_BUTTON_GREN2 ) );

	_table->Set( _machine, "DROP_ITEM", gmVariable( TF_BOT_BUTTON_DROPITEM ) );
	_table->Set( _machine, "DROP_AMMO", gmVariable( TF_BOT_BUTTON_DROPAMMO ) );

	_table->Set( _machine, "BUILDSENTRY", gmVariable( TF_BOT_BUTTON_BUILDSENTRY ) );
	_table->Set( _machine, "BUILDDISPENSER", gmVariable( TF_BOT_BUTTON_BUILDDISPENSER ) );
	_table->Set( _machine, "BUILDDETPACK_5", gmVariable( TF_BOT_BUTTON_BUILDDETPACK_5 ) );
	_table->Set( _machine, "BUILDDETPACK_10", gmVariable( TF_BOT_BUTTON_BUILDDETPACK_10 ) );
	_table->Set( _machine, "BUILDDETPACK_20", gmVariable( TF_BOT_BUTTON_BUILDDETPACK_20 ) );
	_table->Set( _machine, "BUILDDETPACK_30", gmVariable( TF_BOT_BUTTON_BUILDDETPACK_30 ) );

	_table->Set( _machine, "AIMSENTRY", gmVariable( TF_BOT_BUTTON_AIMSENTRY ) );

	_table->Set( _machine, "DETSENTRY", gmVariable( TF_BOT_BUTTON_DETSENTRY ) );
	_table->Set( _machine, "DETDISPENSER", gmVariable( TF_BOT_BUTTON_DETDISPENSER ) );
	_table->Set( _machine, "DETPIPES", gmVariable( TF_BOT_BUTTON_DETPIPES ) );

	_table->Set( _machine, "CALLFORMEDIC", gmVariable( TF_BOT_BUTTON_CALLFORMEDIC ) );
	_table->Set( _machine, "CALLFORENGY", gmVariable( TF_BOT_BUTTON_CALLFORENGY ) );

	_table->Set( _machine, "SABOTAGE_SENTRY", gmVariable( TF_BOT_BUTTON_SABOTAGE_SENTRY ) );
	_table->Set( _machine, "SABOTAGE_DISPENSER", gmVariable( TF_BOT_BUTTON_SABOTAGE_DISPENSER ) );

	_table->Set( _machine, "CLOAK", gmVariable( TF_BOT_BUTTON_CLOAK ) );
	_table->Set( _machine, "SILENT_CLOAK", gmVariable( TF_BOT_BUTTON_SILENT_CLOAK ) );

	_table->Set( _machine, "RADAR", gmVariable( TF_BOT_BUTTON_RADAR ) );
}

const float TF_Game::TF_GetEntityClassTraceOffset( const TargetInfo &_target )
{
	if ( _target.mEntInfo.mGroup == ENT_GRP_PLAYER )
	{
		if ( _target.mEntInfo.mClassId > TF_CLASS_NONE && _target.mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS )
		{
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_PRONED ) )
				return 34.0f;
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_CROUCHED ) )
				return 24.0f;
			return 48.0f;
		}
	}
	else if ( _target.mEntInfo.mGroup == ENT_GRP_BUILDABLE )
	{
		switch ( _target.mEntInfo.mClassId )
		{
		case TF_CLASSEX_SENTRY:
		case TF_CLASSEX_DISPENSER:
			return 32.f;
		}
	}
	return 0.0f;
}

const float TF_Game::TF_GetEntityClassAimOffset( const TargetInfo &_target )
{
	if ( _target.mEntInfo.mGroup == ENT_GRP_PLAYER )
	{
		if ( _target.mEntInfo.mClassId > TF_CLASS_NONE && _target.mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS )
		{
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_PRONED ) )
				return 34.0f;
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_CROUCHED ) )
				return 24.0f;
			return 48.0f;
		}
	}
	else if ( _target.mEntInfo.mGroup == ENT_GRP_BUILDABLE )
	{
		switch ( _target.mEntInfo.mClassId )
		{
		case TF_CLASSEX_SENTRY:
		case TF_CLASSEX_DISPENSER:
			return 32.f;
		}
	}
	return 0.0f;
}

void TF_Game::ProcessEvent( const Message &_message, CallbackParameters &_cb )
{
	IGame::ProcessEvent( _message, _cb );

	//if ( System::mInstance->mNavigation->IsViewOn() && System::mInstance->mNavigation->IsAutoDetectFlagsOn() )
	//{
	//	switch ( _message.GetMessageId() )
	//	{
	//		case GAME_ENTITYCREATED:
	//		{
	//			const Event_EntityCreated *m = _message.Get<Event_EntityCreated>();
	//			if ( m )
	//			{
	//				//if ( m->mEntityClass == TF_CLASSEX_SENTRY ||
	//				//	m->mEntityClass == TF_CLASSEX_DISPENSER ||
	//				//	m->mEntityClass == TF_CLASSEX_DETPACK )
	//				//{
	//				//	Vector3f vEntPosition, vEntFacing, vWpPosition, vWpFacing;
	//				//	EngineFuncs::EntityPosition( m->mEntity, vEntPosition );
	//				//	EngineFuncs::EntityOrientation( m->mEntity, vEntFacing, NULL, NULL );

	//				//	EngineFuncs::EntityPosition( Utils::GetLocalEntity(), vWpPosition );
	//				//	EngineFuncs::EntityOrientation( Utils::GetLocalEntity(), vWpFacing, NULL, NULL );

	//				//	//Utils::GetLocalEntity();
	//				//	//RenderBuffer::AddLine(GetClient()->GetEyePosition(), _aimpos, COLOR::GREEN, 20.f);
	//				//}
	//			}
	//			break;
	//		}
	//		case GAME_ENTITYDELETED:
	//		{
	//			const Event_EntityDeleted *m = _message.Get<Event_EntityDeleted>();
	//			if ( m )
	//			{
	//				/*int index = m->mEntity.GetIndex();
	//				if(.mGameEntities[index]..mEntity.IsValid())
	//				{
	//				#ifdef _DEBUG
	//				const char *pClassName = FindClassName(.mGameEntities[index]..mEntityClass);
	//				Utils::OutputDebug(kNormal, "Entity: %d deleted: %s\n", index, pClassName?pClassName:"<unknown>");
	//				#endif
	//				.mGameEntities[index]..mEntity.Reset();
	//				.mGameEntities[index]..mEntityClass = 0;
	//				.mGameEntities[index]..mEntityCategory.ClearAll();
	//				}

	//				System::mInstance->mGoalManager->RemoveGoalByEntity(m->mEntity);*/
	//			}
	//			break;
	//		}
	//	}
	//}

	//////////////////////////////////////////////////////////////////////////
}

//////////////////////////////////////////////////////////////////////////

float TF_Game::_GetDesirabilityFromTargetClass( int _grentype, int _class )
{
	switch ( _grentype )
	{
	case TF_WP_GRENADE:
	{
		switch ( _class )
		{
		case TF_CLASS_SCOUT: return 0.4f;
		case TF_CLASS_SNIPER: return 0.4f;
		case TF_CLASS_SOLDIER: return 0.55f;
		case TF_CLASS_DEMOMAN: return 0.5f;
		case TF_CLASS_MEDIC: return 0.45f;
		case TF_CLASS_HWGUY: return 0.65f;
		case TF_CLASS_PYRO: return 0.45f;
		case TF_CLASS_SPY: return 0.5f;
		case TF_CLASS_ENGINEER: return 0.4f;
		case TF_CLASS_CIVILIAN: return 0.3f;
		case TF_CLASSEX_SENTRY: return 0.5f;
		case TF_CLASSEX_DISPENSER: return 0.5f;
		default: return 0.f;
		}
	}
	case TF_WP_GRENADE_CONC:
	{
		switch ( _class )
		{
		case TF_CLASS_SCOUT: return 0.4f;
		case TF_CLASS_SNIPER: return 0.55f;
		case TF_CLASS_SOLDIER: return 0.5f;
		case TF_CLASS_DEMOMAN: return 0.5f;
		case TF_CLASS_MEDIC: return 0.45f;
		case TF_CLASS_HWGUY: return 0.7f;
		case TF_CLASS_PYRO: return 0.45f;
		case TF_CLASS_SPY: return 0.5f;
		case TF_CLASS_ENGINEER: return 0.5f;
		case TF_CLASS_CIVILIAN: return 0.3f;
		case TF_CLASSEX_SENTRY: return 0.f;
		case TF_CLASSEX_DISPENSER: return 0.f;
		default: return 0.f;
		}
	}
	case TF_WP_GRENADE_EMP:
	{
		switch ( _class )
		{
		case TF_CLASS_SCOUT: return 0.4f;
		case TF_CLASS_SNIPER: return 0.55f;
		case TF_CLASS_SOLDIER: return 0.5f;
		case TF_CLASS_DEMOMAN: return 0.5f;
		case TF_CLASS_MEDIC: return 0.45f;
		case TF_CLASS_HWGUY: return 0.65f;
		case TF_CLASS_PYRO: return 0.55f;
		case TF_CLASS_SPY: return 0.5f;
		case TF_CLASS_ENGINEER: return 0.4f;
		case TF_CLASS_CIVILIAN: return 0.3f;
		case TF_CLASSEX_SENTRY: return 0.6f;
		case TF_CLASSEX_DISPENSER: return 0.6f;
		default: return 0.f;
		}
	}
	case TF_WP_GRENADE_NAIL:
	{
		switch ( _class )
		{
		case TF_CLASS_SCOUT: return 0.4f;
		case TF_CLASS_SNIPER: return 0.35f;
		case TF_CLASS_SOLDIER: return 0.55f;
		case TF_CLASS_DEMOMAN: return 0.5f;
		case TF_CLASS_MEDIC: return 0.45f;
		case TF_CLASS_HWGUY: return 0.65f;
		case TF_CLASS_PYRO: return 0.45f;
		case TF_CLASS_SPY: return 0.4f;
		case TF_CLASS_ENGINEER: return 0.4f;
		case TF_CLASS_CIVILIAN: return 0.3f;
		case TF_CLASSEX_SENTRY: return 0.6f;
		case TF_CLASSEX_DISPENSER: return 0.6f;
		default: return 0.f;
		}
	}
	case TF_WP_GRENADE_MIRV:
	{
		switch ( _class )
		{
		case TF_CLASS_SCOUT: return 0.25f;
		case TF_CLASS_SNIPER: return 0.35f;
		case TF_CLASS_SOLDIER: return 0.55f;
		case TF_CLASS_DEMOMAN: return 0.5f;
		case TF_CLASS_MEDIC: return 0.45f;
		case TF_CLASS_HWGUY: return 0.65f;
		case TF_CLASS_PYRO: return 0.45f;
		case TF_CLASS_SPY: return 0.4f;
		case TF_CLASS_ENGINEER: return 0.4f;
		case TF_CLASS_CIVILIAN: return 0.3f;
		case TF_CLASSEX_SENTRY: return 0.6f;
		case TF_CLASSEX_DISPENSER: return 0.6f;
		default: return 0.f;
		}
	}
	case TF_WP_GRENADE_GAS:
	{
		switch ( _class )
		{
		case TF_CLASS_SCOUT: return 0.2f;
		case TF_CLASS_SNIPER: return 0.4f;
		case TF_CLASS_SOLDIER: return 0.45f;
		case TF_CLASS_DEMOMAN: return 0.45f;
		case TF_CLASS_MEDIC: return 0.45f;
		case TF_CLASS_HWGUY: return 0.55f;
		case TF_CLASS_PYRO: return 0.45f;
		case TF_CLASS_SPY: return 0.5f;
		case TF_CLASS_ENGINEER: return 0.4f;
		case TF_CLASS_CIVILIAN: return 0.3f;
		case TF_CLASSEX_SENTRY: return 0.f;
		case TF_CLASSEX_DISPENSER: return 0.f;
		default: return 0.f;
		}
	}
	case TF_WP_GRENADE_CALTROPS:
	{
		switch ( _class )
		{
		case TF_CLASS_SCOUT: return 0.4f;
		case TF_CLASS_SNIPER: return 0.4f;
		case TF_CLASS_SOLDIER: return 0.55f;
		case TF_CLASS_DEMOMAN: return 0.5f;
		case TF_CLASS_MEDIC: return 0.45f;
		case TF_CLASS_HWGUY: return 0.65f;
		case TF_CLASS_PYRO: return 0.45f;
		case TF_CLASS_SPY: return 0.5f;
		case TF_CLASS_ENGINEER: return 0.4f;
		case TF_CLASS_CIVILIAN: return 0.3f;
		case TF_CLASSEX_SENTRY: return 0.f;
		case TF_CLASSEX_DISPENSER: return 0.f;
		default: return 0.f;
		}
	}
	case TF_WP_GRENADE_NAPALM:
	{
		switch ( _class )
		{
		case TF_CLASS_SCOUT: return 0.4f;
		case TF_CLASS_SNIPER: return 0.4f;
		case TF_CLASS_SOLDIER: return 0.55f;
		case TF_CLASS_DEMOMAN: return 0.5f;
		case TF_CLASS_MEDIC: return 0.45f;
		case TF_CLASS_HWGUY: return 0.65f;
		case TF_CLASS_PYRO: return 0.45f;
		case TF_CLASS_SPY: return 0.5f;
		case TF_CLASS_ENGINEER: return 0.4f;
		case TF_CLASS_CIVILIAN: return 0.3f;
		case TF_CLASSEX_SENTRY: return 0.2f;
		case TF_CLASSEX_DISPENSER: return 0.2f;
		default: return 0.f;
		}
	}
	}
	return 0.f;
}
