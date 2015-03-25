////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "FilterMostHurt.h"
#include "TargetInfo.h"
#include "InterfaceFuncs.h"

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

void FilterMostHurt::Check( int _index, const MemoryRecord &_record )
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

		// Only alive targets count for shootable
		if ( mCategory.CheckFlag( ENT_CAT_SHOOTABLE ) && _record.mTargetInfo.mEntInfo.mFlags.CheckFlag( ENT_FLAG_DEAD ) )
			return;

		// Make sure it isn't disabled.
		if ( _record.mTargetInfo.mEntInfo.mFlags.CheckFlag( ENT_FLAG_DISABLED ) )
			return;

		// Check the health
		if ( _record.mTargetInfo.mEntInfo.mHealthMax > 0 )
		{
			float mHealthPercent = (float)_record.mTargetInfo.mEntInfo.mHealth / (float)_record.mTargetInfo.mEntInfo.mHealthMax;
			if ( mHealthPercent < mMostHurtHealthPc )
			{
				mMostHurtHealthPc = mHealthPercent;
				mBestEntity = _record.GetEntity();
			}
		}
	}
}