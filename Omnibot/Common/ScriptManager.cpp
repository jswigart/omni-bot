////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptManager.h"

#include "Client.h"
#include "IGameManager.h"
#include "FileSystem.h"

#include "Weapon.h"

// Custom script libraries
#include "gmCall.h"
#include "gmDebug.h"
#include "gmSystemLibApp.h"
#include "gmMathLibrary.h"
#include "gmBotLibrary.h"
#include "gmMachineLib.h"
#include "gmHelpers.h"
#include "gmAABB.h"
#include "gmBot.h"
#include "gmWeapon.h"
#include "gmGameEntity.h"
#include "gmTargetInfo.h"
#include "gmTriggerInfo.h"
#include "gmNamesList.h"
#include "gmMatrix3.h"
#include "gmStringLib.h"
#include "gmUtilityLib.h"
#include "gmSqliteLib.h"
#include "gmTimer.h"
#include "gmScriptGoal.h"
#include "gmSchemaLib.h"

// Custom Objects
#include "gmBind.h"
#include "gmbinder2/gmbinder2.h"

#include <boost/shared_array.hpp>
#include <boost/algorithm/string.hpp>

//////////////////////////////////////////////////////////////////////////

bool ScriptLiveUpdate = false;

struct LiveUpdateEntry
{
	filePath		File;
	int64_t			FileModTime;

	LiveUpdateEntry( const filePath &_path, int64_t _mod )
		: File( _path )
		, FileModTime( _mod )
	{
	}
};

typedef std::vector<LiveUpdateEntry> LiveUpdateList;
static LiveUpdateList g_LiveUpdate;
static int NextLiveUpdateCheck = 0;

//////////////////////////////////////////////////////////////////////////

ThreadScoper::ThreadScoper( int _id ) : mThreadId( _id )
{
}

ThreadScoper::~ThreadScoper()
{
	Kill();
}

bool ThreadScoper::IsActive()
{
	if ( ( mThreadId != GM_INVALID_THREAD ) && ScriptManager::IsInstantiated() )
	{
		gmThread *pTh = ScriptManager::GetInstance()->GetMachine()->GetThread( mThreadId );
		if ( pTh != NULL && pTh->GetState() != gmThread::KILLED && pTh->GetState() != gmThread::EXCEPTION )
			return true;
	}
	return false;
}

void ThreadScoper::Kill()
{
	if ( ( mThreadId != GM_INVALID_THREAD ) && ScriptManager::IsInstantiated() )
	{
		ScriptManager::GetInstance()->GetMachine()->KillThread( mThreadId );
		mThreadId = GM_INVALID_THREAD;
	}
}

//////////////////////////////////////////////////////////////////////////

bool g_RemoteDebuggerInitialized = false;

#if defined(ENABLE_REMOTE_DEBUGGER) && defined(GMDEBUG_SUPPORT)

#include <sfml/Network.hpp>
#pragma comment(lib,"sfml-network.lib")

sf::SocketTCP g_DebugListener;
sf::SocketTCP g_DebugClient;

const int GM_DEBUGGER_PORT = 49001;

#endif

//////////////////////////////////////////////////////////////////////////
//#if GM_LOGALLOCANDDEALLOC
//
//File destructFile;
//void LogDestruct(int _type, void *_addr)
//{
//	if(!destructFile.IsOpen())
//	{
//		FileSystem::FileDelete("user/destruct.txt");
//		destructFile.OpenForWrite("user/destruct.txt", File::Text);
//	}
//
//	if(destructFile.IsOpen())
//	{
//		const char *pTypeName = ScriptManager::GetInstance()->GetMachine()->GetTypeName((gmType)_type);
//		destructFile.WriteString(va("%x, %s", _addr, pTypeName));
//		destructFile.WriteNewLine();
//	}
//}
//
//File allocFile;
//void LogAlloc(int _type, void *_addr)
//{
//	if(!FileSystem::IsInitialized())
//		return;
//
//	if(!allocFile.IsOpen())
//	{
//		FileSystem::FileDelete("user/alloc.txt");
//		allocFile.OpenForWrite("user/alloc.txt", File::Text);
//	}
//
//	if(allocFile.IsOpen())
//	{
//		const char *pTypeName = ScriptManager::GetInstance()->GetMachine()->GetTypeName((gmType)_type);
//		allocFile.WriteString(va("%x, %s", _addr, pTypeName));
//		allocFile.WriteNewLine();
//	}
//}
//
//#endif

//////////////////////////////////////////////////////////////////////////

int ImportModuleImpl( gmThread *a_thread, const char *a_filename, gmVariable &a_this )
{
	try
	{
		int ThreadId = GM_INVALID_THREAD;
		filePath script( "%s.gm", a_filename );
		if ( ScriptManager::GetInstance()->ExecuteFile( script, ThreadId, &a_this ) )
		{
			return GM_OK;
		}
	}
	catch ( const std::exception & ex )
	{
		ex;
	}

	GM_EXCEPTION_MSG( "Unable to execute %s", a_filename );
	return GM_EXCEPTION;
}

