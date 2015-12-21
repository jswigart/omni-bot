////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "ET_FilterClosest.h"
#include "ET_Client.h"
#include "ET_Game.h"

ET_FilterClosest::ET_FilterClosest( Client *_client, AiState::SensoryMemory::Type _type )
	: FilterClosest( _client, _type )
{
}

bool ET_FilterClosest::CheckEx( const MemoryRecord &_record )
{
	// Special consideration for some entity types.
	/*switch ( _record.mTargetInfo.mEntityClass - ET_Game::CLASSEXoffset )
	{
		case ET_CLASSEX_VEHICLE_HVY:
		{
			break;
		}
		case ET_CLASSEX_MG42MOUNT:
		{
			GameEntity mounted = gEngineFuncs->GetMountedPlayerOnMG42( mClient, _record.GetEntity() );
			if ( !mounted.IsValid() || mClient->IsAllied( mounted ) )
				return false;
			MemoryRecord *record2 = mClient->GetSensoryMemory()->GetMemoryRecord( mounted );
			if ( record2 && record2->ShouldIgnore() ) return false;
			break;
		}
		case ET_CLASSEX_BREAKABLE:
		{
			float fBreakableDist = static_cast<ET_Client*>( mClient )->GetBreakableTargetDist();
			float fDistance = ( mClient->GetPosition() - _record.GetLastSensedPosition() ).SquaredLength();
			if ( fDistance > ( fBreakableDist * fBreakableDist ) )
				return false;
			break;
		}
	}*/

	// TODO: Handle disguised covertops
	// cs: done in script for the moment
	//if (_record.mTargetInfo.mEntityFlags.CheckFlag(ET_ENT_FLAG_DISGUISED) && Mathf::UnitRandom() > 0.33f)
	//	return false;

	return true;
}