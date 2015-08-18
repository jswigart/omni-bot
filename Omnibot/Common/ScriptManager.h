////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __SCRIPTMANAGER_H__
#define __SCRIPTMANAGER_H__

class Client;

#include "gmMachine.h"
#include "CommandReciever.h"

class gmUserObject;
class gmStringObject;
class gmTableObject;

//typedef MessageDepot<Event_SystemThreadDestroyed, 256, uint64_t> ThreadDestroyDepot;
//extern ThreadDestroyDepot g_ThreadDestroyedDepot;

typedef std::vector< std::pair<std::string, int> > AutoCompleteList;

class ThreadScoper
{
public:
	bool operator==( const ThreadScoper& r ) const
	{
		return mThreadId == r.ThreadId();
	}
	bool operator!=( const ThreadScoper& r ) const
	{
		return mThreadId != r.ThreadId();
	}

	ThreadScoper& operator=( int _i )
	{
		mThreadId = _i; return *this;
	}
	operator bool() const
	{
		return mThreadId != GM_INVALID_THREAD;
	}
	operator int() const
	{
		return mThreadId;
	}

	int ThreadId() const
	{
		return mThreadId;
	}
	bool IsActive();
	void Reset()
	{
		mThreadId = GM_INVALID_THREAD;
	}

	void Kill();

	ThreadScoper( int _id = GM_INVALID_THREAD );
	~ThreadScoper();
private:
	int	 mThreadId;
};

typedef std::shared_ptr<ThreadScoper> ThreadPtr;
typedef std::list<ThreadPtr> ThreadList;

typedef int LiveUpdateKey;

//////////////////////////////////////////////////////////////////////////
// class: ScriptManager
//		Manages the Game Monkey Scripting system
#if defined(ENABLE_REMOTE_DEBUGGER) && defined(GMDEBUG_SUPPORT)
#include "gmDebug.h"
class ScriptManager : public CommandReciever, public gmDebugSession
#else
class ScriptManager : public CommandReciever
#endif
{
public:

	void Init();
	void Shutdown();
	void Update();

	gmUserObject *AddBotToGlobalTable( Client *_client );
	void RemoveFromGlobalTable( Client *_client );

	bool ExecuteString( const std::string& _string, gmVariable* _this = NULL, bool _now = true );
	bool ExecuteStringLogged( const std::string &_string );
	bool ExecuteFile( const filePath& _file, int &_threadId, gmVariable* _this = NULL );

	void ShowGMStats();

	void GetAutoCompleteList( const std::string &_string, StringVector &_completions );

	LiveUpdateKey RegisterLiveUpdate( const filePath &_file );

	// Helper functions.
	gmTableObject *GetGlobalBotsTable();
	gmTableObject *GetGlobalCommandsTable();
	gmTableObject *GetBotTable( const Client *_client );
	gmVariable ExecBotCallback( Client *_client, const char *_func );

	inline void SetScriptDebugEnabled( bool _enable )
	{
		mDebugScripts = _enable;
	}
	inline bool IsScriptDebugEnabled()
	{
		return mDebugScripts;
	}

	void EnableRemoteDebugger( bool _enable );

#if defined(ENABLE_REMOTE_DEBUGGER) && defined(GMDEBUG_SUPPORT)
	void SendDebuggerMessage(const void * a_command, int a_len);
	const void * PumpDebuggerMessage(int &a_len);
#endif

	static void LogAnyMachineErrorMessages( gmMachine *_machine );

	inline gmMachine* GetMachine()
	{
		return mScriptEngine;
	}
	static ScriptManager* GetInstance();
	static void DeleteInstance();
	static bool IsInstantiated();
protected:
	gmMachine* mScriptEngine;

	bool	 mDebugScripts;

	static void GM_CDECL ScriptSysCallback_Print( gmMachine* a_machine, const char* a_string );
	static bool GM_CDECL ScriptSysCallback_Machine( gmMachine* a_machine, gmMachineCommand a_command, const void* a_context );

	static ScriptManager* mInstance;

	virtual void InitCommands();
	void cmdScriptStats( const StringVector & args );
	void cmdScriptCollect( const StringVector & args );
	void cmdScriptRunFile( const StringVector & args );
	void cmdScriptExecute( const StringVector & args );
	void cmdDebugScriptSystem( const StringVector & args );
	void cmdScriptWriteDocs( const StringVector & args );

	void _DumpTableInfo( gmMachine *_machine, const int _flags, gmTableObject *_table, char *_buffer, int _buflen, int _lvl, std::fstream &_file );

	void CheckLiveUpdates();

	ScriptManager();
	virtual ~ScriptManager();
	ScriptManager &operator=( const ScriptManager& );
};

class ScriptResource
{
public:
	virtual bool InitScriptSource( const filePath &_path );

	LiveUpdateKey GetLiveUpdateKey() const
	{
		return mKey;
	}
	const filePath GetScriptPath() const
	{
		return mScript;
	}

	virtual gmGCRoot<gmUserObject> GetScriptObject( gmMachine *_machine ) const = 0;

	ScriptResource & operator=( const ScriptResource &_rh );
	ScriptResource();
	virtual ~ScriptResource();
private:
	filePath	 mScript;
	LiveUpdateKey mKey;
};

#endif
