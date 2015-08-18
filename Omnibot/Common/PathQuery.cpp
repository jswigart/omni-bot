////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PathQuery.h"

Destination::Destination() 
	: mPosition( 0.f, 0.f, 0.f )
	, mRadius( 0.f )
{
}

Destination::Destination( const Vector3f &_pos, float _radius )
	: mPosition( _pos )
	, mRadius( _radius )
{
}

DeferredQuery::Goal::Goal()
	: mNavCost( -1.0f )
	, mThreatCost( 0.0f )
{
}

DeferredQuery::DeferredQuery()
{

}

DeferredQuery::~DeferredQuery()
{

}
