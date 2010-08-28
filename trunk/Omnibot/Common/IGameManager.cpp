////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompCommon.h"
#include "IGameManager.h"
#include "IGame.h"
#include "mdump.h"

#include "NavigationManager.h"
#include "GoalManager.h"
#include "TriggerManager.h"
#include "ScriptManager.h"
#include "Timer.h"
#include "Interprocess.h"
#include "DebugWindow.h"

#include "WeaponDatabase.h"
#include "FileSystem.h"
#include "FileDownloader.h"
#include "ChunkedFile.h"

//////////////////////////////////////////////////////////////////////////

IGameManager *IGameManager::m_Instance = 0;

IGame *CreateGameInstance();

IGameManager::IGameManager() 
	: m_ScriptManager(0)
	, m_PathPlanner(0)
	, m_GoalManager(0)
	, m_Game(0)
{
	memset(&g_EngineFuncs, 0, sizeof(g_EngineFuncs));
}

omnibot_error IGameManager::CreateGame(IEngineInterface *_pEngineFuncs, int _version)
{
	MiniDumper::Init("Omni-bot");
	
	Timer loadTime;

	LOGFUNCBLOCK;

	InitCommands();

	srand((unsigned int)time(NULL));

	g_EngineFuncs = _pEngineFuncs;

	// Attempt to create the proper instance of IGame based on the game requested.
	m_Game = CreateGameInstance();
	
	// Verify the version is correct.
	if(!m_Game->CheckVersion(_version))
	{
		OB_DELETE(m_Game);
		return BOT_ERROR_WRONGVERSION;
	}

	if(!FileSystem::InitFileSystem())
		return BOT_ERROR_FILESYSTEM;

	FileSystem::SetWriteDirectory(Utils::GetModFolder());

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
	m_ScriptManager = ScriptManager::GetInstance();
	m_ScriptManager->Init();

	//////////////////////////////////////////////////////////////////////////
	// Set up global config options.
	//Options::SetValue("Settings","TickRate",20,false);
	Options::SetValue("Debug","DumpFileEnable",true,false);
	Options::SetValue("Debug","DumpFileDialog",true,false);
	Options::SetValue("Script","LiveUpdate",true,false);
	Options::SetValue("Script","Debug","true",false);
	Options::SetValue("Script","EnableRemoteDebugger","true",false);
	Options::SetValue("Debug Render","EnableInterProcess","true",false);
	Options::SetValue("Downloader","Server","",false);
	Options::SetValue("Downloader","Script","",false);
	Options::SetValue("Downloader","DownloadMissingNav","true",false);

	Options::SetValue("Log","LogInfo","true",false);
	Options::SetValue("Log","LogWarnings","true",false);
	Options::SetValue("Log","LogErrors","true",false);
	Options::SetValue("Log","LogCriticalErrors","true",false);

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

	bool EnableIpc = false;
	Options::GetValue("Debug Render","EnableInterProcess",EnableIpc);
	InterProcess::Enable(EnableIpc);

	/*ChunkedFile cf;
	if(cf.OpenForWrite("user/chunkedfile.cf"))
	{
		const obuint32 CF_MASTER_HEADER = Utils::MakeId32("NVhd");
		cf.Close();
	}*/

	// Create the requested path planner.
	//if(NavigationManager::GetInstance()->CreatePathPlanner(NAVID_RECAST))
	if(NavigationManager::GetInstance()->CreatePathPlanner(m_Game->GetDefaultNavigator()))
	{
		m_PathPlanner = NavigationManager::GetInstance()->GetCurrentPathPlanner();
		m_PathPlanner->RegisterScriptFunctions(m_ScriptManager->GetMachine());
		LOG("Created Nav System : %s",m_PathPlanner->GetPlannerName());
	}
	else
	{
		LOG_BASIC("Unable to Create Nav System");
		return BOT_ERROR_CANTINITBOT;
	}

	// Create the goal manager.
	m_GoalManager = m_Game->GetGoalManager();
	if(m_GoalManager)
	{
		m_GoalManager->Init();
		LOG_BASIC("Goal Manager Created.");
	}
	else
	{
		LOGERR_BASIC("ERROR: Creating Goal Manager.");
		return BOT_ERROR_CANTINITBOT;
	}

	TriggerManager::GetInstance();

	// Initialize the game.
	if(m_Game && m_Game->Init())
	{
		LOG("Created Game Interface : %s",m_Game->GetGameName());
		LOG("Game Interface : %s",g_EngineFuncs->GetGameName());
		LOG("Mod Interface : %s",g_EngineFuncs->GetModName());
	} 
	else
	{
		LOGERR("Unable to CreateGame() : %s", m_Game->GetGameName());
		return BOT_ERROR_CANTINITBOT;
	}

#ifdef ENABLE_FILE_DOWNLOADER
	FileDownloader::Init();
#endif

	// Load the waypoints for this map.
	if(m_PathPlanner->Load(String(g_EngineFuncs->GetMapName())))
	{
		EngineFuncs::ConsoleMessage("Loaded Waypoints.");
	} 
	else
		EngineFuncs::ConsoleError("ERROR Loading Waypoints.");

	EngineFuncs::ConsoleMessagef("Bot Initialized in %.2f seconds.",loadTime.GetElapsedSeconds());
	LOG("Bot Initialized in %.2f seconds.",loadTime.GetElapsedSeconds());

	m_Game->LoadGoalScripts(true);

	return BOT_ERROR_NONE;
}

