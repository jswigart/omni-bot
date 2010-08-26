////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-01-25 08:23:37 -0800 (Fri, 25 Jan 2008) $
// $LastChangedRevision: 2370 $
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompRTCW.h"
#include "RTCW_FilterClosest.h"
//#include "RTCW_Goal_MountMG42.h"

#include "ScriptManager.h"

RTCW_FilterClosest::RTCW_FilterClosest(Client *_client, AiState::SensoryMemory::Type _type) :
	FilterClosest				(_client, _type)
{
}

bool RTCW_FilterClosest::CheckEx(const MemoryRecord &_record)
{
	// Special consideration for some entity types.
	switch(_record.m_TargetInfo.m_EntityClass) 
	{
	case RTCW_CLASSEX_MG42MOUNT:
		{
			GameEntity mounted = InterfaceFuncs::GetMountedPlayerOnMG42(m_Client, _record.GetEntity());
			if(!mounted.IsValid() || m_Client->IsAllied(mounted))
				return false;
			break;
		}
	case RTCW_CLASSEX_BREAKABLE:
		{
			float fBreakableDist = static_cast<RTCW_Client*>(m_Client)->GetBreakableTargetDist();
//			float fBreakableDist = 50.0f;
			float fDistance = (m_Client->GetPosition() - _record.GetLastSensedPosition()).SquaredLength();
			if(fDistance > (fBreakableDist * fBreakableDist))
				return false;
			break;
		}
	}

	return true;
}

