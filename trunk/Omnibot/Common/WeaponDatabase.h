////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2009-06-28 02:31:57 -0500 (Sun, 28 Jun 2009) $
// $LastChangedRevision: 4305 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __WEAPONDATABASE_H__
#define __WEAPONDATABASE_H__

typedef std::map<int, WeaponPtr> WeaponMap;

// class: WeaponDatabase
class WeaponDatabase
{
public:

	void RegisterWeapon(int _weaponId, const WeaponPtr &_wpn);

	WeaponPtr CopyWeapon(Client *_client, int _weaponId);
	void CopyAllWeapons(Client *_client, WeaponList &_list);

	String GetWeaponName(int _weaponId);
	WeaponPtr GetWeapon(int _weaponId);

	void LoadWeaponDefinitions(bool _clearall);
	void Unload();

	void Update();

	void ReloadScript(LiveUpdateKey _key);

	WeaponDatabase();
	~WeaponDatabase();
private:
	WeaponMap	m_WeaponMap;
};

extern WeaponDatabase g_WeaponDatabase;

//////////////////////////////////////////////////////////////////////////

#endif