void IGameManager::UpdateGame()
{
	InterProcess::Update();

#ifdef ENABLE_DEBUG_WINDOW
	DebugWindow::Update();
#endif

	{
		Prof(Omnibot);

		m_Game->UpdateTime();
		m_ScriptManager->Update();
		m_PathPlanner->Update();
		m_Game->UpdateGame();		
		m_GoalManager->Update();
		TriggerManager::GetInstance()->Update();

		{
			Prof(Processes);

			FunctorMap::iterator it = m_UpdateMap.begin();
			for(; it != m_UpdateMap.end(); )
			{
				if((*(*it).second)() == Function_Finished)
				{
					EngineFuncs::ConsoleMessagef("Finished Process: %s", (*it).first.c_str());
					m_UpdateMap.erase(it++);
				}
				else
					++it;
			}
		}
	}

	Options::SaveConfigFileIfChanged("user/omni-bot.cfg");

#ifdef ENABLE_FILE_DOWNLOADER
	FileDownloader::Poll();
#endif

	EngineFuncs::FlushAsyncMessages();
}

void IGameManager::Shutdown()
{
#ifdef ENABLE_FILE_DOWNLOADER
	FileDownloader::Shutdown();
#endif

#ifdef ENABLE_DEBUG_WINDOW
	DebugWindow::Destroy();
#endif

	m_Game->Shutdown();
	g_Blackboard.RemoveAllBBRecords();

	while(!m_UpdateMap.empty())
		RemoveUpdateFunction((*m_UpdateMap.begin()).first);

	LOGFUNCBLOCK;
    
	// Get rid of the path planner.
	NavigationManager::DeleteInstance();	
	m_PathPlanner = 0;

	// Shutdown and clean up the goal manager.
	m_GoalManager->Shutdown();
	m_GoalManager = 0;
	GoalManager::DeleteInstance();

	TriggerManager::DeleteInstance();
	NameManager::DeleteInstance();

	// Shutdown and clean up the game.
	OB_DELETE(m_Game);
	LOG_BASIC("Successfully Shut down Game Interface");

	g_WeaponDatabase.Unload();

	ScriptManager::GetInstance()->Shutdown();
	ScriptManager::DeleteInstance();

	InterProcess::Shutdown();

	MiniDumper::Shutdown();

	Options::Shutdown();

	FileSystem::ShutdownFileSystem();
}

IGameManager *IGameManager::GetInstance()
{
	if(!m_Instance)
		m_Instance = new IGameManager;
	return m_Instance;
}

void IGameManager::DeleteInstance()
{
	OB_DELETE(m_Instance);
}

