////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "NameManager.h"
#include "System.h"
#include "IGame.h"

//////////////////////////////////////////////////////////////////////////
class BotIdentity
{
public:

	BotIdentity()
		: mDesiredClassId( RANDOM_CLASS_IF_NO_CLASS )
		, mDesiredTeamId( RANDOM_TEAM_IF_NO_TEAM )
	{
	}
private:
	std::string	 mName;
	int32_t	 mDesiredClassId;
	int32_t	 mDesiredTeamId;
};
//////////////////////////////////////////////////////////////////////////

NameReference::NameReference( const std::string &_name, const std::string &_profile )
	: mName( _name )
	, mProfileName( _profile )
{
}

NameReference::~NameReference()
{
}

NameManager *NameManager::mInstance = NULL;

NameManager *NameManager::GetInstance()
{
	if ( !mInstance )
		mInstance = new NameManager;
	return mInstance;
}

void NameManager::DeleteInstance()
{
	OB_DELETE( mInstance );
}

bool NameManager::AddName( const std::string &_name, const std::string &_profile )
{
	NamesMap::const_iterator cIt = mNamesMap.find( _name );
	if ( cIt == mNamesMap.end() )
	{
		NamePtr np( new NameReference( _name, _profile ) );
		mNamesMap.insert( std::make_pair( _name, np ) );
		return true;
	}
	return false;
}

void NameManager::DeleteName( const std::string &_name )
{
	NamesMap::iterator it = mNamesMap.find( _name );
	if ( it != mNamesMap.end() )
		mNamesMap.erase( it );
}

const std::string NameManager::GetProfileForName( const std::string &_name ) const
{
	NamesMap::const_iterator cIt = mNamesMap.find( _name );
	if ( cIt != mNamesMap.end() )
	{
		return cIt->second->GetProfileName();
	}
	return std::string();
}

const std::string NameManager::GetProfileForClass( const int _class ) const
{
	DefaultProfileMap::const_iterator it = mProfileMap.find( _class );
	if ( it != mProfileMap.end() )
	{
		return it->second;
	}
	return std::string();
}

void NameManager::ClearNames()
{
	mNamesMap.clear();
}

NamePtr NameManager::GetName( const std::string &_preferred )
{
	if ( !_preferred.empty() )
	{
		NamesMap::iterator it = mNamesMap.find( _preferred );
		if ( it != mNamesMap.end() )
			return it->second;
		return NamePtr( new NameReference( _preferred ) );
	}

	StringVector lst;
	NamesMap::iterator it = mNamesMap.begin(),
		itEnd = mNamesMap.end();
	for ( ; it != itEnd; ++it )
	{
		if ( it->second.use_count() <= 1 )
			lst.push_back( it->first );
	}

	if ( !lst.empty() )
	{
		std::random_shuffle( lst.begin(), lst.end() );
		return GetName( lst.front() );
	}

	return NamePtr();
}

void NameManager::SetProfileForClass( const int classId, const std::string & name )
{
	mProfileMap.insert( std::make_pair( classId, name ) );

	EntityInfo entInfo;
	entInfo.mGroup = ENT_GRP_PLAYER;
	entInfo.mClassId = (uint16_t)classId;

	std::string groupName, className;
	System::mInstance->mGame->FindClassName( groupName, className, entInfo.mGroup, entInfo.mClassId );
	LOG( "Class " << groupName.c_str() << ":" << className.c_str() << " : using profile " << name.c_str() );
}
