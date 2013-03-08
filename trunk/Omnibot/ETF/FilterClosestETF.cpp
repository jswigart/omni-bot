////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart $
// $LastChangedDate: 2010-08-28 00:12:05 -0500 (Sat, 28 Aug 2010) $
// $LastChangedRevision: 32 $
//
////////////////////////////////////////////////////////////////////////////////

#include "ETF_Config.h"
#include "FilterClosestETF.h"

// class: FilterClosestTF
//		This filter is specific to team fortress, and should take into account additional sensory considerations,
//		such as spy disguises, spy feigning

FilterClosestETF::FilterClosestETF(Client *_client, AiState::SensoryMemory::Type _type) :
	FilterClosestTF	(_client, _type)
{
}

bool FilterClosestETF::CheckEx(const GameEntity _ent, const MemoryRecord &_record)
{
	if(m_Client->GetEntityFlags().CheckFlag(ETF_ENT_FLAG_BLIND))
		return false;

	return FilterClosestTF::CheckEx(_ent, _record);
}