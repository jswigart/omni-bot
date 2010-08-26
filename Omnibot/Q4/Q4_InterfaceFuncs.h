////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: crapshoot $
// $LastChangedDate: 2009-01-15 22:49:58 -0800 (Thu, 15 Jan 2009) $
// $LastChangedRevision: 3873 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __Q4_INTERFACEFUNCS_H__
#define __Q4_INTERFACEFUNCS_H__

#include "BotWeaponSystem.h"

namespace InterfaceFuncs
{
	const char *GetLocation(const Vector3f &_pos);
	float GetPlayerCash(const GameEntity _player);
	bool IsBuyingAllowed();
	bool BuySomething(const GameEntity _player, int _item);
};

#endif
