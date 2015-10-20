////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "BlackBoard.h"
#include "BlackBoardItems.h"
#include "IGame.h"
#include "gmbinder2/gmbinder2.h"

static int g_NextScriptItem = bbk_FirstScript;

BlackBoard	gBlackboard;

BlackBoard::BlackBoard()
{
}

BlackBoard::~BlackBoard()
{
}

bool BlackBoard::PostBBRecord( BBRecordPtr _item )
{
	mDB.insert( std::make_pair( _item->GetType(), _item ) );
	return true;
}

int BlackBoard::GetBBRecords( int _type, BBRecordPtr *_items, int _maxitems )
{
	BlackBoardDatabase::iterator it = mDB.lower_bound( _type );
	BlackBoardDatabase::iterator itEnd = mDB.upper_bound( _type );

	int iNum = 0;
	while ( it != itEnd && iNum < _maxitems )
	{
		if ( it->second->GetType() == _type )
			_items[ iNum++ ] = it->second;
		++it;
	}

	return iNum;
}

int BlackBoard::GetNumBBRecords( int _type, int _target )
{
	BlackBoardDatabase::iterator it = mDB.lower_bound( _type );
	BlackBoardDatabase::iterator itEnd = mDB.upper_bound( _type );
	int iNum = 0;
	while ( it != itEnd )
	{
		if ( it->second->GetType() == _type )
		{
			if ( _target == bbk_All || it->second->mTarget == _target )
				++iNum;
		}
		++it;
	}
	return iNum;
}

bool BlackBoard::RecordExistsOwner( int _type, int _owner )
{
	BlackBoardDatabase::iterator it = mDB.lower_bound( _type );
	BlackBoardDatabase::iterator itEnd = mDB.upper_bound( _type );
	while ( it != itEnd )
	{
		if ( it->second->GetType() == _type && it->second->mOwner == _owner )
			return true;
		++it;
	}
	return false;
}

bool BlackBoard::RecordExistsTarget( int _type, int _target )
{
	BlackBoardDatabase::iterator it = mDB.lower_bound( _type );
	BlackBoardDatabase::iterator itEnd = mDB.upper_bound( _type );
	while ( it != itEnd )
	{
		if ( it->second->GetType() == _type && it->second->mTarget == _target )
			return true;
		++it;
	}
	return false;
}

int BlackBoard::RemoveBBRecordByPoster( int _poster, int _type /*= bbk_All*/ )
{
	int iCount = 0;

	BlackBoardDatabase::iterator it;
	BlackBoardDatabase::iterator itEnd;
	if ( _type == bbk_All )
	{
		it = mDB.begin();
		itEnd = mDB.end();
	}
	else
	{
		it = mDB.lower_bound( _type );
		itEnd = mDB.upper_bound( _type );
	}

	while ( it != itEnd )
	{
		if ( it->second->mOwner == _poster )
		{
			mDB.erase( it++ );
			++iCount;
		}
		else
		{
			++it;
		}
	}

	return iCount;
}

int BlackBoard::RemoveBBRecordByTarget( int _target, int _type /*= bbk_All*/ )
{
	int iCount = 0;

	BlackBoardDatabase::iterator it;
	BlackBoardDatabase::iterator itEnd;
	if ( _type == bbk_All )
	{
		it = mDB.begin();
		itEnd = mDB.end();
	}
	else
	{
		it = mDB.lower_bound( _type );
		itEnd = mDB.upper_bound( _type );
	}

	while ( it != itEnd )
	{
		if ( it->second->mTarget == _target )
		{
			mDB.erase( it++ );
			++iCount;
		}
		else
		{
			++it;
		}
	}

	return iCount;
}

int BlackBoard::RemoveAllBBRecords( int _type /*= bbk_All*/ )
{
	int iCount = (int)mDB.size();
	if ( _type == bbk_All )
	{
		mDB.clear();
	}
	else
	{
		mDB.erase( _type );
	}
	return iCount;
}

void BlackBoard::PurgeExpiredRecords()
{
	BlackBoardDatabase::iterator it = mDB.begin();
	BlackBoardDatabase::iterator itEnd = mDB.end();
	while ( it != itEnd )
	{
		if ( it->second->mDeleteOnExpire && it->second->mExpireTime <= IGame::GetTime() )
		{
			mDB.erase( it++ );
		}
		else if ( it->second->mDeleteOnRefCount1 && it->second.use_count() <= 1 )
		{
			mDB.erase( it++ );
		}
		else
		{
			++it;
		}
	}
}

void BlackBoard::DumpBlackBoardContentsToGame( int _type /*= bbk_All*/ )
{
	BlackBoardDatabase::iterator it = mDB.begin();
	BlackBoardDatabase::iterator itEnd = mDB.end();

	EngineFuncs::ConsoleMessage( "-= Global Blackboard =-" );
	for ( ; it != itEnd; ++it )
	{
		if ( it->first == bbk_All || it->first == _type )
		{
			EngineFuncs::ConsoleMessage( va( "Type: %d, Poster: %d, Target: %d",
				it->first, it->second->mOwner, it->second->mTarget ).c_str() );
		}
	}
	EngineFuncs::ConsoleMessage( "-= End Global Blackboard =-" );
}

