////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompCommon.h"

#include "ScriptManager.h"
#include "gmWeapon.h"
#include "WeaponDatabase.h"

WeaponDatabase g_WeaponDatabase;

WeaponDatabase::WeaponDatabase()
{
}

WeaponDatabase::~WeaponDatabase()
{
}

void WeaponDatabase::RegisterWeapon(int _weaponId, const WeaponPtr &_wpn)
{
	WeaponMap::const_iterator it = m_WeaponMap.find(_weaponId);
	if(it == m_WeaponMap.end())
	{
		m_WeaponMap.insert(std::make_pair(_weaponId, _wpn));
	}
	else
	{
		Utils::OutputDebug(kError, Utils::VA("Duplicate Weapon Id: %d", _weaponId));
	}
}

WeaponPtr WeaponDatabase::CopyWeapon(Client *_client, int _weaponId)
{
	WeaponMap::const_iterator it = m_WeaponMap.find(_weaponId);
	if(it != m_WeaponMap.end())
	{
		WeaponPtr wp(new Weapon(_client, (*it).second.get()));
		return wp;
	}
	return WeaponPtr();
}

void WeaponDatabase::CopyAllWeapons(Client *_client, WeaponList &_list)
{
	WeaponMap::iterator it = m_WeaponMap.begin(), itEnd = m_WeaponMap.end();
	for(; it != itEnd; ++it)
	{
		WeaponPtr wp(new Weapon(_client, (*it).second.get()));
		_list.push_back(wp);
	}
}

String WeaponDatabase::GetWeaponName(int _weaponId)
{
	WeaponMap::const_iterator it = m_WeaponMap.find(_weaponId);
	if(it != m_WeaponMap.end())
	{
		return (*it).second->GetWeaponName();
	}
	return "";
}

WeaponPtr WeaponDatabase::GetWeapon(int _weaponId)
{
	WeaponMap::const_iterator it = m_WeaponMap.find(_weaponId);
	if(it != m_WeaponMap.end())
	{
		return (*it).second;
	}
	return WeaponPtr();
}

void WeaponDatabase::LoadWeaponDefinitions(bool _clearall)
{
	if(_clearall)
		m_WeaponMap.clear();

	boost::regex ex("weapon_.*.gm");
	DirectoryList wpnFiles;
	FileSystem::FindAllFiles("scripts/weapons", wpnFiles, ex);

	LOG("Loading %d weapon scripts from: scripts/weapons",wpnFiles.size());
	DirectoryList::const_iterator cIt = wpnFiles.begin(), cItEnd = wpnFiles.end();
	for(; cIt != cItEnd; ++cIt)
	{
		// skip the default weapon script, we just use that for initializing other scripts.
		if((*cIt).leaf() == "weapon_defaults.gm")
			continue;

		WeaponPtr wpn(new Weapon);

		LOG("Loading Weapon Definition: %s",(*cIt).string().c_str());
		if(wpn->InitScriptSource(*cIt))
		{
			if(wpn->GetWeaponID() != 0 && wpn->GetWeaponNameHash())
			{
				RegisterWeapon(wpn->GetWeaponID(), wpn);
			}
		}
		else
		{
			LOGERR("Error Running Weapon Script: %s",(*cIt).string().c_str());
			OBASSERT(0, "Error Running Weapon Script");
		}
	}
}

void WeaponDatabase::Unload()
{
	m_WeaponMap.clear();
}

void WeaponDatabase::ReloadScript(LiveUpdateKey _key)
{
	WeaponMap::iterator it = m_WeaponMap.begin(), itEnd = m_WeaponMap.end();
	for(; it != itEnd; ++it)
	{
		WeaponPtr wpn = (*it).second;
		if(wpn->GetLiveUpdateKey() == _key)
		{
			EngineFuncs::ConsoleMessagef("File changed, reloading %s",wpn->GetScriptPath().string().c_str());
			LOG("Re-Loading Weapon Definition: %s",wpn->GetScriptPath().string().c_str());
			
			WeaponPtr newwpn(new Weapon);

			if(newwpn->InitScriptSource(wpn->GetScriptPath()))
			{
				if(newwpn->GetWeaponID() != 0 && newwpn->GetWeaponNameHash())
				{
					(*it).second = newwpn;
					
					Event_RefreshWeapon d = { wpn->GetWeaponID() };
					MessageHelper evt(MESSAGE_REFRESHWEAPON, &d, sizeof(d));
					IGameManager::GetInstance()->GetGame()->DispatchGlobalEvent(evt);
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

bool WeaponScriptResource::InitScriptSource(const fs::path &_path)
{
	gmMachine * pMachine = ScriptManager::GetInstance()->GetMachine();

	int iThreadId;
	gmGCRoot<gmUserObject> UserObj = GetScriptObject(ScriptManager::GetInstance()->GetMachine());
	gmVariable varThis(UserObj);

	gmTableObject * weaponTable = pMachine->GetGlobals()->Get(pMachine,"WEAPON").GetTableObjectSafe();
	gmTableObject * weaponTableOld = weaponTable ? pMachine->AllocTableObject() : 0;
	if(weaponTableOld)
	{
		weaponTable->CopyTo(pMachine,weaponTableOld);
	}

	const bool a = ScriptManager::GetInstance()->ExecuteFile("scripts/weapons/weapon_defaults.gm", iThreadId, &varThis);
	const bool b = ScriptManager::GetInstance()->ExecuteFile(_path, iThreadId, &varThis);
	const bool c = ScriptResource::InitScriptSource(_path);

	if(a && b && c)
	{
		if(weaponTableOld)
		{
			gmTableIterator tIt;
			gmTableNode * pNode = weaponTable->GetFirst(tIt);
			while(pNode)
			{
				// if this entry didn't exist in the old table, we need to register it
				if(weaponTableOld->Get(pNode->m_key).IsNull())
				{
					const char * weaponName = pNode->m_key.GetCStringSafe(0);
					if(weaponName && pNode->m_value.IsInt())
					{
						if(IGameManager::GetInstance()->GetGame()->AddWeaponId(weaponName,pNode->m_value.GetInt()))
						{
							LOG("Adding new weapon enumeration: %s (%d)",weaponName,pNode->m_value.GetInt());
						}
						else
						{
							LOGERR("Can't add new weapon enumeration: %s (%d)",weaponName,pNode->m_value.GetInt());
						}
					}
				}
				pNode = weaponTable->GetNext(tIt);
			}
		}
		return true;
	}
	return false;
}
