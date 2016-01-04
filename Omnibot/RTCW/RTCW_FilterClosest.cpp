////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "RTCW_FilterClosest.h"
#include "RTCW_Client.h"

#include "ScriptManager.h"

RTCW_FilterClosest::RTCW_FilterClosest( Client *_client, AiState::SensoryMemory::Type _type ) :
FilterClosest( _client, _type )
{
}

bool RTCW_FilterClosest::CheckEx( const MemoryRecord &_record )
{
	// Special consideration for some entity types.
	switch ( _record.mTargetInfo.mEntInfo.mClassId )
	{
		case RTCW_CLASSEX_MG42MOUNT:
		{
			GameEntity mounted = gRTCWFuncs->GetMountedPlayerOnMG42( mClient->GetGameEntity(), _record.GetEntity() );
			if ( !mounted.IsValid() || mClient->IsAllied( mounted ) )
				return false;
			break;
		}
		case RTCW_CLASSEX_BREAKABLE:
		{
			float fBreakableDist = static_cast<RTCW_Client*>( mClient )->GetBreakableTargetDist();
			//			float fBreakableDist = 50.0f;
			float fDistance = ( mClient->GetPosition() - _record.GetLastSensedPosition() ).SquaredLength();
			if ( fDistance > ( fBreakableDist * fBreakableDist ) )
				return false;
			break;
		}
	}

	return true;
}