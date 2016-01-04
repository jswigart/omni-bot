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
#include "Remotery.h"
#include "GoalManager.h"
#include "TriggerManager.h"
#include "ScriptManager.h"
#include "Timer.h"
#include "RenderBuffer.h"
#include "WeaponDatabase.h"
#include "FileSystem.h"

#include "PathPlannerFloodFill.h"
#include "PathPlannerRecast.h"

IEngineInterface *gEngineFuncs = 0;

boost::recursive_mutex gGlobalUpdate;

//////////////////////////////////////////////////////////////////////////

void ProtobufLogHandler( google::protobuf::LogLevel level, const char* filename, int line, const std::string& message )
{
	switch ( level )
	{
		case google::protobuf::LOGLEVEL_INFO:
			EngineFuncs::ConsoleMessage( va( "%s( %d ) : %s", filename, line, message.c_str() ).c_str() );
			break;
		case google::protobuf::LOGLEVEL_WARNING:
		case google::protobuf::LOGLEVEL_ERROR:
		case google::protobuf::LOGLEVEL_FATAL:
			EngineFuncs::ConsoleError( va( "%s( %d ) : %s", filename, line, message.c_str() ).c_str() );
			break;
	}
}

//////////////////////////////////////////////////////////////////////////

static void RemoteyInputHandler( const char* text, void* context )
{
	StringVector tokens;
	Utils::Tokenize( text, " ", tokens );
	if ( tokens.size() > 0 )
	{
		if ( tokens.front() == "bot" || tokens.front() == "ombot" )
			tokens.erase( tokens.begin() );
	}
	CommandReciever::DispatchCommand( tokens );
}

//////////////////////////////////////////////////////////////////////////

IGame *CreateGameInstance();

boost::thread::id IGameManager::sMainThread;

IGameManager::IGameManager()
	: mRemotery( NULL )
{
	rmtSettings* settings = rmt_Settings();
	if ( settings != NULL )
	{
		settings->input_handler = RemoteyInputHandler;
	}
	rmt_CreateGlobalInstance( &mRemotery );
	rmt_SetCurrentThreadName( "Main" );
}

IGameManager::~IGameManager()
{
	rmt_DestroyGlobalInstance( mRemotery );
}

omnibot_error IGameManager::CreateGame( IEngineInterface *engineFuncs, int version )
{
	rmt_ScopedCPUSample( CreateGame );

	sMainThread = boost::this_thread::get_id();

	google::protobuf::SetLogHandler( ProtobufLogHandler );

	MiniDumper::Init( "Omni-bot" );

	Timer loadTime;

	LOGFUNCBLOCK;

	InitCommands();

	srand( (unsigned int)time( NULL ) );

	gEngineFuncs = engineFuncs;

	IGame::InitTime();

	mBotSystem.mGame = CreateGameInstance();

	// Verify the version is correct.
	if ( !mBotSystem.mGame->CheckVersion( version ) )
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
			.mRemote.setMaxConnections( (unsigned short)numConnections );
		}
		int remotePort = .mRemote.getPort();
		if(Options::GetValue("RemoteWindow","Port",remotePort)) {
			.mRemote.setPort( (uint16)remotePort );
		}

		.mRemote.init( 8192, 4096, true );
	}
