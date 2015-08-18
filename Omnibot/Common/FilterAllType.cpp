////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "FilterAllType.h"
#include "BotWeaponSystem.h"

FilterAllType::FilterAllType( Client *_client, AiState::SensoryMemory::Type _type, MemoryRecords &_list ) 
	: FilterSensory( _client, _type )
	, mList( _list )
{
}

void FilterAllType::Check( int _index, const MemoryRecord &_record )
{
	if ( mMemorySpan == 0 )
		mMemorySpan = mClient->GetSensoryMemory()->GetMemorySpan();

	const bool noLOS = _record.mTargetInfo.mEntInfo.mCategory.CheckFlag( ENT_CAT_NOLOS );
	if ( noLOS || ( IGame::GetTime() - _record.GetTimeLastSensed() ) <= mMemorySpan )
	{
		switch ( mType )
		{
			case AiState::SensoryMemory::EntAlly:
				if ( !_record.IsAllied() )
					return;
				break;
			case AiState::SensoryMemory::EntEnemy:
				if ( _record.IsAllied() )
					return;
				break;
			case AiState::SensoryMemory::EntAny:
				break;
		}

		Vector3f vSensoryPos;
		if ( !mNumPositions )
		{
			mClosestPosition = 0;
			vSensoryPos = mClient->GetPosition();
		}
		else
		{
			//////////////////////////////////////////////////////////////////////////
			// Find the closest position
			float fClosest = Utils::FloatMax;
			for ( int p = 0; p < mNumPositions; ++p )
			{
				float fDistSq = ( mPosition[ p ] - _record.GetLastSensedPosition() ).SquaredLength();
				if ( fDistSq < fClosest )
				{
					fClosest = fDistSq;
					vSensoryPos = mPosition[ p ];
					mClosestPosition = p;
				}
			}
		}

		if ( IsBeingIgnored( _record.GetEntity() ) )
			return;

		float fDistanceSq = ( vSensoryPos - _record.GetLastSensedPosition() ).SquaredLength();
		if ( mMaxDistance > 0.f )
		{
			if ( fDistanceSq > Mathf::Sqr( mMaxDistance ) )
				return;
		}

		// Make sure the class matches.
		if ( mAnyPlayerClass )
		{
			if ( _record.mTargetInfo.mEntInfo.mClassId >= ANYPLAYERCLASS )
				return;
		}
		else if ( !PassesFilter( _record.mTargetInfo.mEntInfo ) )
			return;

		// Make sure the category matches.
		if ( mCategory.AnyFlagSet() && !( mCategory & _record.mTargetInfo.mEntInfo.mCategory ).AnyFlagSet() )
			return;

		// Make sure it isn't disabled.
		if ( _record.mTargetInfo.mEntInfo.mFlags.CheckFlag( ENT_FLAG_DISABLED ) )
			return;

		if ( !CheckEx( _record ) )
			return;

		if ( _record.ShouldIgnore() )
			return;

		// Only alive targets count for shootable
		if ( mCategory.CheckFlag( ENT_CAT_SHOOTABLE ) )
		{
			if ( _record.mTargetInfo.mEntInfo.mFlags.CheckFlag( ENT_FLAG_DEAD ) )
				return;

			if ( !mClient->GetWeaponSystem()->CanShoot( _record ) )
				return;
		}

		if ( mSortType == Sort_None )
		{
			mList.push_back( RecordHandle( (int16_t)_index, _record.GetSerial() ) );
		}
		else
		{
			// Get the distance to this person.
			/*float fCurDistanceToSq = (vSensoryPos - _record.mTargetInfo.mLastPosition).SquaredLength();
			fCurDistanceToSq;
			for(uint32_t i = 0; i < .mList.size(); ++i)
			{
			switch(.mSortType)
			{
			case Sort_NearToFar:
			{
			const MemoryRecord *pRec =
			float fDistSq = (.mPosition - .mList[i]->GetLastSensedPosition()).SquaredLength();
			if(fCurDistanceToSq < fDistSq)
			{
			.mList.insert(i, &_record);
			break;
			}
			break;
			}
			case Sort_FarToNear:
			{
			float fDistSq = (.mPosition - .mList[i]->GetLastSensedPosition()).SquaredLength();
			if(fCurDistanceToSq > fDistSq)
			{
			}
			break;
			}
			case Sort_None:
			default:
			break;
			}
			}*/
		}
	}
}