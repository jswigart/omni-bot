////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: ensiform $
// $LastChangedDate: 2008-08-05 16:01:00 -0700 (Tue, 05 Aug 2008) $
// $LastChangedRevision: 3244 $
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompTF.h"
#include "TF_Config.h"
#include "FilterClosestTF.h"

// class: FilterClosestTF
//		This filter is specific to team fortress, and should take into account additional sensory considerations,
//		such as spy disguises, spy feigning

FilterClosestTF::FilterClosestTF(Client *_client, AiState::SensoryMemory::Type _type) :
	FilterClosest	(_client, _type)
{
}

bool FilterClosestTF::CheckEx(const GameEntity _ent, const MemoryRecord &_record)
{
	if(_record.m_TargetInfo.m_EntityPowerups.CheckFlag(TF_PWR_CLOAKED) &&
		m_Client->GetTargetingSystem()->GetLastTarget() != _ent)
		return false;

	if(_record.m_TargetInfo.m_EntityFlags.CheckFlag(TF_ENT_FLAG_BUILDINPROGRESS))
		return false;

	int iTeam = 0, iClass = 0;
	InterfaceFuncs::GetDisguiseInfo(_record.m_TargetInfo.m_EntityPowerups, iTeam, iClass);
	if(iTeam != TF_TEAM_NONE && iTeam != TF_CLASS_NONE)
		return false;

	return true;
}
