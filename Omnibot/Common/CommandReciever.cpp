////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "CommandReciever.h"
#include "ScriptManager.h"

#include "gmCall.h"

//////////////////////////////////////////////////////////////////////////

ScriptCommandExecutor::ScriptCommandExecutor( gmMachine * a_machine, gmTableObject * a_commandTable )
	: mMachine( a_machine )
	, mCommandTable( a_commandTable )
{
}

bool ScriptCommandExecutor::Exec( const StringVector & args, const gmVariable &_this )
{
	const bool bPrintUsage = args.size() > 1 && args[ 1 ] == "?";
	if ( mCommandTable )
	{
		DisableGCInScope gcEn( mMachine );

		gmVariable vEntry = mCommandTable->Get( mMachine, args[ 0 ].c_str() );

		gmFunctionObject *pFn = vEntry.GetFunctionObjectSafe();
		if ( !pFn )
		{
			// See if it's new style, and in a table.
			gmTableObject *pTbl = vEntry.GetTableObjectSafe();
			if ( pTbl )
			{
				pFn = pTbl->Get( mMachine, "Func" ).GetFunctionObjectSafe();

				bool bRunImmediate = false;
				if ( !pTbl->Get( mMachine, "RunImmediate" ).GetBoolSafe( bRunImmediate, false ) )
					bRunImmediate = false;

				if ( bPrintUsage )
				{
					gmVariable vHelp = pTbl->Get( mMachine, "Help" );
					const char *pHelpString = vHelp.GetCStringSafe( 0 );
					if ( pHelpString )
					{
						EngineFuncs::ConsoleMessage( pHelpString );
					}
					else
					{
						gmTableObject *pUsageTbl = vHelp.GetTableObjectSafe();
						if ( pUsageTbl )
						{
							gmTableIterator tIt;
							gmTableNode *pNode = pUsageTbl->GetFirst( tIt );
							while ( pNode )
							{
								const char *pHelp = pNode->m_value.GetCStringSafe( 0 );
								if ( pHelp )
									EngineFuncs::ConsoleMessage( pHelp );
								pNode = pUsageTbl->GetNext( tIt );
							}
						}
					}
					return true;
				}
			}
		}
		if ( bPrintUsage )
		{
			EngineFuncs::ConsoleError( va( "No Usage Info For Command: %s", args[ 0 ].c_str() ).c_str() );
			return true;
		}

		if ( pFn )
		{
			gmCall call;
			if ( call.BeginFunction( mMachine, pFn, _this, true ) )
			{
				// Add all the params
				gmTableObject *pParamTable = mMachine->AllocTableObject();
				if ( args.size() > 1 )
				{
					for ( uint32_t i = 1; i < args.size(); ++i )
					{
						char *endPtr;
						const char *startPtr = args[ i ].c_str();
						long iNum = strtol( startPtr, &endPtr, 10 );
						double dNum;
						if ( endPtr != startPtr && !*endPtr )
						{
							pParamTable->Set( mMachine, i - 1, gmVariable( (int)iNum ) );
						}
						else if ( Utils::ConvertString( args[ i ], dNum ) )
						{
							pParamTable->Set( mMachine, i - 1, gmVariable( (float)dNum ) );
						}
						else
						{
							pParamTable->Set( mMachine, i - 1, gmVariable( mMachine->AllocStringObject( args[ i ].c_str() ) ) );
						}
					}
				}
				call.AddParamTable( pParamTable );
				call.End();
				return true;
			}
		}
		return false;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

CommandReciever::CommandMap CommandReciever::mCommandMap;
CommandReciever::RecieverList CommandReciever::mRecieverList;

CommandReciever::CommandReciever()
{
	mRecieverList.push_back( this );
	static bool bDoOnce = false;
	if ( !bDoOnce )
	{
		SetEx( "help", "Displays a list of commands", this, &CommandReciever::cmdHelp );
		bDoOnce = true;
	}
}

CommandReciever::~CommandReciever()
{
	mRecieverList.remove( this );

	for ( CommandMap::iterator it = mCommandMap.begin(); it != mCommandMap.end(); )
	{
		if ( ( *it ).second.second->IsObj( this ) )
			it = mCommandMap.erase( it );
		else
			++it;
	}
}

bool CommandReciever::DispatchCommand( const StringVector & args )
{
	// Look for default commands first.
	CommandMap::iterator cit = mCommandMap.find( args[ 0 ] );
	if ( cit != mCommandMap.end() )
	{
		( *cit->second.second )( args );
		return true;
	}

	// Look for the command in the script command table.
	gmMachine * mMachine = ScriptManager::GetInstance()->GetMachine();
	gmTableObject *pCommandsTable = ScriptManager::GetInstance()->GetGlobalCommandsTable();
	if ( pCommandsTable )
	{
		ScriptCommandExecutor cmdExec( mMachine, pCommandsTable );
		if ( cmdExec.Exec( args ) )
			return true;
	}

	// iterate all com
	for ( RecieverList::iterator it = mRecieverList.begin();
		it != mRecieverList.end();
		++it )
	{
		if ( ( *it )->UnhandledCommand( args ) )
			return true;
	}
	EngineFuncs::ConsoleError( "Unrecognized command. Use /bot help for a list of commands." );
	return false;
}

void CommandReciever::Alias( const std::string _name, const std::string _existingname )
{
	CommandMap::iterator it = mCommandMap.find( _existingname );
	if ( it != mCommandMap.end() )
	{
		mCommandMap[ _name ] = it->second;
	}
}

void CommandReciever::Remove( const std::string _name )
{
	CommandMap::iterator it = mCommandMap.find( _name );
	if ( it != mCommandMap.end() )
	{
		mCommandMap.erase( it );
	}
}

void CommandReciever::cmdHelp( const StringVector & args )
{
	EngineFuncs::ConsoleMessage( "---- Omni-bot Command Help ----" );
	CommandMap::const_iterator it = mCommandMap.begin();
	for ( ; it != mCommandMap.end(); ++it )
	{
		if ( it->first == "help" )
			continue;
		EngineFuncs::ConsoleMessage( va( "%s : %s", it->first.c_str(), it->second.first.c_str() ).c_str() );
	}

	gmMachine * mMachine = ScriptManager::GetInstance()->GetMachine();
	gmTableObject *pCommandsTable = ScriptManager::GetInstance()->GetGlobalCommandsTable();
	if ( pCommandsTable )
	{
		EngineFuncs::ConsoleMessage( "---- Script Commands ----" );
		gmTableIterator tIt;
		gmTableNode *pNode = pCommandsTable->GetFirst( tIt );
		while ( pNode )
		{
			const char *pCommandName = pNode->m_key.GetCStringSafe();
			if ( pCommandName )
			{
				gmTableObject *pTbl = pNode->m_value.GetTableObjectSafe();
				if ( pTbl )
				{
					gmVariable vHelp = pTbl->Get( mMachine, "Help" );
					const char *pHelpString = vHelp.GetCStringSafe( 0 );
					if ( pHelpString )
					{
						EngineFuncs::ConsoleMessage( va( "%s : %s", pCommandName, pHelpString ).c_str() );
					}
					else
					{
						gmTableObject *pUsageTbl = vHelp.GetTableObjectSafe();
						if ( pUsageTbl )
						{
							pHelpString = pUsageTbl->Get( 0 ).GetCStringSafe( 0 );
							if ( pHelpString )
							{
								EngineFuncs::ConsoleMessage( va( "%s : %s", pCommandName, pHelpString ).c_str() );
							}
						}
					}
				}
			}
			pNode = pCommandsTable->GetNext( tIt );
		}
		EngineFuncs::ConsoleMessage( "---- End Script Commands ----" );
	}

	EngineFuncs::ConsoleMessage( "---- End Omni-bot Command Help ----" );
}