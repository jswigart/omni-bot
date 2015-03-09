////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart@gmail.com $
// $LastChangedDate: 2013-03-07 21:15:21 -0600 (Thu, 07 Mar 2013) $
// $LastChangedRevision: 837 $
//
// Title: MC Message Structure Definitions
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __HL2DM_MESSAGES_H__
#define __HL2DM_MESSAGES_H__

#include "Base_Messages.h"
#include "HL2DM_Config.h"

#pragma pack(push)
#pragma pack(4)

//////////////////////////////////////////////////////////////////////////

// struct: HL2DM_UpgradeModule
struct HL2DM_UpgradeModule
{
	int			m_ModuleId;
};

// struct: HL2DM_CanPhysPickup
struct HL2DM_CanPhysPickup
{
	GameEntity	m_Entity;
	obBool		m_CanPickUp;
};

// struct: HL2DM_PhysGunInfo
struct HL2DM_PhysGunInfo
{
	GameEntity	m_HeldEntity;
	float		m_LaunchSpeed;
};

struct HL2DM_ChargerStatus
{
	float	m_CurrentCharge;
	float	m_MaxCharge;
};

#pragma pack(pop)

#endif