#endif

	RenderBuffer::Init();

	mBotSystem.mNavigation = new PathPlannerRecast;
	if ( mBotSystem.mNavigation->Init( mBotSystem ) )
	{
		// Allow the game to set up its navigation flags.
		mBotSystem.mNavigation->RegisterScriptFunctions( mBotSystem.mScript->GetMachine() );
	}
	else
	{
		return BOT_ERROR_CANTINITBOT;
	}

	mBotSystem.mGoalManager = mBotSystem.mGame->AllocGoalManager();
	mBotSystem.mGoalManager->Init( mBotSystem );

	mBotSystem.mTacticalManager = new TacticalManager();
	mBotSystem.mTacticalManager->Init( mBotSystem );

	mBotSystem.mTriggerManager = TriggerManager::GetInstance();

	const GameVars& vars = mBotSystem.mGame->GetGameVars();

	// Initialize the game.
	{
		rmt_ScopedCPUSample( GameInit );
		if ( mBotSystem.mGame->Init( mBotSystem ) )
		{
			LOG( "Created Game Interface : " << vars.mGameName.c_str() );
			LOG( "Game Interface : " << gEngineFuncs->GetGameName() );
			LOG( "Mod Interface : " << gEngineFuncs->GetModName() );
		}
		else
		{
			LOGERR( "Unable to CreateGame() : " << mBotSystem.mGame->GetGameVars().mGameName );
			return BOT_ERROR_CANTINITBOT;
		}
	}

	// Run the games autoexec.
	int threadId = GM_INVALID_THREAD;
	if ( !mBotSystem.mScript->ExecuteFile( filePath( "scripts/%s_autoexec.gm", vars.mGameAbbrev.c_str() ), threadId ) )
		EngineFuncs::ConsoleError( va( "Unable to execute 'scripts/%s_autoexec.gm'", vars.mGameAbbrev.c_str() ).c_str() );

	GameAnalytics::Keys analyticsKeys;
	//if ( mBotSystem.mGame->GetAnalyticsKeys( analyticsKeys ) )
	{
#ifdef _DEBUG
		analyticsKeys.mVersionKey += ":DBG";
#endif
		mBotSystem.mAnalytics = new GameAnalytics( analyticsKeys, this );

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
				mBotSystem.mAnalytics->SetPublisher( new zmqPublisher( serverIp.c_str(), (unsigned short)serverPort ) );
			}
		}
	}

	if ( mBotSystem.mAnalytics != NULL )
	{
		Analytics::MessageUnion msg;
		msg.set_timestamp( IGame::GetTime() );
		msg.mutable_gameinfo()->set_mapname( gEngineFuncs->GetMapName() );
		mBotSystem.mAnalytics->AddEvent( msg );
	}

	// Load the waypoints for this map.
	if ( mBotSystem.mNavigation->Load() )
	{
		EngineFuncs::ConsoleMessage( "Loaded Waypoints." );
	}
	else
		EngineFuncs::ConsoleError( "ERROR Loading Navigation for map." );

	mBotSystem.mGame->LoadGoalScripts( true );

	EngineFuncs::ConsoleMessage( va( "Bot Initialized in %.2f seconds.", loadTime.GetElapsedSeconds() ).c_str() );
	LOG( "Bot Initialized in " << loadTime.GetElapsedSeconds() << " seconds." );

	return BOT_ERROR_NONE;
}

