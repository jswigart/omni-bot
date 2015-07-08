////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __MC_INTERFACEFUNCS_H__
#define __MC_INTERFACEFUNCS_H__

#include "InterfaceFuncs.h"
#include "HL2DM_Messages.h"

namespace InterfaceFuncs
{
	bool CanPhysPickup( GameEntity _ent, GameEntity _pickup );
	bool GetPhysGunInfo( GameEntity _ent, HL2DM_PhysGunInfo &info );
};

#endif
