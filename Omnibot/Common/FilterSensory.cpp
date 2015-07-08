////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "FilterSensory.h"

int FilterSensory::ANYPLAYERCLASS = 0;

FilterSensory::FilterSensory( Client *_client, AiState::SensoryMemory::Type _type )
	: mClient( _client )
	, mNumPositions( 0 )
	, mMaxDistance( -1.f )
	, mType( _type )
	, mMemorySpan( 0 )
	, mSortType( Sort_None )
	, mAnyPlayerClass( false )
{
	ClearPositions();
	ResetGroups();
	ResetClasses();
	ResetCategory();
}

void FilterSensory::MaxMemorySpan()
{
	mMemorySpan = std::numeric_limits<int>::max();
}

void FilterSensory::AddPosition( const Vector3f &_pos )
{
	if ( mNumPositions < MaxPositions - 1 )
		mPosition[ mNumPositions++ ] = _pos;
}

void FilterSensory::ClearPositions()
{
	mClosestPosition = 0;
	mNumPositions = 0;
	for ( int i = 0; i < MaxPositions; ++i )
		mPosition[ i ] = Vector3f::ZERO;
}

void FilterSensory::AddClass( int classId )
{
	if ( classId == ANYPLAYERCLASS )
		mAnyPlayerClass = true;

	for ( int i = 0; i < MaxClassType; ++i )
	{
		if ( mClassType[ i ] == 0 )
		{
			mClassType[ i ] = classId;
			break;
		}
	}
}

void FilterSensory::AddGroup( EntityGroup groupId )
{
	for ( int i = 0; i < MaxClassType; ++i )
	{
		if ( mGroupType[ i ] == 0 )
		{
			mGroupType[ i ] = groupId;
			break;
		}
	}
}

void FilterSensory::AddCategory( int _category )
{
	mCategory.SetFlag( _category, true );
}

void FilterSensory::AddIgnoreEntity( GameEntity _ent )
{
	for ( int i = 0; i < MaxIgnoreEntity; ++i )
	{
		if ( !mIgnoreEntity[ i ].IsValid() )
		{
			mIgnoreEntity[ i ] = _ent;
			break;
		}
	}
}

bool FilterSensory::IsBeingIgnored( GameEntity _ent )
{
	for ( int i = 0; i < MaxIgnoreEntity; ++i )
	{
		if ( mIgnoreEntity[ i ].IsValid() && mIgnoreEntity[ i ] == _ent )
		{
			return true;
		}
	}
	return false;
}

void FilterSensory::ResetGroups()
{
	for ( int i = 0; i < MaxGroups; ++i )
		mGroupType[ i ] = ENT_GRP_UNKNOWN;
}

void FilterSensory::ResetClasses()
{
	mAnyPlayerClass = false;
	for ( int i = 0; i < MaxClassType; ++i )
		mClassType[ i ] = ENT_CLASS_NONE;
}

void FilterSensory::ResetCategory()
{
	mCategory.ClearAll();
}

void FilterSensory::ResetIgnoreEntity()
{
	for ( int i = 0; i < MaxIgnoreEntity; ++i )
		mIgnoreEntity[ i ].Reset();
}

bool FilterSensory::PassesGroupFilter( const EntityInfo& entInfo ) const
{
	if ( mGroupType[ 0 ] == 0 )
		return true;

	for ( int i = 0; i < MaxGroups; ++i )
	{
		if ( mGroupType[ i ] && mGroupType[ i ] == entInfo.mGroup )
			return true;
	}
	return false;
}
bool FilterSensory::PassesClassFilter( const EntityInfo& entInfo ) const
{
	if ( mClassType[ 0 ] == 0 )
		return true;

	for ( int i = 0; i < MaxClassType; ++i )
	{
		if ( mClassType[ i ] && mClassType[ i ] == entInfo.mClassId )
			return true;
	}
	return false;
}

bool FilterSensory::PassesFilter( const EntityInfo& entInfo ) const
{
	return PassesGroupFilter( entInfo ) && PassesClassFilter( entInfo );
}
