////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: jswigart $
// $LastChangedDate: 2010-08-28 00:12:05 -0500 (Sat, 28 Aug 2010) $
// $LastChangedRevision: 32 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __FilterClosestETF_H__
#define __FilterClosestETF_H__

#include "FilterClosestTF.h"

class FilterClosestETF : public FilterClosestTF
{
public:
	
	virtual bool CheckEx(const GameEntity _ent, const MemoryRecord &_record);

	FilterClosestETF(Client *_client, AiState::SensoryMemory::Type _type);
	virtual ~FilterClosestETF() {}
protected:

	FilterClosestETF();
};

#endif
