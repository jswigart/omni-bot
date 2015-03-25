////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __FILTERMOSTHURT_H__
#define __FILTERMOSTHURT_H__

#include "FilterSensory.h"

// class: FilterMostHurt
//		This filter finds the entity with the least health matching the
//		requested type, category, and class
class FilterMostHurt : public FilterSensory
{
public:
	virtual void Check( int _index, const MemoryRecord &_record );

	virtual void Reset();

	inline float GetMostHurtHealth()
	{
		return mMostHurtHealthPc;
	}

	FilterMostHurt( Client *_client, AiState::SensoryMemory::Type _type );
	virtual ~FilterMostHurt();
protected:

	float		mMostHurtHealthPc;

	FilterMostHurt();
};

#endif
