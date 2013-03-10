// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5QuadToQuadTransforms.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
HmQuadToSqr<Real>::HmQuadToSqr (const Vector2<Real>& P00,
    const Vector2<Real>& P10, const Vector2<Real>& P11,
    const Vector2<Real>& P01)
{
    // Translate to origin.
    mT = P00;
    Vector2<Real> Q10 = P10 - P00;
    Vector2<Real> Q11 = P11 - P00;
    Vector2<Real> Q01 = P01 - P00;

    Matrix2<Real> invM(Q10.X(), Q01.X(), Q10.Y(), Q01.Y());
    mM = invM.Inverse();

    // Compute where p11 is mapped to.
    Vector2<Real> corner = mM*Q11;  // = (a,b)

    // Compute homogeneous transform of quadrilateral
    // {(0,0),(1,0),(a,b),(0,1)} to square {(0,0),(1,0),(1,1),(0,1)}.
    mG.X() = (corner.Y() - (Real)1)/corner.X();
    mG.Y() = (corner.X() - (Real)1)/corner.Y();
    mD.X() = (Real)1 + mG.X();
    mD.Y() = (Real)1 + mG.Y();
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> HmQuadToSqr<Real>::Transform (const Vector2<Real>& P)
{
    Vector2<Real> prod = mM*(P - mT);
    Real invDenom = ((Real)1)/((Real)1 + mG.Dot(prod));
    Vector2<Real> result = invDenom*prod;
    result.X() *= mD.X();
    result.Y() *= mD.Y();
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
HmSqrToQuad<Real>::HmSqrToQuad (const Vector2<Real>& P00,
    const Vector2<Real>& P10, const Vector2<Real>& P11,
    const Vector2<Real>& P01)
{
    // Translate to origin.
    mT = P00;
    mM[0][0] = P10.X() - P00.X();
    mM[0][1] = P01.X() - P00.X();
    mM[1][0] = P10.Y() - P00.Y();
    mM[1][1] = P01.Y() - P00.Y();

    Matrix2<Real> invM = mM.Inverse();

    // Find point which is mapped to p11.
    Vector2<Real> corner = invM*(P11-P00);  // = (a,b)

    // Compute homogeneous transform of square {(0,0),(1,0),(1,1),(0,1)} to
    // quadrilateral {(0,0),(1,0),(a,b),(0,1)}.
    Real invDenom = ((Real)1)/(corner.X() + corner.Y() - (Real)1);
    mG.X() = invDenom*((Real)1 - corner.Y());
    mG.Y() = invDenom*((Real)1 - corner.X());
    mD.X() = invDenom*corner.X();
    mD.Y() = invDenom*corner.Y();
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> HmSqrToQuad<Real>::Transform (const Vector2<Real>& P)
{
    Real invDenom = ((Real)1)/((Real)1 + mG.Dot(P));
    Vector2<Real> result(mD.X()*P.X(), mD.Y()*P.Y());
    Vector2<Real> prod = mM*result;
    result.X() = invDenom*prod.X() + mT.X();
    result.Y() = invDenom*prod.Y() + mT.Y();
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
BiQuadToSqr<Real>::BiQuadToSqr (const Vector2<Real>& P00,
    const Vector2<Real>& P10, const Vector2<Real>& P11,
    const Vector2<Real>& P01)
    :
    mP00(P00)
{
    mB = P10 - P00;
    mC = P01 - P00;
    mD = P11 + P00 - P10 - P01;
    mBC = mB.DotPerp(mC);
    mBD = mB.DotPerp(mD);
    mCD = mC.DotPerp(mD);
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> BiQuadToSqr<Real>::Transform (const Vector2<Real>& P)
{
    Vector2<Real> A = mP00 - P;
    Real AB = A.DotPerp(mB);
    Real AC = A.DotPerp(mC);

    // 0 = ac*bc+(bc^2+ac*bd-ab*cd)*s+bc*bd*s^2 = k0 + k1*s + k2*s^2
    Real k0 = AC*mBC;
    Real k1 = mBC*mBC + AC*mBD - AB*mCD;
    Real k2 = mBC*mBD;

    if (Math<Real>::FAbs(k2) >= Math<Real>::ZERO_TOLERANCE)
    {
        // The s-equation is quadratic.
        Real inv = ((Real)0.5)/k2;
        Real discr = k1*k1 - ((Real)4)*k0*k2;
        Real root = Math<Real>::Sqrt(Math<Real>::FAbs(discr));

        Vector2<Real> result0;
        result0.X() = (-k1 - root)*inv;
        result0.Y() = AB/(mBC + mBD*result0.X());
        Real deviation0 = Deviation(result0);
        if (deviation0 == (Real)0)
        {
            return result0;
        }

        Vector2<Real> result1;
        result1.X() = (-k1 + root)*inv;
        result1.Y() = AB/(mBC + mBD*result1.X());
        Real deviation1 = Deviation(result1);
        if (deviation1 == (Real)0)
        {
            return result1;
        }

        if (deviation0 <= deviation1)
        {
            if (deviation0 <= Math<Real>::ZERO_TOLERANCE)
            {
                return result0;
            }
        }
        else
        {
            if (deviation1 <= Math<Real>::ZERO_TOLERANCE)
            {
                return result1;
            }
        }
    }
    else
    {
        // The s-equation is linear.
        Vector2<Real> result;

        result.X() = -k0/k1;
        result.Y() = AB/(mBC + mBD*result.X());
        Real deviation = Deviation(result);
        if (deviation <= Math<Real>::ZERO_TOLERANCE)
        {
            return result;
        }
    }

    // Point is outside the quadrilateral, return invalid.
    return Vector2<Real>(Math<Real>::MAX_REAL, Math<Real>::MAX_REAL);
}
//----------------------------------------------------------------------------
template <typename Real>
Real BiQuadToSqr<Real>::Deviation (const Vector2<Real>& SPoint)
{
    // Deviation is the squared distance of the point from the unit square.
    Real deviation = (Real)0;
    Real delta;

    if (SPoint.X() < (Real)0)
    {
        deviation += SPoint.X()*SPoint.X();
    }
    else if (SPoint.X() > (Real)1)
    {
        delta = SPoint.X() - (Real)1;
        deviation += delta*delta;
    }

    if (SPoint.Y() < (Real)0)
    {
        deviation += SPoint.Y()*SPoint.Y();
    }
    else if (SPoint.Y() > (Real)1)
    {
        delta = SPoint.Y() - (Real)1;
        deviation += delta*delta;
    }

    return deviation;
}
//----------------------------------------------------------------------------
template <typename Real>
BiSqrToQuad<Real>::BiSqrToQuad (const Vector2<Real>& P00,
    const Vector2<Real>& P10, const Vector2<Real>& P11,
    const Vector2<Real>& P01)
{
    mS00 = P00;
    mS10 = P10;
    mS11 = P11;
    mS01 = P01;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> BiSqrToQuad<Real>::Transform (const Vector2<Real>& P)
{
    Vector2<Real> oneMinusP((Real)1 - P.X(), (Real)1 - P.Y());
    Vector2<Real> result;
    result.X() = oneMinusP.Y()*(oneMinusP.X()*mS00.X() + P.X()*mS10.X()) +
        P.Y()*(oneMinusP.X()*mS01.X() + P.X()*mS11.X());
    result.Y() = oneMinusP.Y()*(oneMinusP.X()*mS00.Y() + P.X()*mS10.Y()) +
        P.Y()*(oneMinusP.X()*mS01.Y() + P.X()*mS11.Y());
    return result;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class HmQuadToSqr<float>;

template WM5_MATHEMATICS_ITEM
class HmSqrToQuad<float>;

template WM5_MATHEMATICS_ITEM
class BiQuadToSqr<float>;

template WM5_MATHEMATICS_ITEM
class BiSqrToQuad<float>;

template WM5_MATHEMATICS_ITEM
class HmQuadToSqr<double>;

template WM5_MATHEMATICS_ITEM
class HmSqrToQuad<double>;

template WM5_MATHEMATICS_ITEM
class BiQuadToSqr<double>;

template WM5_MATHEMATICS_ITEM
class BiSqrToQuad<double>;
//----------------------------------------------------------------------------
}
