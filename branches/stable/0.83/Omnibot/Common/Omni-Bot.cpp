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

#include "PrecompCommon.h"
#include "Omni-Bot.h"

#include "IGame.h"
#include "GoalManager.h"
#include "IGameManager.h"
#include "TriggerManager.h"

IGameManager *g_GameManager = 0;

// TESTING
//#include "MemoryManager.h"
#include "FileDownloader.h"

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

void SetThreadName( DWORD dwThreadID, char* threadName)
{
	Sleep(10);
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
}
#endif

//////////////////////////////////////////////////////////////////////////

omnibot_error BotInitialise(IEngineInterface *_pEngineFuncs, int _version)
{
	Timer loadTime;

#ifdef WIN32
	SetThreadName ((DWORD)-1, "MainThread");
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_CHECK_ALWAYS_DF|_CRTDBG_CHECK_CRT_DF|_CRTDBG_LEAK_CHECK_DF);
#endif

	// Create the Game Manager
	g_GameManager = IGameManager::GetInstance();
	omnibot_error result = g_GameManager->CreateGame(_pEngineFuncs, _version);
	
	if(result==BOT_ERROR_NONE)
	{
		EngineFuncs::ConsoleMessage(va("Bot Initialized in %.2f seconds.", loadTime.GetElapsedSeconds()));
		LOG("Bot Initialized in " << loadTime.GetElapsedSeconds() << " seconds.");
	}
	return result;
}

void BotUpdate()
{
	g_GameManager->UpdateGame();

#ifndef __linux__
	//_ASSERTE( _CrtCheckMemory( ) );
#endif
}

void BotShutdown()
{
	g_GameManager->Shutdown();
	IGameManager::DeleteInstance();
	g_Logger.Stop();
#ifndef __linux__
	//_ASSERTE( _CrtCheckMemory( ) );
#endif
}

void BotConsoleCommand(const Arguments &_args)
{
	StringVector tokList;
	for(int i = 0; i < _args.m_NumArgs; ++i)
	{
		String str = _args.m_Args[i];

		if(i==0)
		{
			std::transform(str.begin(), str.end(), str.begin(), toLower());

			if(str == "bot" || str == "ombot")
				continue;
		}

		tokList.push_back(str);
	}
	if(tokList.empty())
		tokList.push_back("help");
	CommandReciever::DispatchCommand(tokList);
}

void BotSendEvent(int _dest, const MessageHelper &_message) 
{
	IGameManager::GetInstance()->GetGame()->DispatchEvent(_dest, _message);
}

void BotSendGlobalEvent(const MessageHelper &_message)
{
	IGameManager::GetInstance()->GetGame()->DispatchGlobalEvent(_message);
}

void BotAddGoal(const MapGoalDef &goaldef)
{
	GameEntity Entity;
	if(goaldef.Props.GetEntity("Entity",Entity) && Entity.IsValid())
	{
		Event_EntityCreated d;
		d.m_Entity = Entity;
		d.m_EntityClass = ENT_CLASS_GENERIC_GOAL;
		d.m_EntityCategory.SetFlag(ENT_CAT_INTERNAL);
		BotSendGlobalEvent(MessageHelper(GAME_ENTITYCREATED, &d, sizeof(d)));
	}

	GoalManager::GetInstance()->AddGoal(goaldef);
}

void BotSendTrigger(const TriggerInfo &_triggerInfo)
{
	TriggerManager::GetInstance()->HandleTrigger(_triggerInfo);
}

void BotAddBBRecord(BlackBoard_Key _type, int _posterID, int _targetID, obUserData *_data)
{
}

void BotUpdateEntity( GameEntity oldent, GameEntity newent )
{
	GoalManager::GetInstance()->UpdateGoalEntity( oldent, newent );
}

void BotDeleteMapGoal( const char *goalname )
{
	GoalManager::GetInstance()->RemoveGoalByName( goalname );
}

void Omnibot_Load_PrintErr(char const *) {}
void Omnibot_Load_PrintMsg(char const *) {}

//////////////////////////////////////////////////////////////////////////
