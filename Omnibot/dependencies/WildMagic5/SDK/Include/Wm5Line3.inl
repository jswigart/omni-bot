// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Line3<Real>::Line3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Line3<Real>::~Line3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Line3<Real>::Line3 (const Vector3<Real>& origin,
					const Vector3<Real>& direction)
					:
Origin(origin),
	Direction(direction)
{
}
//----------------------------------------------------------------------------
template <typename Real>
void Line3<Real>::FromPoints( const Vector3<Real> & p0, const Vector3<Real> & p1 )
{
	Origin = ( p0+p1 ) * (Real)0.5;
	Direction = p1-p0;
	Direction.Normalize();
}