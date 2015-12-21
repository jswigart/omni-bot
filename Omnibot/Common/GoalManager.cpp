////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "GoalManager.h"
#include "Client.h"
#include "ScriptManager.h"
#include "MapGoal.h"
#include "MapGoalDatabase.h"
#include "gmUtilityLib.h"
#include "gmCall.h"
#include "Timer.h"
#include "RenderBuffer.h"
#include "System.h"

//////////////////////////////////////////////////////////////////////////

const int GoalManager::MapGoalVersion = 1;

//////////////////////////////////////////////////////////////////////////

class PriorityGreaterThan
{
public:
	bool operator()( MapGoalPtr _goal1, MapGoalPtr _goal2 )
	{
		float fG1Priority = mClient ?
			_goal1->GetPriorityForClient( mClient ) :
			_goal1->GetDefaultPriority();

		float fG2Priority = mClient ?
			_goal2->GetPriorityForClient( mClient ) :
			_goal2->GetDefaultPriority();

		return fG1Priority > fG2Priority;
	}

	// functions for equal_range
	bool operator()( const float _priority, MapGoalPtr _goal2 )
	{
		float fG2Priority = mClient ?
			_goal2->GetPriorityForClient( mClient ) :
			_goal2->GetDefaultPriority();
		return _priority == fG2Priority;
	}

	PriorityGreaterThan( Client *_client ) : mClient( _client )
	{
	}
private:
	Client * mClient;

	PriorityGreaterThan();
};

class PriorityLessThan
{
public:
	bool operator()( MapGoalPtr _goal1, MapGoalPtr _goal2 )
	{
		float fG1Priority = mClient ?
			_goal1->GetPriorityForClient( mClient ) :
			_goal1->GetDefaultPriority();
		float fG2Priority = mClient ?
			_goal2->GetPriorityForClient( mClient ) :
			_goal2->GetDefaultPriority();

		return fG1Priority < fG2Priority;
	}

	PriorityLessThan( Client *_client ) : mClient( _client )
	{
	}
private:
	Client * mClient;
	PriorityLessThan();
};

typedef std::pair<int, float> IndexPriority;

class IndexPriorityGreaterThan
{
public:
	bool operator()( IndexPriority _i1, IndexPriority _i2 )
	{
		return _i1.second > _i2.second;
	}

	bool operator()( float _val, IndexPriority _i2 )
	{
		return _val > _i2.second;
	}
};

//////////////////////////////////////////////////////////////////////////

GoalManager::Query::Query( uint32_t _type, Client *_client )
	: mTeam( 0 )
	, mRoleMask( -1 )
	, mClient( _client )
	, mTagName( 0 )
	, mSortType( SORT_BIAS )
	, mError( QueryOk )
	, mSkipNoInProgressSlots( true )
	, mSkipNoInUseSlots( true )
	, mSkipDelayed( true )
	, mSkipInUse( true )
	, mCheckInRadius( false )
	, mCheckRangeProperty( false )	
{
	if ( _type )
	{
		mGoalTypeList[ 0 ] = _type;
		mNumTypes = 1;
	}
	else
	{
		mNumTypes = 0;
	}
	Bot( _client );
}

GoalManager::Query &GoalManager::Query::AddType( uint32_t _type )
{
	if ( mNumTypes < MaxGoalTypes )
	{
		mGoalTypeList[ mNumTypes++ ] = _type;
	}
	return *this;
}

GoalManager::Query &GoalManager::Query::Team( int _team )
{
	mTeam = _team;
	return *this;
}

GoalManager::Query &GoalManager::Query::Bot( Client *_client )
{
	if ( _client )
	{
		mClient = _client;
		mTeam = _client->GetTeam();
		mRoleMask = _client->GetRoleMask();
	}
	return *this;
}

GoalManager::Query &GoalManager::Query::TagName( const char *_tagname )
{
	mTagName = _tagname;
	return *this;
}

GoalManager::Query &GoalManager::Query::Sort( SortType _sort )
{
	mSortType = _sort;
	return *this;
}

GoalManager::Query &GoalManager::Query::Expression( const char *_exp )
{
	mNameExp = _exp ? _exp : "";
	return *this;
}

GoalManager::Query &GoalManager::Query::Group( const char *_group )
{
	mGroupExp = _group ? _group : "";
	return *this;
}

GoalManager::Query &GoalManager::Query::RoleMask( uint32_t _i )
{
	mRoleMask = BitFlag32( _i );
	return *this;
}

//GoalManager::Query &GoalManager::Query::WeaponReq(int *_weaponIds, int _numids)
//{
// mWeaponIds = _weaponIds;
// mNumWeaponIds = _numids;
//	return *this;
//}

GoalManager::Query &GoalManager::Query::Ent( GameEntity _ent )
{
	mEntity = _ent;
	return *this;
}

GoalManager::Query &GoalManager::Query::CheckInRadius( const Vector3f & pos, float radius )
{
	mPosition = pos;
	mRadius = radius;
	mCheckInRadius = true;
	return *this;
}

GoalManager::Query &GoalManager::Query::CheckRangeProperty( bool checkRange )
{
	mCheckRangeProperty = checkRange;
	return *this;
}

GoalManager::Query &GoalManager::Query::NoFilters()
{
	mSkipDelayed = false;
	mSkipNoInProgressSlots = false;
	mSkipNoInUseSlots = false;
	mSkipInUse = false;
	mRoleMask = BitFlag32( -1 );
	return *this;
}

GoalManager::Query &GoalManager::Query::SkipDelayed( bool _skip )
{
	mSkipDelayed = _skip;
	return *this;
}

GoalManager::Query &GoalManager::Query::SkipNoInProgress( bool _skip )
{
	mSkipNoInProgressSlots = _skip;
	return *this;
}

GoalManager::Query &GoalManager::Query::SkipNoInUse( bool _skip )
{
	mSkipNoInUseSlots = _skip;
	return *this;
}
GoalManager::Query &GoalManager::Query::SkipInUse( bool _skip )
{
	mSkipInUse = _skip;
	return *this;
}
bool GoalManager::Query::GetBest( MapGoalPtr &_mg )
{
	if ( !mList.empty() )
	{
		_mg = mList.front();
		return true;
	}
	return false;
}

void GoalManager::Query::DefaultGlobalQuery()
{
	mSkipDelayed = false;
	mSkipNoInProgressSlots = false;
	mSkipNoInUseSlots = false;
}

