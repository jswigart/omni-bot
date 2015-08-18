////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __FILTERBIAS_H__
#define __FILTERBIAS_H__

#include "BotSensoryMemory.h"

class Client;

// class: FilterBias
//		FilterBias allows a sensory filter to take into account extra
//		properties of a target when searching for targets/entities. These
//		biases will always return a multiplier for the distance, so that entities
//		that match the criteria are treated as if they are closer than they really are.
class FilterBias
{
public:
	virtual float CheckBias(const AiState::SensoryMemory::MemoryMap::const_iterator &_it) = 0;
};

typedef std::shared_ptr<FilterBias> FilterBiasPtr;

//////////////////////////////////////////////////////////////////////////

// class: FilterBiasEntityFlag
//		If the entity matches the entity flag, the distance will be multiplied by
class FilterBiasEntityFlag : public FilterBias
{
public:
	float CheckBias(const AiState::SensoryMemory::MemoryMap::const_iterator &_it);

	FilterBiasEntityFlag(const int _entflags, const float _multiplier);
private:
	const int	mEntityFlag;
	const float	mDistanceMultiplier;

	FilterBiasEntityFlag();
};

//////////////////////////////////////////////////////////////////////////

#endif
