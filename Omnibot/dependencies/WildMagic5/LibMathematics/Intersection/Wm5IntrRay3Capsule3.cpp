// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrRay3Capsule3.h"
#include "Wm5DistRay3Segment3.h"
#include "Wm5IntrLine3Capsule3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrRay3Capsule3<Real>::IntrRay3Capsule3 (const Ray3<Real>& rkRay,
    const Capsule3<Real>& rkCapsule)
    :
    mRay(&rkRay),
    mCapsule(&rkCapsule)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray3<Real>& IntrRay3Capsule3<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
const Capsule3<Real>& IntrRay3Capsule3<Real>::GetCapsule () const
{
    return *mCapsule;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay3Capsule3<Real>::Test ()
{
    Real distance = DistRay3Segment3<Real>(*mRay, mCapsule->Segment).Get();
    return distance <= mCapsule->Radius;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay3Capsule3<Real>::Find ()
{
    Real t[2];
    int quantity = IntrLine3Capsule3<Real>::Find(mRay->Origin,
        mRay->Direction, *mCapsule, t);

    mQuantity = 0;
    for (int i = 0; i < quantity; ++i)
    {
        if (t[i] >= (Real)0)
        {
            mPoint[mQuantity++] = mRay->Origin + t[i]*mRay->Direction;
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
int IntrRay3Capsule3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrRay3Capsule3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrRay3Capsule3<float>;

template WM5_MATHEMATICS_ITEM
class IntrRay3Capsule3<double>;
//----------------------------------------------------------------------------
}