void IGameManager::UpdateGame()
{
	RenderBuffer::StartFrame();

	{
		// if any threads want to run outside of the update scope of the 
		// bot, they can attempt to grab this guard
		//boost::lock_guard<boost::recursive_mutex> lock( gGlobalUpdate );

		mBotSystem.mGame->UpdateTime();
		mBotSystem.mScript->Update();
		mBotSystem.mNavigation->Update( mBotSystem );
		mBotSystem.mGame->UpdateGame( mBotSystem );
		mBotSystem.mGoalManager->Update( mBotSystem );
		mBotSystem.mTacticalManager->Update( mBotSystem );
		mBotSystem.mTriggerManager->Update( mBotSystem );
	}

	if ( mBotSystem.mAnalytics != NULL )
	{
		rmt_ScopedCPUSample( GameAnalytics );

		if ( mBotSystem.mAnalytics->GetPublisher() )
		{
			rmt_ScopedCPUSample( Poll );

			Analytics::MessageUnion msg;
			while ( mBotSystem.mAnalytics->Poll( msg ) )
			{

			}
		}

		rmt_ScopedCPUSample( CreateFrameEvent );

		static int nextSendTime = 0;
		if ( nextSendTime < IGame::GetTime() )
		{
			static int delay = 5000;
			nextSendTime = IGame::GetTime() + delay;

			Analytics::MessageUnion msgUnion;
			msgUnion.set_timestamp( IGame::GetTime() );

			IGame::EntityIterator it;
			while ( IGame::IterateEntity( it ) )
			{
				Vector3f entPos( Vector3f::ZERO ), entFace( Vector3f::ZERO );
				EngineFuncs::EntityPosition( it.GetEnt().mEntity, entPos );
				EngineFuncs::EntityOrientation( it.GetEnt().mEntity, entFace, NULL, NULL );

				float hdg, pitch, roll;
				entFace.ToSpherical( hdg, pitch, roll );

				Analytics::GameEntityList * lst = msgUnion.mutable_gameentitylist();
				Analytics::GameEntityInfo * ent = lst->add_entities();
				ent->set_name( EngineFuncs::EntityName( it.GetEnt().mEntity ) );

				SMART_FIELD_SET( ent, entityid, it.GetEnt().mEntity.AsInt() );
				SMART_FIELD_SET( ent, groupid, it.GetEnt().mEntInfo.mGroup );
				SMART_FIELD_SET( ent, classid, it.GetEnt().mEntInfo.mClassId );
				SMART_FIELD_SET( ent, health, it.GetEnt().mEntInfo.mHealth.mNum );
				SMART_FIELD_SET( ent, healthmax, it.GetEnt().mEntInfo.mHealth.mMax );
				SMART_FIELD_SET( ent, armor, it.GetEnt().mEntInfo.mArmor.mNum );
				SMART_FIELD_SET( ent, armormax, it.GetEnt().mEntInfo.mArmor.mMax );
				/*for ( int i = 0; i < EntityInfo::NUM_AMMO_TYPES; ++i )
				{
				if ( it.GetEnt().mEntInfo.mAmmo[ i ].mWeaponId != 0 )
				{
				Analytics::GameEntityInfo_Ammo* ammo = ent->add_ammo();
				ammo->set_ammotype( it.GetEnt().mEntInfo.mAmmo[ i ].mWeaponId );
				SMART_FIELD_SET( ammo, ammocount, it.GetEnt().mEntInfo.mAmmo[ i ].mNum );
				}
				}*/
				SMART_FIELD_SET( ent, positionx, entPos.X() );
				SMART_FIELD_SET( ent, positiony, entPos.Y() );
				SMART_FIELD_SET( ent, positionz, entPos.Z() );
				SMART_FIELD_SET( ent, heading, hdg );
				SMART_FIELD_SET( ent, pitch, pitch );
				SMART_FIELD_SET( ent, roll, roll );

				/*if ( lst->entities_size() > 10 )
					break;*/
			}

			if ( msgUnion.IsInitialized() )
				mBotSystem.mAnalytics->AddEvent( msgUnion );
		}
	}

	Options::SaveConfigFileIfChanged( "user/omni-bot.cfg" );

	if ( mBotSystem.mGame->GetGameVars().mRendersToGame )
	{
		rmt_ScopedCPUSample( RenderToGame );
		RenderBuffer::RenderToGame();
	}
	
	EngineFuncs::FlushAsyncMessages();
}

void IGameManager::Shutdown()
{
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

	gBlackboard.RemoveAllBBRecords();

	LOGFUNCBLOCK;

	mBotSystem.mNavigation->Shutdown();
	mBotSystem.mNavigation = NULL;

	mBotSystem.mGoalManager->Shutdown();
	OB_DELETE( mBotSystem.mGoalManager );

	mBotSystem.mTacticalManager->Shutdown();
	OB_DELETE( mBotSystem.mTacticalManager );

	mBotSystem.mTriggerManager = NULL;
	TriggerManager::DeleteInstance();

	mBotSystem.mNameManager = NULL;
	NameManager::DeleteInstance();

	// Shutdown and clean up the game.
	OB_DELETE( mBotSystem.mGame );
	LOG( "Successfully Shut down Game Interface" );

	gWeaponDatabase.Unload();

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

#if ( ENABLE_FILE_DOWNLOADER )
	SetEx( "update_nav", "Checks the remote waypoint database for updated navigation.",
		this, &IGameManager::cmdUpdateNavFile );
	SetEx( "update_all_nav", "Attempts to download all nav files from the database, including updating existing files.",
		this, &IGameManager::cmdUpdateAllNavFiles );
#endif

	SetEx( "heatmap_save", "Saves a heat map script used to generate a heat map image.",
		this, &IGameManager::cmdSaveHeatMapScript );
	SetEx( "heatmap_saveall", "Saves a heat map script for all events used to generate a heat map image.",
		this, &IGameManager::cmdSaveAllHeatMapScript );
}

