////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "IGameManager.h"
#include "IGame.h"
#include "mdump.h"

#include "GoalManager.h"
#include "TriggerManager.h"
#include "ScriptManager.h"
#include "Timer.h"

#include "RenderBuffer.h"

#include "WeaponDatabase.h"
#include "FileSystem.h"
#include "FileDownloader.h"
#include "InterfaceFuncs.h"

#include "PathPlannerFloodFill.h"
#include "PathPlannerNavmesh.h"
#include "PathPlannerRecast.h"

IEngineInterface *g_EngineFuncs = 0;

boost::recursive_mutex gGlobalUpdate;

//////////////////////////////////////////////////////////////////////////

void ProtobufLogHandler( google::protobuf::LogLevel level, const char* filename, int line, const std::string& message )
{
	switch ( level )
	{
		case google::protobuf::LOGLEVEL_INFO:
			EngineFuncs::ConsoleMessage( va( "%s( %d ) : %s", filename, line, message.c_str() ) );
			break;
		case google::protobuf::LOGLEVEL_WARNING:
		case google::protobuf::LOGLEVEL_ERROR:
		case google::protobuf::LOGLEVEL_FATAL:
			EngineFuncs::ConsoleError( va( "%s( %d ) : %s", filename, line, message.c_str() ) );
			break;
	}
}

//////////////////////////////////////////////////////////////////////////

IGame *CreateGameInstance();

boost::thread::id IGameManager::sMainThread;

IGameManager::IGameManager()
{
	memset( &g_EngineFuncs, 0, sizeof( g_EngineFuncs ) );
}

