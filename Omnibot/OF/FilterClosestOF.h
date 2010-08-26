////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-12-31 10:36:40 -0800 (Wed, 31 Dec 2008) $
// $LastChangedRevision: 3830 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __FilterClosestOF_H__
#define __FilterClosestOF_H__

#include "FilterClosestTF.h"

class FilterClosestOF : public FilterClosestTF
{
public:
	
	virtual bool CheckEx(const GameEntity _ent, const MemoryRecord &_record);

	FilterClosestOF(Client *_client, AiState::SensoryMemory::Type _type);
	virtual ~FilterClosestOF() {}
protected:

	FilterClosestOF();
};

#endif
