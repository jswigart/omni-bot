////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "FilterBias.h"

FilterBiasEntityFlag::FilterBiasEntityFlag( const int _entflags, const float _multiplier )
	: mEntityFlag( _entflags )
	, mDistanceMultiplier( _multiplier )
{
}

float FilterBiasEntityFlag::CheckBias( const AiState::SensoryMemory::MemoryMap::const_iterator &_it )
{
	if ( _it->second.mTargetInfo.mEntInfo.mFlags.CheckFlag( mEntityFlag ) )
		return mDistanceMultiplier;
	return 1.0f;
}

//////////////////////////////////////////////////////////////////////////