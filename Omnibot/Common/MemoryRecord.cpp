////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "MemoryRecord.h"
#include "IGame.h"

int MemoryRecord::GetTimeTargetHasBeenVisible() const
{
	return IsInFOV() ? IGame::GetTime() - GetTimeBecameVisible() : 0;
}

int MemoryRecord::GetTimeHasBeenOutOfView() const
{
	return IGame::GetTime() - GetTimeLastVisible();
}

void MemoryRecord::IgnoreAsTargetForTime( int _milliseconds )
{
	mIgnoreForTargeting = IGame::GetTime() + _milliseconds;
}

bool MemoryRecord::ShouldIgnore() const
{
	return mIgnoreAsTarget || mIgnoreForTargeting > IGame::GetTime();
}

int MemoryRecord::GetAge() const
{
	return IGame::GetTime() - mTimeLastUpdated;
}

void MemoryRecord::MarkUpdated()
{
	mTimeLastUpdated = IGame::GetTime();
}

Vector3f MemoryRecord::PredictPosition( float _time ) const
{
	float fTimeOutOfView = (float)GetTimeHasBeenOutOfView() / 1000.f;
	return Utils::PredictFuturePositionOfTarget(
		mTargetInfo.mLastPosition, mTargetInfo.mLastVelocity, fTimeOutOfView + _time );
}

bool MemoryRecord::IsMovingTowards( const Vector3f &_pos ) const
{
	Vector3f vToTarget = _pos - GetLastSensedPosition();
	return GetLastSensedVelocity().Dot( vToTarget ) > 0.f;
}

void MemoryRecord::Reset( GameEntity _ent )
{
	mEntity = _ent;
	mTargetInfo.mEntInfo = EntityInfo();
	mTimeLastSensed = -999;
	mTimeBecameVisible = -999;
	mIgnoreForTargeting = -999;
	mTimeLastVisible = 0;
	mTimeLastUpdated = 0;
	mInFOV = false;
	mIsAllied = false;
	mIsShootable = false;
	mIgnoreAsTarget = false;
	mSerial++;
}