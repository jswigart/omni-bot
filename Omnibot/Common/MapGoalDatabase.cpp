////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptManager.h"
#include "gmWeapon.h"

#include "MapGoal.h"
#include "MapGoalDatabase.h"
#include "FileSystem.h"

MapGoalDatabase g_MapGoalDatabase;

#ifdef Prof_ENABLED
CustomProfilerZone gDynamicZones;
#endif

MapGoalDatabase::MapGoalDatabase()
{
}

MapGoalDatabase::~MapGoalDatabase()
{
}
//MapGoalPtr MapGoalDatabase::GetMapGoal(const std::string &_type)
//{
//	const obuint32 typeHash = Utils::Hash32(_type.c_str());
//	MapGoalMap::const_iterator it = m_MapGoalMap.find(typeHash);
//	if(it != m_MapGoalMap.end())
//		return it->second;
//	return MapGoalPtr();
//}
MapGoalPtr MapGoalDatabase::GetNewMapGoal(const std::string &_type)
{
	MapGoalPtr ptr;

	const obuint32 typeHash = Utils::Hash32(_type.c_str());
	MapGoalMap::const_iterator it = m_MapGoalMap.find(typeHash);
	if(it != m_MapGoalMap.end())
	{
		ptr.reset(new MapGoal(_type.c_str()));
		ptr->CopyFrom(it->second.get());
		ptr->SetSmartPtr(ptr);
	}
	else
	{
		Utils::OutputDebug(kError, va("Unknown Goal Type: %s", _type.c_str()));
	}
	return ptr;
}

void MapGoalDatabase::RegisterMapGoal(const std::string &_type, const MapGoalPtr &_mg)
{
	const obuint32 typeHash = Utils::Hash32(_type.c_str());
	MapGoalMap::const_iterator it = m_MapGoalMap.find(typeHash);
	if(it == m_MapGoalMap.end())
	{
		_mg->SetProfilerZone(_type.c_str());
		m_MapGoalMap.insert(std::make_pair(typeHash, _mg));
	}
	else
	{
		Utils::OutputDebug(kError, va("Duplicate MapGoal Id: %s", _type.c_str()));
	}
}

void MapGoalDatabase::LoadMapGoalDefinitions(bool _clearall)
{
	if(_clearall)
		Unload();

	DirectoryList mapgoalFiles;
	FileSystem::FindAllFiles("scripts/mapgoals", mapgoalFiles, "mapgoal_.*.gm");

	LOG("Loading " << mapgoalFiles.size() <<
		" MapGoals from: global_scripts/mapgoals & scripts/mapgoals");
	DirectoryList::const_iterator cIt = mapgoalFiles.begin(), cItEnd = mapgoalFiles.end();
	for(; cIt != cItEnd; ++cIt)
	{
		MapGoalPtr mg(new MapGoal(""));

		filePath script( (*cIt).string().c_str() );
		LOG("Loading MapGoal Definition: " << script);
		if(mg->LoadFromFile(script) && !mg->GetGoalType().empty())
		{
			RegisterMapGoal(mg->GetGoalType(),mg);
		}
	}
}

gmGCRoot<gmUserObject> MapGoalDatabase::CreateMapGoalType(const std::string &_typename)
{
	MapGoalPtr mg(new MapGoal(_typename.c_str()));
	RegisterMapGoal(mg->GetGoalType(),mg);
	return mg->GetScriptObject(ScriptManager::GetInstance()->GetMachine());
}

void MapGoalDatabase::Unload()
{
	m_MapGoalMap.clear();
}