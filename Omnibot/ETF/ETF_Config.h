////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// Title: ETF Config
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __ETF_CONFIG_H__
#define __ETF_CONFIG_H__

#include "TF_Config.h"

#include "Omni-Bot_Types.h"
#include "Omni-Bot_Events.h"

typedef enum eETF_Version
{
	ETF_VERSION_0_1 = 17,
	ETF_VERSION_LAST,
	ETF_VERSION_LATEST = ETF_VERSION_LAST - 1
} ETF_Version;

// enumerations: TF_Team
//		ETF_TEAM_RED - Red team.
//		ETF_TEAM_BLUE - Blue team.
//		ETF_TEAM_YELLOW - Yellow team.
//		ETF_TEAM_GREEN - Green team.
typedef enum eETF_Team
{
	ETF_TEAM_NONE = OB_TEAM_NONE,
	ETF_TEAM_RED,
	ETF_TEAM_BLUE,
	ETF_TEAM_YELLOW,
	ETF_TEAM_GREEN,

	// THIS MUST STAY LAST
	ETF_TEAM_MAX
} ETF_Team;

enum ETF_SurfaceFlags
{
	ETF_SURFACE_LANDMINE	=	SURFACE_START_USER
};

// Override this to new value
#ifdef MAX_DEMO_TEAM_PIPES
#undef MAX_DEMO_TEAM_PIPES
#endif
#define MAX_DEMO_TEAM_PIPES 10

typedef enum eETF_Weapon
{
	ETF_WP_GRENADE_FLASH = TF_WP_GRENADE_CALTROPS // remapping this
} ETF_Weapon;

typedef enum eETF_EntityClass
{
	ETF_CLASSEX_FLASH_GRENADE = TF_CLASSEX_CALTROP // remapping this
} ETF_EntityClass;

typedef enum eETF_EntityFlags
{
	ETF_ENT_FLAG_BLIND = TF_ENT_FLAG_CALTROP, // remapping this
	ETF_ENT_FLAG_CONCED = TF_LAST_FLAG + 1,
	ETF_ENT_FLAG_DISGUISED
} ETF_EntityFlags;

typedef enum eETF_Powerups
{
	ETF_PWR_QUAD = TF_PWR_CLOAKED + 1,
	ETF_PWR_SUIT,
	ETF_PWR_HASTE,
	ETF_PWR_INVIS,
	ETF_PWR_REGEN,
	ETF_PWR_FLIGHT,
	ETF_PWR_INVULN,
	ETF_PWR_AQUALUNG,
} ETF_Powerups;

#endif
