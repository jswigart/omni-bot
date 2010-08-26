////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2010-02-10 23:38:20 -0600 (Wed, 10 Feb 2010) $
// $LastChangedRevision: 4682 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MAPGOALDATABASE_H__
#define __MAPGOALDATABASE_H__

typedef std::map<obuint32, MapGoalPtr> MapGoalMap;

// class: MapGoalDatabase
class MapGoalDatabase
{
public:

	void RegisterMapGoal(const String &_type, const MapGoalPtr &_wpn);
	gmGCRoot<gmUserObject> CreateMapGoalType(const String &_typename);

	String GetWeaponName(int _weaponId);

	MapGoalPtr GetNewMapGoal(const String &_type);

	void LoadMapGoalDefinitions(bool _clearall);
	void Unload();

	MapGoalDatabase();
	~MapGoalDatabase();
private:
	MapGoalMap	m_MapGoalMap;
};

extern MapGoalDatabase g_MapGoalDatabase;

#endif
