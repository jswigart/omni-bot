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

#include "TaskManager.h"
#include "GoalManager.h"
#include "TriggerManager.h"
#include "ScriptManager.h"
#include "Timer.h"

#include "RenderBuffer.h"

#include "WeaponDatabase.h"
#include "FileSystem.h"
#include "FileDownloader.h"

#include "PathPlannerWaypoint.h"
#include "PathPlannerFloodFill.h"
#include "PathPlannerNavmesh.h"
#include "PathPlannerRecast.h"

IEngineInterface *g_EngineFuncs = 0;

//////////////////////////////////////////////////////////////////////////

void ProtobufLogHandler(google::protobuf::LogLevel level, const char* filename, int line, const std::string& message)
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

IGameManager::IGameManager()
#ifdef ENABLE_REMOTE_DEBUGGING
	, m_Remote( 0 )
#endif
{
	memset(&g_EngineFuncs, 0, sizeof(g_EngineFuncs));
}

omnibot_error IGameManager::CreateGame(IEngineInterface *_pEngineFuncs, int _version)
{
	google::protobuf::SetLogHandler( ProtobufLogHandler );

	MiniDumper::Init("Omni-bot");

	Timer loadTime;

	LOGFUNCBLOCK;

	InitCommands();

	srand((unsigned int)time(NULL));

	g_EngineFuncs = _pEngineFuncs;

	mBotSystem.mGame = CreateGameInstance();

	// Verify the version is correct.
	if(!mBotSystem.mGame->CheckVersion(_version))
	{
		OB_DELETE(mBotSystem.mGame);
		return BOT_ERROR_WRONGVERSION;
	}

	if(!FileSystem::InitFileSystem())
		return BOT_ERROR_FILESYSTEM;

	FileSystem::SetWriteDirectory(FileSystem::GetModFolder());

	FileSystem::MakeDirectory("user");
	FileSystem::MakeDirectory("nav");
	FileSystem::MakeDirectory("scripts");
	FileSystem::MakeDirectory("gui");
	//FileSystem::MakeDirectory("gui/weapons");

	FileSystem::MountArchives("scripts");
	FileSystem::MountArchives("scripts/weapons");
	FileSystem::MountArchives("scripts/goals");
	FileSystem::MountArchives("user/download","nav");
	FileSystem::MountArchives("nav");
	FileSystem::MountArchives("global_scripts");
	FileSystem::MountArchives("user");

	if(FileSystem::FileDelete("user/logged.gm"))
		EngineFuncs::ConsoleMessage("deleted user/logged.gm");

	if(!Options::LoadConfigFile("user/omni-bot.cfg"))
	{
		if(!Options::LoadConfigFile("config/omni-bot.cfg"))
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
	Options::SetValue("Debug","DumpFileEnable",true,false);
	Options::SetValue("Debug","DumpFileDialog",true,false);
	Options::SetValue("Script","LiveUpdate",true,false);
	Options::SetValue("Script","Debug","true",false);
	Options::SetValue("Script","EnableRemoteDebugger","true",false);
	Options::SetValue("Debug Render","EnableInterProcess","true",false);
#ifdef ENABLE_FILE_DOWNLOADER
	Options::SetValue("Downloader","Server","",false);
	Options::SetValue("Downloader","Script","",false);
	Options::SetValue("Downloader","DownloadMissingNav","true",false);
#endif

	Options::SetValue("Log","LogInfo","true",false);
	Options::SetValue("Log","LogWarnings","true",false);
	Options::SetValue("Log","LogErrors","true",false);
	Options::SetValue("Log","LogCriticalErrors","true",false);

#ifdef ENABLE_REMOTE_DEBUGGING
	Options::SetValue("RemoteWindow","Enabled",0,false);
	Options::SetValue("RemoteWindow","Port",m_Remote.getPort(),false);
#endif

	//////////////////////////////////////////////////////////////////////////
	// logging options
	g_Logger.LogMask() = 0;

	bool l = true;
	if(Options::GetValue("Log","LogInfo",l) && l)
		g_Logger.LogMask() |= Logger::LOG_INFO;
	if(Options::GetValue("Log","LogWarnings",l) && l)
		g_Logger.LogMask() |= Logger::LOG_WARN;
	if(Options::GetValue("Log","LogErrors",l) && l)
		g_Logger.LogMask() |= Logger::LOG_ERR;
	if(Options::GetValue("Log","LogCriticalErrors",l) && l)
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
	switch( mBotSystem.mGame->GetDefaultNavigator() )
	{
	case NAVID_WP:
		mBotSystem.mNavigation = new PathPlannerWaypoint;
		break;
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
		LOGERR("Unknown Path Planner!");
		return BOT_ERROR_CANTINITBOT;
	};

	OBASSERT ( mBotSystem.mNavigation, "No Path Planner!" );
	if ( mBotSystem.mNavigation->Init( mBotSystem ) )
	{
		// Allow the game to set up its navigation flags.
		mBotSystem.mGame->RegisterNavigationFlags( mBotSystem.mNavigation );
		mBotSystem.mNavigation->RegisterScriptFunctions( mBotSystem.mScript->GetMachine() );
	}
	else
	{
		return BOT_ERROR_CANTINITBOT;
	}

	// Create the goal manager.
	mBotSystem.mGoalManager = mBotSystem.mGame->GetGoalManager();
	mBotSystem.mGoalManager->Init( mBotSystem );

	mBotSystem.mTriggerManager = TriggerManager::GetInstance();

	// Initialize the game.
	if ( mBotSystem.mGame->Init( mBotSystem ) )
	{
		LOG("Created Game Interface : " << mBotSystem.mGame->GetGameName());
		LOG("Game Interface : " << g_EngineFuncs->GetGameName());
		LOG("Mod Interface : " << g_EngineFuncs->GetModName());
	}
	else
	{
		LOGERR("Unable to CreateGame() : " << mBotSystem.mGame->GetGameName());
		return BOT_ERROR_CANTINITBOT;
	}

#ifdef ENABLE_FILE_DOWNLOADER
	FileDownloader::Init();
#endif

	//m_TaskManager = new TaskManager();

	// Load the waypoints for this map.
	if( mBotSystem.mNavigation->Load() )
	{
		EngineFuncs::ConsoleMessage("Loaded Waypoints.");
	}
	else
		EngineFuncs::ConsoleError("ERROR Loading Navigation for map.");

	mBotSystem.mGame->LoadGoalScripts(true);

	EngineFuncs::ConsoleMessage(va("Bot Initialized in %.2f seconds.", loadTime.GetElapsedSeconds()));
	LOG("Bot Initialized in " << loadTime.GetElapsedSeconds() << " seconds.");

	return BOT_ERROR_NONE;
}

void IGameManager::UpdateGame()
{
	{
		Prof(Omnibot);

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

	Options::SaveConfigFileIfChanged("user/omni-bot.cfg");

#ifdef ENABLE_FILE_DOWNLOADER
	FileDownloader::Poll();
#endif

	if ( mBotSystem.mGame->RendersToGame() )
		RenderBuffer::RenderToGame();

	EngineFuncs::FlushAsyncMessages();
}

void IGameManager::Shutdown()
{
	OB_DELETE( mBotSystem.mTaskManager );

#ifdef ENABLE_FILE_DOWNLOADER
	FileDownloader::Shutdown();
#endif

#ifdef ENABLE_REMOTE_DEBUGGING
	m_Remote.shutdown();
#endif

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
	LOG("Successfully Shut down Game Interface");

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
	SetEx("version", "Prints out the bot version number.",
		this, &IGameManager::cmdVersion);
	SetEx("navsystem", "Creates a navigation system of a specified type.",
		this, &IGameManager::cmdNavSystem);
	SetEx("printfs", "Prints the whole file system.",
		this, &IGameManager::cmdPrintAllFiles);

#ifdef ENABLE_FILE_DOWNLOADER
	SetEx("update_nav", "Checks the remote waypoint database for updated navigation.",
		this, &IGameManager::cmdUpdateNavFile);
	SetEx("update_all_nav", "Attempts to download all nav files from the database, including updating existing files.",
		this, &IGameManager::cmdUpdateAllNavFiles);
#endif
}

void IGameManager::cmdVersion(const StringVector &_args)
{
	if ( mBotSystem.mGame )
	{
#ifdef _DEBUG
		EngineFuncs::ConsoleMessage(va("Omni-Bot DEBUG Build : %s %s", __DATE__, __TIME__));
#else
		EngineFuncs::ConsoleMessage(va("Omni-Bot : %s %s", __DATE__, __TIME__));
#endif
		EngineFuncs::ConsoleMessage(va("Version : %s", mBotSystem.mGame->GetVersion()));
		EngineFuncs::ConsoleMessage(va("Interface # : %d", mBotSystem.mGame->GetVersionNum()));
	}
}

void IGameManager::cmdNavSystem(const StringVector &_args)
{
	const char *strUsage[] =
	{
		"navsystem type[wp, navmesh, flood]",
		"> type: the type of navigation system to use",
	};

	CHECK_NUM_PARAMS(_args, 2, strUsage);

	NavigatorID navId = NAVID_WP;
	if(_args[1] == "wp")
		navId = NAVID_WP;
	else if(_args[1] == "navmesh")
		navId = NAVID_NAVMESH;
	else if(_args[1] == "flood")
		navId = NAVID_FLOODFILL;
	else if(_args[1] == "recast")
		navId = NAVID_RECAST;
	else
	{
		PRINT_USAGE(strUsage);
		return;
	}

	if( !mBotSystem.mNavigation || mBotSystem.mNavigation->GetPlannerType() != navId )
	{
		mBotSystem.mNavigation->Shutdown();
		OB_DELETE( mBotSystem.mNavigation );

		switch( navId )
		{
		case NAVID_WP:
			mBotSystem.mNavigation = new PathPlannerWaypoint;
			break;
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

		mBotSystem.mNavigation->RegisterScriptFunctions( mBotSystem.mScript->GetMachine() );
		EngineFuncs::ConsoleMessage("Navigation System created.");
	}
}

void IGameManager::cmdPrintAllFiles(const StringVector &_args)
{
	FileSystem::EnumerateFiles("");
}

#ifdef ENABLE_FILE_DOWNLOADER
void IGameManager::cmdUpdateNavFile(const StringVector &_args)
{
	const char *strUsage[] =
	{
		"update_nav mapname",
		"> mapname: the name of the map to update",
	};

	CHECK_NUM_PARAMS(_args, 1, strUsage);
	OPTIONAL_STRING_PARAM(mapname, 1, g_EngineFuncs->GetMapName());

	FileDownloader::UpdateWaypoints(mapname);
}

void IGameManager::cmdUpdateAllNavFiles(const StringVector &_args)
{
	OPTIONAL_BOOL_PARAM(newstufftoo, 1, false);
	FileDownloader::UpdateAllWaypoints(newstufftoo);
}
#endif

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