// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistLine3Circle3.h"
#include "Wm5PolynomialRoots.h"
#include "Wm5DistPoint3Circle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistLine3Circle3<Real>::DistLine3Circle3 (const Line3<Real>& rkLine,
    const Circle3<Real>& rkCircle)
    :
    mLine(&rkLine),
    mCircle(&rkCircle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& DistLine3Circle3<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Circle3<Real>& DistLine3Circle3<Real>::GetCircle () const
{
    return *mCircle;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Circle3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Circle3<Real>::GetSquared ()
{
    Vector3<Real> diff = mLine->Origin - mCircle->Center;
    Real diffSqrLen = diff.SquaredLength();
    Real MdM = mLine->Direction.SquaredLength();
    Real DdM = diff.Dot(mLine->Direction);
    Real NdM = mCircle->Normal.Dot(mLine->Direction);
    Real DdN = diff.Dot(mCircle->Normal);

    Real a0 = DdM;
    Real a1 = MdM;
    Real b0 = DdM - NdM*DdN;
    Real b1 = MdM - NdM*NdM;
    Real c0 = diffSqrLen - DdN*DdN;
    Real c1 = b0;
    Real c2 = b1;
    Real rsqr = mCircle->Radius*mCircle->Radius;

    Real a0sqr = a0*a0;
    Real a1sqr = a1*a1;
    Real twoA0A1 = ((Real)2)*a0*a1;
    Real b0sqr = b0*b0;
    Real b1Sqr = b1*b1;
    Real twoB0B1 = ((Real)2)*b0*b1;
    Real twoC1 = ((Real)2)*c1;

    // The minimum point B+t*M occurs when t is a root of the quartic
    // equation whose coefficients are defined below.
    Polynomial1<Real> poly(4);
    poly[0] = a0sqr*c0 - b0sqr*rsqr;
    poly[1] = twoA0A1*c0 + a0sqr*twoC1 - twoB0B1*rsqr;
    poly[2] = a1sqr*c0 + twoA0A1*twoC1 + a0sqr*c2 - b1Sqr*rsqr;
    poly[3] = a1sqr*twoC1 + twoA0A1*c2;
    poly[4] = a1sqr*c2;

    PolynomialRoots<Real> polyroots(Math<Real>::ZERO_TOLERANCE);
    polyroots.FindB(poly, 6);
    int count = polyroots.GetCount();
    const Real* roots = polyroots.GetRoots();

    Real minSqrDist = Math<Real>::MAX_REAL;
    for (int i = 0; i < count; ++i)
    {
        // Compute distance from P(t) to circle.
        Vector3<Real> P = mLine->Origin + roots[i]*mLine->Direction;
        DistPoint3Circle3<Real> query(P, *mCircle);
        Real sqrDist = query.GetSquared();
        if (sqrDist < minSqrDist)
        {
            minSqrDist = sqrDist;
            mClosestPoint0 = query.GetClosestPoint0();
            mClosestPoint1 = query.GetClosestPoint1();
        }
    }

    return minSqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Circle3<Real>::Get (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin = mLine->Origin + t*velocity0;
    Vector3<Real> movedCenter = mCircle->Center + t*velocity1;
    Line3<Real> movedLine(movedOrigin, mLine->Direction);
    Circle3<Real> movedCircle(movedCenter, mCircle->Direction0,
        mCircle->Direction1, mCircle->Normal, mCircle->Radius);
    return DistLine3Circle3<Real>(movedLine, movedCircle).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Circle3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin = mLine->Origin + t*velocity0;
    Vector3<Real> movedCenter = mCircle->Center + t*velocity1;
    Line3<Real> movedLine(movedOrigin, mLine->Direction);
    Circle3<Real> movedCircle(movedCenter, mCircle->Direction0,
        mCircle->Direction1, mCircle->Normal, mCircle->Radius);
    return DistLine3Circle3<Real>(movedLine, movedCircle).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistLine3Circle3<float>;

template WM5_MATHEMATICS_ITEM
class DistLine3Circle3<double>;
//----------------------------------------------------------------------------
}
