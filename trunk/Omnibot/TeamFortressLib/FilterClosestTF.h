////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: DrEvil $
// $LastChangedDate: 2007-07-10 22:14:48 -0700 (Tue, 10 Jul 2007) $
// $LastChangedRevision: 2076 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __FilterClosestTF_H__
#define __FilterClosestTF_H__

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
