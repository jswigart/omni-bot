// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrSegment3Capsule3.h"
#include "Wm5DistSegment3Segment3.h"
#include "Wm5IntrLine3Capsule3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrSegment3Capsule3<Real>::IntrSegment3Capsule3 (
    const Segment3<Real>& rkSegment, const Capsule3<Real>& rkCapsule)
    :
    mSegment(&rkSegment),
    mCapsule(&rkCapsule)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment3<Real>& IntrSegment3Capsule3<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
const Capsule3<Real>& IntrSegment3Capsule3<Real>::GetCapsule () const
{
    return *mCapsule;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Capsule3<Real>::Test ()
{
    Real distance = DistSegment3Segment3<Real>(*mSegment,
        mCapsule->Segment).Get();
    return distance <= mCapsule->Radius;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Capsule3<Real>::Find ()
{
    int quantity = IntrLine3Capsule3<Real>::Find(mSegment->Center,
        mSegment->Direction, *mCapsule, mParameter);

    mQuantity = 0;
    for (int i = 0; i < quantity; ++i)
    {
        if (Math<Real>::FAbs(mParameter[i]) <= mSegment->Extent)
        {
            mPoint[mQuantity++] = mSegment->Center +
                mParameter[i]*mSegment->Direction;
        }
    }

    if (mQuantity == 2)
    {
        mIntersectionType = IT_SEGMENT;
    }
    else if (mQuantity == 1)
    {
        mIntersectionType = IT_POINT;
    }
    else
    {
        mIntersectionType = IT_EMPTY;
    }

    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrSegment3Capsule3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrSegment3Capsule3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntrSegment3Capsule3<Real>::GetParameter (int i) const
{
    return mParameter[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrSegment3Capsule3<float>;

template WM5_MATHEMATICS_ITEM
class IntrSegment3Capsule3<double>;
//----------------------------------------------------------------------------
}