void GoalManager::Query::FromTable( gmMachine *a_machine, gmTableObject *a_table )
{
	{
		if ( const char *GroupName = a_table->Get( a_machine, "Group" ).GetCStringSafe( 0 ) )
			Group( GroupName );
	}
	{
		gmVariable var = a_table->Get( a_machine, "Role" );
		if ( var.IsInt() )
			RoleMask( var.GetInt() );
	}
	{
		gmVariable var = a_table->Get( a_machine, "Team" );
		if ( var.IsInt() )
			Team( var.GetInt() );
	}
	{
		gmVariable var = a_table->Get( a_machine, "SkipDelayed" );
		if ( var.IsInt() )
			SkipDelayed( var.GetInt() != 0 );
	}
	{
		gmVariable var = a_table->Get( a_machine, "SkipNoInProgress" );
		if ( var.IsInt() )
			SkipNoInProgress( var.GetInt() != 0 );
	}
	{
		gmVariable var = a_table->Get( a_machine, "SkipNoInUse" );
		if ( var.IsInt() )
			SkipNoInUse( var.GetInt() != 0 );
	}
	{
		gmVariable var = a_table->Get( a_machine, "SkipInUse" );
		if ( var.IsInt() )
			SkipInUse( var.GetInt() != 0 );
	}
	{
		gmVariable var = a_table->Get( a_machine, "NoFilters" );
		if ( var.IsInt() && var.GetInt() != 0 )
			NoFilters();
	}
}

bool GoalManager::Query::CheckForMatch( MapGoalPtr & mg )
{
	if ( mNumTypes > 0 )
	{
		for ( int i = 0; mGoalTypeList[ i ] != mg->GetGoalTypeHash(); )
		{
			if ( ++i == mNumTypes ) return false;
		}
	}

	if ( mTeam && !mg->IsAvailable( mTeam ) &&
		!( mClient && mg->GetOwner() == mClient->GetGameEntity() ) )
		return false;

	if ( mg->GetDeleteMe() || mg->GetDisabled() )
		return false;

	if ( mSkipInUse && mg->GetInUse() )
		return false;

	if ( mClient && mg->GetPriorityForClient( mClient ) == 0.f )
		return false;

	// only skippage is when the mapgoal has a role
	if ( mg->GetRoleMask().AnyFlagSet() )
	{
		if ( !( mg->GetRoleMask() & mRoleMask ).AnyFlagSet() )
			return false;
	}

	// make sure the client has one of the weapons needed to use the goal.
	if ( mClient && mg->GetLimitWeapons().Get().AnyFlagSet() )
	{
		if ( !mg->GetLimitWeapons().IsAllowed( mClient ) )
			return false;
	}

	if ( mTagName && mg->GetTagName() != mTagName )
		return false;

	if ( mEntity.IsValid() && mg->GetEntity() != mEntity )
		return false;

	if ( mSkipNoInProgressSlots && mg->GetSlotsOpen( MapGoal::TRACK_INPROGRESS ) < 1 )
		return false;

	if ( mSkipNoInUseSlots && mg->GetSlotsOpen( MapGoal::TRACK_INUSE ) < 1 )
		return false;

	if ( mCheckInRadius )
	{
		if ( ( mPosition - mg->GetPosition() ).SquaredLength() > Mathf::Sqr( mRadius ) )
			return false;
	}

	if ( !mNameExp.empty() && !Utils::RegexMatch( mNameExp.c_str(), mg->GetName().c_str() ) )
		return false;

	if ( !mGroupExp.empty() && !Utils::RegexMatch( mGroupExp.c_str(), mg->GetGroupName().c_str() ) )
		return false;

	if ( mSkipDelayed && mClient )
	{
		if ( mClient->GetBB().GetNumBBRecords( bbk_DelayGoal, mg->GetSerialNum() ) > 0 )
			return false;
	}

	// cs: do this after bb delay check to throttle distance calculations
	if ( mCheckRangeProperty && mClient )
	{
		float range = (float)mg->GetRange();
		if ( range > 0 && ( mClient->GetPosition() - mg->GetPosition() ).SquaredLength() > Mathf::Sqr( range ) )
		{
			return false;
		}
	}

	return true;
}

void GoalManager::Query::OnQueryStart()
{
	mError = QueryOk;
	mList.resize( 0 );
}

void GoalManager::Query::OnQueryFinish()
{
	if ( !mList.empty() )
	{
		switch ( mSortType )
		{
			case SORT_BIAS:
			{
				if ( mList.size() > 1 )
				{
					// Get priorities of all goals in the list
					typedef std::vector<IndexPriority> IndexPriorityList;
					IndexPriorityList list;
					list.reserve( mList.size() );
					int index = 0;
					for ( MapGoalList::iterator it = mList.begin(); it != mList.end(); it++ )
					{
						list.push_back( IndexPriority( index++,
							mClient ? ( *it )->GetPriorityForClient( mClient ) : ( *it )->GetDefaultPriority() ) );
					}

					std::sort( list.begin(), list.end(), IndexPriorityGreaterThan() );

					// Randomize the ranges that have the same bias.
					for ( IndexPriorityList::iterator it = list.begin(); it != list.end(); )
					{
						IndexPriorityList::iterator upper = std::upper_bound(
							it, list.end(), ( *it ).second, IndexPriorityGreaterThan() );

						std::random_shuffle( it, upper );
						it = upper;
					}

					// Reorder goals list
					MapGoalList newList;
					newList.reserve( list.size() );
					for ( IndexPriorityList::iterator it = list.begin(); it != list.end(); it++ )
					{
						newList.push_back( mList[ it->first ] );
					}
					mList.swap( newList );
				}
				break;
			}
			case SORT_RANDOM_FULL:
			{
				if ( mList.size() > 1 )
				{
					std::random_shuffle( mList.begin(), mList.end() );
				}
				break;
			}
			default:
				break;
		}
	}
}

void GoalManager::Query::OnMatch( MapGoalPtr & mg )
{
	mList.push_back( mg );
}

const char *GoalManager::Query::QueryErrorString()
{
	switch ( mError )
	{
		case QueryBadNameExpression:
			return "Bad Name Expression.";
		case QueryBadGroupExpression:
			return "Bad Group Expression.";
		case QueryOk:
		default:
			break;
	}
	return "";
}
//////////////////////////////////////////////////////////////////////////

GoalManager::GoalManager()
	: mEditMode( EditNone )
{
}

GoalManager::~GoalManager()
{
	Shutdown();
}

void GoalManager::Init( System & system )
{
	rmt_ScopedCPUSample( GoalManagerInit );
	InitCommands();
}

