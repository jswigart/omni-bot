// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5TCBSpline2.h"
#include "Wm5Integrate1.h"
#include "Wm5Polynomial1.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
TCBSpline2<Real>::TCBSpline2 (int numSegments, Real* times,
    Vector2<Real>* points, Real* tension, Real* continuity, Real* bias)
    :
    MultipleCurve2<Real>(numSegments,times)
{
    // TO DO.  Add 'boundary type' just as in natural splines.
    assertion(mNumSegments >= 3, "Not enough segments\n");

    // All four of these arrays have mNumSegments+1 elements.
    mPoints = points;
    mTension = tension;
    mContinuity = continuity;
    mBias = bias;

    mA = new1<Vector2<Real> >(mNumSegments);
    mB = new1<Vector2<Real> >(mNumSegments);
    mC = new1<Vector2<Real> >(mNumSegments);
    mD = new1<Vector2<Real> >(mNumSegments);

    // For now, treat the first point as if it occurred twice.
    ComputePoly(0, 0, 1, 2);

    for (int i = 1; i < mNumSegments - 1; ++i)
    {
        ComputePoly(i - 1, i, i + 1, i + 2);
    }

    // For now, treat the last point as if it occurred twice.
    ComputePoly(mNumSegments - 2, mNumSegments - 1, mNumSegments,
        mNumSegments);
}
//----------------------------------------------------------------------------
template <typename Real>
TCBSpline2<Real>::~TCBSpline2 ()
{
    delete1(mPoints);
    delete1(mTension);
    delete1(mContinuity);
    delete1(mBias);
    delete1(mA);
    delete1(mB);
    delete1(mC);
    delete1(mD);
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>* TCBSpline2<Real>::GetPoints () const
{
    return mPoints;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* TCBSpline2<Real>::GetTensions () const
{
    return mTension;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* TCBSpline2<Real>::GetContinuities () const
{
    return mContinuity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* TCBSpline2<Real>::GetBiases () const
{
    return mBias;
}
//----------------------------------------------------------------------------
template <typename Real>
void TCBSpline2<Real>::ComputePoly (int i0, int i1, int i2, int i3)
{
    Vector2<Real> diff = mPoints[i2] - mPoints[i1];
    Real dt = mTimes[i2] - mTimes[i1];

    // Build multipliers at P1.
    Real oneMinusT0 = (Real)1 - mTension[i1];
    Real oneMinusC0 = (Real)1 - mContinuity[i1];
    Real onePlusC0 = (Real)1 + mContinuity[i1];
    Real oneMinusB0 = (Real)1 - mBias[i1];
    Real onePlusB0 = (Real)1 + mBias[i1];
    Real adj0 = ((Real)2)*dt/(mTimes[i2] - mTimes[i0]);
    Real out0 = ((Real)0.5)*adj0*oneMinusT0*onePlusC0*onePlusB0;
    Real out1 = ((Real)0.5)*adj0*oneMinusT0*oneMinusC0*oneMinusB0;

    // Build outgoing tangent at P1.
    Vector2<Real> TOut = out1*diff + out0*(mPoints[i1] - mPoints[i0]);

    // Build multipliers at point P2.
    Real oneMinusT1 = (Real)1 - mTension[i2];
    Real oneMinusC1 = (Real)1 - mContinuity[i2];
    Real onePlusC1 = (Real)1 + mContinuity[i2];
    Real oneMinusB1 = (Real)1 - mBias[i2];
    Real onePlusB1 = (Real)1 + mBias[i2];
    Real adj1 = ((Real)2)*dt/(mTimes[i3] - mTimes[i1]);
    Real in0 = ((Real)0.5)*adj1*oneMinusT1*oneMinusC1*onePlusB1;
    Real in1 = ((Real)0.5)*adj1*oneMinusT1*onePlusC1*oneMinusB1;

    // Build incoming tangent at P2.
    Vector2<Real> TIn = in1*(mPoints[i3] - mPoints[i2]) + in0*diff;

    mA[i1] = mPoints[i1];
    mB[i1] = TOut;
    mC[i1] = ((Real)3)*diff - ((Real)2)*TOut - TIn;
    mD[i1] = ((Real)-2)*diff + TOut + TIn;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> TCBSpline2<Real>::GetPosition (Real t) const
{
    int key;
    Real dt;
    GetKeyInfo(t, key, dt);
    dt /= (mTimes[key + 1] - mTimes[key]);
    return mA[key] + dt*(mB[key] + dt*(mC[key] + dt*mD[key]));
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> TCBSpline2<Real>::GetFirstDerivative (Real t) const
{
    int key;
    Real dt;
    GetKeyInfo(t, key, dt);
    dt /= (mTimes[key + 1] - mTimes[key]);
    return mB[key] + dt*(mC[key]*((Real)2) + mD[key]*(((Real)3)*dt));
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> TCBSpline2<Real>::GetSecondDerivative (Real t) const
{
    int key;
    Real dt;
    GetKeyInfo(t, key, dt);
    dt /= (mTimes[key + 1] - mTimes[key]);
    return mC[key]*((Real)2) + mD[key]*(((Real)6)*dt);
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> TCBSpline2<Real>::GetThirdDerivative (Real t) const
{
    int key;
    Real dt;
    GetKeyInfo(t, key, dt);
    dt /= (mTimes[key + 1] - mTimes[key]);
    return ((Real)6)*mD[key];
}
//----------------------------------------------------------------------------
template <typename Real>
Real TCBSpline2<Real>::GetSpeedKey (int key, Real t) const
{
    Vector2<Real> velocity = mB[key] + t*(mC[key]*((Real)2) +
        mD[key]*(((Real)3)*t));

    return velocity.Length();
}
//----------------------------------------------------------------------------
template <typename Real>
Real TCBSpline2<Real>::GetLengthKey (int key, Real t0, Real t1) const
{
    SplineKey data(this, key);
    return Integrate1<Real>::RombergIntegral(8, t0, t1, GetSpeedWithData,
        (void*)&data);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// TCBSpline2::SplineKey
//----------------------------------------------------------------------------
template <typename Real>
TCBSpline2<Real>::SplineKey::SplineKey (const TCBSpline2* spline, int key)
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
class TCBSpline2<float>;

template WM5_MATHEMATICS_ITEM
class TCBSpline2<double>;
//----------------------------------------------------------------------------
}
