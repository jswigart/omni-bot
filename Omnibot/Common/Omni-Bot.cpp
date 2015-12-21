////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include "windows.h"
#else
// TODO: linux shit here
#endif

#include "Omni-Bot.h"

#include "IGame.h"
#include "GoalManager.h"
#include "IGameManager.h"
#include "TriggerManager.h"
#include "PathPlannerBase.h"

IGameManager *g_GameManager = 0;

// TESTING
//#include "MemoryManager.h"

//////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#define MS_VC_EXCEPTION 0x406D1388

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName( DWORD dwThreadID, char* threadName )
{
	Sleep( 10 );
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof( info ) / sizeof( ULONG_PTR ), (ULONG_PTR*)&info );
	}
	__except ( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}
#endif

//////////////////////////////////////////////////////////////////////////

omnibot_error OmnibotFunctions::Initialize( IEngineInterface * engineFuncs, int version )
{
#ifdef WIN32
	SetThreadName( (DWORD)-1, "MainThread" );
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_CHECK_ALWAYS_DF|_CRTDBG_CHECK_CRT_DF|_CRTDBG_LEAK_CHECK_DF);
#endif

	g_Logger.Start( va( "%s/omnibot_%s.log", engineFuncs->GetLogPath(), engineFuncs->GetMapName() ).c_str(), true );
	
	// Create the Game Manager
	g_GameManager = new IGameManager();
	return g_GameManager->CreateGame( engineFuncs, version );
}

void OmnibotFunctions::Update()
{
	rmt_ScopedCPUSample( Omnibot );

	g_GameManager->UpdateGame();

#ifndef __linux__
	//_ASSERTE( _CrtCheckMemory( ) );
#endif
}
void OmnibotFunctions::Shutdown()
{
	g_GameManager->Shutdown();
	OB_DELETE( g_GameManager );

	g_Logger.Stop();
#ifndef __linux__
	//_ASSERTE( _CrtCheckMemory( ) );
#endif
}
void OmnibotFunctions::ConsoleCommand( const Arguments & args )
{
	StringVector tokList;
	for ( int i = 0; i < args.mNumArgs; ++i )
	{
		std::string str = args.mArgs[ i ];

		if ( i == 0 )
		{
			std::transform( str.begin(), str.end(), str.begin(), toLower() );

			if ( str == "bot" || str == "ombot" )
				continue;
		}

		tokList.push_back( str );
	}
	if ( tokList.empty() )
		tokList.push_back( "help" );
	CommandReciever::DispatchCommand( tokList );
}

void OmnibotFunctions::SendTrigger( const TriggerInfo & triggerInfo )
{
	TriggerManager::GetInstance()->HandleTrigger( triggerInfo );
}

void OmnibotFunctions::AddBlackboardRecord( BlackBoardKey type, int posterID, int targetID, obUserData * data )
{
}

void OmnibotFunctions::SendEvent( const Message & message, GameEntity dest )
{
	System::mInstance->mGame->DispatchEvent( message, dest );
}

void OmnibotFunctions::EntityAdded( GameEntity ent, const EntityInfo& entInfo )
{
	EvEntityCreated::Msg event;
	event.mData.mEntity = ent;
	event.mData.mEntityInfo = entInfo;
	SendEvent( event );
}

void OmnibotFunctions::EntityDestroyed( GameEntity ent )
{
	EvEntityDeleted::Msg event;
	event.mData.mEntity = ent;
	SendEvent( event );
}

void OmnibotFunctions::AddGoal( const MapGoalDef & goaldef )
{
	GameEntity Entity;
	if ( goaldef.Props.GetEntity( "Entity", Entity ) && Entity.IsValid() )
	{
		EvEntityCreated::Msg event;
		event.mData.mEntity = Entity;
		event.mData.mEntityInfo.mGroup = ENT_GRP_GOAL;
		event.mData.mEntityInfo.mCategory.SetFlag( ENT_CAT_INTERNAL );
		SendEvent( event );
	}

	System::mInstance->mGoalManager->AddGoal( goaldef );
}

void OmnibotFunctions::DeleteGoal( const char * goalname )
{
	System::mInstance->mGoalManager->RemoveGoalByName( goalname );
}

void OmnibotFunctions::UpdateEntity( GameEntity oldent, GameEntity newent )
{
	System::mInstance->mGoalManager->UpdateGoalEntity( oldent, newent );
}

void Omnibot_Load_PrintErr( char const * )
{
}
void Omnibot_Load_PrintMsg( char const * )
{
}

//////////////////////////////////////////////////////////////////////////