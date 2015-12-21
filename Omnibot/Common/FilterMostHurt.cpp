////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "FilterMostHurt.h"
#include "TargetInfo.h"

FilterMostHurt::FilterMostHurt( Client *_client, AiState::SensoryMemory::Type _type )
	: FilterSensory( _client, _type )
	, mMostHurtHealthPc( 1.0f )
{
}

FilterMostHurt::~FilterMostHurt()
{
}

void FilterMostHurt::Reset()
{
	FilterSensory::Reset();
	mMostHurtHealthPc = 1.0f;
}

void FilterMostHurt::Check( int _index, const MemoryRecord & record )
{
	if ( mMemorySpan == 0 )
		mMemorySpan = mClient->GetSensoryMemory()->GetMemorySpan();

	const bool noLOS = record.mTargetInfo.mEntInfo.mCategory.CheckFlag( ENT_CAT_NOLOS );
	if ( noLOS || ( IGame::GetTime() - record.GetTimeLastSensed() ) <= mMemorySpan )
	{
		switch ( mType )
		{
			case AiState::SensoryMemory::EntAlly:
				if ( !record.IsAllied() )
					return;
				break;
			case AiState::SensoryMemory::EntEnemy:
				if ( record.IsAllied() )
					return;
				break;
			case AiState::SensoryMemory::EntAny:
				break;
		}

		// Make sure the class matches.
		if ( mAnyPlayerClass )
		{
			if ( record.mTargetInfo.mEntInfo.mClassId >= ANYPLAYERCLASS )
				return;
		}
		else if ( !PassesFilter( record.mTargetInfo.mEntInfo ) )
			return;

		// Make sure the category matches.
		if ( mCategory.AnyFlagSet() && !( mCategory & record.mTargetInfo.mEntInfo.mCategory ).AnyFlagSet() )
			return;

		// Only alive targets count for shootable
		if ( mCategory.CheckFlag( ENT_CAT_SHOOTABLE ) && record.mTargetInfo.mEntInfo.mFlags.CheckFlag( ENT_FLAG_DEAD ) )
			return;

		// Make sure it isn't disabled.
		if ( record.mTargetInfo.mEntInfo.mFlags.CheckFlag( ENT_FLAG_DISABLED ) )
			return;

		// Check the health
		if ( record.mTargetInfo.mEntInfo.mHealth.Percent() > 0 )
		{
			const float healthPercent = record.mTargetInfo.mEntInfo.mHealth.Percent();
			if ( healthPercent < mMostHurtHealthPc )
			{
				mMostHurtHealthPc = healthPercent;
				mBestEntity = record.GetEntity();
			}
		}

		const float healthPc = record.mTargetInfo.mEntInfo.mHealth.Percent();
		if ( healthPc < mMostHurtHealthPc )
		{
			mMostHurtHealthPc = healthPc;
			mBestEntity = record.GetEntity();
		}
	}
}