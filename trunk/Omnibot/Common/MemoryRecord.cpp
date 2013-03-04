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

void MemoryRecord::IgnoreAsTargetForTime(int _milliseconds)
{
	m_IgnoreForTargeting = IGame::GetTime() + _milliseconds; 
}

bool MemoryRecord::ShouldIgnore() const
{
	return m_IgnoreAsTarget || m_IgnoreForTargeting > IGame::GetTime();
}

int MemoryRecord::GetAge() const
{
	return IGame::GetTime() - m_TimeLastUpdated; 
}

void MemoryRecord::MarkUpdated() 
{
	m_TimeLastUpdated = IGame::GetTime(); 
}

Vector3f MemoryRecord::PredictPosition(float _time) const
{
	float fTimeOutOfView = (float)GetTimeHasBeenOutOfView() / 1000.f;
	return Utils::PredictFuturePositionOfTarget(
		m_TargetInfo.m_LastPosition, m_TargetInfo.m_LastVelocity, fTimeOutOfView+_time);
}

bool MemoryRecord::IsMovingTowards(const Vector3f &_pos) const
{
	Vector3f vToTarget = _pos - GetLastSensedPosition();
	return GetLastSensedVelocity().Dot(vToTarget) > 0.f;
}

void MemoryRecord::Reset(GameEntity _ent)
{
	m_Entity = _ent;
	m_TargetInfo.m_EntityClass = 0;
	m_TargetInfo.m_EntityCategory.ClearAll();
	m_TimeLastSensed = -999;
	m_TimeBecameVisible = -999;
	m_IgnoreForTargeting = -999;
	m_TimeLastVisible = 0;
	m_TimeLastUpdated = 0;
	m_InFOV = false;
	m_IsAllied = false;
	m_IsShootable = false;
	m_IgnoreAsTarget = false;
	m_Serial++;
}
