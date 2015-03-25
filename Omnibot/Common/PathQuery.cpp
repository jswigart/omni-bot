////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PathPlannerBase.h"
#include "PathQuery.h"

PathQuery::PathQuery( Client *_client ) :
mClient( _client ),
mStarts( 0 ),
mGoals( 0 ),
mMaxPathDistance( Utils::FloatMax ),
mReturnPartial( false )
{
	/*for(int i = 0; i < MaxDesirabilities; ++i)
 mDesirabilities[i] = 0.f;*/
}

PathQuery &PathQuery::Starts( const DestinationVector *_starts )
{
	mStarts = _starts;
	return *this;
}

PathQuery &PathQuery::Goals( const DestinationVector *_goals )
{
	mGoals = _goals;
	return *this;
}

PathQuery &PathQuery::AddTeam( int _team )
{
	mTeam.SetFlag( _team, true );
	return *this;
}

PathQuery &PathQuery::AddClass( int _class )
{
	mClass.SetFlag( _class, true );
	return *this;
}

PathQuery &PathQuery::SetMovementCaps( const BitFlag32 &_caps )
{
	mMovementCaps = _caps;
	return *this;
}

//PathQuery &PathQuery::AddDesirabiliry(int _type, float _desir)
//{
//	if(InRangeT<int>(_type, 0, MaxDesirabilities))
//	{
//	 mDesirabilities[_type] = _desir;
//	}
//	else
//	{
//		OBASSERT(0, "Desirability Out of Range!");
//	}
//	return *this;
//}