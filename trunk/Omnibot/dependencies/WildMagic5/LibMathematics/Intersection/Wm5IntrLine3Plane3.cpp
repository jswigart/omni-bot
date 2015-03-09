// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrLine3Plane3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrLine3Plane3<Real>::IntrLine3Plane3 (const Line3<Real>& line,
    const Plane3<Real>& plane)
    :
    mLine(&line),
    mPlane(&plane)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& IntrLine3Plane3<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrLine3Plane3<Real>::GetPlane () const
{
    return *mPlane;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine3Plane3<Real>::Test ()
{
    Real DdN = mLine->Direction.Dot(mPlane->Normal);
    if (Math<Real>::FAbs(DdN) > Math<Real>::ZERO_TOLERANCE)
    {
        // The line is not parallel to the plane, so they must intersect.
        // The line parameter is *not* set, since this is a test-intersection
        // query.
        mIntersectionType = IT_POINT;
        return true;
    }

    // The line and plane are parallel.  Determine if they are numerically
    // close enough to be coincident.
    Real signedDistance = mPlane->DistanceTo(mLine->Origin);
    if (Math<Real>::FAbs(signedDistance) <= Math<Real>::ZERO_TOLERANCE)
    {
        mIntersectionType = IT_LINE;
        return true;
    }

    mIntersectionType = IT_EMPTY;
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine3Plane3<Real>::Find ()
{
    Real DdN = mLine->Direction.Dot(mPlane->Normal);
    Real signedDistance = mPlane->DistanceTo(mLine->Origin);
    if (Math<Real>::FAbs(DdN) > Math<Real>::ZERO_TOLERANCE)
    {
        // The line is not parallel to the plane, so they must intersect.
        mLineParameter = -signedDistance/DdN;
        mIntersectionType = IT_POINT;
        return true;
    }

    // The Line and plane are parallel.  Determine if they are numerically
    // close enough to be coincident.
    if (Math<Real>::FAbs(signedDistance) <= Math<Real>::ZERO_TOLERANCE)
    {
        // The line is coincident with the plane, so choose t = 0 for the
        // parameter.
        mLineParameter = (Real)0;
        mIntersectionType = IT_LINE;
        return true;
    }

    mIntersectionType = IT_EMPTY;
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntrLine3Plane3<Real>::GetLineParameter () const
{
    return mLineParameter;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrLine3Plane3<float>;

template WM5_MATHEMATICS_ITEM
class IntrLine3Plane3<double>;
//----------------------------------------------------------------------------
}
