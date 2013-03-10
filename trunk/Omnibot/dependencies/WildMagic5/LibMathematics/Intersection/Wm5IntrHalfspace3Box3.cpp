// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrHalfspace3Box3.h"
#include "Wm5IntrUtility3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrHalfspace3Box3<Real>::IntrHalfspace3Box3 (const Plane3<Real>& halfspace,
    const Box3<Real>& box)
    :
    mHalfspace(&halfspace),
    mBox(&box)
{
    mQuantity = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrHalfspace3Box3<Real>::GetHalfspace () const
{
    return *mHalfspace;
}
//----------------------------------------------------------------------------
template <typename Real>
const Box3<Real>& IntrHalfspace3Box3<Real>::GetBox () const
{
    return *mBox;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrHalfspace3Box3<Real>::Test ()
{
    Real fmin, fmax;
    IntrAxis<Real>::GetProjection(mHalfspace->Normal, *mBox, fmin, fmax);
    return fmin <= mHalfspace->Constant;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrHalfspace3Box3<Real>::Test (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    mContactTime = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;
    Vector3<Real> relVelocity = velocity1 - velocity0;

    Real fmin, fmax;
    IntrAxis<Real>::GetProjection(mHalfspace->Normal, *mBox, fmin, fmax);

    return IntrAxis<Real>::Test(mHalfspace->Normal, relVelocity,
        -Math<Real>::MAX_REAL, mHalfspace->Constant, fmin, fmax, tmax,
        mContactTime, tlast);
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrHalfspace3Box3<Real>::Find (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    mContactTime = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;
    Vector3<Real> relVelocity = velocity1 - velocity0;

    IntrConfiguration<Real> cfg;
    IntrAxis<Real>::GetConfiguration(mHalfspace->Normal, *mBox, cfg);

    if (!IntrAxis<Real>::Test(mHalfspace->Normal, relVelocity,
        -Math<Real>::MAX_REAL, mHalfspace->Constant, cfg.mMin, cfg.mMax,
        tmax, mContactTime, tlast) )
    {
        // Never intersecting.
        return false;
    }

    if (mContactTime == (Real)0)
    {
        // Intersecting now.
        return false;
    }

    // Box on positive side (right).
    if (cfg.mMap == IntrConfiguration<Real>::m1_1)
    {
        // Point intersection.
        mQuantity = 1;
        mPoint[0] = GetPointFromIndex(cfg.mIndex[0], *mBox);
    }
    else if (cfg.mMap == IntrConfiguration<Real>::m2_2)
    {
        // Segment intersection.
        mQuantity = 2;
        mPoint[0] = GetPointFromIndex(cfg.mIndex[0], *mBox);
        mPoint[1] = GetPointFromIndex(cfg.mIndex[1], *mBox);
    }
    else // cfg.mMap == IntrConfiguration<Real>::m44
    {
        // Face intersection.
        mQuantity = 4;
        mPoint[0] = GetPointFromIndex(cfg.mIndex[0], *mBox);
        mPoint[1] = GetPointFromIndex(cfg.mIndex[1], *mBox);
        mPoint[2] = GetPointFromIndex(cfg.mIndex[2], *mBox);
        mPoint[3] = GetPointFromIndex(cfg.mIndex[3], *mBox);
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
int IntrHalfspace3Box3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrHalfspace3Box3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrHalfspace3Box3<float>;

template WM5_MATHEMATICS_ITEM
class IntrHalfspace3Box3<double>;
//----------------------------------------------------------------------------
}
