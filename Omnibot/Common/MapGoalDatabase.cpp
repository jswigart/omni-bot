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
#include "MapGoalDatabase.h"

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
//MapGoalPtr MapGoalDatabase::GetMapGoal(const String &_type)
//{
//	const obuint32 typeHash = Utils::Hash32(_type.c_str());
//	MapGoalMap::const_iterator it = m_MapGoalMap.find(typeHash);
//	if(it != m_MapGoalMap.end())
//		return it->second;
//	return MapGoalPtr();
//}
MapGoalPtr MapGoalDatabase::GetNewMapGoal(const String &_type)
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
	return ptr;
}

void MapGoalDatabase::RegisterMapGoal(const String &_type, const MapGoalPtr &_mg)
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
		Utils::OutputDebug(kError, Utils::VA("Duplicate MapGoal Id: %s", _type.c_str()));
	}
}

void MapGoalDatabase::LoadMapGoalDefinitions(bool _clearall)
{
	if(_clearall)
		Unload();

	boost::regex ex("mapgoal_.*.gm");
	DirectoryList mapgoalFiles;
	FileSystem::FindAllFiles("scripts/mapgoals", mapgoalFiles, ex);

	LOG("Loading %d MapGoals from: global_scripts/mapgoals & scripts/mapgoals",mapgoalFiles.size());
	DirectoryList::const_iterator cIt = mapgoalFiles.begin(), cItEnd = mapgoalFiles.end();
	for(; cIt != cItEnd; ++cIt)
	{
		MapGoalPtr mg(new MapGoal(""));

		LOG("Loading MapGoal Definition: %s",(*cIt).string().c_str());
		if(mg->LoadFromFile(*cIt) && !mg->GetGoalType().empty())
		{
			RegisterMapGoal(mg->GetGoalType(),mg);
		}
	}
}

gmGCRoot<gmUserObject> MapGoalDatabase::CreateMapGoalType(const String &_typename)
{
	MapGoalPtr mg(new MapGoal(_typename.c_str()));
	RegisterMapGoal(mg->GetGoalType(),mg);
	return mg->GetScriptObject(ScriptManager::GetInstance()->GetMachine());
}

void MapGoalDatabase::Unload()
{
	m_MapGoalMap.clear();
}
