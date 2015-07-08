////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __COMMANDRECIEVER_H__
#define __COMMANDRECIEVER_H__

#include "CommandFunctor.h"
#include "gmGCRoot.h"
class gmFunctionObject;

//////////////////////////////////////////////////////////////////////////
// Helper macros

#define PRINT_USAGE(USAGE) \
	int iNumLines = sizeof(USAGE) / sizeof(USAGE[0]); for(int i = 0; i < iNumLines; ++i)  EngineFuncs::ConsoleError(USAGE[i]);

#define CHECK_NUM_PARAMS(TOKLIST, NUM, USAGE) \
	if(TOKLIST.size() < (NUM)) { PRINT_USAGE(USAGE); return; }

#define CHECK_INT_PARAM(VAR, PARAM, USAGE) \
	int VAR; if(args.size() < ((PARAM)+1) || !Utils::ConvertString(args[PARAM], VAR)) \
{ PRINT_USAGE(USAGE); return; }

#define CHECK_FLOAT_PARAM(VAR, PARAM, USAGE) \
	float VAR; if(args.size() < ((PARAM)+1) || !Utils::ConvertString(args[PARAM], VAR)) \
{ PRINT_USAGE(USAGE); return; }

#define CHECK_STRING_PARAM(VAR, PARAM, USAGE) \
	std::string VAR; if(args.size() < ((PARAM)+1) ) { PRINT_USAGE(USAGE); return; } else { VAR = args[PARAM]; } \

#define OPTIONAL_INT_PARAM(VAR, PARAM, DEF) \
	int VAR = DEF; if(args.size() < ((PARAM)+1) || !Utils::ConvertString(args[PARAM], VAR)) \
{ }

#define OPTIONAL_FLOAT_PARAM(VAR, PARAM, DEF) \
	float VAR = DEF; if(args.size() < ((PARAM)+1) || !Utils::ConvertString(args[PARAM], VAR)) \
{ }

#define OPTIONAL_STRING_PARAM(VAR, PARAM, DEF) \
	std::string VAR = DEF; if(args.size() < ((PARAM)+1)) {} else { VAR = args[PARAM]; } \

#define CHECK_BOOL_PARAM(VAR, PARAM, USAGE) \
	bool VAR = false; if(args.size() < ((PARAM)+1)) \
{ PRINT_USAGE(USAGE); return; } else { \
	if(Utils::StringToTrue(args[PARAM])) \
	VAR = true; \
		else if(Utils::StringToFalse(args[PARAM])) \
	VAR = false; }

#define OPTIONAL_BOOL_PARAM(VAR, PARAM, DEF) \
	bool VAR = DEF; if(args.size() < ((PARAM)+1)) \
{} \
		else \
{ \
	if(Utils::StringToTrue(args[PARAM])) \
	VAR = true; \
	if(Utils::StringToFalse(args[PARAM])) \
	VAR = false; \
}

//////////////////////////////////////////////////////////////////////////

class ScriptCommandExecutor
{
public:

	bool Exec( const StringVector & args, const gmVariable &_this = gmVariable::s_null );

	ScriptCommandExecutor( gmMachine * a_machine, gmTableObject * a_commandTable );
private:
	gmMachine		* mMachine;
	gmTableObject	* mCommandTable;
};

//////////////////////////////////////////////////////////////////////////

// class: CommandReciever
//		Acts as a base class for manager or other classes so they are capable
//		of processing console commands recieved from the game.
class CommandReciever
{
public:

	static bool DispatchCommand( const StringVector & args );

	virtual bool UnhandledCommand( const StringVector & args )
	{
		return false;
	}

	CommandReciever();
protected:
	typedef std::pair<std::string, CommandFunctorPtr> CommandInfo;
	typedef std::map<std::string, CommandInfo> CommandMap;

	typedef std::list<CommandReciever*> RecieverList;

	static CommandMap		 mCommandMap;
	static RecieverList		 mRecieverList;

	void Remove( const std::string _name );

	template <typename T, typename Fn>
	void SetEx( const std::string name, const std::string info, T *srcObj, Fn func )
	{
		CommandMap::iterator it = mCommandMap.find( name );
		if ( it != mCommandMap.end() )
		{
			it->second.second = CommandFunctorPtr( new Delegate<T, Fn>( srcObj, func ) );
		}
		else
		{
			mCommandMap[ name ] = CommandInfo( info, CommandFunctorPtr( new Delegate<T, Fn>( srcObj, func ) ) );
		}
	}

	void Alias( const std::string _name, const std::string _existingname );

	void cmdHelp( const StringVector & args );

	virtual ~CommandReciever();
};

#endif
