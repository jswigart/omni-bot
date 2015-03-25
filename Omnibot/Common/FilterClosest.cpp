////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "FilterClosest.h"
#include "BotWeaponSystem.h"

FilterClosest::FilterClosest( Client *_client, AiState::SensoryMemory::Type _type ) 
	: FilterSensory( _client, _type )
	, mBestDistanceSq( Utils::FloatMax )
{
}

FilterClosest::~FilterClosest()
{
}

void FilterClosest::Reset()
{
	FilterSensory::Reset();
	mBestDistanceSq = Utils::FloatMax;
}

void FilterClosest::Check( int _index, const MemoryRecord &_record )
{
	if ( mMemorySpan == 0 )
		mMemorySpan = mClient->GetSensoryMemory()->GetMemorySpan();

	const bool noLOS = _record.mTargetInfo.mEntInfo.mCategory.CheckFlag( ENT_CAT_NOLOS );
	if ( noLOS ||
		( IGame::GetTime() - _record.GetTimeLastSensed() ) <= mMemorySpan )
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

		if ( IsBeingIgnored( _record.GetEntity() ) )
			return;

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

		//////////////////////////////////////////////////////////////////////////
		// Distance
		float fDistanceSq = ( vSensoryPos - _record.GetLastSensedPosition() ).SquaredLength();

		// Early out if this wouldn't have been closer anyhow even if it passed.
		if ( fDistanceSq >= mBestDistanceSq )
			return;

		if ( mMaxDistance > 0.f )
		{
			if ( fDistanceSq > Mathf::Sqr( mMaxDistance ) )
				return;
		}
		//////////////////////////////////////////////////////////////////////////

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

		if ( _record.ShouldIgnore() )
			return;

		// Only alive targets count for shootable
		if ( mCategory.CheckFlag( ENT_CAT_SHOOTABLE ) )
		{
			if ( noLOS && !_record.IsShootable() )
				return;

			if ( _record.mTargetInfo.mEntInfo.mFlags.CheckFlag( ENT_FLAG_DEAD ) )
				return;

			if ( !mClient->GetWeaponSystem()->CanShoot( _record ) )
				return;
		}

		// Get the distance to this person.
		if ( CheckEx( _record ) )
		{
			mBestDistanceSq = fDistanceSq;
			mBestEntity = _record.GetEntity();
		}
	}
}