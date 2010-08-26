////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-07-05 03:18:03 -0700 (Sat, 05 Jul 2008) $
// $LastChangedRevision: 3023 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __ETQW_FILTERCLOSEST_H__
#define __ETQW_FILTERCLOSEST_H__

#include "FilterClosest.h"
#include "BotSensoryMemory.h"

// class: ETQW_FilterClosest
//		This filter finds the closest entity matching the 
//		requested type, category, and class. Also provides
//		additional functionality required in ET to take into
//		account whether the entity is disguised or feigned dead
class ETQW_FilterClosest : public FilterClosest
{
public:
	
	virtual bool CheckEx(const MemoryRecord &_record);

	ETQW_FilterClosest(Client *_client, AiState::SensoryMemory::Type _type);
	virtual ~ETQW_FilterClosest() {}
protected:
	float	m_BestDistance;

	ETQW_FilterClosest();
};

#endif