//////////////////////////////////////////////////////////////////////////

ScriptManager *ScriptManager::mInstance = NULL;

ScriptManager::ScriptManager() :
mScriptEngine( 0 ),
#ifdef _DEBUG
mDebugScripts( true )
#else
mDebugScripts( false )
#endif
{
}

ScriptManager::~ScriptManager()
{
	if ( mScriptEngine )
		Shutdown();
}

ScriptManager *ScriptManager::GetInstance()
{
	if ( !mInstance )
		mInstance = new ScriptManager;
	return mInstance;
}

void ScriptManager::DeleteInstance()
{
	OB_DELETE( mInstance );
}

bool ScriptManager::IsInstantiated()
{
	return mInstance != 0;
}

void ScriptManager::Init()
{
	LOGFUNCBLOCK;
	InitCommands();
	LOG( "Initializing Scripting System.." );
	// Set machine callbacks
	mScriptEngine = new gmMachine;
	mScriptEngine->SetDebugMode( true );
	gmMachine::s_machineCallback = ScriptManager::ScriptSysCallback_Machine;
	gmMachine::s_printCallback = ScriptManager::ScriptSysCallback_Print;

	gmGCRootManager::Init();

	DisableGCInScope gcEn( mScriptEngine );

	const int MEM_USAGE_MB = 2 * 1048576;
	const int MEM_USAGE_KB = 0 * 1024;
	const int HARD_MEM_USAGE = MEM_USAGE_MB + MEM_USAGE_KB;
	mScriptEngine->SetDesiredByteMemoryUsageHard( HARD_MEM_USAGE );
	mScriptEngine->SetDesiredByteMemoryUsageSoft( HARD_MEM_USAGE * 9 / 10 );

	//////////////////////////////////////////////////////////////////////////
	LOG( "Hard Memory Limit: " << Utils::FormatByteString( HARD_MEM_USAGE ) );

	// Allocate some permanent strings for properties that will be used alot.
	mScriptEngine->AllocPermanantStringObject( "CurrentHealth" );
	mScriptEngine->AllocPermanantStringObject( "MaxHealth" );
	mScriptEngine->AllocPermanantStringObject( "CurrentArmor" );
	mScriptEngine->AllocPermanantStringObject( "MaxArmor" );

	// set the callback for executing import scripts
	gmImportExecuteFile = ImportModuleImpl;

	// Bind libraries.
	LOG( "Binding Script Libraries.." );
	gmBindSystemLib( mScriptEngine );
	LOG( "+ System Library Bound." );
	gmBindMathLibrary( mScriptEngine );
	LOG( "+ Math Library Bound." );
	gmBindStringLib( mScriptEngine );
	LOG( "+ std::string Library Bound." );
	gmBindBotLib( mScriptEngine );
	LOG( "+ Bot System Library Bound." );
	BlackBoard::Bind( mScriptEngine );
	LOG( "+ Blackboard Library Bound." );
	gmBindNamesListLib( mScriptEngine );
	gmBot::Initialise( mScriptEngine, true );
	LOG( "+ Bot Library Bound." );
	/*gmMapGoal::Initialise(.mScriptEngine, true);
	LOG("+ MapGoal Library Bound.");*/
	gmTargetInfo::Initialise( mScriptEngine, true );
	LOG( "+ TargetInfo Library Bound." );
	gmTriggerInfo::Initialise( mScriptEngine, true );
	LOG( "+ TriggerInfo Library Bound." );
	gmTimer::Initialise( mScriptEngine, false );
	LOG( "+ Timer Library Bound." );
	//BindAABB(.mScriptEngine);
	gmAABB::Initialise( mScriptEngine, false );

	LOG( "+ AABB Library Bound." );
	gmMatrix3::Initialise( mScriptEngine, false );
	LOG( "+ Matrix3 Library Bound." );
	gmScriptGoal::Initialise( mScriptEngine, true );
	LOG( "+ Script Goal Library Bound." );
	gmBindUtilityLib( mScriptEngine );
	LOG( "+ Utility Library Bound." );
	gmSchema::BindLib( mScriptEngine );
	LOG( "+ Schema Library Bound." );

	BindEntityStackCustom( mScriptEngine );

	// New Bindings
	MapGoal::Bind( mScriptEngine );
	LOG( "+ MapGoal Library Bound." );
	Weapon::Bind( mScriptEngine );
	LOG( "+ Weapon Library Bound." );

#if defined(ENABLE_REMOTE_DEBUGGER) && defined(GMDEBUG_SUPPORT)
	gmBindDebugLib( .mScriptEngine );
#endif

	// Create default global tables.
	mScriptEngine->GetGlobals()->Set( mScriptEngine, "Names", gmVariable( mScriptEngine->AllocUserObject( NULL, GM_NAMESLIST ) ) );
	mScriptEngine->GetGlobals()->Set( mScriptEngine, "BotTable", gmVariable( mScriptEngine->AllocTableObject() ) );
	mScriptEngine->GetGlobals()->Set( mScriptEngine, "Commands", gmVariable( mScriptEngine->AllocTableObject() ) );
	mScriptEngine->GetGlobals()->Set( mScriptEngine, "GOALS", gmVariable( mScriptEngine->AllocTableObject() ) );

	LOG( "+ Name List Created" );

	gmBind2::Global( mScriptEngine, "COLOR" )
		.var( COLOR::BLACK.rgba(), "BLACK" )
		.var( COLOR::RED.rgba(), "RED" )
		.var( COLOR::GREEN.rgba(), "GREEN" )
		.var( COLOR::BLUE.rgba(), "BLUE" )
		.var( COLOR::WHITE.rgba(), "WHITE" )
		.var( COLOR::MAGENTA.rgba(), "MAGENTA" )
		.var( COLOR::LIGHT_GREY.rgba(), "LIGHT_GREY" )
		.var( COLOR::GREY.rgba(), "GREY" )
		.var( COLOR::ORANGE.rgba(), "ORANGE" )
		.var( COLOR::YELLOW.rgba(), "YELLOW" )
		.var( COLOR::CYAN.rgba(), "CYAN" )
		.var( COLOR::PINK.rgba(), "PINK" )
		.var( COLOR::BROWN.rgba(), "BROWN" )
		.var( COLOR::AQUAMARINE.rgba(), "AQUAMARINE" )
		.var( COLOR::LAVENDER.rgba(), "LAVENDER" )
		;

	gmBind2::Global( mScriptEngine, "MoveMode" )
		.var( (int)Run, "Run" )
		.var( (int)Walk, "Walk" )
		;

	gmTableObject *pAimPrioTable = mScriptEngine->AllocTableObject();
	mScriptEngine->GetGlobals()->Set( mScriptEngine, "Priority", gmVariable( pAimPrioTable ) );
	for ( int i = 0; i < Priority::NumPriority; ++i )
		pAimPrioTable->Set( mScriptEngine, Priority::AsString( i ), gmVariable( i ) );

	//////////////////////////////////////////////////////////////////////////
	bool RemoteDebugger = false;
	Options::GetValue( "Script", "EnableRemoteDebugger", RemoteDebugger );
	EnableRemoteDebugger( RemoteDebugger );
	//////////////////////////////////////////////////////////////////////////
	ScriptLiveUpdate = false;
	Options::GetValue( "Script", "LiveUpdate", ScriptLiveUpdate );

	LOG( "done." );
}

