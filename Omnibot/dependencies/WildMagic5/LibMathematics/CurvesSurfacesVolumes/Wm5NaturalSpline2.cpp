// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2012/06/24)

#include "Wm5MathematicsPCH.h"
#include "Wm5NaturalSpline2.h"
#include "Wm5Integrate1.h"
#include "Wm5LinearSystem.h"
#include "Wm5Polynomial1.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
NaturalSpline2<Real>::NaturalSpline2 (BoundaryType type, int numSegments,
    Real* times, Vector2<Real>* points)
    :
    MultipleCurve2<Real>(numSegments, times)
{
    mA = points;

    switch (type)
    {
        case BT_FREE:
        {
            CreateFreeSpline();
            break;
        }
        case BT_CLAMPED:
        {
            Vector2<Real> derStart = mA[1] - mA[0];
            Vector2<Real> derFinal = mA[mNumSegments] - mA[mNumSegments-1];
            CreateClampedSpline(derStart, derFinal);
            break;
        }
        case BT_CLOSED:
        {
            CreateClosedSpline();
            break;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
NaturalSpline2<Real>::NaturalSpline2 (int numSegments, Real* times,
    Vector2<Real>* points, const Vector2<Real>& derivativeStart,
    const Vector2<Real>& derivativeFinal)
    :
    MultipleCurve2<Real>(numSegments, times)
{
    mA = points;
    CreateClampedSpline(derivativeStart, derivativeFinal);
}
//----------------------------------------------------------------------------
template <typename Real>
NaturalSpline2<Real>::~NaturalSpline2 ()
{
    delete1(mA);
    delete1(mB);
    delete1(mC);
    delete1(mD);
}
//----------------------------------------------------------------------------
template <typename Real>
void NaturalSpline2<Real>::CreateFreeSpline ()
{
    Real* dt = new1<Real>(mNumSegments);
    int i;
    for (i = 0; i < mNumSegments; ++i)
    {
        dt[i] = mTimes[i+1] - mTimes[i];
    }

    Real* d2t = new1<Real>(mNumSegments);
    for (i = 1; i < mNumSegments; ++i)
    {
        d2t[i] = mTimes[i+1] - mTimes[i-1];
    }

    Vector2<Real>* alpha = new1<Vector2<Real> >(mNumSegments);
    for (i = 1; i < mNumSegments; ++i)
    {
        Vector2<Real> numer = ((Real)3)*(mA[i+1]*dt[i-1] - mA[i]*d2t[i] +
            mA[i-1]*dt[i]);
        Real invDenom = ((Real)1)/(dt[i-1]*dt[i]);
        alpha[i] = invDenom*numer;
    }

    Real* ell = new1<Real>(mNumSegments + 1);
    Real* mu = new1<Real>(mNumSegments);
    Vector2<Real>* z = new1<Vector2<Real> >(mNumSegments + 1);
    Real inv;

    ell[0] = (Real)1;
    mu[0] = (Real)0;
    z[0] = Vector2<Real>::ZERO;
    for (i = 1; i < mNumSegments; ++i)
    {
        ell[i] = ((Real)2)*d2t[i] - dt[i-1]*mu[i-1];
        inv = ((Real)1)/ell[i];
        mu[i] = inv*dt[i];
        z[i] = inv*(alpha[i] - z[i-1]*dt[i-1]);
    }
    ell[mNumSegments] = (Real)1;
    z[mNumSegments] = Vector2<Real>::ZERO;

    mB = new1<Vector2<Real> >(mNumSegments);
    mC = new1<Vector2<Real> >(mNumSegments + 1);
    mD = new1<Vector2<Real> >(mNumSegments);

    mC[mNumSegments] = Vector2<Real>::ZERO;

    const Real oneThird = ((Real)1)/(Real)3;
    for (i = mNumSegments-1; i >= 0; --i)
    {
        mC[i] = z[i] - mu[i]*mC[i+1];
        inv = ((Real)1)/dt[i];
        mB[i] = inv*(mA[i+1] - mA[i]) - oneThird*dt[i]*(mC[i+1] +
            ((Real)2)*mC[i]);
        mD[i] = oneThird*inv*(mC[i+1] - mC[i]);
    }

    delete1(dt);
    delete1(d2t);
    delete1(alpha);
    delete1(ell);
    delete1(mu);
    delete1(z);
}
//----------------------------------------------------------------------------
template <typename Real>
void NaturalSpline2<Real>::CreateClampedSpline (
    const Vector2<Real>& derivativeStart, const Vector2<Real>& derivativeFinal)
{
    Real* dt = new1<Real>(mNumSegments);
    int i;
    for (i = 0; i < mNumSegments; ++i)
    {
        dt[i] = mTimes[i+1] - mTimes[i];
    }

    Real* d2t = new1<Real>(mNumSegments);
    for (i = 1; i < mNumSegments; ++i)
    {
        d2t[i] = mTimes[i+1] - mTimes[i-1];
    }

    Vector2<Real>* alpha = new1<Vector2<Real> >(mNumSegments + 1);
    Real inv = ((Real)1)/dt[0];
    alpha[0] = ((Real)3)*(inv*(mA[1] - mA[0]) - derivativeStart);
    inv = ((Real)1)/dt[mNumSegments-1];
    alpha[mNumSegments] = ((Real)3)*(derivativeFinal -
        inv*(mA[mNumSegments] - mA[mNumSegments-1]));
    for (i = 1; i < mNumSegments; ++i)
    {
        Vector2<Real> numer = ((Real)3)*(mA[i+1]*dt[i-1] -
            mA[i]*d2t[i] + mA[i-1]*dt[i]);
        Real invDenom = ((Real)1)/(dt[i-1]*dt[i]);
        alpha[i] = invDenom*numer;
    }

    Real* ell = new1<Real>(mNumSegments + 1);
    Real* mu = new1<Real>(mNumSegments);
    Vector2<Real>* z = new1<Vector2<Real> >(mNumSegments + 1);

    ell[0] = ((Real)2)*dt[0];
    mu[0] = (Real)0.5;
    inv = ((Real)1)/ell[0];
    z[0] = inv*alpha[0];

    for (i = 1; i < mNumSegments; ++i)
    {
        ell[i] = ((Real)2)*d2t[i] - dt[i-1]*mu[i-1];
        inv = ((Real)1)/ell[i];
        mu[i] = inv*dt[i];
        z[i] = inv*(alpha[i] - z[i-1]*dt[i-1]);
    }
    ell[mNumSegments] = dt[mNumSegments-1]*((Real)2 - mu[mNumSegments-1]);
    inv = ((Real)1)/ell[mNumSegments];
    z[mNumSegments] = inv*(alpha[mNumSegments] -
        z[mNumSegments-1]*dt[mNumSegments-1]);

    mB = new1<Vector2<Real> >(mNumSegments);
    mC = new1<Vector2<Real> >(mNumSegments + 1);
    mD = new1<Vector2<Real> >(mNumSegments);

    mC[mNumSegments] = z[mNumSegments];

    const Real oneThird = ((Real)1)/(Real)3;
    for (i = mNumSegments-1; i >= 0; --i)
    {
        mC[i] = z[i] - mu[i]*mC[i+1];
        inv = ((Real)1)/dt[i];
        mB[i] = inv*(mA[i+1] - mA[i]) - oneThird*dt[i]*(mC[i+1] +
            ((Real)2)*mC[i]);
        mD[i] = oneThird*inv*(mC[i+1] - mC[i]);
    }

    delete1(dt);
    delete1(d2t);
    delete1(alpha);
    delete1(ell);
    delete1(mu);
    delete1(z);
}
//----------------------------------------------------------------------------
template <typename Real>
void NaturalSpline2<Real>::CreateClosedSpline ()
{
    // TO DO.  A general linear system solver is used here.  The matrix
    // corresponding to this case is actually "cyclic banded", so a faster
    // linear solver can be used.  The current linear system code does not
    // have such a solver.

    Real* dt = new1<Real>(mNumSegments);
    int i;
    for (i = 0; i < mNumSegments; ++i)
    {
        dt[i] = mTimes[i+1] - mTimes[i];
    }

    // Construct matrix of system.
    GMatrix<Real> mat(mNumSegments+1, mNumSegments+1);
    mat[0][0] = (Real)1;
    mat[0][mNumSegments] = (Real)-1;
    for (i = 1; i <= mNumSegments-1; ++i)
    {
        mat[i][i-1] = dt[i-1];
        mat[i][i  ] = ((Real)2)*(dt[i-1] + dt[i]);
        mat[i][i+1] = dt[i];
    }
    mat[mNumSegments][mNumSegments-1] = dt[mNumSegments-1];
    mat[mNumSegments][0] = ((Real)2)*(dt[mNumSegments-1] + dt[0]);
    mat[mNumSegments][1] = dt[0];

    // Construct right-hand side of system.
    mC = new1<Vector2<Real> >(mNumSegments + 1);
    mC[0] = Vector2<Real>::ZERO;
    Real inv0, inv1;
    for (i = 1; i <= mNumSegments-1; ++i)
    {
        inv0 = ((Real)1)/dt[i];
        inv1 = ((Real)1)/dt[i-1];
        mC[i] = ((Real)3)*(inv0*(mA[i+1] - mA[i]) - inv1*(mA[i] - mA[i-1]));
    }
    inv0 = ((Real)1)/dt[0];
    inv1 = ((Real)1)/dt[mNumSegments-1];
    mC[mNumSegments] = ((Real)3)*(inv0*(mA[1] - mA[0]) -
        inv1*(mA[0] - mA[mNumSegments-1]));

    // Solve the linear systems.
    Real* input = new1<Real>(mNumSegments + 1);
    Real* output = new1<Real>(mNumSegments + 1);

    for (i = 0; i <= mNumSegments; ++i)
    {
        input[i] = mC[i].X();
    }
    LinearSystem<Real>().Solve(mat, input, output);
    for (i = 0; i <= mNumSegments; ++i)
    {
        mC[i].X() = output[i];
    }

    for (i = 0; i <= mNumSegments; ++i)
    {
        input[i] = mC[i].Y();
    }
    LinearSystem<Real>().Solve(mat, input, output);
    for (i = 0; i <= mNumSegments; ++i)
    {
        mC[i].Y() = output[i];
    }

    delete1(input);
    delete1(output);
    // End linear system solving.

    const Real oneThird = ((Real)1)/(Real)3;
    mB = new1<Vector2<Real> >(mNumSegments);
    mD = new1<Vector2<Real> >(mNumSegments);
    for (i = 0; i < mNumSegments; ++i)
    {
        inv0 = ((Real)1)/dt[i];
        mB[i] = inv0*(mA[i+1] - mA[i]) - oneThird*(mC[i+1] +
            ((Real)2)*mC[i])*dt[i];
        mD[i] = oneThird*inv0*(mC[i+1] - mC[i]);
    }

    delete1(dt);
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>* NaturalSpline2<Real>::GetPoints () const
{
    return mA;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> NaturalSpline2<Real>::GetPosition (Real t) const
{
    int key;
    Real dt;
    GetKeyInfo(t, key, dt);
    return mA[key] + dt*(mB[key] + dt*(mC[key] + dt*mD[key]));
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> NaturalSpline2<Real>::GetFirstDerivative (Real t) const
{
    int key;
    Real dt;
    GetKeyInfo(t, key, dt);
    return mB[key] + dt*(mC[key]*((Real)2) + mD[key]*(((Real)3)*dt));
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> NaturalSpline2<Real>::GetSecondDerivative (Real t) const
{
    int key;
    Real dt;
    GetKeyInfo(t, key, dt);
    return mC[key]*((Real)2) + mD[key]*(((Real)6)*dt);
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> NaturalSpline2<Real>::GetThirdDerivative (Real t) const
{
    int key;
    Real dt;
    GetKeyInfo(t, key, dt);
    return ((Real)6)*mD[key];
}
//----------------------------------------------------------------------------
template <typename Real>
Real NaturalSpline2<Real>::GetSpeedKey (int key, Real t) const
{
    Vector2<Real> velocity = mB[key] + t*(mC[key]*((Real)2) +
        mD[key]*(((Real)3)*t));

    return velocity.Length();
}
//----------------------------------------------------------------------------
template <typename Real>
Real NaturalSpline2<Real>::GetLengthKey (int key, Real t0, Real t1) const
{
    SplineKey data(this, key);
    return Integrate1<Real>::RombergIntegral(8, t0, t1, GetSpeedWithData,
        (void*)&data);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// NaturalSpline2::SplineKey
//----------------------------------------------------------------------------
template <typename Real>
NaturalSpline2<Real>::SplineKey::SplineKey (const NaturalSpline2* spline,
    int key)
    :
    Spline(spline),
    Key(key)
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class NaturalSpline2<float>;

template WM5_MATHEMATICS_ITEM
class NaturalSpline2<double>;
//----------------------------------------------------------------------------
}
