// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrLine3Torus3.h"
#include "Wm5PolynomialRoots.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrLine3Torus3<Real>::IntrLine3Torus3 (const Line3<Real>& line,
    const Torus3<Real>& torus)
    :
    mLine(&line),
    mTorus(&torus)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& IntrLine3Torus3<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Torus3<Real>& IntrLine3Torus3<Real>::GetTorus () const
{
    return *mTorus;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine3Torus3<Real>::Find ()
{
    // Compute coefficients of quartic polynomial.
    Real ro2 = mTorus->OuterRadius*mTorus->OuterRadius;
    Real ri2 = mTorus->InnerRadius*mTorus->InnerRadius;
    Real dd = mLine->Direction.Dot(mLine->Direction);
    Real de = mLine->Origin.Dot(mLine->Direction);
    Real value = mLine->Origin.Dot(mLine->Origin) - (ro2 + ri2);

    Polynomial1<Real> poly(4);
    Real zOrigin = mLine->Origin.Z();
    Real zDir = mLine->Direction.Z();
    poly[0] = value*value - ((Real)4)*ro2*(ri2 - zOrigin*zOrigin);
    poly[1] = ((Real)4)*de*value + ((Real)8)*ro2*zDir*zOrigin;
    poly[2] = ((Real)2)*dd*value + ((Real)4)*de*de + ((Real)4)*ro2*zDir*zDir;
    poly[3] = ((Real)4)*dd*de;
    poly[4] = dd*dd;

    // Solve the quartic.
    PolynomialRoots<Real> proots(Math<Real>::ZERO_TOLERANCE);
    proots.FindB(poly, 6);
    mQuantity = proots.GetCount();
    const Real* root = proots.GetRoots();

    // Get the intersection points.
    for (int i = 0; i < mQuantity; ++i)
    {
        mPoint[i] = mLine->Origin + root[i]*mLine->Direction;
    }

    mIntersectionType = (mQuantity > 0 ? IT_POINT : IT_EMPTY);
    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrLine3Torus3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrLine3Torus3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrLine3Torus3<float>;

template WM5_MATHEMATICS_ITEM
class IntrLine3Torus3<double>;
//----------------------------------------------------------------------------
}