void ScriptManager::Shutdown()
{
#if defined(ENABLE_REMOTE_DEBUGGER) && defined(GMDEBUG_SUPPORT)
	if ( g_RemoteDebuggerInitialized )
	{
		CloseDebugSession();
		g_DebugListener.Close();
	}
#endif

	mScriptEngine->CollectGarbage( true );

	gmGCRootManager::Get()->DestroyMachine( mScriptEngine );
	gmGCRootManager::Destroy();

	LOGFUNCBLOCK;
	ShowGMStats();
	OB_DELETE( mScriptEngine );
	LOG( "Script System Shut Down." );
}

struct ThreadStatus
{
	int mRunning;
	int mBlocked;
	int mSleeping;
};

void ScriptManager::Update()
{
	rmt_ScopedCPUSample( ScriptManagerUpdate );

	{
		rmt_ScopedCPUSample( gmMachineExecute );
		mScriptEngine->Execute( (gmuint32)IGame::GetDeltaTime() );
	}

	//////////////////////////////////////////////////////////////////////////
#if defined(ENABLE_REMOTE_DEBUGGER) && defined(GMDEBUG_SUPPORT)
	if ( g_RemoteDebuggerInitialized )
	{
		if ( !g_DebugClient.IsValid() )
		{
			sf::IPAddress addr;
			sf::Socket::Status s = g_DebugListener.Accept( g_DebugClient, &addr ); s;

			if ( addr.IsValid() )
			{
				Utils::OutputDebug( kScript, "Remote Debugger Connected: %s", addr.ToString().c_str() );
				OpenDebugSession( .mScriptEngine );
				g_DebugClient.SetBlocking( false );
			}
		}

		if ( g_DebugClient.IsValid() )
		{
			rmt_ScopedCPUSample( RemoteDebugClient );
			UpdateDebugSession();
		}
	}
#endif
	//////////////////////////////////////////////////////////////////////////
	if ( ScriptLiveUpdate )
		CheckLiveUpdates();
}

gmUserObject *ScriptManager::AddBotToGlobalTable( Client *_client )
{
	gmUserObject *pUser = gmBot::WrapObject( mScriptEngine, _client );
	gmTableObject *pGlobalBotsTable = GetGlobalBotsTable();
	if ( pGlobalBotsTable )
	{
		pGlobalBotsTable->Set( mScriptEngine, _client->GetGameID(), gmVariable( pUser ) );
	}
	return pUser;
}