void GoalManager::InitGameGoals()
{
	rmt_ScopedCPUSample( GoalManagerInitGameGoals );

	gEngineFuncs->GetGoals();

	// re-register goals from entities if we can
	IGame::EntityIterator ent;
	while ( IGame::IterateEntity( ent ) )
	{
		CheckEntityForGoal( ent.GetEnt() );
	}
}

void GoalManager::Reset()
{
	mMapGoalList.clear();
}

void GoalManager::InitCommands()
{
	SetEx( "show_goals", "prints out the names of each goal", this, &GoalManager::cmdGoalShow );
	SetEx( "show_goalroutes", "prints route info for matching map goals", this, &GoalManager::cmdGoalShowRoutes );
	SetEx( "draw_goals", "draws debug information for all mapgoals",  this, &GoalManager::cmdGoalDraw );
	SetEx( "draw_goalroutes", "draws debug routes", this, &GoalManager::cmdGoalDrawRoutes );
	SetEx( "goal_edit", "Begins to edit a goal.", this, &GoalManager::cmdGoalEdit );
	SetEx( "goal_editx", "Begins to edit a goal.", this, &GoalManager::cmdGoalEditx );
	SetEx( "goal_help", "Lists all help text to the console.", this, &GoalManager::cmdGoalHelp );
	SetEx( "goal_save", "Saves the goals to their own file.", this, &GoalManager::cmdGoalSave );
	SetEx( "goal_load", "Loads the goals from their own file.", this, &GoalManager::cmdGoalLoad );
	SetEx( "goal_create", "Creates a map goal of a provided type.", this, &GoalManager::cmdGoalCreate );
	SetEx( "goal_delete", "Deletes the currently selected goal.", this, &GoalManager::cmdGoalDelete );
	SetEx( "goal_finish", "Completes edits on the selected goal.", this, &GoalManager::cmdGoalFinish );
	SetEx( "goal_setproperty", "Sets the property of the goal.", this, &GoalManager::cmdGoalSetProperty );
	SetEx( "goal_removeall", "Removes all bot defined goals.", this, &GoalManager::cmdGoalRemoveAll );
	SetEx( "goal_move", "Toggle. Begins or ends moving a goal based on aim position.", this, &GoalManager::cmdGoalMove );
}

bool _GoalNameLT( const MapGoalPtr _pt1, const MapGoalPtr _pt2 )
{
	return _pt1->GetName() < _pt2->GetName();
}

void GoalManager::cmdGoalShow( const StringVector & args )
{
	File f;

	std::string strFile, strExpression = ".*";
	bool bShowSubPriorities = false;
	if ( args.size() > 3 )
	{
		strFile = args[ 3 ];
	}
	if ( args.size() > 1 )
	{
		strExpression = args[ 1 ];
	}
	if ( args.size() > 2 && args[ 2 ] == "p" )
	{
		bShowSubPriorities = true;
	}

	// Open the file if a filename was provided.
	if ( !strFile.empty() )
	{
		char strBuffer[ 1024 ] = {};
		sprintf( strBuffer, "user/%s", strFile.c_str() );
		f.OpenForWrite( strBuffer, File::Text );
	}

	GoalManager::Query qry;
	qry.NoFilters();
	qry.Expression( strExpression.c_str() );
	GetGoals( qry );

	std::sort( qry.mList.begin(), qry.mList.end(), _GoalNameLT );

	EngineFuncs::ConsoleMessage( "- Goal List -" );
	int iCount = 1;
	std::string txt;
	MapGoalList::iterator it = qry.mList.begin();
	for ( ; it != qry.mList.end(); ++it )
	{
		txt = va( "%d: ", iCount++ ).c_str();
		txt += ( *it )->GetName();

		txt += " ->";

		if ( !( *it )->GetGroupName().empty() )
		{
			txt += " group ";
			txt += ( *it )->GetGroupName();
		}

		std::string role = Utils::BuildRoleName( ( *it )->GetRoleMask().GetRawFlags() );
		if ( !role.empty() )
		{
			txt += " role ";
			txt += role;
		}

		txt += " ";

		for ( int team = 1; team <= 4; ++team )
		{
			txt += ( *it )->IsAvailable( team ) ? "1" : "0";
		}
		txt += " serial ";
		txt += va( "%d", ( *it )->GetSerialNum() ).c_str();
		txt += " priority ";
		txt += va( "%.2f", ( *it )->GetDefaultPriority() ).c_str();

		EngineFuncs::ConsoleMessage( txt.c_str() );
		if ( bShowSubPriorities )
		{
			( *it )->GetClassPriorities().GetPriorityText( txt );
		}

		if ( f.IsOpen() )
		{
			f.WriteString( txt.c_str() );
			f.WriteNewLine();
		}
	}
	EngineFuncs::ConsoleMessage( "- End Goal List -" );
}

void GoalManager::cmdGoalShowRoutes( const StringVector & args )
{
	File f;

	std::string strFile, strExpression = ".*";
	if ( args.size() > 2 )
	{
		strFile = args[ 2 ];
	}
	if ( args.size() > 1 )
	{
		strExpression = args[ 1 ];
	}

	// Open the file if a filename was provided.
	if ( !strFile.empty() )
	{
		char strBuffer[ 1024 ] = {};
		sprintf( strBuffer, "user/%s", strFile.c_str() );
		f.OpenForWrite( strBuffer, File::Text );
	}

	GoalManager::Query qry;
	qry.Expression( strExpression.c_str() );
	GetGoals( qry );

	std::sort( qry.mList.begin(), qry.mList.end(), _GoalNameLT );

	EngineFuncs::ConsoleMessage( "- Route List -" );
	int iCount = 1;
	std::string txt;
	MapGoalList::iterator it = qry.mList.begin();
	for ( ; it != qry.mList.end(); ++it )
	{
		if ( ( *it )->GetRoutes().empty() )
			continue;

		const char *pMsg = va( "%d: %s", iCount++, ( *it )->GetName().c_str() ).c_str();
		EngineFuncs::ConsoleMessage( pMsg );
		if ( f.IsOpen() )
		{
			f.WriteString( pMsg );
			f.WriteNewLine();
		}

		int iNumRoutes = 1;
		MapGoal::Routes::const_iterator rIt = ( *it )->GetRoutes().begin(), rItEnd = ( *it )->GetRoutes().end();
		for ( ; rIt != rItEnd; ++rIt )
		{
			const char *pRoute = va( "    %d: %s -> %s", iNumRoutes++, rIt->mStart->GetName().c_str(), rIt->mEnd->GetName().c_str() ).c_str();

			EngineFuncs::ConsoleMessage( pRoute );

			if ( f.IsOpen() )
			{
				f.WriteString( pRoute );
				f.WriteNewLine();
			}
		}
	}
	EngineFuncs::ConsoleMessage( "- End Route List -" );
}

