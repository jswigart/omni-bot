////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __Q4_INTERFACEFUNCS_H__
#define __Q4_INTERFACEFUNCS_H__

#include "InterfaceFuncs.h"
#include "Q4_Messages.h"
#include "common.h"

namespace InterfaceFuncs
{
	const char *GetLocation(const Vector3f &_pos);
	float GetPlayerCash(const GameEntity _player);
	bool IsBuyingAllowed();
	bool BuySomething(const GameEntity _player, int _item);
};

#endif