void ScriptManager::RemoveFromGlobalTable( Client *_client )
{
	gmTableObject *pGlobalBotsTable = GetGlobalBotsTable();
	if ( pGlobalBotsTable )
	{
		pGlobalBotsTable->Set( mScriptEngine, _client->GetGameID(), gmVariable::s_null );
	}
	else
	{
		EngineFuncs::ConsoleError( "Bots script table lost" );
	}
}

gmTableObject *ScriptManager::GetGlobalBotsTable()
{
	gmVariable botVar = mScriptEngine->GetGlobals()->Get( mScriptEngine, "BotTable" );
	gmTableObject *botTable = botVar.GetTableObjectSafe();
	if ( botTable )
	{
		gmTableObject *botSubTable = botVar.GetTableObjectSafe();
		return botSubTable;
	}
	else
	{
		if ( IsScriptDebugEnabled() )
		{
			EngineFuncs::ConsoleError( "Global Bots table lost" );
		}
	}
	return NULL;
}

gmTableObject *ScriptManager::GetGlobalCommandsTable()
{
	gmVariable cmdVar = mScriptEngine->GetGlobals()->Get( mScriptEngine, "Commands" );
	gmTableObject *commandTable = cmdVar.GetTableObjectSafe();
	if ( commandTable )
	{
		return commandTable;
	}
	else
	{
		if ( IsScriptDebugEnabled() )
		{
			EngineFuncs::ConsoleError( "Global commands table lost" );
		}
	}
	return NULL;
}

gmTableObject *ScriptManager::GetBotTable( const Client *_client )
{
	gmTableObject *pBotsTable = GetGlobalBotsTable();

	if ( pBotsTable )
	{
		// Get the table for this specific bot.
		gmVariable botVar = pBotsTable->Get( gmVariable( _client->GetGameID() ) );

		if ( botVar.m_type == gmBot::GetType() )
		{
			return gmBot::GetUserTable( botVar.GetUserObjectSafe( gmBot::GetType() ) );;
		}
		else
		{
			if ( IsScriptDebugEnabled() )
			{
				EngineFuncs::ConsoleError( "Bot entry wrong type!" );
			}
		}
	}
	return NULL;
}

gmVariable ScriptManager::ExecBotCallback( Client *_client, const char *_func )
{
	gmTableObject *pBotTable = GetBotTable( _client );
	if ( pBotTable )
	{
		/*gmVariable v =*/ pBotTable->Get( mScriptEngine, _func );

		gmCall call;
		gmVariable vThis( _client->GetScriptObject() );
		if ( call.BeginTableFunction( mScriptEngine, _func, pBotTable, vThis ) )
		{
			call.End();
			return call.GetReturnedVariable();
		}
	}
	return gmVariable::s_null;
}

