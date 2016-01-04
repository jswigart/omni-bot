////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "Client.h"
#include "CommandReciever.h"
#include "TriggerManager.h"
#include "NameManager.h"
#include "IGameManager.h"
#include "ScriptManager.h"
#include "IGame.h"
#include "MapGoalDatabase.h"
#include "Revision.h"
#include "RenderBuffer.h"

#include "WeaponDatabase.h"

#include "mathlib/quat.h"

#include "gmBotLibrary.h"
#include "gmBot.h"
#include "gmAABB.h"
#include "gmGameEntity.h"
#include "gmMatrix3.h"

// script: BotLibrary
//		Binds various useful functionality to the scripting system.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

// function: EnableDebugWindow
//		Enables/Disables the debug render window.
//
// Parameters:
//
//		int - true/false to enable/disable
//		int - OPTIONAL - width of the debug window(default 800)
//		int - OPTIONAL - height of the debug window(default 600)
//		int - OPTIONAL - bpp of the debug window(default 32 bit)
//
// Returns:
//		none
static int GM_CDECL gmfEnableDebugWindow( gmThread *a_thread )
{
#ifdef ENABLE_DEBUG_WINDOW
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(en, 0);
	GM_INT_PARAM(width, 1, 1024);
	GM_INT_PARAM(height, 2, 768);
	GM_INT_PARAM(bpp, 3, 32);

	if(en)
		DebugWindow::Create(width, height, bpp);
	else
		DebugWindow::Destroy();
	return GM_OK;
#else
	GM_EXCEPTION_MSG( "DebugWindow Not Available." );
	return GM_EXCEPTION;
#endif
}
static int GM_CDECL gmfDumpDebugConsoleToFile( gmThread *a_thread )
{
#ifdef ENABLE_DEBUG_WINDOW
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_STRING_PARAM(filename, 0);

	DW.Console.DumpConsoleToFile(filename);
	return GM_OK;
#else
	GM_EXCEPTION_MSG( "DebugWindow Not Available." );
	return GM_EXCEPTION;
#endif
}

// function: EchoToScreen
//		This function will print a message to the screen of all players.
//
// Parameters:
//
//		float - duration of the message(in seconds)
//		std::string - The message to print
//
// Returns:
//		None
static int GM_CDECL gmfEchoMessageToScreen( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );
	GM_CHECK_FLOAT_OR_INT_PARAM( duration, 0 );
	GM_CHECK_STRING_PARAM( msg, 1 );
	//gEngineFuncs->PrintScreenText(NULL, duration, COLOR::WHITE, msg);
	RenderBuffer::AddString3d( Vector3f::ZERO, COLOR::WHITE, msg );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: EchoError
