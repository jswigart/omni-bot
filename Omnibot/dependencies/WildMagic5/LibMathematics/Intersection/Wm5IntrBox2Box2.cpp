// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrBox2Box2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrBox2Box2<Real>::IntrBox2Box2 (const Box2<Real>& box0,
    const Box2<Real>& box1)
    :
    mBox0(&box0),
    mBox1(&box1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Box2<Real>& IntrBox2Box2<Real>::GetBox0 () const
{
    return *mBox0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Box2<Real>& IntrBox2Box2<Real>::GetBox1 () const
{
    return *mBox1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrBox2Box2<Real>::Test ()
{
    // Convenience variables.
    const Vector2<Real>* A = mBox0->Axis;
    const Vector2<Real>* B = mBox1->Axis;
    const Real* EA = mBox0->Extent;
    const Real* EB = mBox1->Extent;

    // Compute difference of box centers, D = C1-C0.
    Vector2<Real> D = mBox1->Center - mBox0->Center;

    Real AbsAdB[2][2], AbsAdD, RSum;
    
    // axis C0+t*A0
    AbsAdB[0][0] = Math<Real>::FAbs(A[0].Dot(B[0]));
    AbsAdB[0][1] = Math<Real>::FAbs(A[0].Dot(B[1]));
    AbsAdD = Math<Real>::FAbs(A[0].Dot(D));
    RSum = EA[0] + EB[0]*AbsAdB[0][0] + EB[1]*AbsAdB[0][1];
    if (AbsAdD > RSum)
    {
        return false;
    }

    // axis C0+t*A1
    AbsAdB[1][0] = Math<Real>::FAbs(A[1].Dot(B[0]));
    AbsAdB[1][1] = Math<Real>::FAbs(A[1].Dot(B[1]));
    AbsAdD = Math<Real>::FAbs(A[1].Dot(D));
    RSum = EA[1] + EB[0]*AbsAdB[1][0] + EB[1]*AbsAdB[1][1];
    if (AbsAdD > RSum)
    {
        return false;
    }

    // axis C0+t*B0
    AbsAdD = Math<Real>::FAbs(B[0].Dot(D));
    RSum = EB[0] + EA[0]*AbsAdB[0][0] + EA[1]*AbsAdB[1][0];
    if (AbsAdD > RSum)
    {
        return false;
    }

    // axis C0+t*B1
    AbsAdD = Math<Real>::FAbs(B[1].Dot(D));
    RSum = EB[1] + EA[0]*AbsAdB[0][1] + EA[1]*AbsAdB[1][1];
    if (AbsAdD > RSum)
    {
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrBox2Box2<float>;

template WM5_MATHEMATICS_ITEM
class IntrBox2Box2<double>;
//----------------------------------------------------------------------------
}
