////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "HL2DM_InterfaceFuncs.h"
#include "MessageHelper.h"

namespace InterfaceFuncs
{
	bool CanPhysPickup( GameEntity _ent, GameEntity _pickup )
	{
		HL2DM_CanPhysPickup data = { _pickup, False };
		MessageHelper msg( HL2DM_MSG_CAN_PHYSPICKUP, &data, sizeof( data ) );
		return SUCCESS( InterfaceMsg( msg, _ent ) );
	}
	bool GetPhysGunInfo( GameEntity _ent, HL2DM_PhysGunInfo &info )
	{
		MessageHelper msg( HL2DM_MSG_PHYSGUNINFO, &info, sizeof( info ) );
		return SUCCESS( InterfaceMsg( msg, _ent ) );
	}
};
