////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// Title: TF Config
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TF_EVENTS_H__
#define __TF_EVENTS_H__

#include "Omni-Bot_Types.h"
#include "Omni-Bot_Events.h"
#include "IEngineInterface.h"

// enumerations: TF_ButtonFlags
enum TF_ButtonFlags
{
	TF_BOT_BUTTON_GREN1 = BOT_BUTTON_FIRSTUSER,
	TF_BOT_BUTTON_GREN2,
	TF_BOT_BUTTON_DROPITEM,
	TF_BOT_BUTTON_DROPAMMO,
	TF_BOT_BUTTON_BUILDSENTRY,
	TF_BOT_BUTTON_BUILDDISPENSER,
	TF_BOT_BUTTON_BUILD_TELE_ENTRANCE,
	TF_BOT_BUTTON_BUILD_TELE_EXIT,
	TF_BOT_BUTTON_BUILDDETPACK_5,
	TF_BOT_BUTTON_BUILDDETPACK_10,
	TF_BOT_BUTTON_BUILDDETPACK_20,
	TF_BOT_BUTTON_BUILDDETPACK_30,
	TF_BOT_BUTTON_AIMSENTRY,
	TF_BOT_BUTTON_DETSENTRY,
	TF_BOT_BUTTON_DETDISPENSER,
	TF_BOT_BUTTON_DET_TELE_ENTRANCE,
	TF_BOT_BUTTON_DET_TELE_EXIT,
	TF_BOT_BUTTON_DETPIPES,
	TF_BOT_BUTTON_CALLFORMEDIC,
	TF_BOT_BUTTON_CALLFORENGY,
	TF_BOT_BUTTON_SABOTAGE_SENTRY,
	TF_BOT_BUTTON_SABOTAGE_DISPENSER,
	TF_BOT_BUTTON_CLOAK,
	TF_BOT_BUTTON_SILENT_CLOAK,
	TF_BOT_BUTTON_RADAR,
	TF_BOT_BUTTON_CANCELBUILD,

	// THIS MUST BE LAST
	TF_BOT_BUTTON_FIRSTUSER
};

// enumerations: TF_EntityClass
enum TF_EntityClass
{
	TF_CLASS_UNKNOWN = 0,
	TF_CLASS_NONE = 0,
	TF_CLASS_SCOUT,
	TF_CLASS_SNIPER,
	TF_CLASS_SOLDIER,
	TF_CLASS_DEMOMAN,
	TF_CLASS_MEDIC,
	TF_CLASS_HWGUY,
	TF_CLASS_PYRO,
	TF_CLASS_SPY,
	TF_CLASS_ENGINEER,
	TF_CLASS_CIVILIAN,
	TF_CLASS_MAX,
	TF_CLASS_ANY = TF_CLASS_MAX,
	// Other values to identify the "class"
	TF_CLASSEX_SENTRY,
	TF_CLASSEX_DISPENSER,
	TF_CLASSEX_TELEPORTER_ENTRANCE,
	TF_CLASSEX_TELEPORTER_EXIT,
	TF_CLASSEX_DETPACK,
	TF_CLASSEX_GRENADE,
	TF_CLASSEX_EMP_GRENADE,
	TF_CLASSEX_NAIL_GRENADE,
	TF_CLASSEX_MIRV_GRENADE,
	TF_CLASSEX_MIRVLET_GRENADE,
	TF_CLASSEX_NAPALM_GRENADE,
	TF_CLASSEX_GAS_GRENADE,
	TF_CLASSEX_CONC_GRENADE,
	TF_CLASSEX_LASER_GRENADE,
	TF_CLASSEX_SLOW_GRENADE,
	TF_CLASSEX_CALTROP,
	TF_CLASSEX_PIPE,
	TF_CLASSEX_GLGRENADE,
	TF_CLASSEX_ROCKET,
	TF_CLASSEX_NAPALM,
	TF_CLASSEX_SYRINGE,
	TF_CLASSEX_DART,
	TF_CLASSEX_NAIL,
	TF_CLASSEX_RAIL,
	TF_CLASSEX_TURRET,
	TF_CLASSEX_HUNTEDESCAPE,
	TF_CLASSEX_VEHICLE,
	TF_CLASSEX_VEHICLE_NODAMAGE,
	TF_CLASSEX_MANCANNON,

	// THIS MUST STAY LAST
	TF_NUM_CLASSES
};