void IGameManager::InitCommands()
{
	Set("version", "Prints out the bot version number.", 
		CommandFunctorPtr(new CommandFunctorT<IGameManager>(this, &IGameManager::cmdVersion)));
	Set("stopprocess", "Stops a process by its name.", 
		CommandFunctorPtr(new CommandFunctorT<IGameManager>(this, &IGameManager::cmdStopProcess)));
	Set("showprocesses", "Shows current proccesses.", 
		CommandFunctorPtr(new CommandFunctorT<IGameManager>(this, &IGameManager::cmdShowProcesses)));
	Set("navsystem", "Creates a navigation system of a specified type.", 
		CommandFunctorPtr(new CommandFunctorT<IGameManager>(this, &IGameManager::cmdNavSystem)));
	Set("printfs", "Prints the whole file system.", 
		CommandFunctorPtr(new CommandFunctorT<IGameManager>(this, &IGameManager::cmdPrintAllFiles)));

#ifdef ENABLE_FILE_DOWNLOADER
	Set("update_nav", "Checks the remote waypoint database for updated navigation.", 
		CommandFunctorPtr(new CommandFunctorT<IGameManager>(this, &IGameManager::cmdUpdateNavFile)));
	Set("update_all_nav", "Attempts to download all nav files from the database, including updating existing files.", 
		CommandFunctorPtr(new CommandFunctorT<IGameManager>(this, &IGameManager::cmdUpdateAllNavFiles)));
#endif
}

void IGameManager::cmdVersion(const StringVector &_args)
{
	if(m_Game)
	{
#ifdef _DEBUG
		EngineFuncs::ConsoleMessagef("Omni-Bot DEBUG Build : %s %s", __DATE__, __TIME__);
#else
		EngineFuncs::ConsoleMessagef("Omni-Bot : %s %s", __DATE__, __TIME__);
#endif
		EngineFuncs::ConsoleMessagef("Version : %s", m_Game->GetVersion());
		EngineFuncs::ConsoleMessagef("Interface # : %d", m_Game->GetVersionNum());
	}
}

void IGameManager::cmdShowProcesses(const StringVector &_args)
{
	EngineFuncs::ConsoleMessagef("# Processes: %d!", m_UpdateMap.size());
	FunctorMap::iterator it = m_UpdateMap.begin(), itEnd = m_UpdateMap.end();
	for(; it != itEnd; ++it)
	{
		EngineFuncs::ConsoleMessagef("Process: %s!", (*it).first.c_str());
	}
}

void IGameManager::cmdStopProcess(const StringVector &_args)
{
	if(!_args.empty())
	{
		RemoveUpdateFunction(_args[1]);
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

	if(!m_PathPlanner || m_PathPlanner->GetPlannerType() != navId)
	{
		NavigationManager::GetInstance()->DeletePathPlanner();

		// Create the requested path planner.
		if(NavigationManager::GetInstance()->CreatePathPlanner(navId))
		{
			m_PathPlanner = NavigationManager::GetInstance()->GetCurrentPathPlanner();
			m_PathPlanner->RegisterScriptFunctions(m_ScriptManager->GetMachine());
			EngineFuncs::ConsoleMessage("Navigation System created.");
		}
		else
		{
			EngineFuncs::ConsoleError("Unable to create Navigation System.");
		}
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

//////////////////////////////////////////////////////////////////////////

bool IGameManager::AddUpdateFunction(const String &_name, FunctorPtr _func)
{
	if(m_UpdateMap.find(_name) != m_UpdateMap.end())
	{
		EngineFuncs::ConsoleError("That process is already running!");
		return false;
	}
	EngineFuncs::ConsoleMessagef("Process %s has been started! ", _name.c_str());
	m_UpdateMap.insert(std::make_pair(_name, _func));
	return true;
}

bool IGameManager::RemoveUpdateFunction(const String &_name)
{
	FunctorMap::iterator it = m_UpdateMap.find(_name);
	if(it != m_UpdateMap.end())
	{
		EngineFuncs::ConsoleMessagef("Process %s has been stopped! ", _name.c_str());
		m_UpdateMap.erase(_name.c_str());
		return true;
	}
	return false;
}