void GoalManager::cmdGoalDraw( const StringVector & args )
{
	const char *pExpression = 0;
	bool DrawGoals = false;
	switch ( args.size() )
	{
		case 3:
			pExpression = args[ 2 ].c_str();
		case 2:
			if ( Utils::StringToTrue( args[ 1 ] ) )
			{
				DrawGoals = true;
				break;
			}
			else if ( Utils::StringToFalse( args[ 1 ] ) )
			{
				DrawGoals = false;
				break;
			}
		case 1:
			EngineFuncs::ConsoleMessage( "syntax: draw_goals on/off <optional goal name expression>" );
			return;
	}

	//////////////////////////////////////////////////////////////////////////

	int NumSet = 0;
	for ( MapGoalList::iterator it = mMapGoalList.begin();
		it != mMapGoalList.end();
		++it )
	{
		if ( pExpression )
		{
			if ( !Utils::RegexMatch( pExpression, ( *it )->GetName().c_str() ) )
			{
				continue;
			}
		}

		( *it )->SetRenderGoal( DrawGoals );
		++NumSet;
	}
	//////////////////////////////////////////////////////////////////////////
	EngineFuncs::ConsoleMessage( va( "Goals Rendering, %d %s", NumSet, DrawGoals ? "Enabled" : "Disabled" ).c_str() );
}

void GoalManager::cmdGoalDrawRoutes( const StringVector & args )
{
	const char *pExpression = 0;
	bool DrawRoutes = false;
	switch ( args.size() )
	{
		case 3:
			pExpression = args[ 2 ].c_str();
		case 2:
			if ( Utils::StringToTrue( args[ 1 ] ) )
			{
				DrawRoutes = true;
				break;
			}
			else if ( Utils::StringToFalse( args[ 1 ] ) )
			{
				DrawRoutes = false;
				break;
			}
		case 1:
			EngineFuncs::ConsoleMessage( "syntax: draw_goals on/off <optional goal name expression>" );
			return;
	}

	//////////////////////////////////////////////////////////////////////////
	int NumSet = 0;
	for ( MapGoalList::iterator it = mMapGoalList.begin();
		it != mMapGoalList.end();
		++it )
	{
		if ( pExpression && !Utils::RegexMatch( pExpression, ( *it )->GetName().c_str() ) )
			continue;

		( *it )->SetRenderRoutes( DrawRoutes );
		++NumSet;
	}
	//////////////////////////////////////////////////////////////////////////
	EngineFuncs::ConsoleMessage( va( "Goal Routes Rendering, %d %s", NumSet, DrawRoutes ? "Enabled" : "Disabled" ).c_str() );
}

const char *MapGoalTable = "_MG";