omnibot_error IGameManager::CreateGame( IEngineInterface *_pEngineFuncs, int _version )
{
	sMainThread = boost::this_thread::get_id();

	google::protobuf::SetLogHandler( ProtobufLogHandler );

	MiniDumper::Init( "Omni-bot" );

	Timer loadTime;

	LOGFUNCBLOCK;

	InitCommands();

	srand( (unsigned int)time( NULL ) );

	g_EngineFuncs = _pEngineFuncs;

	mBotSystem.mGame = CreateGameInstance();

	// Verify the version is correct.
	if ( !mBotSystem.mGame->CheckVersion( _version ) )
	{
		OB_DELETE( mBotSystem.mGame );
		return BOT_ERROR_WRONGVERSION;
	}

	if ( !FileSystem::InitFileSystem() )
		return BOT_ERROR_FILESYSTEM;

	FileSystem::SetWriteDirectory( FileSystem::GetModFolder() );

	FileSystem::MakeDirectory( "user" );
	FileSystem::MakeDirectory( "nav" );
	FileSystem::MakeDirectory( "scripts" );
	FileSystem::MakeDirectory( "gui" );
	//FileSystem::MakeDirectory("gui/weapons");

	FileSystem::MountArchives( "scripts" );
	FileSystem::MountArchives( "scripts/weapons" );
	FileSystem::MountArchives( "scripts/goals" );
	FileSystem::MountArchives( "user/download", "nav" );
	FileSystem::MountArchives( "nav" );
	FileSystem::MountArchives( "global_scripts" );
	FileSystem::MountArchives( "user" );

	if ( FileSystem::FileDelete( "user/logged.gm" ) )
		EngineFuncs::ConsoleMessage( "deleted user/logged.gm" );

	if ( !Options::LoadConfigFile( "user/omni-bot.cfg" ) )
	{
		if ( !Options::LoadConfigFile( "config/omni-bot.cfg" ) )
		{
			// error
		}
	}

	// Initialize the Scripting System.
	mBotSystem.mScript = ScriptManager::GetInstance();
	mBotSystem.mScript->Init();

	//////////////////////////////////////////////////////////////////////////
	// Set up global config options.
	//Options::SetValue("Settings","TickRate",20,false);
	Options::SetValue( "Debug", "DumpFileEnable", true, false );
	Options::SetValue( "Debug", "DumpFileDialog", true, false );
	Options::SetValue( "Script", "LiveUpdate", true, false );
	Options::SetValue( "Script", "Debug", "true", false );
	Options::SetValue( "Script", "EnableRemoteDebugger", "true", false );
	Options::SetValue( "Debug Render", "EnableInterProcess", "true", false );

	Options::SetValue( "Downloader", "Server", "", false );
	Options::SetValue( "Downloader", "Script", "", false );
	Options::SetValue( "Downloader", "DownloadMissingNav", "true", false );

	Options::SetValue( "Log", "LogInfo", "true", false );
	Options::SetValue( "Log", "LogWarnings", "true", false );
	Options::SetValue( "Log", "LogErrors", "true", false );
	Options::SetValue( "Log", "LogCriticalErrors", "true", false );

#ifdef ENABLE_REMOTE_DEBUGGING
	Options::SetValue("RemoteWindow","Enabled",0,false);
	Options::SetValue("RemoteWindow","Port",m_Remote.getPort(),false);
#endif

	//////////////////////////////////////////////////////////////////////////
	// logging options
	g_Logger.LogMask() = 0;

	bool l = true;
	if ( Options::GetValue( "Log", "LogInfo", l ) && l )
		g_Logger.LogMask() |= Logger::LOG_INFO;
	if ( Options::GetValue( "Log", "LogWarnings", l ) && l )
		g_Logger.LogMask() |= Logger::LOG_WARN;
	if ( Options::GetValue( "Log", "LogErrors", l ) && l )
		g_Logger.LogMask() |= Logger::LOG_ERR;
	if ( Options::GetValue( "Log", "LogCriticalErrors", l ) && l )
		g_Logger.LogMask() |= Logger::LOG_CRIT;
	//////////////////////////////////////////////////////////////////////////

#ifdef ENABLE_REMOTE_DEBUGGING
	{
		int numConnections = 0;
		if(Options::GetValue("RemoteWindow","Enabled",numConnections)) {
			m_Remote.setMaxConnections( (unsigned short)numConnections );
		}
		int remotePort = m_Remote.getPort();
		if(Options::GetValue("RemoteWindow","Port",remotePort)) {
			m_Remote.setPort( (uint16)remotePort );
		}

		m_Remote.init( 8192, 4096, true );
	}
#endif

	RenderBuffer::Init();

	// Create the navigation system
	switch ( mBotSystem.mGame->GetDefaultNavigator() )
	{
		case NAVID_NAVMESH:
			mBotSystem.mNavigation = new PathPlannerNavMesh;
			break;
		case NAVID_FLOODFILL:
			mBotSystem.mNavigation = new PathPlannerFloodFill;
			break;
		case NAVID_RECAST:
			mBotSystem.mNavigation = new PathPlannerRecast;
			break;
		default:
			LOGERR( "Unknown Path Planner!" );
			return BOT_ERROR_CANTINITBOT;
	};

	OBASSERT( mBotSystem.mNavigation, "No Path Planner!" );
	if ( mBotSystem.mNavigation->Init( mBotSystem ) )
	{
		// Allow the game to set up its navigation flags.
		mBotSystem.mNavigation->RegisterScriptFunctions( mBotSystem.mScript->GetMachine() );
	}
	else
	{
		return BOT_ERROR_CANTINITBOT;
	}

	// Create the goal manager.
	mBotSystem.mGoalManager = mBotSystem.mGame->AllocGoalManager();
	mBotSystem.mGoalManager->Init( mBotSystem );

	mBotSystem.mTriggerManager = TriggerManager::GetInstance();

	// Initialize the game.
	if ( mBotSystem.mGame->Init( mBotSystem ) )
	{
		LOG( "Created Game Interface : " << mBotSystem.mGame->GetGameName() );
		LOG( "Game Interface : " << g_EngineFuncs->GetGameName() );
		LOG( "Mod Interface : " << g_EngineFuncs->GetModName() );
	}
	else
	{
		LOGERR( "Unable to CreateGame() : " << mBotSystem.mGame->GetGameName() );
		return BOT_ERROR_CANTINITBOT;
	}

	FileDownloader::Init();

	GameAnalyticsKeys analyticsKeys;
	//if ( mBotSystem.mGame->GetAnalyticsKeys( analyticsKeys ) )
	{
#ifdef _DEBUG
		analyticsKeys.mVersionKey += ":DBG";
#endif
		mBotSystem.mAnalytics = new GameAnalyticsLogger( analyticsKeys );
		mBotSystem.mAnalytics->InitNetwork();

		bool dbEnabled = false, remoteEnabled = false;;
		if ( Options::GetValue( "Analytics", "SendToDatabase", dbEnabled ) )
			mBotSystem.mAnalytics->CreateDatabase( "analytics.db" );
		if ( Options::GetValue( "Analytics", "SendToRemote", remoteEnabled ) )
		{
			std::string serverIp;
			int serverPort = 0;
			if ( remoteEnabled &&
				Options::GetValue( "Analytics", "ServerIp", serverIp ) &&
				Options::GetValue( "Analytics", "ServerPort", serverPort ) )
			{
				mBotSystem.mAnalytics->StartHost( serverIp.c_str(), (unsigned short)serverPort );
			}
		}
	}

	// Load the waypoints for this map.
	if ( mBotSystem.mNavigation->Load() )
	{
		EngineFuncs::ConsoleMessage( "Loaded Waypoints." );
	}
	else
		EngineFuncs::ConsoleError( "ERROR Loading Navigation for map." );

	mBotSystem.mGame->LoadGoalScripts( true );

	EngineFuncs::ConsoleMessage( va( "Bot Initialized in %.2f seconds.", loadTime.GetElapsedSeconds() ) );
	LOG( "Bot Initialized in " << loadTime.GetElapsedSeconds() << " seconds." );

	return BOT_ERROR_NONE;
}

