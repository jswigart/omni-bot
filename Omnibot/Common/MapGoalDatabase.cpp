////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptManager.h"
#include "gmWeapon.h"
#include "FileSystem.h"
#include "MapGoal.h"
#include "MapGoalDatabase.h"
#include "FileSystem.h"

#include <google\protobuf\io\zero_copy_stream_impl_lite.h>

class OmnibotSourceTree : public google::protobuf::compiler::SourceTree
{
public:
	typedef std::map<std::string, std::string> FileDataMap;

	virtual google::protobuf::io::ZeroCopyInputStream* Open( const std::string& filename )
	{
		FileDataMap::iterator it = mFileDataMap.find( filename );
		if ( it != mFileDataMap.end() )
		{
			google::protobuf::io::ArrayInputStream* s = new google::protobuf::io::ArrayInputStream( it->second.c_str(), it->second.length() );
			return s;
		}

		File file;
		if ( !file.OpenForRead( filename.c_str(), File::Text ) )
		{
			mErrorMsg = "File Not Found";
			return NULL;
		}

		std::string& str = mFileDataMap[ filename ];
		file.ReadWholeFile( str );

		return new google::protobuf::io::ArrayInputStream( str.c_str(), str.length() );
	}

	virtual std::string GetLastErrorMessage()
	{
		return mErrorMsg;
	}
private:
	std::string		mErrorMsg;

	FileDataMap		mFileDataMap;
};

MapGoalDatabase gMapGoalDatabase;

MapGoalDatabase::MapGoalDatabase()
{
}

MapGoalDatabase::~MapGoalDatabase()
{
}
//MapGoalPtr MapGoalDatabase::GetMapGoal(const std::string &_type)
//{
//	const uint32_t typeHash = Utils::Hash32(_type.c_str());
//	MapGoalMap::const_iterator it = mMapGoalMap.find(typeHash);
//	if(it != mMapGoalMap.end())
//		return it->second;
//	return MapGoalPtr();
//}
MapGoalPtr MapGoalDatabase::GetNewMapGoal( const std::string &_type )
{
	MapGoalPtr ptr;

	const uint32_t typeHash = Utils::Hash32( _type.c_str() );
	MapGoalMap::const_iterator it = mMapGoalMap.find( typeHash );
	if ( it != mMapGoalMap.end() )
	{
		ptr.reset( new MapGoal( _type.c_str() ) );
		ptr->CopyFrom( it->second.get() );
		ptr->SetSmartPtr( ptr );
	}
	else
	{
		Utils::OutputDebug( kError, va( "Unknown Goal Type: %s", _type.c_str() ).c_str() );
	}
	return ptr;
}

void MapGoalDatabase::RegisterMapGoal( const std::string &_type, const MapGoalPtr &_mg )
{
	const uint32_t typeHash = Utils::Hash32( _type.c_str() );
	MapGoalMap::const_iterator it = mMapGoalMap.find( typeHash );
	if ( it == mMapGoalMap.end() )
	{
		mMapGoalMap.insert( std::make_pair( typeHash, _mg ) );
	}
	else
	{
		Utils::OutputDebug( kError, va( "Duplicate MapGoal Id: %s", _type.c_str() ).c_str() );
	}
}

void MapGoalDatabase::LoadMapGoalProtos()
{
	DirectoryList mapgoalprotos;
	FileSystem::FindAllFiles( "scripts/mapgoals", mapgoalprotos, ".*.proto" );

	if ( mapgoalprotos.empty() )
		return;

	// Set up the source tree.
	OmnibotSourceTree source_tree;

	// Allocate the Importer.
	google::protobuf::compiler::Importer importer( &source_tree, this );

	std::vector<const google::protobuf::FileDescriptor*> dynamicDescriptors;

	for ( size_t i = 0; i < mapgoalprotos.size(); ++i )
	{
		importer.AddUnusedImportTrackFile( mapgoalprotos[ i ].string() );
		const google::protobuf::FileDescriptor* parsed_file = importer.Import( mapgoalprotos[ i ].string() );
		importer.ClearUnusedImportTrackFiles();

		if ( parsed_file )
			dynamicDescriptors.push_back( parsed_file );
	}

	for ( size_t i = 0; i < dynamicDescriptors.size(); ++i )
	{
		const google::protobuf::Descriptor* desc = dynamicDescriptors[ i ]->FindMessageTypeByName( "Attack" );
		if ( desc != NULL )
		{
			const google::protobuf::Message* msg = mMessageFactory.GetPrototype( desc );
			msg;
			EngineFuncs::ConsoleMessage( va( "Registered Goal Type '%s'", desc->name().c_str() ).c_str() );
		}
	}
}

void MapGoalDatabase::AddError( const std::string& filename, int line, int column, const std::string& message )
{
	EngineFuncs::ConsoleError( va( "%s( %d, %d ): %s", filename.c_str(), line, column, message.c_str() ).c_str() );
}

void MapGoalDatabase::LoadMapGoalDefinitions( bool _clearall )
{
	if ( _clearall )
		Unload();

	//LoadMapGoalProtos();

	DirectoryList mapgoalFiles;
	FileSystem::FindAllFiles( "scripts/mapgoals", mapgoalFiles, "mapgoal_.*.gm" );

	LOG( "Loading " << mapgoalFiles.size() <<
		" MapGoals from: global_scripts/mapgoals & scripts/mapgoals" );
	DirectoryList::const_iterator cIt = mapgoalFiles.begin(), cItEnd = mapgoalFiles.end();
	for ( ; cIt != cItEnd; ++cIt )
	{
		MapGoalPtr mg( new MapGoal( "" ) );

		filePath script( ( *cIt ).string().c_str() );
		LOG( "Loading MapGoal Definition: " << script );
		if ( mg->LoadFromFile( script ) && !mg->GetGoalType().empty() )
		{
			RegisterMapGoal( mg->GetGoalType(), mg );
		}
	}
}

gmGCRoot<gmUserObject> MapGoalDatabase::CreateMapGoalType( const std::string &_typename )
{
	MapGoalPtr mg( new MapGoal( _typename.c_str() ) );
	RegisterMapGoal( mg->GetGoalType(), mg );
	return mg->GetScriptObject( ScriptManager::GetInstance()->GetMachine() );
}

void MapGoalDatabase::Unload()
{
	mMapGoalMap.clear();
}