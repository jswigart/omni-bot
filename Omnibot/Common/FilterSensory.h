////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __FILTERSENSORY_H__
#define __FILTERSENSORY_H__

class Client;
#include "BotSensoryMemory.h"

// class: FilterSensory
class FilterSensory
{
public:

	static int ANYPLAYERCLASS;

	enum SortType
	{
		Sort_None, Sort_NearToFar, Sort_FarToNear,
	};
	enum
	{
		MaxGroups = 8, MaxClassType = 8, MaxIgnoreEntity = 8, MaxPositions = 8
	};

	void AddGroup( EntityGroup groupId );
	void AddClass( int classId );
	void AddCategory( int _category );	
	void AddIgnoreEntity( GameEntity _ent );

	bool IsBeingIgnored( GameEntity _ent );

	void ResetGroups();
	void ResetClasses();
	void ResetCategory();
	void ResetIgnoreEntity();

	bool PassesFilter( const EntityInfo& entInfo ) const;

	inline void SetSortType( SortType _sort )
	{
		mSortType = _sort;
	}

	void AddPosition( const Vector3f &_pos );
	void ClearPositions();
	const Vector3f &GetPosition( int i ) const
	{
		return mPosition[ i ];
	}
	int GetNumPositions() const
	{
		return mNumPositions;
	}
	const Vector3f &GetTriggerPosition() const
	{
		return mPosition[ mClosestPosition ];
	}

	float GetMaxDistance() const
	{
		return mMaxDistance;
	}
	void SetMaxDistance( float _rad )
	{
		mMaxDistance = _rad;
	}

	void MemorySpan( int _time )
	{
		mMemorySpan = _time;
	}
	void MaxMemorySpan();

	virtual void Check( int _index, const MemoryRecord &_record ) = 0;
	virtual bool CheckEx( const MemoryRecord &_record )
	{
		return true;
	}

	inline GameEntity GetBestEntity() const
	{
		return mBestEntity;
	}

	virtual bool DetectedSomething() const
	{
		return GetBestEntity().IsValid();
	}

	virtual void Reset()
	{
		mBestEntity.Reset();
	}

	virtual void PostQuery()
	{
	}

	FilterSensory( Client *_client, AiState::SensoryMemory::Type _type );
	virtual ~FilterSensory()
	{
	};
protected:
	Client*							mClient;

	Vector3f						mPosition[ MaxPositions ];
	int								mNumPositions;
	int								mClosestPosition;

	float							mMaxDistance;

	AiState::SensoryMemory::Type	mType;

	int								mMemorySpan;

	EntityGroup						mGroupType[ MaxGroups ];
	int								mClassType[ MaxClassType ];

	BitFlag32						mCategory;
	GameEntity						mIgnoreEntity[ MaxIgnoreEntity ];

	GameEntity						mBestEntity;

	SortType						mSortType;
	bool							mAnyPlayerClass : 1;

	FilterSensory();
private:
	bool PassesGroupFilter( const EntityInfo& entInfo ) const;
	bool PassesClassFilter( const EntityInfo& entInfo ) const;
};

typedef std::shared_ptr<FilterSensory> FilterPtr;

#endif