void IGameManager::UpdateGame()
{
	{
		// if any threads want to run outside of the update scope of the 
		// bot, they can attempt to grab this guard
		//boost::lock_guard<boost::recursive_mutex> lock( gGlobalUpdate );

		Prof( Omnibot );

		RenderBuffer::BeginFrame();

		mBotSystem.mGame->UpdateTime();
		mBotSystem.mScript->Update();
		mBotSystem.mNavigation->Update( mBotSystem );
		mBotSystem.mGame->UpdateGame( mBotSystem );
		mBotSystem.mGoalManager->Update( mBotSystem );
		mBotSystem.mTriggerManager->Update( mBotSystem );

#ifdef ENABLE_REMOTE_DEBUGGING
		{
			Prof(RemoteSync);
			m_Remote.updateConnections( this );
			for( int i = 0; i < m_Remote.getNumConnections(); ++i ) {
				RemoteLib::DebugConnection * conn = static_cast<RemoteLib::DebugConnection*>( m_Remote.getConnection( i ) );
				if ( conn->isConnected() ) {
					/*RemoteLib::DataBuffer & sendBuffer = conn->getSendBuffer();
					// keep alive
					if ( IGame::GetTime() > conn->getUserData() ) {
					sendBuffer.beginWrite( RemoteLib::DataBuffer::WriteModeAllOrNone );
					sendBuffer.startSizeHeader();
					sendBuffer.writeInt32( RemoteLib::ID_ack );
					sendBuffer.endSizeHeader();
					sendBuffer.endWrite();
					conn->setUserData( IGame::GetTime() + 5000 );
					}*/

					conn->updateState.Clear();

					mBotSystem.mGame->UpdateSync( conn, conn->cachedState, conn->updateState );
					//m_PathPlanner->Sync( conn );
					m_GoalManager->Sync( conn );
				}
			}
		}
#endif
	}

	if ( mBotSystem.mAnalytics != NULL )
	{
		if ( mBotSystem.mAnalytics->IsNetworkActive() /*&& mBotSystem.mAnalytics->NumClientsConnected() > 0*/ )
		{
			std::vector<Analytics::MessageUnion*> recievedMessages;
			mBotSystem.mAnalytics->ServiceNetwork( recievedMessages );

			for ( size_t i = 0; i < recievedMessages.size(); ++i )
			{
				EngineFuncs::ConsoleMessage( va( "Analytics Message %s", typeid( *recievedMessages[ i ] ).name() ) );

				delete recievedMessages[ i ];
				recievedMessages[ i ] = NULL;
			}
			
			Analytics::MessageUnion msgUnion;
			msgUnion.set_timestamp( 0 );

			IGame::EntityIterator it;
			while ( IGame::IterateEntity( it ) )
			{
				Vector3f entPos( Vector3f::ZERO ), entFace( Vector3f::ZERO );
				EngineFuncs::EntityPosition( it.GetEnt().m_Entity, entPos );
				EngineFuncs::EntityOrientation( it.GetEnt().m_Entity, entFace, NULL, NULL );
				
				float hdg, pitch, roll;
				entFace.ToSpherical( hdg, pitch, roll );

				Analytics::GameEntityList * lst = msgUnion.mutable_gameentitylist();
				Analytics::GameEntityInfo * ent = lst->add_entities();
				ent->set_entityid( it.GetEnt().m_Entity.AsInt() );
				ent->set_classid( InterfaceFuncs::GetEntityClass( it.GetEnt().m_Entity ) );
				ent->mutable_position()->set_x( entPos.X() );
				ent->mutable_position()->set_y( entPos.Y() );
				ent->mutable_position()->set_z( entPos.Z() );
				ent->mutable_orient()->set_heading( hdg );
				ent->mutable_orient()->set_pitch( pitch );
				ent->mutable_orient()->set_roll( roll );				
			}

			if ( msgUnion.IsInitialized() )
				mBotSystem.mAnalytics->AddEvent( msgUnion );
		}
	}

	Options::SaveConfigFileIfChanged( "user/omni-bot.cfg" );

	FileDownloader::Poll();

	if ( mBotSystem.mGame->RendersToGame() )
		RenderBuffer::RenderToGame();

	EngineFuncs::FlushAsyncMessages();
}