// enumerations: TF_EntityFlags
enum TF_EntityFlags
{
	TF_ENT_FLAG_SAVEME = ENT_FLAG_FIRST_USER,
	TF_ENT_FLAG_ARMORME,
	TF_ENT_FLAG_BURNING,
	TF_ENT_FLAG_TRANQED,
	TF_ENT_FLAG_INFECTED,
	TF_ENT_FLAG_GASSED,
	TF_ENT_FLAG_ASSAULTFIRING,
	TF_ENT_FLAG_LEGSHOT,
	TF_ENT_FLAG_CALTROP,
	TF_ENT_FLAG_RADIOTAGGED,
	TF_ENT_FLAG_CAN_SABOTAGE,
	TF_ENT_FLAG_SABOTAGED,
	TF_ENT_FLAG_SABOTAGED2,
	TF_ENT_FLAG_SABOTAGING,
	TF_ENT_FLAG_MALFUNCTION,
	TF_ENT_FLAG_BUILDING_SG,
	TF_ENT_FLAG_BUILDING_DISP,
	TF_ENT_FLAG_BUILDING_DETP,
	TF_ENT_FLAG_BUILDING_ENTRANCE,
	TF_ENT_FLAG_BUILDING_EXIT,
	TF_ENT_FLAG_BUILDINPROGRESS,
	TF_LAST_FLAG
};

// enumerations: TF_Powerups
 enum TF_Powerups
{
	// Team Disguise
	TF_PWR_DISGUISE_BLUE = PWR_FIRST_USER,
	TF_PWR_DISGUISE_RED,
	TF_PWR_DISGUISE_YELLOW,
	TF_PWR_DISGUISE_GREEN,

	// Class Disguise
	TF_PWR_DISGUISE_SCOUT,
	TF_PWR_DISGUISE_SNIPER,
	TF_PWR_DISGUISE_SOLDIER,
	TF_PWR_DISGUISE_DEMOMAN,
	TF_PWR_DISGUISE_MEDIC,
	TF_PWR_DISGUISE_HWGUY,
	TF_PWR_DISGUISE_PYRO,
	TF_PWR_DISGUISE_ENGINEER,
	TF_PWR_DISGUISE_SPY,
	TF_PWR_DISGUISE_CIVILIAN,

	// Other powerups
	TF_PWR_CLOAKED,
};

// enumerations: TF_Weapon
enum TF_Weapon
{
	TF_WP_NONE = INVALID_WEAPON,
	TF_WP_UMBRELLA,
	TF_WP_AXE,
	TF_WP_CROWBAR,
	TF_WP_MEDKIT,
	TF_WP_KNIFE,
	TF_WP_SPANNER,
	TF_WP_SHOTGUN,
	TF_WP_SUPERSHOTGUN,
	TF_WP_NAILGUN,
	TF_WP_SUPERNAILGUN,
	TF_WP_GRENADE_LAUNCHER,
	TF_WP_ROCKET_LAUNCHER,
	TF_WP_SNIPER_RIFLE,
	TF_WP_RAILGUN,
	TF_WP_FLAMETHROWER,
	TF_WP_MINIGUN,
	TF_WP_AUTORIFLE,
	TF_WP_DARTGUN,
	TF_WP_PIPELAUNCHER,
	TF_WP_NAPALMCANNON,
	TF_WP_TOMMYGUN,
	TF_WP_DEPLOY_SG,
	TF_WP_DEPLOY_DISP,
	TF_WP_DEPLOY_DETP,
	TF_WP_DEPLOY_JUMPPAD,
	TF_WP_FLAG,

	TF_WP_GRENADE1,
	TF_WP_GRENADE2,

	TF_WP_GRENADE,
	TF_WP_GRENADE_CONC,
	TF_WP_GRENADE_EMP,
	TF_WP_GRENADE_NAIL,
	TF_WP_GRENADE_MIRV,
	TF_WP_GRENADE_GAS,
	TF_WP_GRENADE_CALTROPS,
	TF_WP_GRENADE_NAPALM,

	TF_WP_DETPACK,

	// special values unspecific weapon values
	TF_WP_PRIMARY,
	TF_WP_SECONDARY,

	// THIS MUST STAY LAST
	TF_WP_MAX
};

// enumerations: TF_Team
//		TF_TEAM_BLUE - Blue team.
//		TF_TEAM_RED - Red team.
//		TF_TEAM_YELLOW - Yellow team.
//		TF_TEAM_GREEN - Green team.
enum TF_Team
{
	TF_TEAM_NONE = 0,
	TF_TEAM_BLUE, // - Blue team.
	TF_TEAM_RED, // - Red team.
	TF_TEAM_YELLOW, //  - Yellow team.
	TF_TEAM_GREEN, //  - Green team.
	TF_TEAM_MAX = 5
};

