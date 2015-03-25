////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "ETF_Config.h"
#include "FilterClosestETF.h"

// class: FilterClosestTF
//		This filter is specific to team fortress, and should take into account additional sensory considerations,
//		such as spy disguises, spy feigning

FilterClosestETF::FilterClosestETF( Client *_client, AiState::SensoryMemory::Type _type )
	: FilterClosestTF( _client, _type )
{
}

FilterClosestETF::~FilterClosestETF()
{
}

bool FilterClosestETF::CheckEx( const GameEntity _ent, const MemoryRecord &_record )
{
	if ( mClient->GetEntityFlags().CheckFlag( ETF_ENT_FLAG_BLIND ) )
		return false;

	return FilterClosestTF::CheckEx( _ent, _record );
}