void IGameManager::Shutdown()
{
	FileDownloader::Shutdown();

#ifdef ENABLE_REMOTE_DEBUGGING
	m_Remote.shutdown();
#endif

	if ( mBotSystem.mAnalytics != NULL )
	{
		Timer submitTimer;
		const size_t eventsD = mBotSystem.mAnalytics->SubmitDesignEvents();
		const size_t eventsQ = mBotSystem.mAnalytics->SubmitQualityEvents();
		LOG( "Submitting Game Analytics took : " << submitTimer.GetElapsedSeconds() << " seconds." );
		LOG( "Game Analytics Design Events Submitted: " << eventsD );
		LOG( "Game Analytics Quality Events Submitted: " << eventsQ );
		OB_DELETE( mBotSystem.mAnalytics );
	}

	mBotSystem.mGame->Shutdown();

	g_Blackboard.RemoveAllBBRecords();

	LOGFUNCBLOCK;

	// Get rid of the path planner.
	mBotSystem.mNavigation->Shutdown();
	mBotSystem.mNavigation = NULL;

	// Shutdown and clean up the goal manager.
	mBotSystem.mGoalManager->Shutdown();
	mBotSystem.mGoalManager = NULL;
	GoalManager::DeleteInstance();

	mBotSystem.mTriggerManager = NULL;
	TriggerManager::DeleteInstance();

	mBotSystem.mNameManager = NULL;
	NameManager::DeleteInstance();

	// Shutdown and clean up the game.
	OB_DELETE( mBotSystem.mGame );
	LOG( "Successfully Shut down Game Interface" );

	g_WeaponDatabase.Unload();

	mBotSystem.mScript->Shutdown();
	mBotSystem.mScript = NULL;
	ScriptManager::DeleteInstance();

	MiniDumper::Shutdown();

	Options::Shutdown();

	FileSystem::ShutdownFileSystem();
}

