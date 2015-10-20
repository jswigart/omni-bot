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

#include <google\protobuf\dynamic_message.h>
#include <google\protobuf\compiler\importer.h>

typedef std::map<uint32_t, MapGoalPtr> MapGoalMap;

// class: MapGoalDatabase
class MapGoalDatabase : public google::protobuf::compiler::MultiFileErrorCollector
{
public:
	void RegisterMapGoal( const std::string &_type, const MapGoalPtr &_wpn );
	gmGCRoot<gmUserObject> CreateMapGoalType( const std::string &_typename );

	std::string GetWeaponName( int _weaponId );

	MapGoalPtr GetNewMapGoal( const std::string &_type );

	void LoadMapGoalDefinitions( bool _clearall );
	void Unload();

	virtual void AddError( const std::string& filename, int line, int column, const std::string& message );

	MapGoalDatabase();
	~MapGoalDatabase();
private:
	MapGoalMap	mMapGoalMap;

	void LoadMapGoalProtos();

	google::protobuf::DynamicMessageFactory		mMessageFactory;
};

extern MapGoalDatabase gMapGoalDatabase;

#endif
