////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __MAPGOALDATABASE_H__
#define __MAPGOALDATABASE_H__

typedef std::map<obuint32, MapGoalPtr> MapGoalMap;

// class: MapGoalDatabase
class MapGoalDatabase
{
public:

	void RegisterMapGoal(const std::string &_type, const MapGoalPtr &_wpn);
	gmGCRoot<gmUserObject> CreateMapGoalType(const std::string &_typename);

	std::string GetWeaponName(int _weaponId);

	MapGoalPtr GetNewMapGoal(const std::string &_type);

	void LoadMapGoalDefinitions(bool _clearall);
	void Unload();

	MapGoalDatabase();
	~MapGoalDatabase();
private:
	MapGoalMap	m_MapGoalMap;
};

extern MapGoalDatabase g_MapGoalDatabase;

#endif