//		This function will print a message using the default error printing
//		method the game uses(Usually colored)
//
// Parameters:
//
//		std::string - The message to print.
//
// Returns:
//		None
static int GM_CDECL gmfEchoError( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_STRING_PARAM( msg, 0 );
	EngineFuncs::ConsoleError( msg );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: Log
//		Write some text to the bots log file.
//
// Parameters:
//
//		std::string - The message to log.
//
// Returns:
//		None
static int GM_CDECL gmfLog( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_STRING_PARAM( msg, 0 );
	LOG( msg );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: OnTrigger
//		This function will register a script function callback for certain triggers.
//
// Parameters:
//
//		std::string - The trigger name to use
//		function - The script function to be called when the trigger is recieved
//
// Returns:
//		None
static int gmfRegisterTriggerCallback( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );
	GM_CHECK_STRING_PARAM( triggername, 0 );
	GM_CHECK_FUNCTION_PARAM( callbackfunction, 1 );
	if ( triggername )
	{
		TriggerManager::GetInstance()->SetScriptCallback( triggername,
			gmGCRoot<gmFunctionObject>( callbackfunction, a_thread->GetMachine() ) );
		LOG( "Trigger Callback: " << callbackfunction->GetDebugName() <<
			" : For Function: " << triggername << " Set." );
	}
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: RunScript
//		This function will attempt to execute a gm script file.
//
// Parameters:
//
//		std::string - The file name of the script to attempt to execute.
//
// Returns:
//		int - true if success, false if failure.
static int GM_CDECL gmfRunScript( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_STRING_PARAM( scriptname, 0 );

	try
	{
		if ( scriptname )
		{
			gmVariable vThis = *a_thread->GetThis();

			int threadId = GM_INVALID_THREAD;
			if ( ScriptManager::GetInstance()->ExecuteFile( scriptname, threadId, &vThis ) )
			{
				a_thread->PushInt( 1 );
				return GM_OK;
			}
		}
	}
	catch ( const std::exception& e )
	{
		e;
	}
	a_thread->PushInt( 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: AddBot
//		This function will add a bot to the game, using optional supplied arguments.
//
// Parameters:
//
//		int - OPTIONAL - The team to add the bot to.
//		int - OPTIONAL - The class to add the bot to.
//		std::string - OPTIONAL - The name to give the bot.
//		std::string - OPTIONAL - The spawnpoint name to spawn bot at.
//		std::string - OPTIONAL - The model for the bot to use.
//		std::string - OPTIONAL - The skin for the bot to use.
//
// Returns:
//		None
static int GM_CDECL gmfAddBot( gmThread *a_thread )
{
	ParamsAddbot b;
	memset( &b, 0, sizeof( b ) );

	if ( a_thread->ParamType( 0 ) == GM_TABLE )
	{
		gmTableObject *pTable = a_thread->ParamTable( 0 );
		gmVariable vName = pTable->Get( a_thread->GetMachine(), "name" );
		if ( vName.IsNull() )
			vName = pTable->Get( a_thread->GetMachine(), "Name" );
		gmVariable vTeam = pTable->Get( a_thread->GetMachine(), "team" );
		if ( vTeam.IsNull() )
			vTeam = pTable->Get( a_thread->GetMachine(), "Team" );
		gmVariable vClass = pTable->Get( a_thread->GetMachine(), "class" );
		if ( vClass.IsNull() )
			vClass = pTable->Get( a_thread->GetMachine(), "Class" );
		gmVariable vSpawnPoint = pTable->Get( a_thread->GetMachine(), "vSpawnPoint" );
		if ( vSpawnPoint.IsNull() )
			vSpawnPoint = pTable->Get( a_thread->GetMachine(), "SpawnPoint" );
		gmVariable vModel = pTable->Get( a_thread->GetMachine(), "model" );
		if ( vModel.IsNull() )
			vModel = pTable->Get( a_thread->GetMachine(), "Model" );
		gmVariable vSkin = pTable->Get( a_thread->GetMachine(), "skin" );
		if ( vSkin.IsNull() )
			vSkin = pTable->Get( a_thread->GetMachine(), "Skin" );

		b.mTeam = vTeam.IsInt() ? vTeam.GetInt() : RANDOM_TEAM_IF_NO_TEAM;
		b.mClass = vClass.IsInt() ? vClass.GetInt() : RANDOM_CLASS_IF_NO_CLASS;
		Utils::StringCopy( b.mName, vName.GetCStringSafe() ? vName.GetCStringSafe() : "", sizeof( b.mName ) );
		Utils::StringCopy( b.mModel, vModel.GetCStringSafe() ? vModel.GetCStringSafe() : "", sizeof( b.mModel ) );
		Utils::StringCopy( b.mSkin, vSkin.GetCStringSafe() ? vSkin.GetCStringSafe() : "", sizeof( b.mSkin ) );
		Utils::StringCopy( b.mSpawnPointName, vSpawnPoint.GetCStringSafe() ? vSpawnPoint.GetCStringSafe() : "", sizeof( b.mSpawnPointName ) );
	}
	else
	{
		GM_INT_PARAM( iAddTeam, 0, RANDOM_TEAM );
		GM_INT_PARAM( iAddClass, 1, RANDOM_CLASS );
		GM_STRING_PARAM( pAddName, 2, "" );
		GM_STRING_PARAM( spawnPoint, 3, "" );
		GM_STRING_PARAM( model, 4, "" );
		GM_STRING_PARAM( skin, 5, "" );

		Utils::StringCopy( b.mName, pAddName, sizeof( b.mName ) );
		Utils::StringCopy( b.mModel, model, sizeof( b.mModel ) );
		Utils::StringCopy( b.mSkin, skin, sizeof( b.mSkin ) );
		Utils::StringCopy( b.mSpawnPointName, spawnPoint, sizeof( b.mSpawnPointName ) );
		b.mTeam = iAddTeam;
		b.mClass = iAddClass;
	}
	System::mInstance->mGame->AddBot( b, true );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: KickBot
//		This function will kick a bot from the game.
//
// Parameters:
//
//		std::string or int - The name of the bot to kick or gameid
//
// Returns:
//		None
static int GM_CDECL gmfKickBot( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );

	std::string s;
	switch ( a_thread->ParamType( 0 ) )
	{
		case GM_STRING:
			s = a_thread->Param( 0 ).GetCStringSafe();
			break;
		case GM_INT:
			Utils::ConvertString( a_thread->Param( 0 ).GetInt(), s );
			break;
	}

	StringVector tl;
	tl.push_back( "kickbot" );
	tl.push_back( s );
	CommandReciever::DispatchCommand( tl );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: KickBotFromTeam
//		This function will kick a bot from a team.
//
// Parameters:
//
//		int - team to kick a bot from
//
// Returns:
//		None
static int GM_CDECL gmfKickBotFromTeam( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( team, 0 );

	ClientPtr lastBotOnTeam;
	if ( System::mInstance->mGame )
	{
		for ( int i = 0; i < Constants::MAX_PLAYERS; ++i )
		{
			ClientPtr cp = System::mInstance->mGame->GetClientByIndex( i );
			if ( cp && cp->GetTeam() == team )
				lastBotOnTeam = cp;
		}
	}

	if ( lastBotOnTeam )
	{
		std::string strGameId;
		Utils::ConvertString( lastBotOnTeam->GetGameID(), strGameId );

		StringVector tl;
		tl.push_back( "kickbot" );
		tl.push_back( strGameId );
		CommandReciever::DispatchCommand( tl );
	}
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: MoveBotToAnotherTeam
//		This function will move a bot from a team to another team
//
// Parameters:
//
//		int - source team
//		int - destination team
//
// Returns:
//		None
static int GM_CDECL gmfMoveBotToAnotherTeam( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );
	GM_CHECK_INT_PARAM( sourceteam, 0 );
	GM_CHECK_INT_PARAM( destinationteam, 1 );

	ClientPtr lastBotOnTeam;
	if ( System::mInstance->mGame )
	{
		for ( int i = 0; i < Constants::MAX_PLAYERS; ++i )
		{
			ClientPtr cp = System::mInstance->mGame->GetClientByIndex( i );
			if ( cp && cp->GetTeam() == sourceteam )
				lastBotOnTeam = cp;
		}
	}

	if ( lastBotOnTeam )
	{
		lastBotOnTeam->ChangeTeam( destinationteam );
	}
	return GM_OK;
}
//////////////////////////////////////////////////////////////////////////

// function: KickAll
//		This function will kick all bots from the game.
//
// Parameters:
//
//		None
//
// Returns:
//		None
static int GM_CDECL gmfKickAll( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	StringVector tl;
	tl.push_back( "kickall" );
	CommandReciever::DispatchCommand( tl );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: ExecCommand
//		This function will execute a bot command as if it were a console command.
//
// Parameters:
//
//		std::string - The name of the bot to kick
//
// Returns:
//		None
static int GM_CDECL gmfExecCommand( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_STRING_PARAM( cmd, 0 );

	StringVector tokList;
	Utils::Tokenize( cmd, " ", tokList );
	CommandReciever::DispatchCommand( tokList );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: EntityKill
//		Kills a specified enemy. Requires cheats to be enabled
//
// Parameters:
//
//		<GameEntity> - The entity to kill
//		- OR -
//		<int> - The gameId for the entity to kill
//
// Returns:
//		true if successful, false if not
static int GM_CDECL gmfEntityKill( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	a_thread->PushInt( gEngineFuncs->Suicide( gameEnt ) ? 1 : 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetLocationName
//		Get the name of a location by its position
//
// Parameters:
//
//		<Vec3> - position to get name for
//
// Returns:
//		string if successful, null if not
static int GM_CDECL gmfGetLocationName( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	
	Vector3f pos;
	if ( a_thread->GetNumParams() == 1 )
	{
		GM_CHECK_VECTOR_PARAM( v, 1 );
		pos = Vector3f( v );
	}
	else if ( a_thread->GetNumParams() == 3 )
	{
		GM_CHECK_FLOAT_OR_INT_PARAM( x, 0 );
		GM_CHECK_FLOAT_OR_INT_PARAM( y, 1 );
		GM_CHECK_FLOAT_OR_INT_PARAM( z, 2 );
		pos.X() = x;
		pos.Y() = y;
		pos.Z() = z;
	}

	obStringBuffer str;
	if ( gEngineFuncs->GetLocationName( pos, str.mBuffer, obStringBuffer::BUFFER_LENGTH ) )
	{
		a_thread->PushNewString( str.mBuffer );
	}
	else
	{
		a_thread->PushNull();
	}
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: RegisterDefaultProfile
//		This function assign a default script to use for a specific classId.
//
// Parameters:
//
//		int - The class Id to set the default profile for. See the global CLASS table.
//		std::string - The script name to use for this classes default profile.
//
// Returns:
//		None
static int GM_CDECL gmfRegisterDefaultProfile( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );
	GM_CHECK_INT_PARAM( botclass, 0 );
	GM_CHECK_STRING_PARAM( profilename, 1 );
	if ( profilename )
	{
		NameManager::GetInstance()->SetProfileForClass( botclass, profilename );
	}
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetMapGoal
//		This function gets a reference to a map goal, by its name.
//
// Parameters:
//
//		std::string - The name of the map goal to get
//
// Returns:
//		<gm - mapgoal> - The map goal with the provided name.
//		- OR -
//		null - If no map goals matched the name.
static int GM_CDECL gmfGetMapGoal( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_STRING_PARAM( name, 0 );
	if ( name )
	{
		MapGoalPtr pGoal = System::mInstance->mGoalManager->GetGoal( name );
		if ( pGoal )
		{
			gmGCRoot<gmUserObject> pUser = pGoal->GetScriptObject( a_thread->GetMachine() );
			a_thread->PushUser( pUser );
		}
		else
		{
			a_thread->PushNull();
			LOGWARN( "Map Goal not found: " << name );
		}
	}
	return GM_OK;
}

// function: GetMapGoals
//		This function gets a table of map goals, by its name.
//		Similar to <GetMapGoal>, though this can return more than 1
//		and supports regular expressions for the goal name.
//
// Parameters:
//
//		table	- The table to put the matching goals into.
//		team	- The team the goal should be for. 0 means any team.
//		exp		- The expression to use to match the goal names. 0 to match any.
//
// Returns:
//		none
static int GM_CDECL gmfGetMapGoals( gmThread *a_thread )
{
	GM_CHECK_TABLE_PARAM( table, 0 );
	GM_INT_PARAM( iTeam, 1, 0 );
	GM_STRING_PARAM( pExpr, 2, 0 );
	GM_TABLE_PARAM( params, 3, 0 );

	GoalManager::Query qry;
	qry.DefaultGlobalQuery();
	qry.Expression( pExpr ).Team( iTeam );

	// parse optional parameters
	if ( params )
	{
		qry.FromTable( a_thread->GetMachine(), params );
	}

	if ( qry.GetError() != GoalManager::Query::QueryOk )
	{
		GM_EXCEPTION_MSG( qry.QueryErrorString() );
		return GM_EXCEPTION;
	}

	System::mInstance->mGoalManager->GetGoals( qry );
	if ( qry.GetError() != GoalManager::Query::QueryOk )
	{
		GM_EXCEPTION_MSG( qry.QueryErrorString() );
		return GM_EXCEPTION;
	}

	if ( !qry.mList.empty() )
	{
		gmMachine *pMachine = a_thread->GetMachine();
		DisableGCInScope gcEn( pMachine );

		for ( uint32_t i = 0; i < qry.mList.size(); ++i )
		{
			gmUserObject *pUser = qry.mList[ i ]->GetScriptObject( a_thread->GetMachine() );
			table->Set( pMachine, i, gmVariable( pUser ) );
		}
	}
	return GM_OK;
}

// function: SetMapGoalProperties
//		This functions is for setting properties on 1 or more map goals.
//
// Parameters:
//
//		std::string	- Expression of goals to apply properties to.
//		table	- Property table.
//
// Returns:
//		none
static int GM_CDECL gmfSetMapGoalProperties( gmThread *a_thread )
{
	GM_CHECK_STRING_PARAM( expr, 0 );
	GM_CHECK_TABLE_PARAM( props, 1 );

	GoalManager::Query qry;
	qry.NoFilters();
	qry.Expression( expr );
	System::mInstance->mGoalManager->GetGoals( qry );
	for ( uint32_t i = 0; i < qry.mList.size(); ++i )
	{
		qry.mList[ i ]->FromScriptTable( a_thread->GetMachine(), props, false );
	}
	return GM_OK;
}

// function: SetAvailableMapGoals
//		This function enables/disables map goals
//
// Parameters:
//
//		int		- The team the goal should be for. 0 means any team. See global team table.
//		int		- True to enable, false to disable.
//		std::string	- OPTIONAL - The expression to use to match the goal names.
//
// Returns:
//		none
static int GM_CDECL gmfSetAvailableMapGoals( gmThread *a_thread )
{
	GM_CHECK_INT_PARAM( team, 0 );
	GM_CHECK_INT_PARAM( enable, 1 );
	bool bFoundGoal = false;

	const char *pExpression = 0;
	if ( a_thread->GetNumParams() > 2 )
	{
		if ( a_thread->ParamType( 2 ) == GM_STRING )
		{
			pExpression = a_thread->ParamString( 2 );
		}
		else
		{
			EngineFuncs::ConsoleMessage( "SetAvailableMapGoals: Parameter 3 must be a std::string" );
			LOGWARN( "SetAvailableMapGoals: Parameter 3 must be a std::string" );
			a_thread->PushInt( 0 );
			return GM_OK;
		}
	}

	GoalManager::Query qry;
	qry.Expression( pExpression );
	qry.NoFilters();
	System::mInstance->mGoalManager->GetGoals( qry );
	for ( uint32_t i = 0; i < qry.mList.size(); ++i )
	{
		if ( !bFoundGoal )
			bFoundGoal = true;

		if ( team == 0 )
		{
			for ( int t = 1; t <= 4; ++t )
				qry.mList[ i ]->SetAvailable( t, enable != 0 );
		}
		else
		{
			qry.mList[ i ]->SetAvailable( team, enable != 0 );
		}
	}

	if ( bFoundGoal )
		a_thread->PushInt( 1 );
	else
	{
		EngineFuncs::ConsoleMessage( va( "SetAvailableMapGoals: goal query for %s has no results!", pExpression ).c_str() );
		LOGWARN( "SetAvailableMapGoals: goal query for " << pExpression << " has no results!" );
		a_thread->PushInt( 0 );
	}

	return GM_OK;
}

// function: SetGoalPriority
//		This function sets the bias for a selection of goals
//
// Parameters:
//
//		std::string	- The expression to use to match the goal names.
//		int		- Team Id to set priority for.
//		int		- Class Id to set priority for.
//		float	- The priority to set
//
// Returns:
//		none
static int GM_CDECL gmfSetGoalPriorityForTeamClass( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );
	GM_CHECK_STRING_PARAM( exp, 0 );
	GM_CHECK_FLOAT_OR_INT_PARAM( priority, 1 );
	GM_INT_PARAM( teamId, 2, 0 );
	GM_INT_PARAM( classId, 3, 0 );
	GM_INT_PARAM( persis, 4, 0 );

	GoalManager::Query qry;
	qry.Expression( exp );
	qry.NoFilters();
	System::mInstance->mGoalManager->GetGoals( qry );
	for ( uint32_t i = 0; i < qry.mList.size(); ++i )
	{
		qry.mList[ i ]->SetPriorityForClass( teamId, classId, priority );
	}

	if ( persis )
	{
		MapGoal::SetPersistentPriorityForClass( exp, teamId, classId, priority );
	}
	return GM_OK;
}

// function: SetGoalGroup
//		This function sets the group for a selection of goals
//
// Parameters:
//
//		std::string	- The group expression to use to match the goal names. -OR- indexed std::string table of goal expressions
//		std::string - The group name to use.
//
// Returns:
//		none
static int GM_CDECL gmfSetGoalGroup( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );
	GM_CHECK_STRING_PARAM( group, 1 );

	if ( a_thread->ParamType( 0 ) == GM_TABLE )
	{
		GM_TABLE_PARAM( tbl, 0, 0 );
		if ( tbl )
		{
			gmTableIterator tIt;
			gmTableNode *pNode = tbl->GetFirst( tIt );
			while ( pNode )
			{
				const char *pGoalName = pNode->m_value.GetCStringSafe( 0 );
				MapGoalPtr mg = pGoalName ? System::mInstance->mGoalManager->GetGoal( pGoalName ) : MapGoalPtr();
				if ( mg )
				{
					mg->SetGroupName( group );
				}
				pNode = tbl->GetNext( tIt );
			}
		}
	}
	else if ( a_thread->ParamType( 0 ) == GM_STRING )
	{
		GM_STRING_PARAM( exp, 0, 0 );
		GoalManager::Query qry;
		qry.Expression( exp );
		qry.NoFilters();
		System::mInstance->mGoalManager->GetGoals( qry );
		for ( uint32_t i = 0; i < qry.mList.size(); ++i )
		{
			qry.mList[ i ]->SetGroupName( group );
		}
	}
	else
	{
		GM_EXCEPTION_MSG( "expected param 0 as table or std::string" );
		return GM_EXCEPTION;
	}
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetGameEntityFromId
//		This function gets a <GameEntity> from a numeric game Id.
//
// Parameters:
//
//		int - The game Id to get the entity for.
//
// Returns:
//		<GameEntity> - The <GameEntity>, if it exists.
//		- OR -
//		null - If no <GameEntity> exists for that Id.
static int gmfGetGameEntityFromId( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( Id, 0 );

	GameEntity ent = gEngineFuncs->EntityFromID( Id );

	if ( ent.IsValid() )
	{
		gmVariable v;
		v.SetEntity( ent.AsInt() );
		a_thread->Push( v );
	}
	else
		a_thread->PushNull();

	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetGameIdFromEntity
//		This function gets a numeric game Id from a <GameEntity>.
//
// Parameters:
//
//		<GameEntity> - The <GameEntity> of a game entity.
//
// Returns:
//		int - Numeric game Id for the provided entity.
//		- OR -
//		null - If the entity is invalid
static int gmfGetGameIdFromEntity( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	GameId Id = gEngineFuncs->IDFromEntity( gameEnt );
	if ( Id != -1 )
		a_thread->PushInt( Id );
	else
		a_thread->PushNull();

	return GM_OK;
}
//////////////////////////////////////////////////////////////////////////

// function: TraceLine
//		This function does a custom collision ray-test. Useful for line of sight tests.
//
// Parameters:
//
//		Vector3 - 3d Start position of the trace.
//		Vector3 - 3d End position of the trace.
//		int - AABB to use for trace line.
//		int - Collision masks to use for the <TraceLine>. See the global TRACE table for flags. default MASK_SHOT
//		int - GameId of an entity to ignore in the trace. Usually the source entity of the trace. default -1
//		int	- true/false to use PVS or not. Results in faster check, but doesn't return info about the collision. default false
//
// Returns:
//		table - traceline result table, with the following values. fraction, startsolid, entity, normal, end.
//				first 2 are guaranteed, others are conditional on if the ray hit something
static int gmfTraceLine( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );
	GM_CHECK_VECTOR_PARAM( sv, 0 );
	GM_CHECK_VECTOR_PARAM( ev, 1 );
	GM_GMBIND_PARAM( AABB*, gmAABB, bbox, 2, NULL );
	GM_INT_PARAM( iMask, 3, TR_MASK_SHOT );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 4 );
	//GM_INT_PARAM(iUser, 4, -1);
	
	GM_INT_PARAM( iUsePVS, 5, false );

	const int iUser = gameEnt.IsValid() ? gEngineFuncs->IDFromEntity( gameEnt ) : -1;

	obTraceResult tr;
	EngineFuncs::TraceLine( tr, Vector3f( sv.x, sv.y, sv.z ), Vector3f( ev.x, ev.y, ev.z ), bbox,
		iMask, iUser, iUsePVS != 0 );

	gmMachine *pMachine = a_thread->GetMachine();
	DisableGCInScope gcEn( pMachine );

	gmTableObject *pTable = pMachine->AllocTableObject();
	pTable->Set( pMachine, "fraction", gmVariable( tr.mFraction ) );
	pTable->Set( pMachine, "startsolid", gmVariable( tr.mStartSolid ? 1 : 0 ) );
	if ( tr.mFraction < 1.0 )
	{
		if ( tr.mHitEntity.IsValid() )
		{
			gmVariable v;
			v.SetEntity( tr.mHitEntity.AsInt() );
			pTable->Set( pMachine, "entity", v );
		}

		pTable->Set( pMachine, "normal", gmVariable( tr.mNormal[ 0 ], tr.mNormal[ 1 ], tr.mNormal[ 2 ] ) );
		pTable->Set( pMachine, "end", gmVariable( tr.mEndpos[ 0 ], tr.mEndpos[ 1 ], tr.mEndpos[ 2 ] ) );
	}

	a_thread->PushTable( pTable );
	return GM_OK;
}
//////////////////////////////////////////////////////////////////////////

// function: GroundPoint
//		This function returns a point on the ground directly below the provided point
//
// Parameters:
//
//		Vector3 - 3d position of the trace.
//		int - Collision masks to use for the <TraceLine>. See the global TRACE table for flags. default MASK_SHOT
//
// Returns:
//		table - traceline result table, with the following values. fraction, startsolid, entity, normal, end.
//				first 2 are guaranteed, others are conditional on if the ray hit something
static int gmfGroundPoint( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_VECTOR_PARAM( pt, 0 );
	GM_FLOAT_OR_INT_PARAM( offset, 1, 0.f )
		GM_INT_PARAM( iMask, 1, TR_MASK_SHOT );

	Vector3f vPt( pt.x, pt.y, pt.z );

	obTraceResult tr;
	EngineFuncs::TraceLine( tr, vPt, vPt + Vector3f( 0, 0, -1024.f ), NULL, iMask, -1, false );
	if ( tr.mFraction < 1.f )
		vPt = tr.mEndpos;

	vPt.Z() -= offset;

	a_thread->PushVector( vPt );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: DrawDebugLine
//		This function will draw a colored line in the game world. Useful for debugging.
//
// Parameters:
//
//		Vector3 - 3d Start position of the line.
//		Vector3 - 3d End position of the line.
//		int - Color of the line.
//		float - duration of the line
//
// Returns:
//		None
static int gmfDrawDebugLine( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 4 );
	GM_CHECK_VECTOR_PARAM( sv, 0 );
	GM_CHECK_VECTOR_PARAM( ev, 1 );
	GM_CHECK_INT_PARAM( color, 2 );
	GM_CHECK_FLOAT_OR_INT_PARAM( duration, 3 );
	RenderBuffer::AddLine( Vector3f( sv.x, sv.y, sv.z ), Vector3f( ev.x, ev.y, ev.z ), obColor( color ), duration );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: DrawDebugArrow
//		This function will draw a colored line in the game world. Useful for debugging.
//
// Parameters:
//
//		Vector3 - 3d Start position of the line.
//		Vector3 - 3d End position of the line.
//		int - Color of the line.
//		float - duration of the line
//
// Returns:
//		None
static int gmfDrawDebugArrow( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 4 );
	GM_CHECK_VECTOR_PARAM( sv, 0 );
	GM_CHECK_VECTOR_PARAM( ev, 1 );
	GM_CHECK_INT_PARAM( color, 2 );
	GM_CHECK_FLOAT_OR_INT_PARAM( duration, 3 );
	RenderBuffer::AddArrow( Vector3f( sv.x, sv.y, sv.z ), Vector3f( ev.x, ev.y, ev.z ), obColor( color ) );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: DrawDebugAABB
//		This function will draw a colored AABB in the game world. Useful for debugging.
//
// Parameters:
//
//		AABB - AABB to draw.
//		int - Color of the AABB.
//		float - duration of the line
//
// Returns:
//		None
static int gmfDrawDebugAABB( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 3 );
	if ( a_thread->ParamType( 0 ) != gmAABB::GetType() )
	{
		GM_EXCEPTION_MSG( "expecting param 0 as user type %d", gmAABB::GetType() );
		return GM_EXCEPTION;
	}
	GM_CHECK_GMBIND_PARAM( AABB*, gmAABB, aabb, 0 );
	GM_CHECK_INT_PARAM( color, 1 );
	GM_CHECK_FLOAT_OR_INT_PARAM( duration, 2 );

	RenderBuffer::AddAABB( *aabb, obColor( color ) );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: DrawDebugRadius
//		This function will draw a colored AABB in the game world. Useful for debugging.
//
// Parameters:
//
//		AABB - AABB to draw.
//		int - Color of the AABB.
//		float - duration of the line
//
// Returns:
//		None
static int gmfDrawDebugRadius( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 4 );
	GM_CHECK_VECTOR_PARAM( v, 0 );
	GM_CHECK_FLOAT_OR_INT_PARAM( rad, 1 );
	GM_CHECK_INT_PARAM( color, 2 );
	GM_CHECK_FLOAT_OR_INT_PARAM( duration, 3 );
	RenderBuffer::AddCircle( Vector3f( v.x, v.y, v.z ), rad, obColor( color ) );
	return GM_OK;
}

// function: DrawDebugText3d
//		This function will Draw Text to a 3d location.
//
// Parameters:
//
//		float - duration of the message(in seconds)
//		std::string - The message to print
//
// Returns:
//		None
static int GM_CDECL gmfDrawDebugText3d( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 4 );
	GM_CHECK_VECTOR_PARAM( v, 0 );
	GM_CHECK_STRING_PARAM( msg, 1 );
	GM_CHECK_INT_PARAM( color, 2 );
	GM_CHECK_FLOAT_OR_INT_PARAM( duration, 3 );
	GM_FLOAT_OR_INT_PARAM( radius, 4, 1024.f );

	if ( radius != Utils::FloatMax )
	{
		Vector3f vLocalPos;
		if ( Utils::GetLocalPosition( vLocalPos ) && Length( vLocalPos, Vector3f( v ) ) >= radius )
		{
			return GM_OK;
		}
	}

	RenderBuffer::AddString3d( Vector3f( v ), obColor( color ), msg );
	return GM_OK;
}

static int GM_CDECL gmfTransformAndDrawLineList( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 4 );
	GM_CHECK_TABLE_PARAM( veclist, 0 );
	GM_CHECK_INT_PARAM( color, 1 );
	GM_CHECK_FLOAT_OR_INT_PARAM( duration, 2 );
	GM_CHECK_VECTOR_PARAM( position, 3 );
	GM_CHECK_VECTOR_PARAM( euler, 4 );

	/*Matrix3f m;
	m.FromEulerAnglesZXY(euler.x,euler.y,euler.z);*/

	Quaternionf q;
	q.FromAxisAngle( Vector3f::UNIT_Z, -euler.x );

	const int PointCount = veclist->Count();
	if ( PointCount > 2 )
	{
		Vector3f *vecs = (Vector3f*)StackAlloc( sizeof( Vec3 )*PointCount );

		int CurrentPoint = 0;
		gmTableIterator tIt;
		gmTableNode *pNode = veclist->GetFirst( tIt );
		while ( pNode )
		{
			Vec3 v;
			if ( !pNode->m_value.GetVector( v ) )
			{
				GM_EXCEPTION_MSG( "Expected table of Vec3" );
				return GM_EXCEPTION;
			}

			vecs[ CurrentPoint++ ] = Vector3f( position ) + q.Rotate( Vector3f( v ) );
			pNode = veclist->GetNext( tIt );
		}

		// draw the line list.
		for ( int i = 0; i < PointCount; i += 2 )
		{
			RenderBuffer::AddLine(
				vecs[ i ],
				vecs[ i + 1 ],
				obColor( color ),
				duration );
		}
	}
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: DrawTrajectory
//		This function will draw a colored line in the game world. Useful for debugging.
//
// Parameters:
//
//		Vector3 - 3d Start position of the trajectory.
//		Vector3 - 3d velocity of the trajectory.
//		float or int - interval to sample trajectory for drawing
//		float or int - max duration of the trajectory to draw
//		int - Color of the line.
//		float - duration of the line
//
// Returns:
//		None
static int gmfDrawTrajectory( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 3 );
	GM_CHECK_TABLE_PARAM( tbl, 0 );
	GM_CHECK_INT_PARAM( color, 1 );
	GM_CHECK_FLOAT_OR_INT_PARAM( duration, 2 );

	Trajectory::TrajectorySim traj;
	const int Res = traj.FromTable( a_thread, tbl );
	if ( Res == GM_OK )
	{
		traj.Render( obColor( color ), duration );
		a_thread->PushVector( traj.mStopPos );
	}
	return Res;
}

//////////////////////////////////////////////////////////////////////////

// function: GetMapName
//		This function will get the map name for the currently running map.
//
// Parameters:
//
//		None
//
// Returns:
//		std::string - The currently running map name.
//		- OR -
//		null - If no map is loaded, or there was an error.
static int GM_CDECL gmfGetMapName( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );

	const char *pMapName = gEngineFuncs->GetMapName();
	if ( pMapName )
	{
		a_thread->PushString( a_thread->GetMachine()->AllocStringObject( pMapName ) );
	}
	else
	{
		a_thread->PushNull();
	}

	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetMapExtents
//		This function will get the map extents for the currently running map.
//
// Parameters:
//
//		None
//
// Returns:
//		<gmAABB> - Axis aligned box for the map extents.
static int GM_CDECL gmfGetMapExtents( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	GM_GMBIND_PARAM( AABB*, gmAABB, aabb, 0, NULL );
	AABB mapaabb;
	gEngineFuncs->GetMapExtents( mapaabb );
	if ( aabb )
		*aabb = mapaabb;
	else
		gmAABB::PushObject( a_thread, mapaabb );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetPointContents
//		This function will get the contents of a 3d point.
//
// Parameters:
//
//		<Vector3> -
//
// Returns:
//		int - The point content flags for this position.
static int GM_CDECL gmfGetPointContents( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_VECTOR_PARAM( v, 0 );
	a_thread->PushInt( gEngineFuncs->GetPointContents( Vector3f( v.x, v.y, v.z ) ) );
	return GM_OK;
}

////////////////////////////////////////////////////////////////////////////

// function: GetTime
//		This function will get the time from the bot. This time isn't necessarily match time,
//		or real time, so it should only be used on relative time comparisons.
//
// Parameters:
//
//		None
//
// Returns:
//		int - The current time, in seconds
static int GM_CDECL gmfGetTime( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	a_thread->PushFloat( IGame::GetTimeSecs() );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// function: GetEntName
//		This function gets the name of the entity. Only useful for getting the
//		player names of certain clients. Undefined if used on non clients->
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//
// Returns:
//		std::string - The name of this entity
//		- OR -
//		null - If there was an error or the parameter was invalid
static int gmfGetEntityName( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );

	// See if we can get a proper gameentity
	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	obStringBuffer nameBuffer;
	if ( SUCCESS( gEngineFuncs->GetEntityName( gameEnt, nameBuffer ) ) )
		a_thread->PushNewString( nameBuffer.mBuffer );
	else
		a_thread->PushNull();
	return GM_OK;
}

// function: GetEntFacing
//		This function gets the facing of this entity.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//
// Returns:
//		Vector3 - 3d facing of this entity
//		- OR -
//		null - If there was an error or the parameter was invalid
static int gmfGetEntityFacing( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );

	// See if we can get a proper gameentity
	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	Vector3f v = Vector3f::ZERO;
	if ( gameEnt.IsValid() && EngineFuncs::EntityOrientation( gameEnt, v, NULL, NULL ) )
		a_thread->PushVector( v.X(), v.Y(), v.Z() );
	else
		a_thread->PushNull();
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetEntPosition
//		This function gets the position of this entity.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//
// Returns:
//		Vector3 - 3d position of this entity
//		- OR -
//		null - If there was an error or the parameter was invalid
static int gmfGetEntityPosition( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	Vector3f v = Vector3f::ZERO;
	if ( gameEnt.IsValid() && EngineFuncs::EntityPosition( gameEnt, v ) )
		a_thread->PushVector( v.X(), v.Y(), v.Z() );
	else
		a_thread->PushNull();
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetEntCenterBounds
//		This function gets the center of an entity world bounding box
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//
// Returns:
//		Vector3 - 3d position of this entity
//		- OR -
//		null - If there was an error or the parameter was invalid
static int gmfGetEntCenterBounds( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	Box3f obb;
	if ( EngineFuncs::EntityWorldOBB( gameEnt, obb ) )
	{
		a_thread->PushVector( obb.Center.X(), obb.Center.Y(), obb.Center.Z() );
	}
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetEntBonePosition
//		This function gets the position of a bone on this entity.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//		<int> - Bone to look for, See <SkeletonBone> enum
//
// Returns:
//		Vector3 - 3d position of this entity
//		- OR -
//		null - If there was an error or the parameter was invalid
static int gmfGetEntityBonePosition( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	GM_CHECK_INT_PARAM( boneid, 1 );

	Vector3f v = Vector3f::ZERO;
	if ( gameEnt.IsValid() && EngineFuncs::EntityBonePosition( gameEnt, boneid, v ) )
		a_thread->PushVector( v.X(), v.Y(), v.Z() );
	else
		a_thread->PushNull();
	return GM_OK;
}
// function: GetEntEyePosition
//		This function gets the eye position of this entity.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//
// Returns:
//		Vector3 - 3d eye position of this entity
//		- OR -
//		null - If there was an error or the parameter was invalid
static int gmfGetEntEyePosition( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	Vector3f v = Vector3f::ZERO;
	if ( gameEnt.IsValid() && EngineFuncs::EntityEyePosition( gameEnt, v ) )
		a_thread->PushVector( v.X(), v.Y(), v.Z() );
	else
		a_thread->PushNull();
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetEntVelocity
//		This function gets the velocity of this entity.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//
// Returns:
//		Vector3 - 3d velocity of this entity
//		- OR -
//		null - If there was an error or the parameter was invalid
static int gmfGetEntityVelocity( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	Vector3f v = Vector3f::ZERO;
	if ( gameEnt.IsValid() && EngineFuncs::EntityVelocity( gameEnt, v ) )
		a_thread->PushVector( v.X(), v.Y(), v.Z() );
	else
		a_thread->PushNull();
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetEntRotationMatrix
//		This function gets the full matrix transform for this entity.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//
// Returns:
//		Matrix3 - transform matrix of this entity
//		- OR -
//		null - If there was an error or the parameter was invalid
static int gmfGetEntRotationMatrix( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	GM_GMBIND_PARAM( gmMat3Type*, gmMatrix3, mat, 1, NULL );

	Vector3f vForward, vRight, vUp;
	if ( gameEnt.IsValid() && EngineFuncs::EntityOrientation( gameEnt, vForward, vRight, vUp ) )
	{
		if ( mat )
		{
			*mat = Matrix3f( vRight, vForward, vUp, true );
			a_thread->PushInt( 1 );
		}
		else
			gmMatrix3::PushObject( a_thread, Matrix3f( vRight, vForward, vUp, true ) );
	}
	else
		a_thread->PushNull();
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetEntFlags
//		This function gets the flags of this entity.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//		<int> - The flag to check for
//		.. - Any number of additional flags to check for
//
// Returns:
//		int - true if entity has ANY flag passed
static int gmfGetEntityFlags( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	EntityInfo entInfo;
	if ( IGame::GetEntityInfo( gameEnt, entInfo ) )
	{
		for ( int i = 1; i < a_thread->GetNumParams(); ++i )
		{
			GM_CHECK_INT_PARAM( flag, i );
			if ( entInfo.mFlags.CheckFlag( flag ) )
			{
				a_thread->PushInt( 1 );
				return GM_OK;
			}
		}
	}
	a_thread->PushInt( 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////
// function: GetEntPowerups
//		This function gets the powerups on this entity.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//
// Returns:
//		int - true if entity has ALL powerups passed
static int gmfGetEntityPowerups( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	//bool bSuccess = false;
	EntityInfo entInfo;
	if ( IGame::GetEntityInfo( gameEnt, entInfo ) )
	{
		//bSuccess = true;
		for ( int i = 1; i < a_thread->GetNumParams(); ++i )
		{
			GM_CHECK_INT_PARAM( flag, i );
			if ( entInfo.mPowerUps.CheckFlag( flag ) )
			{
				a_thread->PushInt( 1 );
				return GM_OK;
			}
		}
	}
	a_thread->PushInt( 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetEntWorldAABB
//		This function gets the world AABB for this entity.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//		<AABB> - OPTIONAL - If provided, will recieve the entity aabb rather than the return value(saves memory allocations)
//
// Returns:
//		<gm - aabb> - The axis aligned bounding box for this entity.
//		- OR -
//		null - if there was an error or the parameter was invalid
//
// Returns:
//		<gm - aabb> - AABB of this entity.
//		- OR -
//		null - if there was an error or the parameter was invalid
static int gmfGetEntityLocalAABB( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	GM_GMBIND_PARAM( AABB*, gmAABB, aabb, 1, NULL );

	AABB worldaabb;
	if ( gameEnt.IsValid() && EngineFuncs::EntityLocalAABB( gameEnt, worldaabb ) )
	{
		if ( aabb )
		{
			*aabb = worldaabb;
			a_thread->PushInt( 1 );
		}
		else
			gmAABB::PushObject( a_thread, worldaabb );
	}
	else
		a_thread->PushNull();
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetEntOwner
//		This function gets the current owner of this goal. Usually for
//		flags or carryable goals it will get who is carrying it.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//
// Returns:
//		<GameEntity> of the owner of this entity
//		- OR -
//		null - If no owner.
static int gmfGetEntityOwner( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	GameEntity owner = gEngineFuncs->GetEntityOwner( gameEnt );
	if ( owner.IsValid() )
		a_thread->PushEntity( owner.AsInt() );
	else
		a_thread->PushNull();

	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetEntTeam
//		This function gets the current team of this entity.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//
// Returns:
//		int - team of the owner of this entity
//		- OR -
//		null - If no owner.
static int gmfGetEntityTeam( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	int iTeam = gameEnt.IsValid() ? gEngineFuncs->GetEntityTeam( gameEnt ) : 0;
	if ( iTeam != 0 )
		a_thread->PushInt( iTeam );
	else
		a_thread->PushNull();
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetEntInfo
//		This function gets the current class of this entity.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//
// Returns:
//		int - team of the owner of this entity
//		- OR -
//		null - If no owner.
static int gmfGetEntityInfo( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	GM_TABLE_PARAM( infoTbl, 1, 0 );

	EntityInfo entInfo;
	if ( IGame::GetEntityInfo( gameEnt, entInfo ) )
	{
		gmMachine *pMachine = a_thread->GetMachine();

		DisableGCInScope gcEn( pMachine );
		if ( infoTbl == NULL )
			infoTbl = pMachine->AllocTableObject();

		infoTbl->Set( pMachine, "Group", gmVariable( entInfo.mGroup ) );
		infoTbl->Set( pMachine, "ClassId", gmVariable( entInfo.mClassId ) );
		infoTbl->Set( pMachine, "Energy", gmVariable( entInfo.mEnergy.mNum ) );
		infoTbl->Set( pMachine, "EnergyMax", gmVariable( entInfo.mEnergy.mMax ) );
		infoTbl->Set( pMachine, "Health", gmVariable( entInfo.mHealth.mNum ) );
		infoTbl->Set( pMachine, "MaxHealth", gmVariable( entInfo.mHealth.mMax ) );
		infoTbl->Set( pMachine, "Armor", gmVariable( entInfo.mArmor.mNum ) );
		infoTbl->Set( pMachine, "MaxArmor", gmVariable( entInfo.mArmor.mMax ) );
		
		a_thread->PushTable( infoTbl );
	}
	else
		a_thread->PushNull();
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetEntCategory
//		This function gets the current category of this entity.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//
// Returns:
//		int - category of the entity, as a bitmask.
//		- OR -
//		null - If no owner.
static int gmfGetEntCategory( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	EntityInfo entInfo;
	if ( IGame::GetEntityInfo( gameEnt, entInfo ) )
	{
		for ( int i = 1; i < a_thread->GetNumParams(); ++i )
		{
			GM_CHECK_INT_PARAM( flag, i );
			if ( entInfo.mCategory.CheckFlag( flag ) )
			{
				a_thread->PushInt( 1 );
				return GM_OK;
			}
		}
	}
	a_thread->PushInt( 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetEntityLocalSpace
//		This function gets the local space position of a vector.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//		<Vector3> - The 3d vector to convert to local space.
//
// Returns:
//		<Vector3> - The local space coordinate
static int gmfGetEntityToLocalSpace( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	GM_CHECK_VECTOR_PARAM( v, 1 );

	Vector3f vl;
	if ( Utils::ToLocalSpace( gameEnt, Vector3f( v.x, v.y, v.z ), vl ) )
		a_thread->PushVector( vl.X(), vl.Y(), vl.Z() );
	else
		a_thread->PushNull();
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetEntityWorldSpace
//		This function gets the world space position of a vector.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//		<Vector3> - The 3d vector to convert to world space.
//
// Returns:
//		<Vector3> - The world space coordinate
static int gmfGetEntityToWorldSpace( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	GM_CHECK_VECTOR_PARAM( v, 1 );

	Vector3f vw;
	if ( Utils::ToWorldSpace( gameEnt, Vector3f( v.x, v.y, v.z ), vw ) )
		a_thread->PushVector( vw.X(), vw.Y(), vw.Z() );
	else
		a_thread->PushNull();
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: IterateEntities
//		This function iterates all known entities.
//
// Parameters:
//
//		<GameEntity>
//		- OR -
//		<int> - The gameId to start from
//
// Returns:
//		<GameEntity> - The next entity
static int gmfIterateEntities( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );

	GameEntity ent;
	GM_GAMEENTITY_FROM_PARAM( ent, 0, GameEntity() );
	
	IGame::EntityIterator it( ent );
	while ( IGame::IterateEntity( it ) )
	{
		a_thread->PushEntity( it.GetEnt().mEntity.AsInt() );
		return GM_OK;
	}
	
	a_thread->PushNull();
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetEntityInSphere
//		This function searches for an entity of a certain type.
//
// Parameters:
//
//		<Vector3> - The position to search at
//		<float> - The radius to search within
//		<int> - Class Id to search for
//		<GameEntity>
//		- OR -
//		<int> - The gameId to start from
//
// Returns:
//		<GameEntity> - The next entity within the radius of the class requested
static int gmfGetEntityInSphere( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 4 );
	GM_CHECK_VECTOR_PARAM( v, 0 );
	GM_CHECK_FLOAT_OR_INT_PARAM( radius, 1 );
	GM_CHECK_INT_PARAM( groupId, 2 );
	GM_CHECK_INT_PARAM( classId, 3 );

	GameEntity lastEntity;
	GM_GAMEENTITY_FROM_PARAM( lastEntity, 3, GameEntity() );

	const Vector3f searchPos( v.x, v.y, v.z );

	GameEntity foundEntity;

	IGame::EntityIterator it( lastEntity );
	while ( IGame::IterateEntity( it ) )
	{
		if ( it.GetEnt().mEntInfo.mGroup == groupId && it.GetEnt().mEntInfo.mClassId == classId )
		{
			Vector3f entPos;
			if ( EngineFuncs::EntityPosition( it.GetEnt().mEntity, entPos ) )
			{
				if ( SquaredLength( searchPos, entPos ) < Mathf::Sqr( radius ) )
				{
					foundEntity = it.GetEnt().mEntity;
					break;
				}
			}
		}
	}

	if ( foundEntity.IsValid() )
	{
		gmVariable out;
		out.SetEntity( foundEntity.AsInt() );
		a_thread->Push( out );
	}
	else
	{
		a_thread->PushNull();
	}
	return GM_OK;
}

static int gmfGetEntityByName( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_STRING_PARAM( entname, 0 );

	GameEntity ent = gEngineFuncs->EntityByName( entname );
	if ( ent.IsValid() )
	{
		gmVariable v;
		v.SetEntity( ent.AsInt() );
		a_thread->Push( v );
	}
	else
	{
		a_thread->PushNull();
	}
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: EntityIsValid
//		Checks if a given entity is valid
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//
// Returns:
//		bool
static int gmfEntityIsValid( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	bool valid = gameEnt.IsValid();

	if ( valid && a_thread->ParamType( 0 ) == GM_ENTITY &&
		gEngineFuncs->IDFromEntity( gameEnt ) == -1 )
		valid = false;

	a_thread->PushInt( valid ? 1 : 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: DistanceBetween
//		Gets the distance between 2 entities.
//
// Parameters:
//		<Vector3> - The 3d vector to convert to calculate distance to.
//		- OR -
//		<GameEntity> - The first entity to use
//		- OR -
//		<int> - The gameId for the first entity to use
//		<Vector3> - The 3d vector to convert to calculate distance to.
//		- OR -
//		<GameEntity> - The second entity to use
//		- OR -
//		<int> - The gameId for the second entity to use
//
// Returns:
//		<float> - Distance between 2 entities
//		- OR -
//		null - If one or both entities were invalid.
static int gmfDistanceBetween( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );
	Vector3f vPosition1, vPosition2;

	if ( a_thread->ParamType( 0 ) == GM_VEC3 )
	{
		GM_CHECK_VECTOR_PARAM( v, 0 );
		vPosition1 = Vector3f( v.x, v.y, v.z );
	}
	else
	{
		GameEntity gameEnt;
		GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
		if ( !EngineFuncs::EntityPosition( gameEnt, vPosition1 ) )
		{
			GM_EXCEPTION_MSG( "Expected Vector3/GameEntity/GameId for param %d got %s!", 0,
				a_thread->GetMachine()->GetTypeName( a_thread->ParamType( 0 ) ) );
			return GM_EXCEPTION;
		}
	}

	if ( a_thread->ParamType( 1 ) == GM_VEC3 )
	{
		GM_CHECK_VECTOR_PARAM( v, 1 );
		vPosition2 = Vector3f( v.x, v.y, v.z );
	}
	else
	{
		GameEntity gameEnt;
		GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 1 );
		if ( !EngineFuncs::EntityPosition( gameEnt, vPosition2 ) )
		{
			GM_EXCEPTION_MSG( "Expected Vector3/GameEntity/GameId for param %d got %s!", 1,
				a_thread->GetMachine()->GetTypeName( a_thread->ParamType( 1 ) ) );
			return GM_EXCEPTION;
		}
	}

	a_thread->PushFloat( ( vPosition1 - vPosition2 ).Length() );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: CalcTrajectory
//		Calculate an aim vector for a trajectory.
//
// Parameters:
//		<Vector3> - Start Position
//		<Vector3> - Target Position
//		<int> OR <float> - Projectile Speed
//		<int> OR <float> - Projectile Gravity
//		<table> optional - to fill in
//
// Returns:
//		<table> - 1 or 2 aim vectors for valid trajectories
//		- OR -
//		null - If no valid trajectory exists
static int gmfCalculateTrajectory( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 4 );
	GM_CHECK_VECTOR_PARAM( v1, 0 );
	GM_CHECK_VECTOR_PARAM( v2, 1 );
	GM_CHECK_FLOAT_OR_INT_PARAM( fProjectileSpeed, 2 );
	GM_CHECK_FLOAT_OR_INT_PARAM( fProjectileGravity, 3 );
	GM_TABLE_PARAM( tblOut, 4, NULL );
	
	Trajectory::AimTrajectory traj[ 2 ];
	int t = Trajectory::Calculate(
		Vector3f( v1.x, v1.y, v1.z ),
		Vector3f( v2.x, v2.y, v2.z ),
		fProjectileSpeed,
		IGame::GetGravity() * fProjectileGravity,
		traj );

	if ( t > 0 )
	{
		gmMachine *pMachine = a_thread->GetMachine();
		DisableGCInScope gcEn( pMachine );

		if ( tblOut == NULL )
			tblOut = a_thread->GetMachine()->AllocTableObject();

		for ( int i = 0; i < t; ++i )
		{
			gmVariable var;

			Vector3f vAimPt = Vector3f( v2.x, v2.y, v2.z ), vStartPos = Vector3f( v1.x, v1.y, v1.z );
			Vector3f vAimDir = vAimPt - vStartPos;
			vAimPt.Z() = vStartPos.Z() + Mathf::Tan( traj[ 0 ].mAngle ) * Vector2f( vAimDir.X(), vAimDir.Y() ).Length();
			vAimDir = vAimPt - vStartPos;
			vAimDir.Normalize();

			var.SetVector( vAimDir.X(), vAimDir.Y(), vAimDir.Z() );
			tblOut->Set( pMachine, i, var );
		}

		a_thread->PushTable( tblOut );
	}
	else
		a_thread->PushNull();
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: DrawEntityAABB
//		This function draws the AABB around an entity.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//		<float/int> - OPTIONAL -  - Duration to draw the aabb, in seconds default 2 seconds.
//		<int> - OPTIONAL - Color to draw the bounding box
//
// Returns:
//		int - 1 if successful, 0 if failure
static int gmfDrawEntityLocalAABB( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	GM_FLOAT_OR_INT_PARAM( duration, 1, 2.0f );
	GM_INT_PARAM( color, 2, COLOR::WHITE.rgba() );

	AABB aabb;
	if ( EngineFuncs::EntityLocalAABB( gameEnt, aabb ) )
	{
		RenderBuffer::AddAABB( aabb, obColor( color ) );
		a_thread->PushInt( 1 );
	}

	a_thread->PushInt( 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: gmfDrawEntityOBB
//		This function draws the AABB around an entity.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//		<float/int> - OPTIONAL -  - Duration to draw the aabb, in seconds default 2 seconds.
//		<int> - OPTIONAL - Color to draw the bounding box
//
// Returns:
//		int - 1 if successful, 0 if failure
static int gmfDrawEntityOBB( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	GM_FLOAT_OR_INT_PARAM( duration, 1, 2.0f );
	GM_INT_PARAM( color, 2, COLOR::WHITE.rgba() );

	Box3f obb;
	if ( EngineFuncs::EntityWorldOBB( gameEnt, obb ) )
	{
		RenderBuffer::AddOBB( obb, obColor( color ) );
		a_thread->PushInt( 1 );
	}

	a_thread->PushInt( 0 );
	return GM_OK;
}

static int gmfCheckEntityBoundsIntersect( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );

	GameEntity gameEnt0, gameEnt1;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt0, 0 );
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt1, 1 );

	Box3f obb0, obb1;
	if ( EngineFuncs::EntityWorldOBB( gameEnt0, obb0 ) &&
		EngineFuncs::EntityWorldOBB( gameEnt1, obb1 ) )
	{
		IntrBox3Box3f intr( obb0, obb1 );
		a_thread->PushInt( intr.Test() ? 1 : 0 );
		return GM_OK;
	}
	a_thread->PushInt( 0 );
	return GM_OK;
}

// function: GetGameState
//		Gets the current game state as a std::string, see <eGameState> and <GameStates>
//
// Parameters:
//
//		none
//
// Returns:
//		std::string - current game state
static int GM_CDECL gmfGetGameState( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	
	switch ( gEngineFuncs->GetGameState() )
	{
		case GAME_STATE_WAITINGFORPLAYERS:
			a_thread->PushNewString( "WaitingForPlayers" );
			break;
		case GAME_STATE_WARMUP:
			a_thread->PushNewString( "Warmup" );
			break;
		case GAME_STATE_WARMUP_COUNTDOWN:
			a_thread->PushNewString( "WarmupCountdown" );
			break;
		case GAME_STATE_PLAYING:
			a_thread->PushNewString( "Playing" );
			break;
		case GAME_STATE_SUDDENDEATH:
			a_thread->PushNewString( "SuddenDeath" );
			break;
		case GAME_STATE_INVALID:
			a_thread->PushNewString( "Invalid" );
			break;
		case GAME_STATE_INTERMISSION:
			a_thread->PushNewString( "Intermission" );
			break;
		case GAME_STATE_SCOREBOARD:
			a_thread->PushNewString( "Scoreboard" );
			break;
		case GAME_STATE_PAUSED:
			a_thread->PushNewString( "Paused" );
			break;
	}
	return GM_OK;
}

// function: GetGameTimeLeft
//		Gets the current game time left.
//
// Parameters:
//
//		none
//
// Returns:
//		float - seconds left in game
static int GM_CDECL gmfGetGameTimeLeft( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	const int timeleft = gEngineFuncs->GetGameTimeLeft();
	if ( timeleft >= 0 )
		a_thread->PushFloat( (float)timeleft / 1000.0f );
	else
		a_thread->PushNull();
	return GM_OK;
}

// function: ExecCommandOnClient
//		This function executes a command for the entity. Usually equivilent to console commands.
//
// Parameters:
//
//		<GameEntity> - The entity to use
//		- OR -
//		<int> - The gameId for the entity to use
//		<std::string> - command to execute for the client
//
// Returns:
//		None
static int GM_CDECL gmfExecCommandOnClient( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );
	GM_CHECK_STRING_PARAM( msg, 1 );

	if ( gameEnt.IsValid() )
	{
		const int gameId = gEngineFuncs->IDFromEntity( gameEnt );
		gEngineFuncs->BotCommand( gameId, msg );
	}
	return GM_OK;
}

// function: GetGameName
//		Gets the name of the currently running game.
// Parameters:
//
//		None
//
// Returns:
//		<std::string> - Name of current game.
static int GM_CDECL gmfGetGameName( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	a_thread->PushNewString( gEngineFuncs->GetGameName() );
	return GM_OK;
}

// function: GetModName
//		Gets the name of the currently running mod.
// Parameters:
//
//		None
//
// Returns:
//		<std::string> - Name of current mod.
static int GM_CDECL gmfGetModName( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	a_thread->PushNewString( gEngineFuncs->GetModName() );
	return GM_OK;
}

// function: GetModVersion
//		Gets the version of the currently running mod.
// Parameters:
//
//		None
//
// Returns:
//		<std::string> - Version of current mod.
static int GM_CDECL gmfGetModVersion( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	a_thread->PushNewString( gEngineFuncs->GetModVers() );
	return GM_OK;
}

// function: ShowPaths
//		Echo the paths of the mod out. Debugging purposes.
// Parameters:
//
//		None
//
// Returns:
//		<std::string> - Version of current mod.
static int GM_CDECL gmfShowPaths( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );

	if ( System::mInstance->mGame )
	{
		EngineFuncs::ConsoleMessage( va( "Omni-bot %s, Revision %s, %s",
			System::mInstance->mGame->GetGameVars().mVersionString.c_str(),
			Revision::Number().c_str(),
			Revision::Date().c_str() ).c_str() );
		EngineFuncs::ConsoleMessage( va( "Game: %s", System::mInstance->mGame->GetGameVars().mVersionString.c_str() ).c_str() );
		EngineFuncs::ConsoleMessage( va( "Mod Folder: %s", FileSystem::GetModFolder().string().c_str() ).c_str() );
		EngineFuncs::ConsoleMessage( va( "Nav Folder: %s", FileSystem::GetNavFolder().string().c_str() ).c_str() );
		EngineFuncs::ConsoleMessage( va( "Script Folder: %s", FileSystem::GetScriptFolder().string().c_str() ).c_str() );
	}
	return GM_OK;
}

// function: GetGravity
//		Gets the current gravity.
// Parameters:
//
//		None
//
// Returns:
//		<float> - Current Gravity.
static int GM_CDECL gmfGetGravity( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	a_thread->PushFloat( IGame::GetGravity() );
	return GM_OK;
}

// function: GetCheats
//		Gets the current cheat setting.
// Parameters:
//
//		None
//
// Returns:
//		<int> - true if cheats enabled, false if not.
static int GM_CDECL gmfGetCheats( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	a_thread->PushInt( IGame::GetCheatsEnabled() );
	return GM_OK;
}

// function: ServerCommand
//		Runs a server command.
// Parameters:
//
//		std::string - Command to execute on the server.
//
// Returns:
//		none
static int GM_CDECL gmfServerCommand( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_STRING_PARAM( cmd, 0 );
	gEngineFuncs->ServerCommand( cmd );
	return GM_OK;
}

static int GM_CDECL gmfServerScriptFunction( gmThread *a_thread )
{
	GM_CHECK_STRING_PARAM( entname, 0 );
	GM_CHECK_STRING_PARAM( funcname, 1 );

	int numParms = 0;
	static const int MAX_PARMS = 8;
	obUserData params[ MAX_PARMS ];
	for ( int i = 2; i < a_thread->GetNumParams() && numParms < MAX_PARMS; ++i )
	{
		GM_STRING_PARAM( p1, i, "" );
		params[ i ] = obUserData( p1 );
	}	
	gEngineFuncs->ServerScriptEvent( funcname, entname, params, numParms );
	return GM_OK;
}

static int GM_CDECL gmfGetLocalEntity( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	if ( Utils::GetLocalEntity().IsValid() )
		a_thread->PushEntity( Utils::GetLocalEntity().AsInt() );
	else
		a_thread->PushNull();
	return GM_OK;
}

static int GM_CDECL gmfGetLocalPosition( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	Vector3f v;
	if ( Utils::GetLocalPosition( v ) )
		a_thread->PushVector( v );
	else
		a_thread->PushNull();
	return GM_OK;
}

static int GM_CDECL gmfGetLocalGroundPosition( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	Vector3f v;
	if ( Utils::GetLocalGroundPosition( v, TR_MASK_FLOODFILL ) )
		a_thread->PushVector( v );
	else
		a_thread->PushNull();
	return GM_OK;
}

static int GM_CDECL gmfGetLocalEyePosition( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	Vector3f v;
	if ( Utils::GetLocalEyePosition( v ) )
		a_thread->PushVector( v );
	else
		a_thread->PushNull();
	return GM_OK;
}

static int GM_CDECL gmfGetLocalFacing( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	Vector3f v;
	if ( Utils::GetLocalFacing( v ) )
		a_thread->PushVector( v );
	else
		a_thread->PushNull();
	return GM_OK;
}

static int GM_CDECL gmfGetLocalAABB( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	AABB aabb;
	if ( Utils::GetLocalAABB( aabb ) )
		gmAABB::PushObject( a_thread, aabb );
	else
		a_thread->PushNull();
	return GM_OK;
}

static int GM_CDECL gmfGetLocalAimPosition( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	GM_INT_PARAM( mask, 0, TR_MASK_FLOODFILL );
	Vector3f v, n;
	if ( Utils::GetLocalAimPoint( v, &n, mask ) )
		a_thread->PushVector( v );
	else
		a_thread->PushNull();
	return GM_OK;
}

static int GM_CDECL gmfGetLocalAimNormal( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	GM_INT_PARAM( mask, 0, TR_MASK_FLOODFILL );
	Vector3f v, n;
	if ( Utils::GetLocalAimPoint( v, &n, mask ) )
		a_thread->PushVector( n );
	else
		a_thread->PushNull();
	return GM_OK;
}

static int GM_CDECL gmfGetNearestNonSolid( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );
	GM_CHECK_VECTOR_PARAM( sv, 0 );
	GM_CHECK_VECTOR_PARAM( ev, 0 );
	GM_INT_PARAM( mask, 0, TR_MASK_FLOODFILL );
	Vector3f v;
	if ( Utils::GetNearestNonSolid( v, Vector3f( sv.x, sv.y, sv.z ), Vector3f( ev.x, ev.y, ev.z ), mask ) )
		a_thread->PushVector( v );
	else
		a_thread->PushNull();
	return GM_OK;
}

static int GM_CDECL gmfReloadGoalScripts( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	System::mInstance->mGame->LoadGoalScripts( true );
	return GM_OK;
}

static int GM_CDECL gmfAllocGoalSerialNum( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	a_thread->PushInt( GetMapGoalSerial() );
	return GM_OK;
}

static int GM_CDECL gmfOnTriggerRegion( gmThread *a_thread )
{
	if ( a_thread->ParamType( 0 ) == gmAABB::GetType() &&
		a_thread->Param( 1 ).GetTableObjectSafe() )
	{
		GM_CHECK_GMBIND_PARAM( AABB*, gmAABB, aabb, 0 );
		GM_CHECK_TABLE_PARAM( tbl, 1 );

		Box3f obb;
		obb.Identity( 0.f );
		aabb->CenterPoint( obb.Center );
		aabb->GetSize( obb.Extent );

		int serial = TriggerManager::GetInstance()->AddTrigger( obb, a_thread->GetMachine(), tbl );
		if ( serial > 0 )
			a_thread->PushInt( serial );
		else
			a_thread->PushNull();
		return GM_OK;
	}
	else if ( a_thread->Param( 0 ).IsVector() &&
		a_thread->Param( 1 ).IsNumber() &&
		a_thread->Param( 2 ).GetTableObjectSafe() )
	{
		GM_CHECK_VECTOR_PARAM( v, 0 );
		GM_CHECK_FLOAT_OR_INT_PARAM( rad, 1 );
		GM_CHECK_TABLE_PARAM( tbl, 2 );

		int serial = TriggerManager::GetInstance()->AddTrigger( Vector3f( v.x, v.y, v.z ), rad, a_thread->GetMachine(), tbl );
		if ( serial > 0 )
			a_thread->PushInt( serial );
		else
			a_thread->PushNull();
		return GM_OK;
	}
	GM_EXCEPTION_MSG( "Expected (AABB,Func,Func) or (Vector3,#,Func,Func)" );
	return GM_EXCEPTION;
}

static int GM_CDECL gmfDeleteTriggerRegion( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	if ( a_thread->Param( 0 ).IsInt() )
		TriggerManager::GetInstance()->DeleteTrigger( a_thread->Param( 0 ).GetInt() );
	else if ( a_thread->Param( 0 ).GetCStringSafe( NULL ) )
		TriggerManager::GetInstance()->DeleteTrigger( a_thread->Param( 0 ).GetCStringSafe() );
	else
	{
		GM_EXCEPTION_MSG( "Expected std::string(name) or int(serial#)" );
		return GM_EXCEPTION;
	}
	return GM_OK;
}

static int GM_CDECL gmfSendTrigger( gmThread *a_thread )
{
	GM_CHECK_TABLE_PARAM( tbl, 0 );

	gmMachine *pM = a_thread->GetMachine();

	TriggerInfo ti;

	{
		gmVariable vEnt = tbl->Get( pM, "Entity" );
		if ( vEnt.IsEntity() )
			ti.mEntity.FromInt( vEnt.GetEntity() );
	}
	{
		gmVariable vActivator = tbl->Get( pM, "Activator" );
		if ( vActivator.IsEntity() )
			ti.mActivator.FromInt( vActivator.GetEntity() );
	}
	{
		gmVariable vTagName = tbl->Get( pM, "TagName" );
		if ( vTagName.GetCStringSafe( 0 ) )
			Utils::StringCopy( ti.mTagName, vTagName.GetCStringSafe( 0 ), TriggerBufferSize );
	}
	{
		gmVariable vAction = tbl->Get( pM, "Action" );
		if ( vAction.GetCStringSafe( 0 ) )
			Utils::StringCopy( ti.mAction, vAction.GetCStringSafe( 0 ), TriggerBufferSize );
	}

	if ( ti.mAction[ 0 ] && ti.mTagName[ 0 ] )
	{
		TriggerManager::GetInstance()->HandleTrigger( ti );
		return GM_OK;
	}

	GM_EXCEPTION_MSG( "No TagName or Action defined!" );
	return GM_OK;
}

static int GM_CDECL gmfConfigSet( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 3 );
	GM_CHECK_STRING_PARAM( section, 0 );
	GM_CHECK_STRING_PARAM( key, 1 );
	gmVariable vValue = a_thread->Param( 2, gmVariable::s_null );
	GM_INT_PARAM( overwrite, 3, 1 );

	enum
	{
		BufferSize = 1024
	};
	char buffer[ BufferSize ] = {};
	Options::SetValue( section, key, std::string( vValue.AsString( a_thread->GetMachine(), buffer, BufferSize ) ), overwrite != 0 );

	return GM_OK;
}

static int GM_CDECL gmfConfigGet( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );
	GM_CHECK_STRING_PARAM( section, 0 );
	GM_CHECK_STRING_PARAM( key, 1 );

	enum
	{
		BufferSize = 1024
	};
	char buffer[ BufferSize ] = {};
	gmVariable vValue = a_thread->Param( 2, gmVariable::s_null );

	//////////////////////////////////////////////////////////////////////////

	std::string sValue;
	if ( Options::GetValue( section, key, sValue ) )
	{
		int IntVal = 0;
		float FloatVal = 0.f;

		if ( vValue.IsInt() && Utils::ConvertString( sValue, IntVal ) )
			a_thread->PushInt( IntVal );
		else if ( vValue.IsFloat() && Utils::ConvertString( sValue, FloatVal ) )
			a_thread->PushFloat( FloatVal );
		else
			a_thread->PushNewString( sValue.c_str(), (int)sValue.length() );
	}
	else if ( !a_thread->Param( 2 ).IsNull() )
	{
		Options::SetValue( section, key, std::string( vValue.AsString( a_thread->GetMachine(), buffer, BufferSize ) ) );

		a_thread->Push( a_thread->Param( 2 ) );
	}
	else
		a_thread->PushNull();

	return GM_OK;
}

static int GM_CDECL gmfCreateMapGoal( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_STRING_PARAM( mgtype, 0 );
	gmGCRoot<gmUserObject> obj = gMapGoalDatabase.CreateMapGoalType( mgtype );
	a_thread->PushUser( obj );
	return GM_OK;
}

static int GM_CDECL gmfEntityIsOutside( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );

	GameEntity gameEnt;
	GM_CHECK_GAMEENTITY_FROM_PARAM( gameEnt, 0 );

	Vector3f v = Vector3f::ZERO;
	if ( gameEnt.IsValid() && EngineFuncs::EntityPosition( gameEnt, v ) )
	{
		a_thread->PushInt( gEngineFuncs->IsOutSide( v ) );
	}
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// function: GetWeapon
//		This function will get the given <Weapon> object.
//
// Parameters:
//
//		<int> - The weapon Id to search for
//
// Returns:
//		<Weapon> - The weapon script object
//		- OR -
//		<null> - If weapon not found.
static int GM_CDECL gmfGetWeapon( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( weaponId, 0 );

	WeaponPtr wp = gWeaponDatabase.GetWeapon( weaponId );
	if ( wp )
		a_thread->PushUser( wp->GetScriptObject( a_thread->GetMachine() ) );
	else
	{
		a_thread->PushNull();
	}
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: GetGameType
//		Gets the current game type
//
// Parameters:
//
//		none
//
// Returns:
//		int - game type
static int GM_CDECL gmfGetGameType( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	a_thread->PushInt( gEngineFuncs->GetGameType() );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////

// function: SetCvar
//		This function will set a game cvar
//
// Parameters:
//
//		char   - the cvar to set
//		char   - the value of the cvar to be set
//
//
// Returns:
//		None
static int GM_CDECL gmfSetCvar( gmThread *a_thread )
{
	if ( a_thread->GetNumParams() > 1 )
	{
		const int bufferSize = 512;
		char buffer[ bufferSize ];

		int iPos = 0;
		const int cvarSize = 2048;
		const int valueSize = 2048;
		char cvar[ cvarSize ] = { 0 };
		char value[ valueSize ] = { 0 };

		const char *pAsString = a_thread->Param( 0 ).AsString( a_thread->GetMachine(), buffer, bufferSize );
		if ( pAsString )
		{
			int len = strlen( pAsString );
			if ( cvarSize - iPos > len )
			{
				Utils::StringCopy( &cvar[ iPos ], pAsString, len );
				iPos += len;
			}
		}

		iPos = 0;
		// and for the message..
		for ( int i = 1; i < a_thread->GetNumParams(); ++i )
		{
			const char *pAsString = a_thread->Param( i ).AsString( a_thread->GetMachine(), buffer, bufferSize );
			if ( pAsString )
			{
				int len = strlen( pAsString );
				if ( valueSize - iPos > len )
				{
					Utils::StringCopy( &value[ iPos ], pAsString, len );
					iPos += len;
				}
			}
		}
		
		a_thread->PushInt( gEngineFuncs->ServerSetVariable( cvar, value ) );
		return GM_OK;
	}

	GM_EXCEPTION_MSG( "Expected 2+ parameters" );
	return GM_EXCEPTION;
}

//////////////////////////////////////////////////////////////////////////

// function: GetCvar
//		This function will get the value of a game cvar
//
// Parameters:
//
//		char   - the cvar to get
//
//
// Returns:
//		The value of the cvar
static int GM_CDECL gmfGetCvar( gmThread *a_thread )
{
	if ( a_thread->GetNumParams() > 0 )
	{
		const int bufferSize = 512;
		char buffer[ bufferSize ];

		int iPos = 0;
		const int cvarSize = 2048;
		char cvar[ cvarSize ] = { 0 };

		const char *pAsString = a_thread->Param( 0 ).AsString( a_thread->GetMachine(), buffer, bufferSize );
		if ( pAsString )
		{
			int len = strlen( pAsString );
			if ( cvarSize - iPos > len )
			{
				Utils::StringCopy( &cvar[ iPos ], pAsString, len );
				iPos += len;
			}
		}
		
		char varValue[ bufferSize ] = {};
		gEngineFuncs->ServerGetVariable( cvar, varValue, bufferSize );
		a_thread->PushNewString( buffer );
		return GM_OK;
	}

	GM_EXCEPTION_MSG( "Expected 1 parameter" );
	return GM_EXCEPTION;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// package: Global Bot Library Functions
static gmFunctionEntry s_botLib [] =
{
	{ "EnableDebugWindow", gmfEnableDebugWindow },
	{ "DumpConsoleToFile", gmfDumpDebugConsoleToFile },

	{ "EchoToScreen", gmfEchoMessageToScreen },
	{ "Error", gmfEchoError },
	{ "Log", gmfLog },
	{ "RunScript", gmfRunScript },
	{ "ExecScript", gmfRunScript },
	{ "AddBot", gmfAddBot },
	{ "KickBot", gmfKickBot },
	{ "KickBotFromTeam", gmfKickBotFromTeam },
	{ "MoveBotToAnotherTeam", gmfMoveBotToAnotherTeam },

	{ "KickAll", gmfKickAll },
	{ "RegisterDefaultProfile", gmfRegisterDefaultProfile },
	{ "OnTrigger", gmfRegisterTriggerCallback },
	{ "GetGoal", gmfGetMapGoal },
	{ "GetGoals", gmfGetMapGoals },

	{ "SetMapGoalProperties", gmfSetMapGoalProperties },

	{ "SetAvailableMapGoals", gmfSetAvailableMapGoals },
	{ "SetGoalPriority", gmfSetGoalPriorityForTeamClass },
	{ "SetGoalGroup", gmfSetGoalGroup },

	{ "CreateMapGoal", gmfCreateMapGoal },

	{ "GetGameEntityFromId", gmfGetGameEntityFromId },
	{ "GetGameIdFromEntity", gmfGetGameIdFromEntity },
	{ "TraceLine", gmfTraceLine },
	{ "GroundPoint", gmfGroundPoint },

	{ "DrawDebugLine", gmfDrawDebugLine },
	{ "DrawDebugAABB", gmfDrawDebugAABB },
	{ "DrawLine", gmfDrawDebugLine },
	{ "DrawArrow", gmfDrawDebugArrow },
	{ "DrawAABB", gmfDrawDebugAABB },
	{ "DrawRadius", gmfDrawDebugRadius },
	{ "DrawText3d", gmfDrawDebugText3d },
	{ "DrawTrajectory", gmfDrawTrajectory },
	{ "TransformAndDrawLineList", gmfTransformAndDrawLineList },

	{ "DrawEntityLocalAABB", gmfDrawEntityLocalAABB },
	{ "DrawEntityOBB", gmfDrawEntityOBB },
	{ "CheckEntityBoundsIntersect", gmfCheckEntityBoundsIntersect },

	{ "GetMapName", gmfGetMapName },
	{ "GetMapExtents", gmfGetMapExtents },

	{ "GetPointContents", gmfGetPointContents },

	{ "GetGameState", gmfGetGameState },
	{ "GetGameTimeLeft", gmfGetGameTimeLeft },

	{ "GetTime", gmfGetTime },
	{ "ExecCommand", gmfExecCommand },

	{ "EntityKill", gmfEntityKill },
	{ "GetLocationName", gmfGetLocationName },

	{ "OnTriggerRegion", gmfOnTriggerRegion },
	{ "DeleteTriggerRegion", gmfDeleteTriggerRegion },

	{ "SendTrigger", gmfSendTrigger },

	{ "ConfigSet", gmfConfigSet },
	{ "ConfigGet", gmfConfigGet },

	{ "GetWeapon", gmfGetWeapon },

	// Unified functions that should work on GameId and GameEntity.
	{ "GetEntityName", gmfGetEntityName },
	{ "GetEntName", gmfGetEntityName },
	{ "GetEntFacing", gmfGetEntityFacing },
	{ "GetEntPosition", gmfGetEntityPosition },
	{ "GetEntCenterBounds", gmfGetEntCenterBounds },
	{ "GetEntBonePosition", gmfGetEntityBonePosition },
	{ "GetEntEyePosition", gmfGetEntEyePosition },
	{ "GetEntVelocity", gmfGetEntityVelocity },
	{ "GetEntRotationMatrix", gmfGetEntRotationMatrix },
	{ "GetEntFlags", gmfGetEntityFlags },
	{ "GetEntPowerups", gmfGetEntityPowerups },
	{ "GetEntLocalAABB", gmfGetEntityLocalAABB },
	{ "GetEntOwner", gmfGetEntityOwner },
	{ "GetEntTeam", gmfGetEntityTeam },
	{ "GetEntInfo", gmfGetEntityInfo },
	{ "GetEntCategory", gmfGetEntCategory },
	{ "EntityIsValid", gmfEntityIsValid },
	{ "EntityIsOutside", gmfEntityIsOutside },
	
	{ "DistanceBetween", gmfDistanceBetween },
	{ "CalcTrajectory", gmfCalculateTrajectory },

	{ "GetEntityLocalSpace", gmfGetEntityToLocalSpace },
	{ "GetEntityWorldSpace", gmfGetEntityToWorldSpace },

	{ "IterateEntities", gmfIterateEntities },
	{ "GetEntityInSphere", gmfGetEntityInSphere },
	{ "GetEntityByName", gmfGetEntityByName },

	{ "ExecCommandOnClient", gmfExecCommandOnClient },
	
	{ "GetGravity", gmfGetGravity },
	{ "CheatsEnabled", gmfGetCheats },
	{ "ServerCommand", gmfServerCommand },
	{ "ServerScriptFunction", gmfServerScriptFunction },

	{ "GetGameName", gmfGetGameName },
	{ "GetModName", gmfGetModName },
	{ "GetModVersion", gmfGetModVersion },
	{ "ShowPaths", gmfShowPaths },

	{ "GetLocalEntity", gmfGetLocalEntity },
	{ "GetLocalPosition", gmfGetLocalPosition },
	{ "GetLocalGroundPosition", gmfGetLocalGroundPosition },
	{ "GetLocalEyePosition", gmfGetLocalEyePosition },
	{ "GetLocalFacing", gmfGetLocalFacing },
	{ "GetLocalAABB", gmfGetLocalAABB },
	{ "GetLocalAimPosition", gmfGetLocalAimPosition },
	{ "GetLocalAimNormal", gmfGetLocalAimNormal },
	{ "GetNearestNonSolid", gmfGetNearestNonSolid },

	{ "ReloadGoalScripts", gmfReloadGoalScripts },

	{ "AllocGoalSerialNum", gmfAllocGoalSerialNum },
	
	{ "GetGameType", gmfGetGameType },
	{ "SetCvar", gmfSetCvar },
	{ "GetCvar", gmfGetCvar },
};

//////////////////////////////////////////////////////////////////////////

void gmBindBotLib( gmMachine * a_machine )
{
	// Register the bot functions.
	a_machine->RegisterLibrary( s_botLib, sizeof( s_botLib ) / sizeof( s_botLib[ 0 ] ) );
}