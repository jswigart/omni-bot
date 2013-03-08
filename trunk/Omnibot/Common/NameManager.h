////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __NAMEMANAGER_H__
#define __NAMEMANAGER_H__

#include "common.h"

class NameReference
{
public:
	friend class NameManager;

	const std::string &GetName() { return m_Name; }
	const std::string &GetProfileName() { return m_ProfileName; }

	NameReference(const std::string &_name = "", const std::string &_profile = "");
	~NameReference();
private:
	std::string		m_Name;
	std::string		m_ProfileName;
};

typedef boost::shared_ptr<NameReference> NamePtr;

// class: NameManager
class NameManager
{
public:

	bool AddName(const std::string &_name, const std::string &_profile);
	void DeleteName(const std::string &_name);
	void ClearNames();

	void SetProfileForClass(const int _class, const std::string &_name);

	NamePtr GetName(const std::string &_preferred = "");

	const std::string GetProfileForName(const std::string &_name) const;
	const std::string GetProfileForClass(int _class) const;

	//void LoadBotNames();

	static NameManager *GetInstance();
	static void DeleteInstance();
protected:

	typedef std::map<std::string, NamePtr> NamesMap;

	NamesMap	m_NamesMap;

	// typedef: DefaultProfileMap
	typedef std::map<int, std::string> DefaultProfileMap;
	DefaultProfileMap		m_ProfileMap;

	static NameManager		*m_Instance;
	NameManager() {};
	~NameManager() {};
	NameManager &operator=(const NameManager&);
};

#endif
