// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrPlane3Plane3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrPlane3Plane3<Real>::IntrPlane3Plane3 (
    const Plane3<Real>& rkPlane0, const Plane3<Real>& rkPlane1)
    :
    mPlane0(&rkPlane0),
    mPlane1(&rkPlane1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrPlane3Plane3<Real>::GetPlane0 () const
{
    return *mPlane0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrPlane3Plane3<Real>::GetPlane1 () const
{
    return *mPlane1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Plane3<Real>::Test ()
{
    // If Cross(N0,N1) is zero, then either planes are parallel and separated
    // or the same plane.  In both cases, 'false' is returned.  Otherwise, the
    // planes intersect.  To avoid subtle differences in reporting between
    // Test() and Find(), the same parallel test is used.  Mathematically,
    //   |Cross(N0,N1)|^2 = Dot(N0,N0)*Dot(N1,N1)-Dot(N0,N1)^2
    //                    = 1 - Dot(N0,N1)^2
    // The last equality is true since planes are required to have unit-length
    // normal vectors.  The test |Cross(N0,N1)| = 0 is the same as
    // |Dot(N0,N1)| = 1.  I test the latter condition in Test() and Find().

    Real dot = mPlane0->Normal.Dot(mPlane1->Normal);
    return Math<Real>::FAbs(dot) < (Real)1 - Math<Real>::ZERO_TOLERANCE;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Plane3<Real>::Find ()
{
    // If N0 and N1 are parallel, either the planes are parallel and separated
    // or the same plane.  In both cases, 'false' is returned.  Otherwise,
    // the intersection line is
    //   L(t) = t*Cross(N0,N1)/|Cross(N0,N1)| + c0*N0 + c1*N1
    // for some coefficients c0 and c1 and for t any real number (the line
    // parameter).  Taking dot products with the normals,
    //   d0 = Dot(N0,L) = c0*Dot(N0,N0) + c1*Dot(N0,N1) = c0 + c1*d
    //   d1 = Dot(N1,L) = c0*Dot(N0,N1) + c1*Dot(N1,N1) = c0*d + c1
    // where d = Dot(N0,N1).  These are two equations in two unknowns.  The
    // solution is
    //   c0 = (d0 - d*d1)/det
    //   c1 = (d1 - d*d0)/det
    // where det = 1 - d^2.

    Real dot = mPlane0->Normal.Dot(mPlane1->Normal);
    if (Math<Real>::FAbs(dot) >= (Real)1 - Math<Real>::ZERO_TOLERANCE)
    {
        // The planes are parallel.  Check if they are coplanar.
        Real cDiff;
        if (dot >= (Real)0)
        {
            // Normals are in same direction, need to look at c0-c1.
            cDiff = mPlane0->Constant - mPlane1->Constant;
        }
        else
        {
            // Normals are in opposite directions, need to look at c0+c1.
            cDiff = mPlane0->Constant + mPlane1->Constant;
        }

        if (Math<Real>::FAbs(cDiff) < Math<Real>::ZERO_TOLERANCE)
        {
            // Planes are coplanar.
            mIntersectionType = IT_PLANE;
            mIntrPlane = *mPlane0;
            return true;
        }

        // Planes are parallel, but distinct.
        mIntersectionType = IT_EMPTY;
        return false;
    }

    Real invDet = ((Real)1)/((Real)1 - dot*dot);
    Real c0 = (mPlane0->Constant - dot*mPlane1->Constant)*invDet;
    Real c1 = (mPlane1->Constant - dot*mPlane0->Constant)*invDet;
    mIntersectionType = IT_LINE;
    mIntrLine.Origin = c0*mPlane0->Normal + c1*mPlane1->Normal;
    mIntrLine.Direction = mPlane0->Normal.UnitCross(mPlane1->Normal);
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Plane3<Real>::Test (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Real dot = mPlane0->Normal.Dot(mPlane1->Normal);
    if (Math<Real>::FAbs(dot) < (Real)1 - Math<Real>::ZERO_TOLERANCE)
    {
        // The planes are initially intersecting.  Linear velocities will
        // not change the fact that they are intersecting.
        mContactTime = (Real)0;
        mIntersectionType = IT_LINE;
        return true;
    }

    // Check if planes are already coplanar.
    Real cDiff;
    if (dot >= (Real)0)
    {
        // Normals are in same direction, need to look at c0-c1.
        cDiff = mPlane0->Constant - mPlane1->Constant;
    }
    else
    {
        // Normals are in opposite directions, need to look at c0+c1.
        cDiff = mPlane0->Constant + mPlane1->Constant;
    }

    if (Math<Real>::FAbs(cDiff) < Math<Real>::ZERO_TOLERANCE)
    {
        // Planes are initially the same.
        mContactTime = (Real)0;
        mIntersectionType = IT_PLANE;
        return true;
    }

    // The planes are parallel and separated.  Determine when they will
    // become coplanar.
    Vector3<Real> relVelocity = velocity1 - velocity0;
    dot = mPlane0->Normal.Dot(relVelocity);
    if (Math<Real>::FAbs(dot) < Math<Real>::ZERO_TOLERANCE)
    {
        // The relative motion of the planes keeps them parallel.
        mIntersectionType = IT_EMPTY;
        return false;
    }

    mContactTime = cDiff/dot;
    if ((Real)0 <= mContactTime && mContactTime <= tmax)
    {
        // The planes are moving towards each other and will meet within the
        // specified time interval.
        mIntersectionType = IT_PLANE;
        return true;
    }

    mIntersectionType = IT_EMPTY;
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Plane3<Real>::Find (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Real dot = mPlane0->Normal.Dot(mPlane1->Normal);
    if (Math<Real>::FAbs(dot) < (Real)1 - Math<Real>::ZERO_TOLERANCE)
    {
        // The planes are initially intersecting.  Linear velocities will
        // not change the fact that they are intersecting.
        mContactTime = (Real)0;

        Real invDet = ((Real)1)/((Real)1 - dot*dot);
        Real c0 = (mPlane0->Constant - dot*mPlane1->Constant)*invDet;
        Real c1 = (mPlane1->Constant - dot*mPlane0->Constant)*invDet;
        mIntersectionType = IT_LINE;
        mIntrLine.Origin = c0*mPlane0->Normal + c1*mPlane1->Normal;
        mIntrLine.Direction = mPlane0->Normal.UnitCross(mPlane1->Normal);
        return true;
    }

    // Check if planes are already coplanar.
    Real cDiff;
    if (dot >= (Real)0)
    {
        // Normals are in same direction, need to look at c0-c1.
        cDiff = mPlane0->Constant - mPlane1->Constant;
    }
    else
    {
        // Normals are in opposite directions, need to look at c0+c1.
        cDiff = mPlane0->Constant + mPlane1->Constant;
    }

    if (Math<Real>::FAbs(cDiff) < Math<Real>::ZERO_TOLERANCE)
    {
        // Planes are initially the same.
        mContactTime = (Real)0;
        mIntersectionType = IT_PLANE;
        mIntrPlane = *mPlane0;
        return true;
    }

    // The planes are parallel and separated.  Determine when they will
    // become coplanar.
    Vector3<Real> relVelocity = velocity1 - velocity0;
    dot = mPlane0->Normal.Dot(relVelocity);
    if (Math<Real>::FAbs(dot) < Math<Real>::ZERO_TOLERANCE)
    {
        // The relative motion of the planes keeps them parallel.
        mIntersectionType = IT_EMPTY;
        return false;
    }

    mContactTime = cDiff/dot;
    if ((Real)0 <= mContactTime && mContactTime <= tmax)
    {
        // The planes are moving towards each other and will meet within the
        // specified time interval.
        mIntersectionType = IT_PLANE;
        mIntrPlane.Normal = mPlane0->Normal;
        mIntrPlane.Constant = mPlane0->Constant +
            mContactTime*(mPlane0->Normal.Dot(velocity0));
        return true;
    }

    mIntersectionType = IT_EMPTY;
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& IntrPlane3Plane3<Real>::GetIntersectionLine () const
{
    return mIntrLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrPlane3Plane3<Real>::GetIntersectionPlane () const
{
    return mIntrPlane;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrPlane3Plane3<float>;

template WM5_MATHEMATICS_ITEM
class IntrPlane3Plane3<double>;
//----------------------------------------------------------------------------
}
