////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TRACKABLE_H__
#define __TRACKABLE_H__

#include "common.h"

// class: Trackable
class Trackable
{
public:

	void AddReference(uint32_t _type);
	void DelReference(uint32_t _type);

	uint32_t GetRefCount(uint32_t _type);

	bool IsReferenced();

	Trackable();
	virtual ~Trackable();
protected:
	void _CheckIndex(uint32_t _type);

	typedef std::vector<int32_t> TrackList;
	TrackList	mTrackList;
};

#endif
