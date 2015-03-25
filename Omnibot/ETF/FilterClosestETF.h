////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __FILTERCLOSESTETF_H__
#define __FILTERCLOSESTETF_H__

#include "FilterClosestTF.h"

class FilterClosestETF : public FilterClosestTF
{
public:
	virtual bool CheckEx( const GameEntity _ent, const MemoryRecord &_record );

	FilterClosestETF( Client *_client, AiState::SensoryMemory::Type _type );
	~FilterClosestETF();
protected:
	FilterClosestETF();
};

#endif
