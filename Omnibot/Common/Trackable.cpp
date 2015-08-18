////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "Trackable.h"

Trackable::Trackable()
{
}

Trackable::~Trackable()
{
}

void Trackable::AddReference(uint32_t _type)
{
	_CheckIndex(_type);
	++mTrackList[_type];
}

void Trackable::DelReference(uint32_t _type)
{
	_CheckIndex(_type);
	--mTrackList[_type];
}

uint32_t Trackable::GetRefCount(uint32_t _type)
{
	_CheckIndex(_type);
	return mTrackList[_type];
}

void Trackable::_CheckIndex(uint32_t _type)
{
	if(_type >= mTrackList.size())
	{
		mTrackList.resize(_type+1, 0);
	}
}

bool Trackable::IsReferenced()
{
	for(uint32_t i = 0; i < mTrackList.size(); ++i)
	{
		if(mTrackList[i] != 0)
		{
			return true;
		}
	}
	return false;
}