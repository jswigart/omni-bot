// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Segment3<Real>::Segment3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Segment3<Real>::~Segment3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Segment3<Real>::Segment3 (const Vector3<Real>& p0, const Vector3<Real>& p1)
    :
    P0(p0),
    P1(p1)
{
    ComputeCenterDirectionExtent();
}
//----------------------------------------------------------------------------
template <typename Real>
Segment3<Real>::Segment3 (const Vector3<Real>& center,
    const Vector3<Real>& direction, Real extent)
    :
    Center(center),
    Direction(direction),
    Extent(extent)
{
    ComputeEndPoints();
}
//----------------------------------------------------------------------------
template <typename Real>
void Segment3<Real>::ComputeCenterDirectionExtent ()
{
    Center = ((Real)0.5)*(P0 + P1);
    Direction = P1 - P0;
    Extent = ((Real)0.5)*Direction.Normalize();
}
//----------------------------------------------------------------------------
template <typename Real>
void Segment3<Real>::ComputeEndPoints ()
{
    P0 = Center - Extent*Direction;
    P1 = Center + Extent*Direction;
}
//----------------------------------------------------------------------------
