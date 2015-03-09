// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistCircle3Circle3.h"
#include "Wm5PolynomialRoots.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistCircle3Circle3<Real>::DistCircle3Circle3 (const Circle3<Real>& circle0,
    const Circle3<Real>& circle1)
    :
    mCircle0(&circle0),
    mCircle1(&circle1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Circle3<Real>& DistCircle3Circle3<Real>::GetCircle0 () const
{
    return *mCircle0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Circle3<Real>& DistCircle3Circle3<Real>::GetCircle1 () const
{
    return *mCircle1;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistCircle3Circle3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistCircle3Circle3<Real>::GetSquared ()
{
    Vector3<Real> diff = mCircle1->Center - mCircle0->Center;
    Real u0u1 = mCircle0->Direction0.Dot(mCircle1->Direction0);
    Real u0v1 = mCircle0->Direction0.Dot(mCircle1->Direction1);
    Real v0u1 = mCircle0->Direction1.Dot(mCircle1->Direction0);
    Real v0v1 = mCircle0->Direction1.Dot(mCircle1->Direction1);

    Real a0 = -diff.Dot(mCircle0->Direction0);
    Real a1 = -mCircle1->Radius*u0u1;
    Real a2 = -mCircle1->Radius*u0v1;
    Real a3 = diff.Dot(mCircle0->Direction1);
    Real a4 = mCircle1->Radius*v0u1;
    Real a5 = mCircle1->Radius*v0v1;

    Real b0 = -diff.Dot(mCircle1->Direction0);
    Real b1 = mCircle0->Radius*u0u1;
    Real b2 = mCircle0->Radius*v0u1;
    Real b3 = diff.Dot(mCircle1->Direction1);
    Real b4 = -mCircle0->Radius*u0v1;
    Real b5 = -mCircle0->Radius*v0v1;

    // Compute polynomial p0 = p00+p01*z+p02*z^2.
    Polynomial1<Real> p0(2);
    p0[0] = a2*b1 - a5*b2;
    p0[1] = a0*b4 - a3*b5;
    p0[2] = a5*b2 - a2*b1 + a1*b4 - a4*b5;

    // Compute polynomial p1 = p10+p11*z.
    Polynomial1<Real> p1(1);
    p1[0] = a0*b1 - a3*b2;
    p1[1] = a1*b1 - a5*b5 + a2*b4 - a4*b2;

    // Compute polynomial q0 = q00+q01*z+q02*z^2.
    Polynomial1<Real> q0(2);
    q0[0] = a0*a0 + a2*a2 + a3*a3 + a5*a5;
    q0[1] = ((Real)2)*(a0*a1 + a3*a4);
    q0[2] = a1*a1 - a2*a2 + a4*a4 - a5*a5;

    // Compute polynomial q1 = q10+q11*z.
    Polynomial1<Real> q1(1);
    q1[0] = ((Real)2)*(a0*a2 + a3*a5);
    q1[1] = ((Real)2)*(a1*a2 + a4*a5);

    // Compute coefficients of r0 = r00+r02*z^2.
    Polynomial1<Real> r0(2);
    r0[0] = b0*b0;
    r0[1] = (Real)0;
    r0[2] = b3*b3 - b0*b0;

    // Compute polynomial r1 = r11*z.
    Polynomial1<Real> r1(1);
    r1[0] = (Real)0;
    r1[1] = ((Real)2)*b0*b3;

    // Compute polynomial g0 = g00+g01*z+g02*z^2+g03*z^3+g04*z^4.
    Polynomial1<Real> g0(4);
    g0[0] = p0[0]*p0[0] + p1[0]*p1[0] - q0[0]*r0[0];
    g0[1] = ((Real)2)*(p0[0]*p0[1] + p1[0]*p1[1]) - q0[1]*r0[0] - q1[0]*r1[1];
    g0[2] = p0[1]*p0[1] + ((Real)2)*p0[0]*p0[2] - p1[0]*p1[0] +
        p1[1]*p1[1] - q0[2]*r0[0] - q0[0]*r0[2] - q1[1]*r1[1];
    g0[3] = ((Real)2)*(p0[1]*p0[2] - p1[0]*p1[1]) - q0[1]*r0[2] + q1[0]*r1[1];
    g0[4] = p0[2]*p0[2] - p1[1]*p1[1] - q0[2]*r0[2] + q1[1]*r1[1];

    // Compute polynomial g1 = g10+g11*z+g12*z^2+g13*z^3.
    Polynomial1<Real> g1(3);
    g1[0] = ((Real)2)*p0[0]*p1[0] - q1[0]*r0[0];
    g1[1] = ((Real)2)*(p0[1]*p1[0] + p0[0]*p1[1]) - q1[1]*r0[0] - q0[0]*r1[1];
    g1[2] = ((Real)2)*(p0[2]*p1[0] + p0[1]*p1[1]) - q1[0]*r0[2] - q0[1]*r1[1];
    g1[3] = ((Real)2)*p0[2]*p1[1] - q1[1]*r0[2] - q0[2]*r1[1];

    // Compute polynomial h = sum_{i=0}^8 h_i z^i.
    Polynomial1<Real> h(8);
    h[0] = g0[0]*g0[0] - g1[0]*g1[0];
    h[1] = ((Real)2)*(g0[0]*g0[1] - g1[0]*g1[1]);
    h[2] = g0[1]*g0[1] + g1[0]*g1[0] - g1[1]*g1[1] +
        ((Real)2)*(g0[0]*g0[2] - g1[0]*g1[2]);
    h[3] = ((Real)2)*(g0[1]*g0[2] + g0[0]*g0[3] + g1[0]*g1[1] -
        g1[1]*g1[2] - g1[0]*g1[3]);
    h[4] = g0[2]*g0[2] + g1[1]*g1[1] - g1[2]*g1[2] +
        ((Real)2)*(g0[1]*g0[3] + g0[0]*g0[4] + g1[0]*g1[2] -
        g1[1]*g1[3]);
    h[5] = ((Real)2)*(g0[2]*g0[3] + g0[1]*g0[4] + g1[1]*g1[2] +
        g1[0]*g1[3] - g1[2]*g1[3]);
    h[6] = g0[3]*g0[3] + g1[2]*g1[2] - g1[3]*g1[3] +
        ((Real)2)*(g0[2]*g0[4] + g1[1]*g1[3]);
    h[7] = ((Real)2)*(g0[3]*g0[4] + g1[2]*g1[3]);
    h[8] = g0[4]*g0[4] + g1[3]*g1[3];

    PolynomialRoots<Real> polyroots(Math<Real>::ZERO_TOLERANCE);
    polyroots.FindB(h, (Real)-1.01, (Real)1.01, 6);
    int count = polyroots.GetCount();
    const Real* roots = polyroots.GetRoots();

    Real minSqrDist = Math<Real>::MAX_REAL;
    Real cs0, sn0, cs1, sn1;

    for (int i = 0; i < count; ++i)
    {
        cs1 = roots[i];
        if (cs1 < (Real)-1)
        {
            cs1 = (Real)-1;
        }
        else if (cs1 > (Real)1)
        {
            cs1 = (Real)1;
        }

        // You can also try sn1 = -g0(cs1)/g1(cs1) to avoid the sqrt call,
        // but beware when g1 is nearly zero.  For now I use g0 and g1 to
        // determine the sign of sn1.
        sn1 = Math<Real>::Sqrt(Math<Real>::FAbs((Real)1 - cs1*cs1));

        Real g0cs1 = g0(cs1);
        Real g1cs1 = g1(cs1);
        Real product = g0cs1*g1cs1;
        if (product > (Real)0)
        {
            sn1 = -sn1;
        }
        else if (product < (Real)0)
        {
            // sn1 already has correct sign
        }
        else if (g1cs1 != (Real)0)
        {
            // g0 == 0.0
            // assert( sn1 == 0.0 );
        }
        else // g1 == 0.0
        {
            // TO DO:  When g1 = 0, there is no constraint on sn1.
            // What should be done here?  In this case, cs1 is a root
            // to the quartic equation g0(cs1) = 0.  Is there some
            // geometric significance?
            assertion(false, "Unexpected case\n");
        }

        Real m00 = a0 + a1*cs1 + a2*sn1;
        Real m01 = a3 + a4*cs1 + a5*sn1;
        Real m10 = b2*sn1 + b5*cs1;
        Real m11 = b1*sn1 + b4*cs1;
        Real det = m00*m11 - m01*m10;
        if (Math<Real>::FAbs(det) >= Math<Real>::ZERO_TOLERANCE)
        {
            Real invDet = ((Real)1)/det;
            Real lambda = -(b0*sn1 + b3*cs1);
            cs0 = lambda*m00*invDet;
            sn0 = -lambda*m01*invDet;

            // Unitize in case of numerical error.  Remove if you feel
            // confident of the accuracy for cs0 and sn0.
            Real tmp = Math<Real>::InvSqrt(cs0*cs0 + sn0*sn0);
            cs0 *= tmp;
            sn0 *= tmp;

            Vector3<Real> closest0 = mCircle0->Center +
                mCircle0->Radius*(cs0*mCircle0->Direction0 +
                sn0*mCircle0->Direction1);

            Vector3<Real> closest1 = mCircle1->Center +
                mCircle1->Radius*(cs1*mCircle1->Direction0 +
                sn1*mCircle1->Direction1);

            diff = closest1 - closest0;

            Real sqrDist = diff.SquaredLength();
            if (sqrDist < minSqrDist)
            {
                minSqrDist = sqrDist;
                mClosestPoint0 = closest0;
                mClosestPoint1 = closest1;
            }
        }
        else
        {
            // TO DO:  Handle this case.  Is there some geometric
            // significance?
            assertion(false, "Unexpected case\n");
        }
    }

    return minSqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistCircle3Circle3<Real>::Get (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedCenter0 = mCircle0->Center + t*velocity0;
    Vector3<Real> movedCenter1 = mCircle1->Center + t*velocity1;
    Circle3<Real> movedCircle0(movedCenter0, mCircle0->Direction0,
        mCircle0->Direction1, mCircle0->Normal, mCircle0->Radius);
    Circle3<Real> movedCircle1(movedCenter1, mCircle1->Direction0,
        mCircle1->Direction1, mCircle1->Normal, mCircle1->Radius);
    return DistCircle3Circle3<Real>(movedCircle0, movedCircle1).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistCircle3Circle3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedCenter0 = mCircle0->Center + t*velocity0;
    Vector3<Real> movedCenter1 = mCircle1->Center + t*velocity1;
    Circle3<Real> movedCircle0(movedCenter0, mCircle0->Direction0,
        mCircle0->Direction1, mCircle0->Normal, mCircle0->Radius);
    Circle3<Real> movedCircle1(movedCenter1, mCircle1->Direction0,
        mCircle1->Direction1, mCircle1->Normal, mCircle1->Radius);
    return DistCircle3Circle3<Real>(movedCircle0, movedCircle1).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistCircle3Circle3<float>;

template WM5_MATHEMATICS_ITEM
class DistCircle3Circle3<double>;
//----------------------------------------------------------------------------
}
