// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrLine3Sphere3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrLine3Sphere3<Real>::IntrLine3Sphere3 (const Line3<Real>& line,
    const Sphere3<Real>& sphere)
    :
    mLine(&line),
    mSphere(&sphere)
{
    mQuantity = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& IntrLine3Sphere3<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Sphere3<Real>& IntrLine3Sphere3<Real>::GetSphere () const
{
    return *mSphere;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine3Sphere3<Real>::Test ()
{
    Vector3<Real> diff = mLine->Origin - mSphere->Center;
    Real a0 = diff.Dot(diff) - mSphere->Radius*mSphere->Radius;
    Real a1 = mLine->Direction.Dot(diff);
    Real discr = a1*a1 - a0;
    return discr >= (Real)0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine3Sphere3<Real>::Find ()
{
    Vector3<Real> diff = mLine->Origin - mSphere->Center;
    Real a0 = diff.Dot(diff) - mSphere->Radius*mSphere->Radius;
    Real a1 = mLine->Direction.Dot(diff);
    Real discr = a1*a1 - a0;

    if (discr < (Real)0)
    {
        mIntersectionType = IT_EMPTY;
        mQuantity = 0;
    }
    else if (discr >= Math<Real>::ZERO_TOLERANCE)
    {
        Real root = Math<Real>::Sqrt(discr);
        mLineParameter[0] = -a1 - root;
        mLineParameter[1] = -a1 + root;
        mPoint[0] = mLine->Origin + mLineParameter[0]*mLine->Direction;
        mPoint[1] = mLine->Origin + mLineParameter[1]*mLine->Direction;
        mIntersectionType = IT_SEGMENT;
        mQuantity = 2;
    }
    else
    {
        mLineParameter[0] = -a1;
        mPoint[0] = mLine->Origin + mLineParameter[0]*mLine->Direction;
        mIntersectionType = IT_POINT;
        mQuantity = 1;
    }

    return mQuantity > 0;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrLine3Sphere3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrLine3Sphere3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntrLine3Sphere3<Real>::GetLineParameter (int i) const
{
    return mLineParameter[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrLine3Sphere3<float>;

template WM5_MATHEMATICS_ITEM
class IntrLine3Sphere3<double>;
//----------------------------------------------------------------------------
}