void IGameManager::cmdVersion( const StringVector & args )
{
	if ( mBotSystem.mGame )
	{
#ifdef _DEBUG
		EngineFuncs::ConsoleMessage( va( "Omni-Bot DEBUG Build : %s %s", __DATE__, __TIME__ ).c_str() );
#else
		EngineFuncs::ConsoleMessage( va( "Omni-Bot : %s %s", __DATE__, __TIME__ ).c_str() );
#endif
		EngineFuncs::ConsoleMessage( va( "Version : %s", mBotSystem.mGame->GetGameVars().mVersionString.c_str() ).c_str() );
		EngineFuncs::ConsoleMessage( va( "Interface # : %d", mBotSystem.mGame->GetGameVars().mGameVersion ).c_str() );
	}
}

void IGameManager::cmdNavSystem( const StringVector & args )
{
	const char *strUsage [] =
	{
		"navsystem type[wp, recast, flood]",
		"> type: the type of navigation system to use",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );

	NavigatorID navId = NAVID_RECAST;
	/*if(args[1] == "waypoint")
		navId = NAVID_WAYPOINT;
		else*/
	if ( args[ 1 ] == "flood" )
		navId = NAVID_FLOODFILL;
	else if ( args[ 1 ] == "recast" )
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
			case NAVID_FLOODFILL:
				//mBotSystem.mNavigation = new PathPlannerFloodFill;
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

void IGameManager::cmdPrintAllFiles( const StringVector & args )
{
	FileSystem::EnumerateFiles( "" );
}

void IGameManager::cmdUpdateNavFile( const StringVector & args )
{
#if ( ENABLE_FILE_DOWNLOADER )
	const char *strUsage [] =
	{
		"update_nav mapname",
		"> mapname: the name of the map to update",
	};

	CHECK_NUM_PARAMS( args, 1, strUsage );
	OPTIONAL_STRING_PARAM( mapname, 1, gEngineFuncs->GetMapName() );

	FileDownloader::UpdateWaypoints( mapname );
#else
	EngineFuncs::ConsoleError( "File Downloader Disabled" );
#endif
}

void IGameManager::cmdUpdateAllNavFiles( const StringVector & args )
{
#if ( ENABLE_FILE_DOWNLOADER )
	OPTIONAL_BOOL_PARAM( newstufftoo, 1, false );
	FileDownloader::UpdateAllWaypoints( newstufftoo );
#else
	EngineFuncs::ConsoleError( "File Downloader Disabled" );
#endif
}

void IGameManager::cmdSaveAllHeatMapScript( const StringVector & args )
{
	if ( System::mInstance->mAnalytics )
	{
		const char *strUsage [] =
		{
			"heatmap_saveall eventradius",
			"> eventradius: The radius of the event to dump out a script for",
			"> imageSize[optional]: The largest size dimension of image",
		};

		CHECK_NUM_PARAMS( args, 2, strUsage );
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

void IGameManager::cmdSaveHeatMapScript( const StringVector & args )
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

		CHECK_NUM_PARAMS( args, 3, strUsage );
		CHECK_STRING_PARAM( eventName, 1, strUsage );
		CHECK_FLOAT_PARAM( eventRadius, 2, strUsage );
		OPTIONAL_INT_PARAM( imageSize, 3, 5000 );

		const AxisAlignedBox3f & bnds = System::mInstance->mNavigation->GetNavigationBounds();

		GameAnalytics::HeatmapDef def;
		def.mAreaId = gEngineFuncs->GetMapName();
		def.mEventId = eventName.c_str();
		def.mEventRadius = eventRadius;
		def.mImageSize = imageSize;
		def.mWorldMins[ 0 ] = (float)bnds.Min.X();
		def.mWorldMins[ 1 ] = (float)bnds.Min.Y();
		def.mWorldMaxs[ 0 ] = (float)bnds.Max.X();
		def.mWorldMaxs[ 1 ] = (float)bnds.Max.Y();

		std::string script;
		System::mInstance->mAnalytics->WriteHeatmapScript( def, script );

		const std::string filename = va( "heatmap_%s_%s.mgk", def.mAreaId, def.mEventId ).c_str();

		File fileOut;
		if ( fileOut.OpenForWrite( filename.c_str(), File::Text, false ) )
		{
			EngineFuncs::ConsoleMessage( va( "Writing %s", filename.c_str() ).c_str() );

			fileOut.WriteString( script );
			fileOut.Close();
		}
	}
}

void IGameManager::Error( const char * str )
{
	EngineFuncs::ConsoleError( str );
}
