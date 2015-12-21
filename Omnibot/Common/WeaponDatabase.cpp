////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptManager.h"
#include "WeaponDatabase.h"
#include "IGameManager.h"
#include "gmWeapon.h"

WeaponDatabase gWeaponDatabase;

WeaponDatabase::WeaponDatabase()
{
}

WeaponDatabase::~WeaponDatabase()
{
}

void WeaponDatabase::RegisterWeapon( int _weaponId, const WeaponPtr &_wpn )
{
	WeaponMap::const_iterator it = mWeaponMap.find( _weaponId );
	if ( it == mWeaponMap.end() )
	{
		mWeaponMap.insert( std::make_pair( _weaponId, _wpn ) );
	}
	else
	{
		Utils::OutputDebug( kError, va( "Duplicate Weapon Id: %d", _weaponId ).c_str() );
	}
}

WeaponPtr WeaponDatabase::CopyWeapon( Client *_client, int _weaponId )
{
	WeaponMap::const_iterator it = mWeaponMap.find( _weaponId );
	if ( it != mWeaponMap.end() )
	{
		WeaponPtr wp( new Weapon( _client, ( *it ).second.get() ) );
		return wp;
	}
	return WeaponPtr();
}

void WeaponDatabase::CopyAllWeapons( Client *_client, WeaponList &_list )
{
	WeaponMap::iterator it = mWeaponMap.begin(), itEnd = mWeaponMap.end();
	for ( ; it != itEnd; ++it )
	{
		WeaponPtr wp( new Weapon( _client, ( *it ).second.get() ) );
		_list.push_back( wp );
	}
}

std::string WeaponDatabase::GetWeaponName( int _weaponId )
{
	WeaponMap::const_iterator it = mWeaponMap.find( _weaponId );
	if ( it != mWeaponMap.end() )
	{
		return ( *it ).second->GetWeaponName();
	}
	return "";
}

WeaponPtr WeaponDatabase::GetWeapon( int _weaponId )
{
	WeaponMap::const_iterator it = mWeaponMap.find( _weaponId );
	if ( it != mWeaponMap.end() )
	{
		return ( *it ).second;
	}
	return WeaponPtr();
}

void WeaponDatabase::LoadWeaponDefinitions( bool _clearall )
{
	if ( _clearall )
		mWeaponMap.clear();

	DirectoryList wpnFiles;
	FileSystem::FindAllFiles( "scripts/weapons", wpnFiles, "weapon_.*.gm" );

	LOG( "Loading " << wpnFiles.size() << " weapon scripts from: scripts/weapons" );
	DirectoryList::const_iterator cIt = wpnFiles.begin(), cItEnd = wpnFiles.end();
	for ( ; cIt != cItEnd; ++cIt )
	{
		// skip the default weapon script, we just use that for initializing other scripts.
		if ( ( *cIt ).leaf() == "weapon_defaults.gm" )
			continue;

		WeaponPtr wpn( new Weapon );

		LOG( "Loading Weapon Definition: " << ( *cIt ).string() );

		filePath script( ( *cIt ).string().c_str() );
		if ( wpn->InitScriptSource( script ) )
		{
			if ( wpn->GetWeaponID() != 0 && wpn->GetWeaponNameHash() )
			{
				RegisterWeapon( wpn->GetWeaponID(), wpn );
			}
		}
		else
		{
			LOGERR( "Error Running Weapon Script: " << ( *cIt ).string() );
		}
	}
}

void WeaponDatabase::Unload()
{
	mWeaponMap.clear();
}

void WeaponDatabase::ReloadScript( LiveUpdateKey _key )
{
	WeaponMap::iterator it = mWeaponMap.begin(), itEnd = mWeaponMap.end();
	for ( ; it != itEnd; ++it )
	{
		WeaponPtr wpn = ( *it ).second;
		if ( wpn->GetLiveUpdateKey() == _key )
		{
			EngineFuncs::ConsoleMessage( va( "File changed, reloading %s", wpn->GetScriptPath().c_str() ).c_str() );
			LOG( "Re-Loading Weapon Definition: " << wpn->GetScriptPath().c_str() );

			WeaponPtr newwpn( new Weapon );

			if ( newwpn->InitScriptSource( wpn->GetScriptPath() ) )
			{
				if ( newwpn->GetWeaponID() != 0 && newwpn->GetWeaponNameHash() )
				{
					( *it ).second = newwpn;

					EvRefreshWeapon::Msg event;
					event.mData.mWeaponId = wpn->GetWeaponID();
					System::mInstance->mGame->DispatchEvent( event );
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

bool WeaponScriptResource::InitScriptSource( const filePath &_path )
{
	gmMachine * pMachine = ScriptManager::GetInstance()->GetMachine();

	int iThreadId;
	gmGCRoot<gmUserObject> UserObj = GetScriptObject( ScriptManager::GetInstance()->GetMachine() );
	gmVariable varThis( UserObj );

	gmTableObject * weaponTable = pMachine->GetGlobals()->Get( pMachine, "WEAPON" ).GetTableObjectSafe();
	gmTableObject * weaponTableOld = weaponTable ? pMachine->AllocTableObject() : 0;
	if ( weaponTableOld )
	{
		weaponTable->CopyTo( pMachine, weaponTableOld );
	}

	const filePath wpnDefault( "scripts/weapons/weapon_defaults.gm" );
	const bool a = ScriptManager::GetInstance()->ExecuteFile( wpnDefault, iThreadId, &varThis );
	const bool b = ScriptManager::GetInstance()->ExecuteFile( _path, iThreadId, &varThis );
	const bool c = ScriptResource::InitScriptSource( _path );

	if ( a && b && c )
	{
		if ( weaponTableOld )
		{
			gmTableIterator tIt;
			gmTableNode * pNode = weaponTable->GetFirst( tIt );
			while ( pNode )
			{
				// if this entry didn't exist in the old table, we need to register it
				if ( weaponTableOld->Get( pNode->m_key ).IsNull() )
				{
					const char * weaponName = pNode->m_key.GetCStringSafe( 0 );
					if ( weaponName && pNode->m_value.IsInt() )
					{
						if ( System::mInstance->mGame->AddWeaponId( weaponName, pNode->m_value.GetInt() ) )
						{
							LOG( "Adding new weapon enumeration: " << weaponName << "(" << pNode->m_value.GetInt() << ")" );
						}
						else
						{
							LOG( "Can't add new weapon enumeration: " << weaponName << "(" << pNode->m_value.GetInt() << ")" );
						}
					}
				}
				pNode = weaponTable->GetNext( tIt );
			}
		}
		return true;
	}
	return false;
}
