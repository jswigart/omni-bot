// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2012/07/08)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrEllipse2Ellipse2.h"
#include "Wm5IntrBox2Box2.h"
#include "Wm5PolynomialRoots.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrEllipse2Ellipse2<Real>::IntrEllipse2Ellipse2 (
    const Ellipse2<Real>& ellipse0, const Ellipse2<Real>& ellipse1)
    :
    DIGITS_ACCURACY(10),
    mEllipse0(&ellipse0),
    mEllipse1(&ellipse1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Ellipse2<Real>& IntrEllipse2Ellipse2<Real>::GetEllipse0 () const
{
    return *mEllipse0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Ellipse2<Real>& IntrEllipse2Ellipse2<Real>::GetEllipse1 () const
{
    return *mEllipse1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrEllipse2Ellipse2<Real>::Test ()
{
    return Find();
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrEllipse2Ellipse2<Real>::Find ()
{
    mQuantity = 0;

    // Test for separation of oriented bounding boxes of ellipses.  This is
    // a fast-out attempt.
    Box2<Real> box0, box1;
    box0.Center = mEllipse0->Center;
    box0.Axis[0] = mEllipse0->Axis[0];
    box0.Axis[1] = mEllipse0->Axis[1];
    box0.Extent[0] = mEllipse0->Extent[0];
    box0.Extent[1] = mEllipse0->Extent[1];
    box1.Center = mEllipse1->Center;
    box1.Axis[0] = mEllipse1->Axis[0];
    box1.Axis[1] = mEllipse1->Axis[1];
    box1.Extent[0] = mEllipse1->Extent[0];
    box1.Extent[1] = mEllipse1->Extent[1];
    if(!IntrBox2Box2<Real>(box0, box1).Test())
    {
        // The boxes do not overlap, so neither do the ellipses.
        mIntersectionType = IT_EMPTY;
        return false;
    }

    // Compute the 4th-degree polynomial whose roots lead to intersections of
    // the ellipses, and then compute its roots.
    Polynomial1<Real> poly = GetQuartic(*mEllipse0, *mEllipse1);
    PolynomialRoots<Real> proots(Math<Real>::ZERO_TOLERANCE);
    proots.FindB(poly, DIGITS_ACCURACY);
    int yCount = proots.GetCount();
    const Real* yRoot = proots.GetRoots();
    if (yCount == 0)
    {
        mIntersectionType = IT_EMPTY;
        return false;
    }

    // TODO:  Adjust the comments.
    // Compute the coefficients of a polynomial in s = sin(angle) and
    // c = cos(angle) that relates ellipse0 to ellipse1
    // affinely transformed to a circle.  The polynomial is
    //   d0 + d1*c + d2*s + d3*c^2 + d4*c*s + d5*s^2 = 0
    // where c^2 + s^2 = 1.
    Vector2<Real> C0mC1 = mEllipse0->Center - mEllipse1->Center;
    Matrix2<Real> M1;
    mEllipse1->GetM(M1);
    Vector2<Real> M1C0mC1 = M1*C0mC1;
    Vector2<Real> M1A0 = M1*mEllipse0->Axis[0];
    Vector2<Real> M1A1 = M1*mEllipse0->Axis[1];
    Real coeff[6];
    coeff[0] = M1C0mC1.Dot(C0mC1) - (Real)1;
    coeff[1] = ((Real)2)*mEllipse0->Extent[0]*(M1A0.Dot(C0mC1));
    coeff[2] = ((Real)2)*mEllipse0->Extent[1]*(M1A1.Dot(C0mC1));
    coeff[3] = mEllipse0->Extent[0]*mEllipse0->Extent[0]*
        (M1A0.Dot(mEllipse0->Axis[0]));
    coeff[4] = ((Real)2)*mEllipse0->Extent[0]*mEllipse0->Extent[1]*
        (M1A0.Dot(mEllipse0->Axis[1]));
    coeff[5] = mEllipse0->Extent[1]*mEllipse0->Extent[1]*
        (M1A1.Dot(mEllipse0->Axis[1]));

    // Evaluate the quadratics, saving the values to test later for closeness
    // to zero and for root polishing.
    Real qp0[6], qp1[6];
    mEllipse0->ToCoefficients(qp0);
    mEllipse1->ToCoefficients(qp1);
    std::vector<Measurement> measure(8);  // store <x,y,sqrt(Q0^2+S1^2)>
    Vector2<Real> point;
    int i;
    for (int iy = 0; iy < yCount; ++iy)
    {
        point[1] = yRoot[iy];
        PolynomialRoots<Real> ar(Math<Real>::ZERO_TOLERANCE);
        Polynomial1<Real> apoly(2);
        apoly[0] = qp0[0] + point[1]*(qp0[2] + point[1]*qp0[5]);
        apoly[1] = qp0[1] + point[1]*qp0[4];
        apoly[2] = qp0[3];
        ar.FindB(apoly, DIGITS_ACCURACY);
        int xCount = ar.GetCount();
        const Real* xRoot = ar.GetRoots();
        for (int ix = 0; ix < xCount; ++ix)
        {
            point[0] = xRoot[ix];
            Real q0 = mEllipse0->Evaluate(point);
            Real q1 = mEllipse1->Evaluate(point);

            Real angle0;
            bool transverse = RefinePoint(coeff, point, q0, q1, angle0);

            i = ix + 2*iy;
            measure[i].Point = point;
            measure[i].Q0 = q0;
            measure[i].Q1 = q1;
            measure[i].Norm = Math<Real>::Sqrt(q0*q0 + q1*q1);
            measure[i].Angle0 = angle0;
            measure[i].Transverse = transverse;
        }
    }
    std::sort(measure.begin(), measure.end());

    for (i = 0; i < 8; ++i)
    {
        if (measure[i].Norm < Math<Real>::ZERO_TOLERANCE)
        {
            int j;
            Real adiff;
            for (j = 0; j < mQuantity; ++j)
            {
                adiff = measure[i].Angle0 - measure[j].Angle0;
                if (Math<Real>::FAbs(adiff) < Math<Real>::ZERO_TOLERANCE)
                {
                    break;
                }
            }
            if (j == mQuantity)
            {
                mPoint[mQuantity] = measure[i].Point;
                mTransverse[mQuantity] = measure[i].Transverse;
                if (++mQuantity == 4)
                {
                    break;
                }
            }
        }
    }

    if (mQuantity == 0)
    {
        mIntersectionType = IT_EMPTY;
        return false;
    }

    mIntersectionType = IT_POINT;
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrEllipse2Ellipse2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrEllipse2Ellipse2<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
const Ellipse2<Real>& IntrEllipse2Ellipse2<Real>::GetIntersectionEllipse ()
    const
{
    return *mEllipse0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrEllipse2Ellipse2<Real>::IsTransverseIntersection (int i) const
{
    return mTransverse[i];
}
//----------------------------------------------------------------------------
template <typename Real>
typename IntrEllipse2Ellipse2<Real>::Classification
IntrEllipse2Ellipse2<Real>::GetClassification () const
{
    // Get the parameters of ellipe0.
    Vector2<Real> K0 = mEllipse0->Center;
    Matrix2<Real> R0(mEllipse0->Axis, true);
    Matrix2<Real> D0(
        ((Real)1)/(mEllipse0->Extent[0]*mEllipse0->Extent[0]),
        ((Real)1)/(mEllipse0->Extent[1]*mEllipse0->Extent[1]));

    // Get the parameters of ellipse1.
    Vector2<Real> K1 = mEllipse1->Center;
    Matrix2<Real> R1(mEllipse1->Axis, true);
    Matrix2<Real> D1(
        ((Real)1)/(mEllipse1->Extent[0]*mEllipse1->Extent[0]),
        ((Real)1)/(mEllipse1->Extent[1]*mEllipse1->Extent[1]));

    // Compute K2.
    Matrix2<Real> D0NegHalf(
        mEllipse0->Extent[0],
        mEllipse0->Extent[1]);
    Matrix2<Real> D0Half(
        ((Real)1)/mEllipse0->Extent[0],
        ((Real)1)/mEllipse0->Extent[1]);
    Vector2<Real> K2 = D0Half*((K1 - K0)*R0);

    // Compute M2.
    Matrix2<Real> R1TR0D0NegHalf = R1.TransposeTimes(R0*D0NegHalf);
    Matrix2<Real> M2 = R1TR0D0NegHalf.TransposeTimes(D1)*R1TR0D0NegHalf;

    // Factor M2 = R*D*R^T.
    Matrix2<Real> R, D;
    M2.EigenDecomposition(R, D);

    // Compute K = R^T*K2.
    Vector2<Real> K = K2*R;

    // Transformed ellipsoid0 is Z^T*Z = 1 and transformed ellipsoid1 is
    // (Z-K)^T*D*(Z-K) = 0.

    // The minimum and maximum squared distances from the origin of points on
    // transformed ellipse1 are used to determine whether the ellipses
    // intersect, are separated, or one contains the other.
    Real minSqrDistance = Math<Real>::MAX_REAL;
    Real maxSqrDistance = (Real)0;
    int i;

    if (K == Vector2<Real>::ZERO)
    {
        // The special case of common centers must be handled separately.  It
        // is not possible for the ellipsoids to be separated.
        for (i = 0; i < 2; ++i)
        {
            Real invD = ((Real)1)/D[i][i];
            if (invD < minSqrDistance)
            {
                minSqrDistance = invD;
            }
            if (invD > maxSqrDistance)
            {
                maxSqrDistance = invD;
            }
        }

        if (maxSqrDistance < (Real)1)
        {
            return EC_ELLIPSE0_CONTAINS_ELLIPSE1;
        }
        else if (minSqrDistance > (Real)1)
        {
            return EC_ELLIPSE1_CONTAINS_ELLIPSE0;
        }
        else
        {
            return EC_ELLIPSES_INTERSECTING;
        }
    }

    // The closest point P0 and farthest point P1 are solutions to
    // s0*D*(P0 - K) = P0 and s1*D1*(P1 - K) = P1 for some scalars s0 and s1
    // that are roots to the function
    //   f(s) = d0*k0^2/(d0*s-1)^2 + d1*k1^2/(d1*s-1)^2 - 1
    // where D = diagonal(d0,d1) and K = (k0,k1).
    Real d0 = D[0][0], d1 = D[1][1];
    Real c0 = K2[0]*K2[0], c1 = K2[1]*K2[1];

    // Sort the values so that d0 >= d1.  This allows us to bound the roots of
    // f(s), of which there are at most 4.
    std::vector<std::pair<Real,Real> > param(2);
    if (d0 >= d1)
    {
        param[0] = std::make_pair(d0, c0);
        param[1] = std::make_pair(d1, c1);
    }
    else
    {
        param[0] = std::make_pair(d1, c1);
        param[1] = std::make_pair(d0, c0);
    }

    std::vector<std::pair<Real,Real> > valid;
    valid.reserve(2);
    if (param[0].first > param[1].first)
    {
        // d0 > d1
        for (i = 0; i < 2; ++i)
        {
            if (param[i].second > (Real)0)
            {
                valid.push_back(param[i]);
            }
        }
    }
    else
    {
        // d0 = d1
        param[0].second += param[1].second;
        if (param[0].second > (Real)0)
        {
            valid.push_back(param[0]);
        }
    }

    size_t numValid = valid.size();
    int numRoots;
    Real roots[4];
    if (numValid == 2)
    {
        GetRoots(
            valid[0].first, valid[1].first,
            valid[0].second, valid[1].second,
            numRoots, roots);
    }
    else if (numValid == 1)
    {
        GetRoots(
            valid[0].first,
            valid[0].second,
            numRoots, roots);
    }
    else
    {
        // numValid cannot be zero because we already handled case K = 0
        assertion(false, "Unexpected condition.\n");
        return EC_ELLIPSES_INTERSECTING;
    }

    for (int i = 0; i < numRoots; ++i)
    {
        Real s = roots[i];
        Real p0 = d0*K[0]*s/(d0*s - (Real)1);
        Real p1 = d1*K[1]*s/(d1*s - (Real)1);
        Real sqrDistance = p0*p0 + p1*p1;
        if (sqrDistance < minSqrDistance)
        {
            minSqrDistance = sqrDistance;
        }
        if (sqrDistance > maxSqrDistance)
        {
            maxSqrDistance = sqrDistance;
        }
    }

    if (maxSqrDistance < (Real)1)
    {
        return EC_ELLIPSE0_CONTAINS_ELLIPSE1;
    }

    if (minSqrDistance > (Real)1)
    {
        if (d0*c0 + d1*c1 > (Real)1)
        {
            return EC_ELLIPSES_SEPARATED;
        }
        else
        {
            return EC_ELLIPSE1_CONTAINS_ELLIPSE0;
        }
    }

    return EC_ELLIPSES_INTERSECTING;
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrEllipse2Ellipse2<Real>::BisectF (Real d0, Real d1, Real d0c0,
    Real d1c1, Real smin, Real fmin, Real smax, Real fmax, Real& s, Real& f)
{
    bool increasing = (fmin < (Real)0);

    const int maxIterations = 256;
    for (int i = 0; i < maxIterations; ++i)
    {
        s = ((Real)0.5)*(smin + smax);
        if (smin < s)
        {
            if (s < smax)
            {
                Real invN0 = ((Real)1)/(d0*s - (Real)1);
                Real invN1 = ((Real)1)/(d1*s - (Real)1);
                Real invN0Sqr = invN0*invN0;
                Real invN1Sqr = invN1*invN1;
                f = d0c0*invN0Sqr + d1c1*invN1Sqr - (Real)1;
                if (f < (Real)0)
                {
                    if (increasing)
                    {
                        smin = s;
                        fmin = f;
                    }
                    else
                    {
                        smax = s;
                        fmax = f;
                    }
                }
                else if (f > (Real)0)
                {
                    if (increasing)
                    {
                        smax = s;
                        fmax = f;
                    }
                    else
                    {
                        smin = s;
                        fmin = f;
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                f = fmax;
                break;
            }
        }
        else
        {
            f = fmin;
            break;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrEllipse2Ellipse2<Real>::BisectDF (Real d0, Real d1, Real d0c0,
    Real d1c1, Real smin, Real dfmin, Real smax, Real dfmax, Real& s,
    Real& df)
{
    const int maxIterations = 256;
    for (int i = 0; i < maxIterations; ++i)
    {
        s = ((Real)0.5)*(smin + smax);
        if (smin < s)
        {
            if (s < smax)
            {
                Real invN0 = ((Real)1)/(d0*s - (Real)1);
                Real invN1 = ((Real)1)/(d1*s - (Real)1);
                Real invN0Cub = invN0*invN0*invN0;
                Real invN1Cub = invN1*invN1*invN1;
                df = ((Real)-2)*(d0*d0c0*invN0Cub + d1*d1c1*invN1Cub);
                if (df < (Real)0)
                {
                    smin = s;
                    dfmin = df;
                }
                else if (df > (Real)0)
                {
                    smax = s;
                    dfmax = df;
                }
                else
                {
                    break;
                }
            }
            else
            {
                df = dfmax;
                break;
            }
        }
        else
        {
            df = dfmin;
            break;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrEllipse2Ellipse2<Real>::GetRoots (Real d0, Real d1, Real c0,
    Real c1, int& numRoots, Real* roots)
{
    // f(s) = d0*c0/(d0*s-1)^2 + d1*c1/(d1*s-1)^2 - 1
    // with d0 > d1

    numRoots = 0;

    Real epsilon = (Real)0.001;
    Real multiplier0 = Math<Real>::Sqrt(((Real)2)/((Real)1 - epsilon));
    Real multiplier1 = Math<Real>::Sqrt(((Real)1)/((Real)1 + epsilon));
    Real d0c0 = d0*c0;
    Real d1c1 = d1*c1;
    Real sqrtd0c0 = Math<Real>::Sqrt(d0c0);
    Real sqrtd1c1 = Math<Real>::Sqrt(d1c1);
    Real invD0 = ((Real)1)/d0;
    Real invD1 = ((Real)1)/d1;
    Real temp0, temp1, smin, smax, s, fmin, fmax, f;
    Real invN0, invN1, invN0Sqr, invN1Sqr;

    // Compute root in (-infinity,1/d0).
    temp0 = ((Real)1 - multiplier0*sqrtd0c0)*invD0;
    temp1 = ((Real)1 - multiplier0*sqrtd1c1)*invD1;
    smin = (temp0 < temp1 ? temp0 : temp1);
    invN0 = ((Real)1)/(d0*smin - (Real)1);
    invN1 = ((Real)1)/(d1*smin - (Real)1);
    invN0Sqr = invN0*invN0;
    invN1Sqr = invN1*invN1;
    fmin = d0c0*invN0Sqr + d1c1*invN1Sqr - (Real)1;
    assertion(fmin < (Real)0, "Unexpected condition.\n");
    smax = ((Real)1 - multiplier1*sqrtd0c0)*invD0;
    invN0 = ((Real)1)/(d0*smax - (Real)1);
    invN1 = ((Real)1)/(d1*smax - (Real)1);
    invN0Sqr = invN0*invN0;
    invN1Sqr = invN1*invN1;
    fmax = d0c0*invN0Sqr + d1c1*invN1Sqr - (Real)1;
    assertion(fmax > (Real)0, "Unexpected condition.\n");
    BisectF(d0, d1, d0c0, d1c1, smin, fmin, smax, fmax, s, f);
    roots[numRoots++] = s;

    // Compute roots (if any) in (1/d0,1/d1).
    Real smid, fmid, df;
    BisectDF(d0, d1, d0c0, d1c1, invD0, -Math<Real>::MAX_REAL, invD1,
        Math<Real>::MAX_REAL, smid, df);
    invN0 = ((Real)1)/(d0*smid - (Real)1);
    invN1 = ((Real)1)/(d1*smid - (Real)1);
    invN0Sqr = invN0*invN0;
    invN1Sqr = invN1*invN1;
    fmid = d0c0*invN0Sqr + d1c1*invN1Sqr - (Real)1;
    if (fmid < (Real)0)
    {
        BisectF(d0, d1, d0c0, d1c1, invD0, Math<Real>::MAX_REAL, smid, fmid,
            s, f);
        roots[numRoots++] = s;
        BisectF(d0, d1, d0c0, d1c1, smid, fmid, invD1, Math<Real>::MAX_REAL,
            s, f);
        roots[numRoots++] = s;
    }

    // Compute root in (1/d1,+infinity).
    temp0 = ((Real)1 + multiplier0*sqrtd0c0)*invD0;
    temp1 = ((Real)1 + multiplier0*sqrtd1c1)*invD1;
    smax = (temp0 > temp1 ? temp0 : temp1);
    invN0 = ((Real)1)/(d0*smax - (Real)1);
    invN1 = ((Real)1)/(d1*smax - (Real)1);
    invN0Sqr = invN0*invN0;
    invN1Sqr = invN1*invN1;
    fmax = d0c0*invN0Sqr + d1c1*invN1Sqr - (Real)1;
    assertion(fmax < (Real)0, "Unexpected condition.\n");
    smin = ((Real)1 + multiplier1*sqrtd1c1)*invD1;
    invN0 = ((Real)1)/(d0*smin - (Real)1);
    invN1 = ((Real)1)/(d1*smin - (Real)1);
    invN0Sqr = invN0*invN0;
    invN1Sqr = invN1*invN1;
    fmin = d0c0*invN0Sqr + d1c1*invN1Sqr - (Real)1;
    assertion(fmin > (Real)0, "Unexpected condition.\n");
    BisectF(d0, d1, d0c0, d1c1, smin, fmin, smax, fmax, s, f);
    roots[numRoots++] = s;
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrEllipse2Ellipse2<Real>::GetRoots (Real d0, Real c0,
    int& numRoots, Real* roots)
{
    // f(s) = d0*c0/(d0*s-1)^2 - 1
    Real temp = Math<Real>::Sqrt(d0*c0);
    Real inv = ((Real)1)/d0;
    numRoots = 2;
    roots[0] = ((Real)1 - temp)*inv;
    roots[1] = ((Real)1 + temp)*inv;
}
//----------------------------------------------------------------------------
template <typename Real>
Polynomial1<Real> IntrEllipse2Ellipse2<Real>::GetQuartic (
    const Ellipse2<Real>& ellipse0, const Ellipse2<Real>& ellipse1)
{
    Real p0[6], p1[6];
    ellipse0.ToCoefficients(p0);
    ellipse1.ToCoefficients(p1);

    // The polynomials are
    //   P0 = a0 + a1*x + a2*y + a3*x^2 + a4*x*y + a5*y^2
    //      = (a0 + a2*y + a5*y^2) + (a1 + a4*y)*x + (a3)*x^2
    //      = u0(y) + u1(y)*x + u2(y)*x^2
    //   P1 = b0 + b1*x + b2*y + b3*x^2 + b4*x*y + b5*y^2
    //      = (b0 + b2*y + b5*y^2) + (b1 + b4*y)*x + (b3)*x^2
    //      = v0(y) + v1(y)*x + v2(y)*x^2
    // The Bezout determinant eliminates the variable x when solving the
    // equations P0(x,y) = 0 and P1(x,y) = 0.  We have
    //   0 = P0 = u0 + u1*x + u2*x^2
    //   0 = P1 = v0 + v1*x + v2*x^2
    //   0 = v2*P0 - u2*P1 = (u0*v2 - u2*v0) + (u1*v2 - u2*v1)*x
    //   0 = v1*P0 - u1*P1 = (u0*v1 - u1*v0) + (u2*v1 - u1*v2)*x^2
    // Solve the equation 0 = v2*P0-u2*P1 for x and substitute in the other
    // equation and simplify to
    //   Q(y) = (u0*v1-v1*u0)*(u1*v2-u2*v1) - (u0*v2-u2*v0)^2 = 0
    //        = c0 + c1*y + c2*y^2 + c3*y^3 + c4*y^4
    // Define dij = ai*bj - aj*bi for various indices i and j.  For example,
    // d01 = a0*b1-b1*a0.  The coefficients of Q(y) are
    //   c0 = d01*d13 - d30^2
    //   c1 = d01*d43 + (d04+d21)*d13 - 2*d30*d32
    //   c2 = (d04+d21)*d43 + (d24+d51)*d13 - 2*d30*d35 - d32^2
    //   c3 = (d24+d51)*d43 + d54*d13 - 2*d32*d35
    //   c4 = d54*d43 - d35^2

    Real d01 = p0[0]*p1[1] - p0[1]*p1[0];
    Real d04 = p0[0]*p1[4] - p0[4]*p1[0];
    Real d13 = p0[1]*p1[3] - p0[3]*p1[1];
    Real d21 = p0[2]*p1[1] - p0[1]*p1[2];
    Real d24 = p0[2]*p1[4] - p0[4]*p1[2];
    Real d30 = p0[3]*p1[0] - p0[0]*p1[3];
    Real d32 = p0[3]*p1[2] - p0[2]*p1[3];
    Real d35 = p0[3]*p1[5] - p0[5]*p1[3];
    Real d43 = p0[4]*p1[3] - p0[3]*p1[4];
    Real d51 = p0[5]*p1[1] - p0[1]*p1[5];
    Real d54 = p0[5]*p1[4] - p0[4]*p1[5];
    Real d04p21 = d04 + d21;
    Real d24p51 = d24 + d51;

    Polynomial1<Real> poly(4);
    poly[0] = d01*d13 - d30*d30;
    poly[1] = d01*d43 + d04p21*d13 - ((Real)2)*d30*d32;
    poly[2] = d04p21*d43 + d24p51*d13 - ((Real)2)*d30*d35 - d32*d32;
    poly[3] = d24p51*d43 + d54*d13 - ((Real)2)*d32*d35;
    poly[4] = d54*d43 - d35*d35;

    return poly;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrEllipse2Ellipse2<Real>::RefinePoint (const Real coeff[6],
    Vector2<Real>& point, Real& q0, Real& q1, Real& angle0)
{
    // The incoming polynomial is
    //   f(angle) = d0 + d1*c + d2*s + d3*c^2 + d4*c*s + d5*s^2
    // where s = sin(angle) and c = cos(angle).  The derivative is
    //   f'(angle) = -d1*s + d2*c + (d5 - d3)*2*c*s + d4*(c^2 - s^2)

    Vector2<Real> diff = point - mEllipse0->Center;
    Real cs = diff.Dot(mEllipse0->Axis[0])/mEllipse0->Extent[0];
    Real sn = diff.Dot(mEllipse0->Axis[1])/mEllipse0->Extent[1];
    Real a0 = Math<Real>::ATan2(sn, cs);
    Real f0 = coeff[0] + coeff[1]*cs + coeff[2]*sn + coeff[3]*cs*cs +
        coeff[4]*cs*sn + coeff[5]*sn*sn;
    Real df0 = -coeff[1]*sn + coeff[2]*cs +
        ((Real)2)*(coeff[5] - coeff[3])*cs*sn +
        coeff[4]*(cs*cs - sn*sn);

    Real a1 = (Real)0, f1, df1;

    // The value f0 should match q1 (to within floating-point round-off
    // error).  Try to force f0 to zero using bisection.  This requires
    // finding an angle such that the corresponding function value is
    // opposite in sign to f0.  If the search fails, the input point is
    // either a tangential intersection or not an intersection at all.
    int maxIterations = 32;
    int i;
    for (i = 0; i < maxIterations; ++i)
    {
        a1 = a0 - f0/df0;
        cs = Math<Real>::Cos(a1);
        sn = Math<Real>::Sin(a1);
        f1 = coeff[0] + coeff[1]*cs + coeff[2]*sn + coeff[3]*cs*cs +
            coeff[4]*cs*sn + coeff[5]*sn*sn;

        if (f0*f1 < (Real)0)
        {
            // Switch to bisection.
            break;
        }

        df1 = -coeff[1]*sn + coeff[2]*cs +
            ((Real)2)*(coeff[5] - coeff[3])*cs*sn +
            coeff[4]*(cs*cs - sn*sn);

        if (df1*df0 < (Real)0)
        {
            // Try a steeper slope in hopes of finding an opposite sign
            // value.
            df0 *= (Real)2;
            continue;
        }

        if (Math<Real>::FAbs(f1) < Math<Real>::FAbs(f0))
        {
            // We failed to find an opposite-sign value, but the new
            // function value is closer to zero, so try again with the
            // new value.
            a0 = a1;
            f0 = f1;
            df0 = df1;
        }
    }

    Real angle = a0;
    bool transverse;
    if (i < maxIterations)
    {
        // Apply bisection.  Determine number of iterations to get 10 digits
        // of accuracy.
        Real tmp0 = Math<Real>::Log(Math<Real>::FAbs(a1 - a0));
        Real tmp1 = ((Real)DIGITS_ACCURACY)*Math<Real>::Log((Real)10);
        Real arg = (tmp0 + tmp1)/Math<Real>::Log((Real)2);
        maxIterations = (int)(arg + (Real)0.5);
        for (i = 0; i < maxIterations; ++i)
        {
            angle = ((Real)0.5)*(a0 + a1);
            cs = Math<Real>::Cos(angle);
            sn = Math<Real>::Sin(angle);
            f1 = coeff[0] + coeff[1]*cs + coeff[2]*sn + coeff[3]*cs*cs +
                coeff[4]*cs*sn + coeff[5]*sn*sn;

            Real product = f0*f1;
            if (product < (Real)0)
            {
                a1 = angle;
            }
            else if (product > (Real)0)
            {
                a0 = angle;
                f0 = f1;
            }
            else
            {
                break;
            }
        }
        transverse = true;
    }
    else
    {
        transverse = false;
    }

    point = mEllipse0->Center +
        mEllipse0->Extent[0]*cs*mEllipse0->Axis[0] +
        mEllipse0->Extent[1]*sn*mEllipse0->Axis[1];

    q0 = mEllipse0->Evaluate(point);
    q1 = mEllipse1->Evaluate(point);
    angle0 = angle;

    return transverse;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// IntrEllipse2Ellipse2::Measurement
//----------------------------------------------------------------------------
template <typename Real>
IntrEllipse2Ellipse2<Real>::Measurement::Measurement ()
    :
    Point(Math<Real>::MAX_REAL, Math<Real>::MAX_REAL),
    Q0(Math<Real>::MAX_REAL),
    Q1(Math<Real>::MAX_REAL),
    Norm(Math<Real>::MAX_REAL),
    Angle0(Math<Real>::MAX_REAL),
    Transverse(false)
{
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrEllipse2Ellipse2<Real>::Measurement::operator< (
    const Measurement& measure) const
{
    if (Transverse == measure.Transverse)
    {
        return Norm < measure.Norm;
    }
    else
    {
        return Transverse;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrEllipse2Ellipse2<float>;

template WM5_MATHEMATICS_ITEM
class IntrEllipse2Ellipse2<double>;
//----------------------------------------------------------------------------
}