enum TF_SoundType
{
	TF_SND_RADAR = SND_MAX_SOUNDS,

	// THIS MUST BE LAST
	TF_SND_MAX_SOUNDS
};

// enum:  TF_BuildableStatus
//		Enumerations for TF building status.
enum TF_BuildableStatus
{
	BUILDABLE_INVALID,
	BUILDABLE_BUILDING,
	BUILDABLE_BUILT,
};

struct ParamsSentryStatus_TF
{
	GameEntity	 mEntity;
	float		 mPosition[ 3 ];
	float		 mFacing[ 3 ];
	int			 mLevel;
	int			 mHealth;
	int			 mMaxHealth;
	int			 mShells[ 2 ];
	int			 mRockets[ 2 ];
	bool		 mSabotaged;
};

struct ParamsDispenserStatus_TF
{
	GameEntity	 mEntity;
	float		 mPosition[ 3 ];
	float		 mFacing[ 3 ];
	int			 mHealth;
	int			 mMaxHealth;
	int			 mCells;
	int			 mNails;
	int			 mRockets;
	int			 mShells;
	int			 mArmor;
	bool		 mSabotaged;
};

struct ParamsTeleporterStatus_TF
{
	GameEntity	 mEntityEntrance;
	GameEntity	 mEntityExit;
	int			 mNumTeleports;
	int			 mTimeToActivation;
	bool		 mSabotagedEntry;
	bool		 mSabotagedExit;
};

struct Event_DetpackStatus_TF
{
	GameEntity	 mEntity;
	float		 mTimeLeft;
};

struct ParamsHealTarget
{
	GameEntity				 mTarget;
	float					 mUberLevel;
	bool					 mHealing;
};

struct ParamsDisguiseOptions_TF
{
	int		 mCheckTeam;
	bool	 mTeam[ TF_TEAM_MAX ];
	bool	 mClass[ TF_CLASS_MAX ];
};

struct ParamsHudHint
{
	GameEntity mTargetPlayer;
	int32_t	 mId;
	char	 mMessage[ 1024 ];
};

struct ParamsHudMenu
{
	enum GuiType
	{
		GuiAlert,
		GuiMenu,
		GuiTextBox,
	};
	GameEntity mTargetPlayer;
	GuiType	 mMenuType;
	char	 mTitle[ 32 ];
	char	 mCaption[ 32 ];
	char	 mMessage[ 512 ];
	char	 mOption[ 10 ][ 64 ];
	char	 mCommand[ 10 ][ 64 ];
	int		 mLevel;
	float	 mTimeOut;
	obColor	 mColor;
};

struct ParamsHudText
{
	enum MsgType
	{
		MsgConsole,
		MsgHudCenter,
	};
	GameEntity mTargetPlayer;
	MsgType	 mMessageType;
	char	 mMessage[ 512 ];
};

class TeamFortress_Interface : public IEngineInterface
{
public:
	virtual bool GetBuildStatus( GameEntity ent, ParamsSentryStatus_TF& params ) = 0;
	virtual bool GetBuildStatus( GameEntity ent, ParamsDispenserStatus_TF& params ) = 0;
	virtual bool GetBuildStatus( GameEntity ent, ParamsTeleporterStatus_TF& params ) = 0;
	virtual bool GetBuildStatus( GameEntity ent, Event_DetpackStatus_TF& params ) = 0;

	virtual int GetPlayerPipeCount( GameEntity ent );
	virtual bool DisguiseOptions( GameEntity ent, ParamsDisguiseOptions_TF &_disguiseoptions );
	virtual bool Disguise( GameEntity ent, int32_t _team, int32_t _class );
	virtual bool GetDisguiseInfo( GameEntity ent, int &_team, int &_class );
	virtual bool GetDisguiseInfo( const EntityInfo & entInfo, int &_team, int &_class );
	virtual bool Cloak( GameEntity ent, bool silent );

	virtual bool GetHealTargetInfo( GameEntity ent, ParamsHealTarget& params );

	virtual bool LockPlayerPosition( GameEntity ent, bool _lock );

	virtual void ShowHudHint( GameEntity _player, int32_t _id, const char * msg );
	virtual void ShowHudMenu( const ParamsHudMenu & data );
	virtual void ShowHudText( const ParamsHudText & data );
};

extern TeamFortress_Interface* gTeamFortressFuncs;

#endif