bool GoalManager::Save( const std::string &_map, ErrorObj &_err )
{
	const std::string filePath = std::string( "nav/" ) + std::string( gEngineFuncs->GetMapName() ) + "_goals.gm";

	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

	if ( !mLoadedMapGoals )
		mLoadedMapGoals.Set( pMachine->AllocTableObject(), pMachine );

	mLoadedMapGoals->Set( pMachine, "Version", gmVariable( MapGoalVersion ) );

	int GoalsSaved = 0;
	int GoalsSavedFailed = 0;
	int GoalsSavedSkipped = 0;

	for ( MapGoalList::iterator it = mMapGoalList.begin();
		it != mMapGoalList.end();
		++it )
	{
		if ( !( *it )->GetDontSave() )
		{
			if ( ( *it )->SaveToTable( pMachine, mLoadedMapGoals, _err ) )
				GoalsSaved++;
			else
				GoalsSavedFailed++;
		}
		else
		{
			GoalsSavedSkipped++;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	_err.AddInfo( "%d Goals Saved Successfully, %d Goals could not save, %d skipped.",
		GoalsSaved,
		GoalsSavedFailed,
		GoalsSavedSkipped );

	File outFile;
	outFile.OpenForWrite( filePath.c_str(), File::Text );
	return gmUtility::DumpTable( pMachine, outFile, MapGoalTable, mLoadedMapGoals, gmUtility::DUMP_ALL );
}

bool GoalManager::Load( const std::string &_map, ErrorObj &_err )
{
	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();

	int GoalsLoaded = 0;
	int GoalsDeferred = 0;
	int GoalsLoadedFailed = 0;

	Timer loadTime;

	bool LoadedOk = true;

	filePath script( "nav/%s_goals.gm", _map.c_str() );

	int ThreadId = GM_INVALID_THREAD;
	pMachine->GetGlobals()->Set( pMachine, MapGoalTable, gmVariable::s_null );
	if ( ScriptManager::GetInstance()->ExecuteFile( script, ThreadId ) )
	{
		//////////////////////////////////////////////////////////////////////////
		// remove old goals.
		MapGoalList::iterator it = mMapGoalList.begin();
		for ( ; it != mMapGoalList.end(); )
		{
			if ( !( *it )->GetInterfaceGoal() )
				it = mMapGoalList.erase( it );
			else
				++it;
		}
		//////////////////////////////////////////////////////////////////////////

		gmVariable var = pMachine->GetGlobals()->Get( pMachine, MapGoalTable );
		gmTableObject *tbl = var.GetTableObjectSafe();
		if ( tbl )
		{
			mLoadedMapGoals.Set( tbl, pMachine );

			int Version = -1;
			gmVariable gmVersion = tbl->Get( pMachine, "Version" );
			if ( gmVersion.GetInt( Version, -1 ) && Version == MapGoalVersion )
			{
				tbl->Set( pMachine, "Version", gmVariable::s_null );

				gmTableIterator tIt;
				gmTableNode *pNode = tbl->GetFirst( tIt );
				while ( pNode )
				{
					gmTableObject *mgTbl = pNode->m_value.GetTableObjectSafe();
					if ( mgTbl )
					{
						const std::string goalName = mgTbl->Get( pMachine, "Name" ).GetCStringSafe( "" );
						const std::string goalType = mgTbl->Get( pMachine, "GoalType" ).GetCStringSafe( "" );
						if ( !goalType.empty() && !goalName.empty() )
						{
							// if the goal exists already
							gmGCRoot<gmTableObject> goalTbl( mgTbl, pMachine );
							MapGoalPtr existingGoal = GetGoal( goalName );
							if ( existingGoal )
							{
								if ( existingGoal->LoadFromTable( pMachine, goalTbl, _err ) )
								{
									GoalsLoaded++;
								}
								else
								{
									_err.AddError( "Problem reloading goal : %s", goalName.c_str() );
									GoalsLoadedFailed++;
								}
							}
							else
							{
								MapGoalPtr mgPtr = gMapGoalDatabase.GetNewMapGoal( goalType );
								if ( mgPtr )
								{
									if ( mgPtr->LoadFromTable( pMachine, goalTbl, _err ) )
									{
										if ( mgPtr->GetCreateOnLoad() )
										{
											mgPtr->InternalInitEntityState();
											AddGoal( mgPtr );
											GoalsLoaded++;
										}
										else
										{
											GoalsDeferred++;
										}
									}
									else
									{
										GoalsLoadedFailed++;
									}
								}
								else
								{
									GoalsLoadedFailed++;
									_err.AddError( "Unknown Goal Type : %s", goalType.c_str() );
								}
							}
						}
					}
					pNode = tbl->GetNext( tIt );
				}
			}
			else
			{
				// TODO: attempt to run versioning scripts?
				if ( !gmVersion.IsInt() )
					EngineFuncs::ConsoleError( va( "Map Goals Script couldn't find Version field, expecting: Version = %d", MapGoalVersion ).c_str() );
				else
					EngineFuncs::ConsoleError( va( "Map Goals Script got version %d, expecting: Version = %d", Version, MapGoalVersion ).c_str() );
				LoadedOk = false;

				// release the reference to the table
				mLoadedMapGoals = 0; //NULL;
			}
		}
	}
	else // GetInstance()->ExecuteFile failed
	{
		LoadedOk = false;
	}

	pMachine->GetGlobals()->Set( pMachine, MapGoalTable, gmVariable::s_null );
	pMachine->CollectGarbage( true );

	_err.AddInfo( "%d Goals Loaded, %d Goals Deferred, %d Goals could not load. elapsed time: %.2f seconds",
		GoalsLoaded, GoalsDeferred, GoalsLoadedFailed, loadTime.GetElapsedSeconds() );

	return LoadedOk;
}

void GoalManager::cmdGoalSave( const StringVector & args )
{
	ErrorObj err;
	Save( gEngineFuncs->GetMapName(), err );
	err.PrintToConsole();
}

void GoalManager::cmdGoalLoad( const StringVector & args )
{
	ErrorObj err;
	Load( gEngineFuncs->GetMapName(), err );
	err.PrintToConsole();
}

MapGoalPtr GoalManager::_GetGoalInRange( const Vector3f &_pos, float _radius, bool _onlydrawenabled )
{
	float NearestDistance = Utils::FloatMax;

	MapGoalPtr nearest;

	for ( MapGoalList::iterator it = mMapGoalList.begin();
		it != mMapGoalList.end();
		++it )
	{
		if ( _onlydrawenabled && !( *it )->GetRenderGoal() )
			continue;

		const float Dist = Length( ( *it )->GetPosition(), _pos );
		if ( Dist < _radius && ( !nearest || Dist < NearestDistance ) )
		{
			nearest = ( *it );
			NearestDistance = Dist;
		}
	}
	return nearest;
}

void GoalManager::_SetActiveGoal( MapGoalPtr _mg )
{
	mEditMode = EditNone;

	if ( mActiveGoal )
		EngineFuncs::ConsoleMessage( va( "Unselected Goal :%s", mActiveGoal->GetName().c_str() ).c_str() );

	mActiveGoal = _mg;

	if ( mActiveGoal )
		EngineFuncs::ConsoleMessage( va( "Selected Goal :%s", mActiveGoal->GetName().c_str() ).c_str() );
	else
		EngineFuncs::ConsoleMessage( "No Goal Selected" );
}

void GoalManager::_UpdateEditModes()
{
	if ( mEditMode == EditMove && mActiveGoal )
	{
		Vector3f AimPt;
		if ( Utils::GetLocalAimPoint( AimPt ) )
			mActiveGoal->SetPosition( AimPt );
	}
}

void GoalManager::cmdGoalEdit( const StringVector & args )
{
	MapGoalPtr nearest;

	Vector3f vPos;
	if ( Utils::GetLocalPosition( vPos ) )
	{
		nearest = _GetGoalInRange( vPos, 150.f, true );
	}

	_SetActiveGoal( nearest );
	if ( !mActiveGoal )
	{
		EngineFuncs::ConsoleError( "No Goal In Range!" );
	}
}

void GoalManager::cmdGoalEditx( const StringVector & args )
{
	_SetActiveGoal( mHighlightedGoal );
	if ( !mActiveGoal )
	{
		EngineFuncs::ConsoleError( "No Goal In Range!" );
	}
}

void GoalManager::cmdGoalHelp( const StringVector & args )
{
	if ( mActiveGoal )
		mActiveGoal->ShowHelp();
}

void GoalManager::cmdGoalFinish( const StringVector & args )
{
	_SetActiveGoal( MapGoalPtr() );
}

void GoalManager::cmdGoalCreate( const StringVector & args )
{
	const char *strUsage [] =
	{
		"goal_create goaltype[std::string]",
		"> goaltype: type name of goal to create",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );
	OPTIONAL_STRING_PARAM( goalname, 2, "" );

	Box3f obb;
	Vector3f facing;
	if ( !Utils::GetLocalWorldOBB( obb ) || !Utils::GetLocalFacing( facing ) )
	{
		EngineFuncs::ConsoleError( "Can't get local aabb or facing!" );
		return;
	}

	MapGoalDef def;
	def.Props.SetString( "Type", args[ 1 ].c_str() );
	def.Props.SetVector( "Position", obb.GetCenterBottom() );
	def.Props.SetVector( "Facing", facing );
	def.Props.SetFloat( "MinRadius", 32.f );
	def.Props.SetString( "TagName", goalname.c_str() );
	def.Props.SetInt( "NewGoal", 1 );

	MapGoalPtr mg = AddGoal( def );
	_SetActiveGoal( mg );
	if ( mg )
		mg->ShowHelp();
	if ( !mActiveGoal )
	{
		EngineFuncs::ConsoleError( va( "Unknown goal type: %s", args[ 0 ].c_str() ).c_str() );
	}
}

void GoalManager::cmdGoalDelete( const StringVector & args )
{
	if ( !mActiveGoal )
	{
		EngineFuncs::ConsoleError( "Select a goal for edit by adding a new one or using goal_edit" );
		return;
	}
	mActiveGoal->SetDeleteMe( true );
	_SetActiveGoal( MapGoalPtr() );
}

void GoalManager::cmdGoalSetProperty( const StringVector & args )
{
	if ( !mActiveGoal )
	{
		EngineFuncs::ConsoleError( "Select a goal for edit by adding a new one or using goal_edit" );
		return;
	}
	const char *strUsage [] =
	{
		"goal_setproperty propertyname[std::string] propertyvalue[std::string]",
		"> propertyname: name of property to set",
		"> propertyvalue: optional value of property to set, also accepts keywords",
		"        <facing>, <position>, <aimpoint>, <aimnormal>",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );
	const std::string &propname = args[ 1 ];
	std::string propval = args.size() > 2 ? args[ 2 ] : "";

	obUserData val( propval.c_str() );

	Vector3f v, v2;
	if ( propval == "<facing>" && Utils::GetLocalFacing( v ) )
	{
		Utils::ConvertString( v, propval );
		val = obUserData( v );
	}
	if ( propval == "<position>" && Utils::GetLocalPosition( v ) )
	{
		Utils::ConvertString( v, propval );
		val = obUserData( v );
	}
	if ( propval == "<aimpoint>" && Utils::GetLocalAimPoint( v ) )
	{
		Utils::ConvertString( v, propval );
		val = obUserData( v );
	}
	if ( propval == "<aimnormal>" && Utils::GetLocalAimPoint( v, &v2 ) )
	{
		Utils::ConvertString( v2, propval );
		val = obUserData( v );
	}

	std::string oldName = mActiveGoal->GetName();
	mActiveGoal->SetProperty( propname, val );

	if ( mLoadedMapGoals && oldName != mActiveGoal->GetName() )
	{
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
		mLoadedMapGoals->Set( pMachine, oldName.c_str(), gmVariable::s_null );
	}
}

void GoalManager::cmdGoalRemoveAll( const StringVector & args )
{
	int Removed = 0;
	MapGoalList::iterator it = mMapGoalList.begin();
	for ( ; it != mMapGoalList.end(); )
	{
		if ( !( *it )->GetInterfaceGoal() )
		{
			++Removed;
			it = mMapGoalList.erase( it );
		}
		else
			++it;
	}

	EngineFuncs::ConsoleMessage( va( "Removed %d goals.", Removed ).c_str() );
}

void GoalManager::cmdGoalMove( const StringVector & args )
{
	OPTIONAL_STRING_PARAM( toPlayer, 1, "" );
	OPTIONAL_BOOL_PARAM( ground, 2, true );

	if ( mEditMode != EditMove )
	{
		if ( mActiveGoal )
		{
			if ( Utils::StringToLower( toPlayer ) == "toplayer" )
			{
				Vector3f vLocalPos;

				if ( ground == true )
				{
					Vector3f vGroundPos;
					Utils::GetLocalGroundPosition( vGroundPos, TR_MASK_SHOT );
					mActiveGoal->SetPosition( vGroundPos );
				}
				else if ( Utils::GetLocalPosition( vLocalPos ) )
					mActiveGoal->SetPosition( vLocalPos );
			}
			else
			{
				mEditMode = EditMove;
				EngineFuncs::ConsoleMessage( va( "Moving %s.", mActiveGoal->GetName().c_str() ).c_str() );
			}
		}
		else
		{
			EngineFuncs::ConsoleMessage( va( "Select a goal for edit first! (goal_edit/goal_editx)" ).c_str() );
		}
	}
	else
	{
		mEditMode = EditNone;
		EngineFuncs::ConsoleMessage( va( "Moving %s stopped.", mActiveGoal->GetName().c_str() ).c_str() );
	}
}

void GoalManager::Shutdown()
{
	mLoadedMapGoals = 0; //NULL;
	mMapGoalList.clear();
}

void GoalManager::Update( System & system )
{
	rmt_ScopedCPUSample( GoalManagerUpdate );

	//////////////////////////////////////////////////////////////////////////
	// see which goal we should highlight, if any
	bool render = false;
	if ( Utils::GetLocalEntity().IsValid() )
	{
		for ( MapGoalList::iterator it1 = mMapGoalList.begin(); it1 != mMapGoalList.end(); ++it1 )
		{
			if ( ( *it1 )->GetRenderGoal() )
			{
				render = true;
				break;
			}
		}
	}

	Vector3f AimPt;
	if ( render && Utils::GetLocalAimPoint( AimPt ) )
	{
		mHighlightedGoal = _GetGoalInRange( AimPt, 256.f, true );
	}

	if ( mHighlightedGoal )
	{
		RenderBuffer::AddCircle(
			mHighlightedGoal->GetPosition() + Vector3f( 0, 0, 32.f ),
			mHighlightedGoal->GetRadius(),
			COLOR::YELLOW );
	}

	//////////////////////////////////////////////////////////////////////////
	MapGoalList::iterator it = mMapGoalList.begin();
	while ( it != mMapGoalList.end() )
	{
		if ( ( *it )->GetDeleteMe() )
		{
			OnGoalDelete( ( *it ) );

			( *it )->SetAvailabilityTeams( 0 );
#ifdef _DEBUG
			LOG( "Goal Deleted: " << ( *it )->GetGoalType() << ", " << ( *it )->GetName().c_str() );
#endif
			it = mMapGoalList.erase( it );
		}
		else
		{
			( *it )->RenderDebug( mActiveGoal == ( *it ), mHighlightedGoal == ( *it ) );
			( *it )->Update();
			++it;
		}
	}
	
	_UpdateEditModes();
}

void GoalManager::AddGoal( MapGoalPtr newGoal )
{
	mMapGoalList.push_back( newGoal );

	//////////////////////////////////////////////////////////////////////////
	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
	if ( pMachine )
	{
		gmGCRoot<gmUserObject> pUser = newGoal->GetScriptObject( pMachine );

		gmCall call;
		if ( call.BeginGlobalFunction( pMachine, "OnGoalAdded", gmVariable::s_null, true ) )
		{
			if ( pUser )
				call.AddParamUser( pUser );
			call.End();
		}
	}
	//////////////////////////////////////////////////////////////////////////
}

MapGoalPtr GoalManager::AddGoal( const MapGoalDef &_goaldef )
{
	MapGoalDef goaldef = _goaldef;

	const char *GoalType = 0;
	goaldef.Props.GetString( "Type", GoalType );
	if ( !GoalType )
		return MapGoalPtr();

	MapGoalPtr newGoal = gMapGoalDatabase.GetNewMapGoal( GoalType );
	//////////////////////////////////////////////////////////////////////////
	// If no goals of this type, the script can give us an alias
	// useful for deprecating old goals but still being able to load/convert them
	// to a new and improved goal
	if ( !newGoal )
	{
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
		gmCall call;
		if ( call.BeginGlobalFunction( pMachine, "GetMapGoalUpgradeType" ) )
		{
			const std::string strLower = Utils::StringToLower( GoalType );
			call.AddParamString( strLower.c_str() );
			call.End();

			const char *NewName = 0;
			call.GetReturnedString( NewName );
			if ( NewName )
			{
				goaldef.Props.SetString( "OldType", GoalType );
				GoalType = NewName;
				newGoal = gMapGoalDatabase.GetNewMapGoal( NewName );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	if ( newGoal )
	{
		// new
		int Teams = 0;
		if ( goaldef.Props.GetInt( "Team", Teams ) && Teams )
		{
			newGoal->SetAvailabilityTeamsInit( Teams );
			newGoal->SetAvailabilityTeams( Teams );
		}
		else
		{
			for ( int i = 1; i <= 4; ++i )
			{
				newGoal->SetAvailable( i, true ); // everyone by default
				newGoal->SetAvailableInitial( i, true );
			}
		}
		const char *TagName = 0;
		goaldef.Props.GetString( "TagName", TagName );
		if ( TagName && TagName[ 0 ] )
			newGoal->SetTagName( TagName );

		GameEntity Entity;
		if ( goaldef.Props.GetEntity( "Entity", Entity ) )
			newGoal->SetEntity( Entity );

		Vector3f Position;
		if ( goaldef.Props.GetVector( "Position", Position ) )
			newGoal->SetPosition( Position );

		Vector3f Facing;
		if ( goaldef.Props.GetVector( "Facing", Facing ) )
			newGoal->SetFacing( Facing );

		float MinRadius;
		if ( goaldef.Props.GetFloat( "MinRadius", MinRadius ) )
			newGoal->SetMinRadius( MinRadius );

		float Radius;
		if ( goaldef.Props.GetFloat( "Radius", Radius ) )
			newGoal->SetRadius( Radius );

		int AutoGen = 0;
		if ( goaldef.Props.GetInt( "InterfaceGoal", AutoGen ) )
			newGoal->SetInterfaceGoal( AutoGen != 0 );

		int New = 0;
		if ( goaldef.Props.GetInt( "NewGoal", New ) && New != 0 )
		{
			newGoal->SetDisabled( true );
			newGoal->SetRenderGoal( true );
		}

		// If no version defined, assume 0
		int Version = 0;
		if ( !goaldef.Props.GetInt( "Version", Version ) )
			goaldef.Props.SetInt( "Version", 0 );

		newGoal->InternalInitEntityState();

		newGoal->GenerateName();

		// convert the keyvals to a script table
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
		gmGCRoot<gmTableObject> Props( pMachine->AllocTableObject(), pMachine );
		Utils::KeyValsToTable( goaldef.Props, Props, pMachine );

		// find the table from the save file and pass it in as well to
		// load script authoritative data
		gmGCRoot<gmTableObject> LoadFileProps;
		if ( mLoadedMapGoals )
		{
			gmVariable var = mLoadedMapGoals->Get( pMachine, newGoal->GetName().c_str() );
			gmTableObject * varTbl = var.GetTableObjectSafe();
			if ( varTbl )
			{
				LoadFileProps.Set( varTbl, pMachine );
			}
		}

		// merge the safe file properties with the property list before we call init
		bool LoadFromTable = false;
		if ( LoadFileProps )
		{
			gmTableIterator tIt;
			gmTableNode * pSaveNode = LoadFileProps->GetFirst( tIt );
			while ( pSaveNode )
			{
				Props->Set( pMachine, pSaveNode->m_key, pSaveNode->m_value );
				pSaveNode = LoadFileProps->GetNext( tIt );
			}
			LoadFromTable = true;
		}

		if ( LoadFromTable )
		{
			ErrorObj err;
			if ( newGoal->LoadFromTable( pMachine, Props, err ) )
			{
#ifdef _DEBUG
				LOG( "Goal Created: " << newGoal->GetName() << ", partially loaded from script" );
#endif
				AddGoal( newGoal );
			}
			else
			{
				err.PrintToConsole();
			}
		}
		else if ( newGoal->InternalInit( Props, New != 0 ) )
		{
#ifdef _DEBUG
			LOG( "Goal Created: " << newGoal->GetName() );
#endif
			AddGoal( newGoal );
		}
		else
		{
			LOG( "Goal Unable to Init: " <<
				newGoal->GetGoalType() << ", tag: " <<
				newGoal->GetTagName() );
		}
	}
	else
	{
		const char *TagName = 0;
		goaldef.Props.GetString( "TagName", TagName );
		TagName = TagName ? TagName : "noname";

		if ( !GoalType )
		{
			LOG( "Goal Type Not Provided: " << TagName );
		}
		else
		{
			LOG( "Unknown Goal Type: " << GoalType << ", tag: " << TagName );
		}
	}
	return newGoal;
}

void GoalManager::RemoveGoalByName( const char *_goalname )
{
	const std::string goalnm = _goalname;
	MapGoalList::iterator it = mMapGoalList.begin();
	while ( it != mMapGoalList.end() )
	{
		const std::string temp = ( *it )->GetName();
		if ( ( *it )->GetName() == _goalname )
		{
			OnGoalDelete( ( *it ) );
#ifdef _DEBUG
			LOG( "Goal Deleted: " << ( *it )->GetGoalType() << ", " << ( *it )->GetName().c_str() );
#endif
			it = mMapGoalList.erase( it );
		}
		else
			++it;
	}
}

void GoalManager::RemoveGoalsByType( const char *_goaltype )
{
	MapGoalList::iterator it = mMapGoalList.begin();
	while ( it != mMapGoalList.end() )
	{
		if ( ( *it )->GetGoalType() == _goaltype )
		{
			OnGoalDelete( ( *it ) );
			( *it )->SetAvailabilityTeams( 0 );
#ifdef _DEBUG
			LOG( "Goal Deleted: " << ( *it )->GetGoalType() << ", " << ( *it )->GetName().c_str() );
#endif
			it = mMapGoalList.erase( it );
		}
		else
			++it;
	}
}

void GoalManager::RemoveGoalByEntity( GameEntity _ent )
{
	MapGoalList::iterator it = mMapGoalList.begin();
	while ( it != mMapGoalList.end() )
	{
		if ( ( *it )->GetEntity().GetIndex() == _ent.GetIndex() )
			Utils::OutputDebug( kInfo, "SameIndex" );

		if ( ( *it )->GetEntity() == _ent )
		{
			( *it )->SetAvailabilityTeams( 0 );

			if ( ( *it )->GetRemoveWithEntity() )
			{
				OnGoalDelete( ( *it ) );
				( *it )->SetDeleteMe( true );
#ifdef _DEBUG
				LOG( "Goal Deleted: " << ( *it )->GetGoalType() << ", " << ( *it )->GetName().c_str() );
#endif
				it = mMapGoalList.erase( it );
			}
			else
			{
				( *it )->SetDisabled( true );
				( *it )->SetEntity( GameEntity() );
#ifdef _DEBUG
				LOG( "Goal Disabled: " << ( *it )->GetGoalType() << ", " << ( *it )->GetName().c_str() );
#endif
				++it;
			}
		}
		else
			++it;
	}
}

void GoalManager::OnGoalDelete( const MapGoalPtr &_goal )
{
	if ( mHighlightedGoal == _goal )
		mHighlightedGoal = NULL;

	gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
	if ( mLoadedMapGoals )
		mLoadedMapGoals->Set( pMachine, _goal->GetName().c_str(), gmVariable::s_null );
}

MapGoalPtr GoalManager::GetGoal( const std::string &_goalname )
{
	MapGoalPtr ptr;

	MapGoalList::iterator it = mMapGoalList.begin(),
		itEnd = mMapGoalList.end();
	while ( it != itEnd )
	{
		if ( ( *it )->GetName() == _goalname )
		{
			ptr = ( *it );
			break;
		}
		++it;
	}
	return ptr;
}

MapGoalPtr GoalManager::GetGoal( int _serialNum )
{
	MapGoalPtr ptr;

	MapGoalList::iterator it = mMapGoalList.begin(),
		itEnd = mMapGoalList.end();
	while ( it != itEnd )
	{
		if ( ( *it )->GetSerialNum() == _serialNum )
		{
			ptr = ( *it );
			break;
		}
		++it;
	}

	return ptr;
}

void GoalManager::UpdateGoalEntity( GameEntity oldent, GameEntity newent )
{
	MapGoalList::iterator it = mMapGoalList.begin();
	while ( it != mMapGoalList.end() )
	{
		if ( ( *it )->GetEntity() == oldent )
		{
			( *it )->SetEntity( newent );
			LOG( "Goal Update: " << ( *it )->GetName() );
		}
		else
			++it;
	}
}

void GoalManager::GetGoals( Query &_qry )
{
	rmt_ScopedCPUSample( GetGoals );

	//////////////////////////////////////////////////////////////////////////

	std::string strTagName = _qry.mTagName ? _qry.mTagName : "";

	_qry.OnQueryStart();

	for ( MapGoalList::iterator it = mMapGoalList.begin(); it != mMapGoalList.end(); ++it )
	{
		if ( _qry.CheckForMatch( ( *it ) ) )
		{
			_qry.OnMatch( ( *it ) );
		}
	}
	_qry.OnQueryFinish();
}

void GoalManager::CheckEntityForGoal( const EntityInstance &ei )
{
	const std::string entName = EngineFuncs::EntityName( ei.mEntity );

	MapGoalDef goalDef;

	// Parse common properties
	int allowedTeams = 0;
	if ( ei.mEntInfo.mFlags.CheckFlag( ENT_FLAG_TEAM1 ) )
		allowedTeams |= ( 1 << OB_TEAM_1 );
	if ( ei.mEntInfo.mFlags.CheckFlag( ENT_FLAG_TEAM2 ) )
		allowedTeams |= ( 1 << OB_TEAM_2 );
	if ( ei.mEntInfo.mFlags.CheckFlag( ENT_FLAG_TEAM3 ) )
		allowedTeams |= ( 1 << OB_TEAM_3 );
	if ( ei.mEntInfo.mFlags.CheckFlag( ENT_FLAG_TEAM4 ) )
		allowedTeams |= ( 1 << OB_TEAM_4 );

	goalDef.Props.SetEntity( "Entity", ei.mEntity );
	goalDef.Props.SetString( "TagName", entName.c_str() );
	goalDef.Props.SetInt( "Team", allowedTeams );

	switch ( ei.mEntInfo.mGroup )
	{
		case ENT_GRP_BUTTON:
			goalDef.Props.SetString( "Type", "Button" );
			break;
		/*case ENT_GRP_HEALTH:
			goalDef.Props.SetString( "Type", "Health" );
			goalDef.Props.SetInt( "Quantity", ei.mEntInfo.mQuantity.mNum );
			break;
		case ENT_GRP_AMMO1:
		case ENT_GRP_AMMO2:
			goalDef.Props.SetString( "Type", "Ammo" );
			goalDef.Props.SetInt( "WeaponClass", ei.mEntInfo.mClassId );
			goalDef.Props.SetInt( "Quantity", ei.mEntInfo.mQuantity.mNum );
			break;
		case ENT_GRP_ARMOR:
			goalDef.Props.SetString( "Type", "Armor" );
			goalDef.Props.SetInt( "Quantity", ei.mEntInfo.mQuantity.mNum );
			break;
		case ENT_GRP_ENERGY:
			goalDef.Props.SetString( "Type", "Energy" );
			goalDef.Props.SetInt( "Quantity", ei.mEntInfo.mQuantity.mNum );
			break;*/
		case ENT_GRP_FLAG:
			goalDef.Props.SetString( "Type", "Flag" );
			break;
		case ENT_GRP_FLAGCAPPOINT:
			goalDef.Props.SetString( "Type", "CapPoint" );
			break;
		case ENT_GRP_WEAPON:
			goalDef.Props.SetString( "Type", "Weapon" );
			goalDef.Props.SetInt( "WeaponClass", ei.mEntInfo.mClassId );
			break;
		case ENT_GRP_MONSTER:
			goalDef.Props.SetString( "Type", "Monster" );
			goalDef.Props.SetInt( "MonsterClass", ei.mEntInfo.mClassId );
			break;
		case ENT_GRP_POWERUP:
			goalDef.Props.SetString( "Type", "Powerup" );
			goalDef.Props.SetInt( "PowerupClass", ei.mEntInfo.mClassId );
			break;
			/*case ENT_GRP_DISPENSER:
			goalDef.Props.SetString( "Type", "Dispenser" );
			break;
			case ENT_GRP_BUILDABLE:
			goalDef.Props.SetString( "Type", "Buildable" );
			break;
			case ENT_GRP_MOUNTABLE:
			goalDef.Props.SetString( "Type", "Mountable" );
			break;*/
		default:
			return;
	}

	AddGoal( goalDef );
}

void GoalManager::EntityCreated( const EntityInstance &ei )
{
	CheckEntityForGoal( ei );
}

void GoalManager::EntityDeleted( const EntityInstance &ei )
{
	RemoveGoalByEntity( ei.mEntity );
}
