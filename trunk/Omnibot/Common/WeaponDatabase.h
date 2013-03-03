////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __WEAPONDATABASE_H__
#define __WEAPONDATABASE_H__

#include "Weapon.h"
#include "BotWeaponSystem.h"

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