bool ScriptManager::ExecuteFile( const filePath &_file, int &_threadId, gmVariable *_this/* = NULL*/ )
{
	//Utils::OutputDebug(kScript,"ExecuteFile: %s",_file.c_str());

	GM_ASSERT( mScriptEngine );
	if ( mScriptEngine != NULL )
	{
		_threadId = GM_INVALID_THREAD;

		// Find the file
		File InFile;

		filePath localFilePath = _file;
		InFile.OpenForRead( localFilePath, File::Binary );
		if ( !InFile.IsOpen() )
		{
			localFilePath = filePath( "scripts/%s", _file.c_str() );
			InFile.OpenForRead( localFilePath, File::Binary );
			if ( !InFile.IsOpen() )
			{
				localFilePath = filePath( "global_scripts/%s", _file.c_str() );
				InFile.OpenForRead( localFilePath, File::Binary );
			}
		}

		if ( InFile.IsOpen() )
		{
			uint32_t fileSize = (uint32_t)InFile.FileLength();
			boost::shared_array<char> pBuffer( new char[ fileSize + 1 ] );

			InFile.Read( pBuffer.get(), fileSize );
			pBuffer[ fileSize ] = 0;
			InFile.Close();

			LOG( "Running script: " << _file );

			if ( fileSize > 0 )
			{
				int errors = mScriptEngine->ExecuteString( pBuffer.get(), &_threadId, true, _file, _this );
				if ( errors )
				{
					bool b = IsScriptDebugEnabled();
					SetScriptDebugEnabled( true );
					LogAnyMachineErrorMessages( mScriptEngine );
					SetScriptDebugEnabled( b );
				}
				else
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool ScriptManager::ExecuteString( const std::string &_string, gmVariable *_this/* = NULL*/, bool _now )
{
	GM_ASSERT( mScriptEngine );
	if ( mScriptEngine )
	{
		int threadId = GM_INVALID_THREAD;
		int errors = mScriptEngine->ExecuteString( _string.c_str(), &threadId, _now, NULL, _this );
		if ( errors )
		{
			LogAnyMachineErrorMessages( mScriptEngine );
			return false;
		}
	}
	return true;
}

bool ScriptManager::ExecuteStringLogged( const std::string &_string )
{
	bool b = false;
	File f;
	f.OpenForWrite( "user/logged.gm", File::Text, true );
	if ( f.IsOpen() )
	{
		b = f.WriteString( _string ) && f.WriteNewLine();
	}
	EngineFuncs::ConsoleMessage( va( "ExecString: %s", _string.c_str() ).c_str() );

	return ( b && ExecuteString( _string ) );
}

void ScriptManager::LogAnyMachineErrorMessages( gmMachine *_machine )
{
	bool bFirst = true;
	const char *pMessage = 0;
	while ( ( pMessage = _machine->GetLog().GetEntry( bFirst ) ) )
	{
		std::string msg = pMessage;

		// remove line endings at the end, its already handled elsewhere
		while ( !msg.empty() )
		{
			if ( msg.back() == '\n' )
				msg.pop_back();
			else if ( msg.back() == '\r' )
				msg.pop_back();
			else break;
		}

		// convert internal line endings
		boost::replace_all( msg, "\t", "    " ); // tabs to spaces
		boost::replace_all( msg, "\r\n", "\n" ); // tabs to spaces

		Utils::OutputDebugBasic( kError, "Script Error:" );
		Utils::OutputDebugBasic( kError, msg.c_str() );
		LOGERR( msg.c_str() );

#ifdef ENABLE_DEBUG_WINDOW
		std::string s = pMessage;
		DW.Console.AddLine( s );
#endif
		if ( _machine->GetDebugMode() )
		{
			EngineFuncs::ConsoleError( msg.c_str() );
		}
	}
	_machine->GetLog().Reset();
}

void GM_CDECL ScriptManager::ScriptSysCallback_Print( gmMachine* a_machine, const char* a_string )
{
	if ( a_string )
	{
#ifdef ENABLE_DEBUG_WINDOW
		DW.Console.AddLine( a_string );
#endif
		EngineFuncs::ConsoleMessage( a_string );
	}
}

bool GM_CDECL ScriptManager::ScriptSysCallback_Machine( gmMachine* a_machine, gmMachineCommand a_command, const void* a_context )
{
	const gmThread *pThread = static_cast<const gmThread*>( a_context );

	switch ( a_command )
	{
		case MC_THREAD_EXCEPTION:
		{
			LogAnyMachineErrorMessages( a_machine );
			break;
		}
		case MC_THREAD_CREATE:
		{
			// Called when a thread is created.  a_context is the thread.
			Client *pClient = 0;
			const gmVariable *pThis = pThread->GetThis();
			gmUserObject *pBotUserObj = pThis ? pThis->GetUserObjectSafe( gmBot::GetType() ) : 0;
			if ( pBotUserObj )
			{
				// See if it's the bots goal thread that is being destroyed.
				pClient = gmBot::GetNative( pBotUserObj );
				if ( pClient )
				{
					/*Event_SystemThreadCreated d = { pThread->GetId() };
					Message evt(SYSTEM_THREAD_CREATED, &d, sizeof(d));
					pClient->SendEvent(evt);*/
				}
			}

			// Debug Output
			bool bScriptDebugEnabled = ScriptManager::GetInstance()->IsScriptDebugEnabled();
			if ( bScriptDebugEnabled || pClient )
			{
				const gmFunctionObject *pFn = pThread->GetFunctionObject();
				const char *pFuncName = pFn != NULL ? pFn->GetDebugName() : 0;

				const char *pSource = 0, *pFileName = 0;
				if ( pFn )
				{
					a_machine->GetSourceCode( pFn->GetSourceId(), pSource, pFileName );
				}

				std::string errMsg = va( "Thread Created <%s>: %s : %s Id: %d time: %d",
					( pClient ? pClient->GetName().c_str() : "" ),
					( pFileName ? pFileName : "<unknown file>" ),
					( pFuncName ? pFuncName : "<noname>" ),
					pThread->GetId(),
					IGame::GetTime() ).c_str();

				if ( bScriptDebugEnabled )
				{
					//EngineFuncs::ConsoleMessage(errMsg.str().c_str());
					Utils::OutputDebug( kScript, errMsg.c_str() );
				}
			}
			break;
		}
		case MC_THREAD_DESTROY:
		{
			/*Event_SystemThreadDestroyed d = { pThread->GetId() };
			Message evt(SYSTEM_THREAD_DESTROYED, &d, sizeof(d));
			System::mInstance->mGame->DispatchGlobalEvent(evt);*/
			System::mInstance->mGame->AddDeletedThread( pThread->GetId() );
		}
		default:
			break;
	}
	return false;
}

static bool countThreadStatus( gmThread *a_thread, void *a_context )
{
	ThreadStatus *pThreadStatus = static_cast<ThreadStatus*>( a_context );
	switch ( a_thread->GetState() )
	{
		case gmThread::RUNNING:
		case gmThread::SYS_PENDING:
		case gmThread::SYS_YIELD:
			pThreadStatus->mRunning++;
			break;
		case gmThread::SLEEPING:
			pThreadStatus->mSleeping++;
			break;
		case gmThread::BLOCKED:
			pThreadStatus->mBlocked++;
			break;
		case gmThread::KILLED:
		case gmThread::EXCEPTION:
		case gmThread::SYS_EXCEPTION:
			break;
	}
	return true;
}

void ScriptManager::ShowGMStats()
{
	if ( mScriptEngine )
	{
		ThreadStatus st = { 0, 0, 0 };
		mScriptEngine->ForEachThread( countThreadStatus, &st );

		std::string fmtMemUsage = va( "Current Memory Usage %s", Utils::FormatByteString( mScriptEngine->GetCurrentMemoryUsage() ).c_str() ).c_str();
		std::string fmtSoftMemLimit = va( "Soft Memory Usage %s", Utils::FormatByteString( mScriptEngine->GetDesiredByteMemoryUsageSoft() ).c_str() ).c_str();
		std::string fmtHardMemLimit = va( "Hard Memory Limit %s", Utils::FormatByteString( mScriptEngine->GetDesiredByteMemoryUsageHard() ).c_str() ).c_str();
		std::string fmtSysMemUsage = va( "System Memory Usage %s", Utils::FormatByteString( mScriptEngine->GetSystemMemUsed() ).c_str() ).c_str();
		std::string fmtFullCollects = va( "Full Collects %d", mScriptEngine->GetStatsGCNumFullCollects() ).c_str();
		std::string fmtIncCollects = va( "Inc Collects %d", mScriptEngine->GetStatsGCNumIncCollects() ).c_str();
		std::string fmtGCWarnings = va( "GC Warnings %d", mScriptEngine->GetStatsGCNumWarnings() ).c_str();
		std::string fmtThreadInfo = va( "Threads: %d, %d Running, %d Blocked, %d Sleeping",
			( st.mBlocked + st.mRunning + st.mSleeping ),
			st.mRunning,
			st.mBlocked,
			st.mSleeping ).c_str();

		EngineFuncs::ConsoleMessage( "-- Script System Info --" );
		EngineFuncs::ConsoleMessage( fmtMemUsage.c_str() );
		EngineFuncs::ConsoleMessage( fmtSoftMemLimit.c_str() );
		EngineFuncs::ConsoleMessage( fmtHardMemLimit.c_str() );
		EngineFuncs::ConsoleMessage( fmtSysMemUsage.c_str() );
		EngineFuncs::ConsoleMessage( fmtFullCollects.c_str() );
		EngineFuncs::ConsoleMessage( fmtIncCollects.c_str() );
		EngineFuncs::ConsoleMessage( fmtGCWarnings.c_str() );
		EngineFuncs::ConsoleMessage( fmtThreadInfo.c_str() );

		LOG( fmtMemUsage.c_str() );
		LOG( fmtSoftMemLimit.c_str() );
		LOG( fmtHardMemLimit.c_str() );
		LOG( fmtSysMemUsage.c_str() );
		LOG( fmtFullCollects.c_str() );
		LOG( fmtIncCollects.c_str() );
		LOG( fmtGCWarnings.c_str() );
	}
	else
	{
		EngineFuncs::ConsoleError( "No Script System!" );
	}
}

void ScriptManager::GetAutoCompleteList( const std::string &_string, StringVector &_completions )
{
	//try
	//{
	//	std::string strCopy = _string;
	//	Utils::StringTrimCharacters(strCopy, "[]");

	//	boost::regex exp(strCopy + ".*", REGEX_OPTIONS);

	//	gmTableObject *pTable = mScriptEngine->GetGlobals();

	//	// Look into tables if the std::string contains them.
	//	std::string prefix, entry;
	//	uint32_t iLastDot = strCopy.find_last_of(".");
	//	if(iLastDot != strCopy.npos)
	//	{
	//		gmVariable v = mScriptEngine->Lookup(strCopy.substr(0,iLastDot).c_str());
	//		if(v.GetTableObjectSafe())
	//		{
	//			prefix = strCopy.substr(0,iLastDot+1);
	//			pTable = v.GetTableObjectSafe();
	//			exp = boost::regex(strCopy.substr(iLastDot+1) + ".*", REGEX_OPTIONS);
	//		}
	//	}

	//	const int bufsize = 256;
	//	char buffer[bufsize];

	//	gmTableIterator tIt;
	//	gmTableNode *pNode = pTable->GetFirst(tIt);
	//	while(pNode)
	//	{
	//		const char *pName = pNode->mkey.AsString(.mScriptEngine, buffer, 1024);
	//		if(boost::regex_match(pName, exp))
	//		{
	//			switch(pNode->mkey.mtype)
	//			{
	//			case GM_STRING:
	//				_completions.push_back(prefix + pName);
	//				break;
	//			case GM_INT:
	//				_completions.push_back((std::string)va("%s[%s]", prefix.c_str(), pName));
	//				break;
	//			default:
	//				_completions.push_back(prefix + pName);
	//				break;
	//			}
	//		}
	//		pNode = pTable->GetNext(tIt);
	//	}
	//}
	//catch(const std::exception&e)
	//{
	//	e;
	//}
}

void ScriptManager::InitCommands()
{
	SetEx( "script_stats", "Shows scripting system memory usage/stats",
		this, &ScriptManager::cmdScriptStats );
	SetEx( "script_collect", "Performs a garbage collection",
		this, &ScriptManager::cmdScriptCollect );
	SetEx( "script_runfile", "Executes a specified script file",
		this, &ScriptManager::cmdScriptRunFile );
	SetEx( "script_debug", "Enables/disables debug messages in the scripting system.",
		this, &ScriptManager::cmdDebugScriptSystem );
	SetEx( "script_run", "Executes a std::string as a script snippet.",
		this, &ScriptManager::cmdScriptExecute );
	SetEx( "script_docs", "Dumps a file of gm bound type info.",
		this, &ScriptManager::cmdScriptWriteDocs );
}

void ScriptManager::cmdScriptStats( const StringVector & args )
{
	ShowGMStats();
}

void ScriptManager::cmdDebugScriptSystem( const StringVector & args )
{
	if ( args.size() >= 2 )
	{
		if ( !mDebugScripts && Utils::StringToTrue( args[ 1 ] ) )
		{
			EngineFuncs::ConsoleMessage( "Script Debug Messages On." );
			mDebugScripts = true;
		}
		else if ( mDebugScripts && Utils::StringToFalse( args[ 1 ] ) )
		{
			EngineFuncs::ConsoleMessage( "Script Debug Messages Off." );
			mDebugScripts = false;
		}
	}
}

void ScriptManager::cmdScriptCollect( const StringVector & args )
{
	if ( mScriptEngine )
	{
		EngineFuncs::ConsoleMessage( "Before Collection" );
		cmdScriptStats( args );
		mScriptEngine->CollectGarbage( true );
		EngineFuncs::ConsoleMessage( "After Collection" );
		cmdScriptStats( args );
	}
	else
	{
		EngineFuncs::ConsoleError( "No Script System!" );
	}
}

void ScriptManager::cmdScriptRunFile( const StringVector & args )
{
	if ( args.size() >= 2 )
	{
		try
		{
			int threadId;
			if ( ExecuteFile( args[ 1 ].c_str(), threadId ) )
				return;
		}
		catch ( const std::exception & ex )
		{
			//ex;
			LOGCRIT( "Filesystem Exception: " << ex.what() );
		}
	}

	EngineFuncs::ConsoleError( "Error Running Script." );
}

void ScriptManager::cmdScriptExecute( const StringVector & args )
{
	if ( args.size() >= 2 )
	{
		try
		{
			std::string str;
			for ( uint32_t i = 1; i < args.size(); ++i )
			{
				str += " ";
				str += args[ i ];
			}

			std::string::iterator sIt = str.begin();
			for ( ; sIt != str.end(); ++sIt )
			{
				if ( *sIt == '\'' )
					*sIt = '\"';
			}

			if ( *str.rbegin() != ';' )
				str.push_back( ';' );

			if ( ExecuteString( str, NULL ) )
				return;
		}
		catch ( const std::exception & ex )
		{
			//ex;
			LOGCRIT( "Filesystem Exception: " << ex.what() );
		}
	}

	EngineFuncs::ConsoleError( "Error Running Script." );
}

void ScriptManager::cmdScriptWriteDocs( const StringVector & args )
{
#if(GMBIND2_DOCUMENT_SUPPORT)
	DisableGCInScope gcEn( mScriptEngine );

	gmBind2::TableConstructor tc( mScriptEngine );
	tc.Push( "Weapon" );
	gmBind2::Class<Weapon>::GetPropertyTable( mScriptEngine, tc.Top() );
	tc.Pop();
	tc.Push( "FireMode" );
	gmBind2::Class<Weapon::WeaponFireMode>::GetPropertyTable( mScriptEngine, tc.Top() );
	tc.Pop();
	tc.Push( "MapGoal" );
	gmBind2::Class<MapGoal>::GetPropertyTable( mScriptEngine, tc.Top() );
	tc.Pop();

	File f;
	if ( f.OpenForWrite( "user/docs.gm", File::Text ) )
		gmUtility::DumpTable( mScriptEngine, f, "Docs", tc.Root(), gmUtility::DUMP_RECURSE );
#endif
}

ScriptResource::ScriptResource() : mKey( -1 )
{
}
ScriptResource::~ScriptResource()
{
}
ScriptResource & ScriptResource::operator=( const ScriptResource &_rh )
{
	mKey = _rh.mKey;
	mScript = _rh.mScript;
	return *this;
}
bool ScriptResource::InitScriptSource( const filePath &_path )
{
	mScript = _path;
	mKey = ScriptManager::GetInstance()->RegisterLiveUpdate( _path );
	return true;
}

LiveUpdateKey ScriptManager::RegisterLiveUpdate( const filePath &_file )
{
	// check if it already exists.
	for ( uint32_t i = 0; i < g_LiveUpdate.size(); ++i )
	{
		if ( g_LiveUpdate[ i ].File == _file )
			return i;
	}

	// add it to the list.
	const LiveUpdateKey key = (int)g_LiveUpdate.size();

	LiveUpdateEntry entry( _file, FileSystem::FileModifiedTime( _file ) );
	g_LiveUpdate.push_back( entry );

	return key;
}

void ScriptManager::CheckLiveUpdates()
{
	if ( IGame::GetTime() >= NextLiveUpdateCheck )
	{
		NextLiveUpdateCheck = IGame::GetTime() + 1000;

		for ( uint32_t i = 0; i < g_LiveUpdate.size(); ++i )
		{
			LiveUpdateEntry &entry = g_LiveUpdate[ i ];
			const int64_t modTime = FileSystem::FileModifiedTime( entry.File );
			if ( modTime > entry.FileModTime )
			{
				// send an event and update the local time.
				EvScriptChanged::Msg event;
				event.mData.mScriptKey = i;
				System::mInstance->mGame->DispatchEvent( event );

				entry.FileModTime = modTime;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

#if defined(ENABLE_REMOTE_DEBUGGER) && defined(GMDEBUG_SUPPORT)

struct PacketHeader
{
	static const unsigned int MAGIC_NUM = 0xDEADB33F;

	const unsigned int magicNum;
	const unsigned int dataSize;
	PacketHeader( unsigned int sz ) : magicNum( MAGIC_NUM ), dataSize( sz )
	{
	}
};

std::string			g_RecieveBuffer;
std::string			g_LastCommand;

void ScriptManager::SendDebuggerMessage( const void * a_command, int a_len )
{
	if ( g_DebugClient.IsValid() )
	{
		PacketHeader hdr( a_len );

		sf::Packet packet;
		packet.Append( (const char *)&hdr, sizeof( hdr ) );
		packet.Append( (const char *)a_command, a_len );
		if ( g_DebugClient.Send( packet, false ) == sf::Socket::Disconnected )
		{
			g_DebugClient.Close();
			return;
		}
		Utils::OutputDebug( kScript, "%d sent", packet.GetDataSize() );
	}
}

const void * ScriptManager::PumpDebuggerMessage( int &a_len )
{
	// grab all the data from the network we can
	enum
	{
		BufferSize = 4096
	};
	char inBuffer[ BufferSize ];
	std::size_t inSize = 0;
	do
	{
		if ( g_DebugClient.Receive( inBuffer, BufferSize, inSize ) == sf::Socket::Done )
		{
			Utils::OutputDebug( kScript, va( "%d recieved", inSize ) );
			if ( inSize > 0 )
				g_RecieveBuffer.append( inBuffer, inSize );
		}
	}
	while ( inSize > 0 );

	if ( g_RecieveBuffer.size() > 0 )
	{
		// see if we have an entire packets worth of data waiting
		const char * dataPtr = g_RecieveBuffer.c_str();
		const PacketHeader * hdr = reinterpret_cast<const PacketHeader *>( dataPtr );
		dataPtr += sizeof( PacketHeader );
		if ( hdr->magicNum != PacketHeader::MAGIC_NUM )
		{
			// ERROR! Malformed Packet!
			return NULL;
		}

		if ( g_RecieveBuffer.size() >= hdr->dataSize + sizeof( PacketHeader ) )
		{
			a_len = hdr->dataSize;
			g_LastCommand.resize( 0 );
			g_LastCommand.append( dataPtr, hdr->dataSize );
			g_RecieveBuffer.erase( 0, hdr->dataSize + sizeof( PacketHeader ) );
			return g_LastCommand.c_str();
		}
	}
	return NULL;
}

#endif

//////////////////////////////////////////////////////////////////////////

void ScriptManager::EnableRemoteDebugger( bool _enable )
{
#if defined(ENABLE_REMOTE_DEBUGGER) && defined(GMDEBUG_SUPPORT)
	if ( _enable && !g_RemoteDebuggerInitialized )
	{
		if ( g_DebugListener.Listen( GM_DEBUGGER_PORT ) )
		{
			g_DebugListener.SetBlocking( false );
			g_RemoteDebuggerInitialized = true;
		}
		else
		{
			//const char *pError = XPSock_TranslateErrorLong(g_ScriptDebuggerSocket.GetLastError());
			//LOGERR("Error Initializing Remote Debug Socket, disabling");
			//LOGERR(pError);
			g_RemoteDebuggerInitialized = false;
		}
	}
	else if ( g_RemoteDebuggerInitialized )
	{
		// Shut it down!
		g_DebugListener.Close();
		g_RemoteDebuggerInitialized = false;
	}
#endif
}