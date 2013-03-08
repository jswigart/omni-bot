////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __FILTERCLOSESTTF_H__
#define __FILTERCLOSESTTF_H__

#include "FilterClosest.h"

class FilterClosestTF : public FilterClosest
{
public:

	virtual bool CheckEx(const GameEntity _ent, const MemoryRecord &_record);

	FilterClosestTF(Client *_client, AiState::SensoryMemory::Type _type);
	virtual ~FilterClosestTF() {}
protected:

	FilterClosestTF();
};

#endif