//////////////////////////////////////////////////////////////////////////

int BlackBoard::MakeKey()
{
	return g_NextScriptItem++;
}

BBRecordPtr BlackBoard::AllocRecord( int _type )
{
	BBRecordPtr ptr;
	switch ( _type )
	{
		case bbk_DelayGoal:
			ptr.reset( new bbDelayGoal );
			break;
		case bbk_IsTaken:
			ptr.reset( new bbIsTaken );
			break;

			//////////////////////////////////////////////////////////////////////////
		case bbk_All:
		default:
			if ( _type >= bbk_FirstScript && _type < g_NextScriptItem )
				ptr.reset( new bbScriptItem( _type ) );
	}
	return ptr;
}

//////////////////////////////////////////////////////////////////////////
static int GM_CDECL gmfPostRecord( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );
	GM_CHECK_INT_PARAM( type, 0 );
	GM_CHECK_TABLE_PARAM( props, 1 );

	BBRecordPtr bbr = BlackBoard::AllocRecord( type );
	if ( bbr )
	{
		bbr->FromScriptTable( a_thread->GetMachine(), props );
		gBlackboard.PostBBRecord( bbr );
	}
	else
	{
		GM_EXCEPTION_MSG( "Invalid Blackboard Item Type" );
		return GM_EXCEPTION;
	}
	return GM_OK;
}

static int GM_CDECL gmfGetRecords( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( type, 0 );

	gmTableObject *pRecords = NULL;

	enum
	{
		MaxRecords = 64
	};
	BBRecordPtr records[ MaxRecords ];
	int c = gBlackboard.GetBBRecords( type, records, MaxRecords );
	if ( c > 0 )
	{
		DisableGCInScope gcEn( a_thread->GetMachine() );

		pRecords = a_thread->GetMachine()->AllocTableObject();
		for ( int i = 0; i < c; ++i )
		{
			gmTableObject *pTbl = a_thread->GetMachine()->AllocTableObject();
			if ( records[ i ]->ToScriptTable( a_thread->GetMachine(), pTbl ) )
				pRecords->Set( a_thread->GetMachine(), i, gmVariable( pTbl ) );
		}
		a_thread->PushTable( pRecords );
	}
	else
		a_thread->PushNull();
	return GM_OK;
}
static int GM_CDECL gmfGetNumRecords( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 1 );
	GM_CHECK_INT_PARAM( type, 0 );
	GM_INT_PARAM( target, 1, bbk_All );
	a_thread->PushInt( gBlackboard.GetNumBBRecords( type, target ) );
	return GM_OK;
}
static int GM_CDECL gmfRemoveByPoster( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	GM_CHECK_INT_PARAM( poster, 0 );
	GM_INT_PARAM( type, 1, bbk_All );
	a_thread->PushInt( gBlackboard.RemoveBBRecordByPoster( poster, type ) );
	return GM_OK;
}

static int GM_CDECL gmfRemoveByTarget( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	GM_CHECK_INT_PARAM( target, 0 );
	GM_INT_PARAM( type, 1, bbk_All );
	a_thread->PushInt( gBlackboard.RemoveBBRecordByTarget( target, type ) );
	return GM_OK;
}
static int GM_CDECL gmfRecordExistsOwner( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );
	GM_CHECK_INT_PARAM( type, 0 );
	GM_CHECK_INT_PARAM( owner, 1 );
	gBlackboard.RecordExistsOwner( type, owner );
	return GM_OK;
}

static int GM_CDECL gmfRecordExistsTarget( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 2 );
	GM_CHECK_INT_PARAM( type, 0 );
	GM_CHECK_INT_PARAM( target, 1 );
	gBlackboard.RecordExistsTarget( type, target );
	return GM_OK;
}

static int GM_CDECL gmfPrint( gmThread *a_thread )
{
	GM_CHECK_NUM_PARAMS( 0 );
	GM_INT_PARAM( type, 0, bbk_All );
	gBlackboard.DumpBlackBoardContentsToGame( type );
	return GM_OK;
}

void BlackBoard::Bind( gmMachine *a_machine )
{
	gmBind2::Global( a_machine, "Blackboard" )
		.func( BlackBoard::MakeKey, "MakeKey"/*,"Creates a new unique key for blackboard data."*/ )
		.func( gmfRecordExistsOwner, "RecordExistsOwner"/*,"Checks if a record of a certain type exists for a specific owner id."*/ )
		.func( gmfRecordExistsTarget, "RecordExistsOwner"/*,"Checks if a record of a certain type exists for a specific target id."*/ )
		.func( gmfPostRecord, "PostRecord"/*,"Post a record to the blackboard of a given type."*/ )
		.func( gmfGetRecords, "GetRecords"/*,"Gets all the records of a certain type."*/ )
		.func( gmfGetNumRecords, "GetNumRecords"/*,"Gets the number of records of a certain type on an optional target id."*/ )
		.func( gmfRemoveByPoster, "RemoveByPoster"/*, "Remove records by a certain owner and optional message type"*/ )
		.func( gmfRemoveByTarget, "RemoveByTarget"/*, "Remove records by a certain target and optional message type"*/ )
		.func( gmfPrint, "PrintBlackboard"/*,"Prints all blackboard records, filtering by an optional type."*/ )
		;
}