// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrCapsule3Capsule3.h"
#include "Wm5DistSegment3Segment3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrCapsule3Capsule3<Real>::IntrCapsule3Capsule3 (
    const Capsule3<Real>& capsule0, const Capsule3<Real>& capsule1)
    :
    mCapsule0(&capsule0),
    mCapsule1(&capsule1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Capsule3<Real>& IntrCapsule3Capsule3<Real>::GetCapsule0 () const
{
    return *mCapsule0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Capsule3<Real>& IntrCapsule3Capsule3<Real>::GetCapsule1 () const
{
    return *mCapsule1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrCapsule3Capsule3<Real>::Test ()
{
    Real distance = DistSegment3Segment3<Real>(mCapsule0->Segment,
        mCapsule1->Segment).Get();
    Real rSum = mCapsule0->Radius + mCapsule1->Radius;
    return distance <= rSum;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrCapsule3Capsule3<float>;

template WM5_MATHEMATICS_ITEM
class IntrCapsule3Capsule3<double>;
//----------------------------------------------------------------------------
}