void IGameManager::InitCommands()
{
	SetEx( "version", "Prints out the bot version number.",
		this, &IGameManager::cmdVersion );
	SetEx( "navsystem", "Creates a navigation system of a specified type.",
		this, &IGameManager::cmdNavSystem );
	SetEx( "printfs", "Prints the whole file system.",
		this, &IGameManager::cmdPrintAllFiles );

	SetEx( "update_nav", "Checks the remote waypoint database for updated navigation.",
		this, &IGameManager::cmdUpdateNavFile );
	SetEx( "update_all_nav", "Attempts to download all nav files from the database, including updating existing files.",
		this, &IGameManager::cmdUpdateAllNavFiles );

	SetEx( "heatmap_save", "Saves a heat map script used to generate a heat map image.",
		this, &IGameManager::cmdSaveHeatMapScript );
	SetEx( "heatmap_saveall", "Saves a heat map script for all events used to generate a heat map image.",
		this, &IGameManager::cmdSaveAllHeatMapScript );
}

void IGameManager::cmdVersion( const StringVector &_args )
{
	if ( mBotSystem.mGame )
	{
#ifdef _DEBUG
		EngineFuncs::ConsoleMessage( va( "Omni-Bot DEBUG Build : %s %s", __DATE__, __TIME__ ) );
#else
		EngineFuncs::ConsoleMessage(va("Omni-Bot : %s %s", __DATE__, __TIME__));
#endif
		EngineFuncs::ConsoleMessage( va( "Version : %s", mBotSystem.mGame->GetVersion() ) );
		EngineFuncs::ConsoleMessage( va( "Interface # : %d", mBotSystem.mGame->GetVersionNum() ) );
	}
}

void IGameManager::cmdNavSystem( const StringVector &_args )
{
	const char *strUsage [] =
	{
		"navsystem type[wp, recast, flood]",
		"> type: the type of navigation system to use",
	};

	CHECK_NUM_PARAMS( _args, 2, strUsage );

	NavigatorID navId = NAVID_RECAST;
	/*if(_args[1] == "waypoint")
		navId = NAVID_WAYPOINT;
		else*/ if ( _args[ 1 ] == "navmesh" )
			navId = NAVID_NAVMESH;
		else if ( _args[ 1 ] == "flood" )
			navId = NAVID_FLOODFILL;
		else if ( _args[ 1 ] == "recast" )
			navId = NAVID_RECAST;
		else
		{
			PRINT_USAGE( strUsage );
			return;
		}

		if ( !mBotSystem.mNavigation || mBotSystem.mNavigation->GetPlannerType() != navId )
		{
			mBotSystem.mNavigation->Shutdown();
			OB_DELETE( mBotSystem.mNavigation );

			switch ( navId )
			{
				/*case NAVID_WAYPOINT:
				mBotSystem.mNavigation = new PathPlannerWaypoint;
				break;*/
				case NAVID_NAVMESH:
					mBotSystem.mNavigation = new PathPlannerNavMesh;
					break;
				case NAVID_FLOODFILL:
					mBotSystem.mNavigation = new PathPlannerFloodFill;
					break;
				case NAVID_RECAST:
					mBotSystem.mNavigation = new PathPlannerRecast;
					break;
				default:
					break;
			};

			if ( mBotSystem.mNavigation->Init( mBotSystem ) )
			{
				// Allow the game to set up its navigation flags.
				mBotSystem.mNavigation->RegisterScriptFunctions( mBotSystem.mScript->GetMachine() );
				mBotSystem.mNavigation->Load();
			}

			EngineFuncs::ConsoleMessage( "Navigation System created." );
		}
}

void IGameManager::cmdPrintAllFiles( const StringVector &_args )
{
	FileSystem::EnumerateFiles( "" );
}

void IGameManager::cmdUpdateNavFile( const StringVector &_args )
{
#if ( ENABLE_FILE_DOWNLOADER )
	const char *strUsage [] =
	{
		"update_nav mapname",
		"> mapname: the name of the map to update",
	};

	CHECK_NUM_PARAMS( _args, 1, strUsage );
	OPTIONAL_STRING_PARAM( mapname, 1, g_EngineFuncs->GetMapName() );

	FileDownloader::UpdateWaypoints( mapname );
#else
	EngineFuncs::ConsoleError( "File Downloader Disabled" );
#endif
}

