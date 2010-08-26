////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2009-12-07 23:36:07 -0600 (Mon, 07 Dec 2009) $
// $LastChangedRevision: 4562 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MC_INTERFACEFUNCS_H__
#define __MC_INTERFACEFUNCS_H__

#include "BotWeaponSystem.h"

namespace InterfaceFuncs
{	
	bool GetPlayerStats(GameEntity _ent, MC_PlayerStats &stats);
	bool GetModuleStats(GameEntity _ent, MC_ModuleStats &stats);
	bool UpgradeModule(GameEntity _ent, int moduleId);
	bool GetWeaponStats(GameEntity _ent, MC_WeaponUpgradeStats &stats);
	bool UpgradeWeapon(GameEntity _ent, int upgradeId);
	bool CanPhysPickup(GameEntity _ent, GameEntity _pickup);
	bool GetPhysGunInfo(GameEntity _ent, MC_PhysGunInfo &info);
	bool GetChargerStatus(GameEntity _ent, MC_ChargerStatus &status);
};

#endif
