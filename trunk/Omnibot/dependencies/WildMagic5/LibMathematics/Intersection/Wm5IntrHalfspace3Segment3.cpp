// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrHalfspace3Segment3.h"
#include "Wm5IntrUtility3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrHalfspace3Segment3<Real>::IntrHalfspace3Segment3 (
    const Plane3<Real>& halfspace, const Segment3<Real>& segment)
    :
    mHalfspace(&halfspace),
    mSegment(&segment)
{
    mQuantity = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrHalfspace3Segment3<Real>::GetHalfspace () const
{
    return *mHalfspace;
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment3<Real>& IntrHalfspace3Segment3<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrHalfspace3Segment3<Real>::Test ()
{
    Vector3<Real> segment[2] =
    {
        mSegment->P0,
        mSegment->P1
    };

    Real fmin, fmax;
    IntrAxis<Real>::GetProjection(mHalfspace->Normal, segment, fmin, fmax);
    return fmin <= mHalfspace->Constant;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrHalfspace3Segment3<Real>::Find ()
{
    // Start with the segment and clip it against the plane.
    mQuantity = 2;
    mPoint[0] = mSegment->P0;
    mPoint[1] = mSegment->P1;

    ClipConvexPolygonAgainstPlane<Real>(-mHalfspace->Normal,
        -mHalfspace->Constant, mQuantity, mPoint);
    
    return mQuantity > 0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrHalfspace3Segment3<Real>::Test (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    mContactTime = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;
    Vector3<Real> relVelocity = velocity1 - velocity0;

    Vector3<Real> segment[2] =
    {
        mSegment->P0,
        mSegment->P1
    };

    Real fmin, fmax;
    IntrAxis<Real>::GetProjection(mHalfspace->Normal, segment, fmin, fmax);

    return IntrAxis<Real>::Test(mHalfspace->Normal, relVelocity,
        -Math<Real>::MAX_REAL, mHalfspace->Constant, fmin, fmax, tmax,
        mContactTime, tlast);
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrHalfspace3Segment3<Real>::Find (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    mContactTime = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;
    Vector3<Real> relVelocity = velocity1 - velocity0;

    Vector3<Real> segment[2] =
    {
        mSegment->P0,
        mSegment->P1
    };

    IntrConfiguration<Real> cfg;
    IntrAxis<Real>::GetConfiguration(mHalfspace->Normal, segment, cfg);

    if (!IntrAxis<Real>::Test(mHalfspace->Normal, relVelocity,
        -Math<Real>::MAX_REAL, mHalfspace->Constant, cfg.mMin, cfg.mMax,
        tmax, mContactTime, tlast))
    {
        // Never intersecting.
        return false;
    }

    if (mContactTime == (Real)0)
    {
        // Intersecting now.
        return false;
    }

    // Line on positive side (right).
    if (cfg.mMap == IntrConfiguration<Real>::m11)
    {
        mQuantity = 1;
        mPoint[0] = segment[cfg.mIndex[0]];
    }
    else // cfg.mMap == IntrConfiguration<Real>::m2
    {
        mQuantity = 2;
        mPoint[0] = segment[cfg.mIndex[0]];
        mPoint[1] = segment[cfg.mIndex[1]];
    }

    // Adjust points to the correct place in time, as well.
    Vector3<Real> diff = mContactTime*velocity1;
    for (int i = 0; i < mQuantity; ++i)
    {
        mPoint[i] += diff;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrHalfspace3Segment3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrHalfspace3Segment3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrHalfspace3Segment3<float>;

template WM5_MATHEMATICS_ITEM
class IntrHalfspace3Segment3<double>;
//----------------------------------------------------------------------------
}