void IGameManager::cmdUpdateAllNavFiles( const StringVector &_args )
{
#if ( ENABLE_FILE_DOWNLOADER )
	OPTIONAL_BOOL_PARAM( newstufftoo, 1, false );
	FileDownloader::UpdateAllWaypoints( newstufftoo );
#else
	EngineFuncs::ConsoleError( "File Downloader Disabled" );
#endif
}

void IGameManager::cmdSaveAllHeatMapScript( const StringVector &_args )
{
	if ( System::mInstance->mAnalytics )
	{
		const char *strUsage [] =
		{
			"heatmap_saveall eventradius",
			"> eventradius: The radius of the event to dump out a script for",
			"> imageSize[optional]: The largest size dimension of image",
		};

		CHECK_NUM_PARAMS( _args, 2, strUsage );
		CHECK_FLOAT_PARAM( eventRadius, 1, strUsage );
		OPTIONAL_INT_PARAM( imageSize, 2, 5000 );

		std::vector< std::string > eventNames;
		System::mInstance->mAnalytics->GetUniqueEventNames( eventNames );

		for ( size_t i = 0; i < eventNames.size(); ++i )
		{
			StringVector cmd;
			cmd.push_back( "heatmap_save" );
			cmd.push_back( eventNames[ i ] );
			cmd.push_back( va( "%g", eventRadius ).c_str() );
			cmd.push_back( va( "%d", imageSize ).c_str() );
			CommandReciever::DispatchCommand( cmd );
		}
	}
}

void IGameManager::cmdSaveHeatMapScript( const StringVector &_args )
{
	if ( System::mInstance->mAnalytics )
	{
		const char *strUsage [] =
		{
			"heatmap_save eventname eventradius",
			"> eventname: The name of the event to dump out a script for",
			"> eventradius: The radius of the event to dump out a script for",
			"> imageSize[optional]: The largest size dimension of image",
		};

		CHECK_NUM_PARAMS( _args, 3, strUsage );
		CHECK_STRING_PARAM( eventName, 1, strUsage );
		CHECK_FLOAT_PARAM( eventRadius, 2, strUsage );
		OPTIONAL_INT_PARAM( imageSize, 3, 5000 );

		const AxisAlignedBox3f & bnds = System::mInstance->mNavigation->GetNavigationBounds();

		GameAnalyticsLogger::HeatmapDef def;
		def.mAreaId = g_EngineFuncs->GetMapName();
		def.mEventId = eventName.c_str();
		def.mEventRadius = eventRadius;
		def.mImageSize = imageSize;
		def.mWorldMins[ 0 ] = (float)bnds.Min.X();
		def.mWorldMins[ 1 ] = (float)bnds.Min.Y();
		def.mWorldMaxs[ 0 ] = (float)bnds.Max.X();
		def.mWorldMaxs[ 1 ] = (float)bnds.Max.Y();

		std::string script;
		System::mInstance->mAnalytics->WriteHeatmapScript( def, script );

		const std::string filename = va( "heatmap_%s_%s.mgk", def.mAreaId, def.mEventId );

		File fileOut;
		if ( fileOut.OpenForWrite( filename.c_str(), File::Text, false ) )
		{
			EngineFuncs::ConsoleMessage( va( "Writing %s", filename.c_str() ) );

			fileOut.WriteString( script );
			fileOut.Close();
		}
	}
}

#ifdef ENABLE_REMOTE_DEBUGGING
void IGameManager::SyncRemoteDelete( int entityHandle )
{
	/*if ( m_Remote.getNumConnections() > 0 ) {
	RemoteLib::DataBufferStatic<128> db;

	db.beginWrite( RemoteLib::DataBuffer::WriteModeAllOrNone );
	db.startSizeHeader();
	db.writeInt32( RemoteLib::ID_delete );
	db.writeInt32( entityHandle );
	db.endSizeHeader();
	db.endWrite();
	m_Remote.sendToAll( db );
	}*/
}

void IGameManager::SyncRemoteMessage( const RemoteLib::DataBuffer & db ) {
	m_Remote.sendToAll( db );
}

RemoteLib::Connection * IGameManager::CreateNewConnection( RemoteLib::TcpSocket & socket ) {
	return new RemoteLib::DebugConnection( socket );
}

#endif