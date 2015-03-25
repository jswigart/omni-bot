////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __MEMORYRECORD_H__
#define __MEMORYRECORD_H__

#include "Omni-Bot_Types.h"
#include "TargetInfo.h"

namespace AiState
{
	class SensoryMemory;
}

// class: MemoryRecord
//		Represents an element in a bots memory system.
class MemoryRecord
{
public:
	friend class AiState::SensoryMemory;

	// var: mTargetInfo
	//		Latest info about this target entity
	TargetInfo mTargetInfo;

	inline GameEntity GetEntity() const
	{
		return mEntity;
	}

	inline int GetTimeLastSensed() const
	{
		return mTimeLastSensed;
	}
	inline int GetTimeBecameVisible() const
	{
		return mTimeBecameVisible;
	}
	inline int GetTimeLastVisible() const
	{
		return mTimeLastVisible;
	}

	int GetTimeTargetHasBeenVisible() const;
	int GetTimeHasBeenOutOfView() const;
	inline int GetIgnoreTargetTime() const
	{
		return mIgnoreForTargeting;
	}

	inline void IgnoreAsTarget( bool _ignore )
	{
		mIgnoreAsTarget = _ignore;
	}
	void IgnoreAsTargetForTime( int _milliseconds );
	bool ShouldIgnore() const;

	inline const Vector3f &GetLastSensedPosition() const
	{
		return mTargetInfo.mLastPosition;
	}
	inline const Vector3f &GetLastSensedVelocity() const
	{
		return mTargetInfo.mLastVelocity;
	}
	inline bool IsInFOV() const
	{
		return mInFOV;
	}
	inline bool IsAllied() const
	{
		return mIsAllied;
	}
	inline bool IsShootable() const
	{
		return mIsShootable;
	}

	Vector3f PredictPosition( float _time ) const;

	bool IsMovingTowards( const Vector3f &_pos ) const;

	int GetAge() const;
	void MarkUpdated();

	void Reset( GameEntity _ent = GameEntity() );

	int16_t GetSerial() const
	{
		return mSerial;
	}

	MemoryRecord()
		: mSerial( 0 )
	{
		Reset();
	}
	~MemoryRecord()
	{
	};
private:
	GameEntity	 mEntity;

	// int: mTimeLastSensed
	//		Records the time the entity was last sensed (seen or heard).
	//		Used to determine if a bot can 'remember' this record or not.
	int			 mTimeLastSensed;

	// int: mTimeBecameVisible
	//		How long the opponent has been visible. Useful for implementing reaction time.
	int			 mTimeBecameVisible;

	// int: mTimeLastVisible
	//		The last time this opponent was seen. "Freshness" of this sensed entity
	int			 mTimeLastVisible;

	int			 mTimeLastUpdated;

	// int: mIgnoreForTargeting
	//		The time until which the target should not be considered for targeting.
	int			 mIgnoreForTargeting;

	int16_t		 mSerial;

	// bool: mInFOV
	//		Marks whether this entity is current in this bots field of view.
	bool		 mInFOV : 1;

	// bool: mIsShootable
	//		Marks whether this entity can be shot(has clear line of sight)
	bool		 mIsShootable : 1;

	// bool: mIsAllied
	//		Marks whether this entity is an ally of this bot.
	bool		 mIsAllied : 1;

	// bool: mIgnoreAsTarget
	//		Ignore this entity as a valid target.
	bool		 mIgnoreAsTarget : 1;
};

class RecordHandle
{
public:
	int16_t GetIndex() const
	{
		return udata.mShort[ 0 ];
	}
	int16_t GetSerial() const
	{
		return udata.mShort[ 1 ];
	}

	int32_t AsInt() const
	{
		return udata.mInt;
	}
	void FromInt( int32_t _n )
	{
		udata.mInt = _n;
	}

	void Reset()
	{
		*this = RecordHandle();
	}

	bool IsValid() const
	{
		return udata.mShort[ 0 ] >= 0;
	}

	explicit RecordHandle( int16_t _index, int16_t _serial )
	{
		udata.mShort[ 0 ] = _index;
		udata.mShort[ 1 ] = _serial;
	}
	RecordHandle()
	{
		udata.mShort[ 0 ] = -1;
		udata.mShort[ 1 ] = 0;
	}
private:
	union udatatype
	{
		int32_t		 mInt;
		int16_t		 mShort[ 2 ];
	} udata;
};

// typedef: MemoryRecords
//		This type is defined as a vector of MemoryRecords for various usages
typedef std::vector<RecordHandle> MemoryRecords;

#endif
