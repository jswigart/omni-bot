// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5NURBSCurve2.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
NURBSCurve2<Real>::NURBSCurve2 (int numCtrlPoints,
    const Vector2<Real>* ctrlPoint, const Real* ctrlWeight, int degree,
    bool loop, bool open)
    :
    SingleCurve2<Real>((Real)0, (Real)1),
    mLoop(loop)
{
    assertion(numCtrlPoints >= 2, "Invalid input\n");
    assertion(1 <= degree && degree <= numCtrlPoints-1, "Invalid input\n");

    mNumCtrlPoints = numCtrlPoints;
    mReplicate = (loop ? (open ? 1 : degree) : 0);
    CreateControl(ctrlPoint, ctrlWeight);
    mBasis.Create(mNumCtrlPoints + mReplicate, degree, open);
}
//----------------------------------------------------------------------------
template <typename Real>
NURBSCurve2<Real>::NURBSCurve2 (int numCtrlPoints,
    const Vector2<Real>* ctrlPoint, const Real* ctrlWeight, int degree,
    bool loop, const Real* knot)
    :
    SingleCurve2<Real>((Real)0, (Real)1),
    mLoop(loop)
{
    assertion(numCtrlPoints >= 2, "Invalid input\n");
    assertion(1 <= degree && degree <= numCtrlPoints-1, "Invalid input\n");

    mNumCtrlPoints = numCtrlPoints;
    mReplicate = (loop ? 1 : 0);
    CreateControl(ctrlPoint, ctrlWeight);
    mBasis.Create(mNumCtrlPoints + mReplicate, degree, knot);
}
//----------------------------------------------------------------------------
template <typename Real>
NURBSCurve2<Real>::~NURBSCurve2 ()
{
    delete1(mCtrlPoint);
    delete1(mCtrlWeight);
}
//----------------------------------------------------------------------------
template <typename Real>
void NURBSCurve2<Real>::CreateControl (const Vector2<Real>* ctrlPoint,
    const Real* ctrlWeight)
{
    int newNumCtrlPoints = mNumCtrlPoints + mReplicate;

    mCtrlPoint = new1<Vector2<Real> >(newNumCtrlPoints);
    memcpy(mCtrlPoint, ctrlPoint, mNumCtrlPoints*sizeof(Vector2<Real>));

    mCtrlWeight = new1<Real>(newNumCtrlPoints);
    memcpy(mCtrlWeight, ctrlWeight, mNumCtrlPoints*sizeof(Real));

    for (int i = 0; i < mReplicate; ++i)
    {
        mCtrlPoint[mNumCtrlPoints + i] = ctrlPoint[i];
        mCtrlWeight[mNumCtrlPoints + i] = ctrlWeight[i];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
int NURBSCurve2<Real>::GetNumCtrlPoints () const
{
    return mNumCtrlPoints;
}
//----------------------------------------------------------------------------
template <typename Real>
int NURBSCurve2<Real>::GetDegree () const
{
    return mBasis.GetDegree();
}
//----------------------------------------------------------------------------
template <typename Real>
bool NURBSCurve2<Real>::IsOpen () const
{
    return mBasis.IsOpen();
}
//----------------------------------------------------------------------------
template <typename Real>
bool NURBSCurve2<Real>::IsUniform () const
{
    return mBasis.IsUniform();
}
//----------------------------------------------------------------------------
template <typename Real>
bool NURBSCurve2<Real>::IsLoop () const
{
    return mLoop;
}
//----------------------------------------------------------------------------
template <typename Real>
void NURBSCurve2<Real>::SetControlPoint (int i, const Vector2<Real>& ctrl)
{
    if (0 <= i && i < mNumCtrlPoints)
    {
        // Set the control point.
        mCtrlPoint[i] = ctrl;

        // Set the replicated control point.
        if (i < mReplicate)
        {
            mCtrlPoint[mNumCtrlPoints + i] = ctrl;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> NURBSCurve2<Real>::GetControlPoint (int i) const
{
    if (0 <= i && i < mNumCtrlPoints)
    {
        return mCtrlPoint[i];
    }

    return Vector2<Real>(Math<Real>::MAX_REAL, Math<Real>::MAX_REAL);
}
//----------------------------------------------------------------------------
template <typename Real>
void NURBSCurve2<Real>::SetControlWeight (int i, Real weight)
{
    if (0 <= i && i < mNumCtrlPoints)
    {
        // Set the control weight.
        mCtrlWeight[i] = weight;

        // Set the replicated control weight.
        if (i < mReplicate)
        {
            mCtrlWeight[mNumCtrlPoints + i] = weight;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Real NURBSCurve2<Real>::GetControlWeight (int i) const
{
    if (0 <= i && i < mNumCtrlPoints)
    {
        return mCtrlWeight[i];
    }

    return Math<Real>::MAX_REAL;
}
//----------------------------------------------------------------------------
template <typename Real>
void NURBSCurve2<Real>::SetKnot (int i, Real knot)
{
    mBasis.SetKnot(i, knot);
}
//----------------------------------------------------------------------------
template <typename Real>
Real NURBSCurve2<Real>::GetKnot (int i) const
{
    return mBasis.GetKnot(i);
}
//----------------------------------------------------------------------------
template <typename Real>
void NURBSCurve2<Real>::Get (Real t, Vector2<Real>* pos,
    Vector2<Real>* der1, Vector2<Real>* der2, Vector2<Real>* der3) const
{
    int i, imin, imax;
    if (der3)
    {
        mBasis.Compute(t, 0, imin, imax);
        mBasis.Compute(t, 1, imin, imax);
        mBasis.Compute(t, 2, imin, imax);
        mBasis.Compute(t, 3, imin, imax);
    }
    else if (der2)
    {
        mBasis.Compute(t, 0, imin, imax);
        mBasis.Compute(t, 1, imin, imax);
        mBasis.Compute(t, 2, imin, imax);
    }
    else if (der1)
    {
        mBasis.Compute(t, 0, imin, imax);
        mBasis.Compute(t, 1, imin, imax);
    }
    else  // pos
    {
        mBasis.Compute(t, 0, imin, imax);
    }

    Real tmp;

    // Compute position.
    Vector2<Real> X = Vector2<Real>::ZERO;
    Real w = (Real)0;
    for (i = imin; i <= imax; ++i)
    {
        tmp = mBasis.GetD0(i)*mCtrlWeight[i];
        X += tmp*mCtrlPoint[i];
        w += tmp;
    }
    Real invW = ((Real)1)/w;
    Vector2<Real> P = invW*X;
    if (pos)
    {
        *pos = P;
    }

    if (!der1 && !der2 && !der3)
    {
        return;
    }

    // Compute first derivative.
    Vector2<Real> XDer1 = Vector2<Real>::ZERO;
    Real wDer1 = (Real)0;
    for (i = imin; i <= imax; ++i)
    {
        tmp = mBasis.GetD1(i)*mCtrlWeight[i];
        XDer1 += tmp*mCtrlPoint[i];
        wDer1 += tmp;
    }
    Vector2<Real> PDer1 = invW*(XDer1 - wDer1*P);
    if (der1)
    {
        *der1 = PDer1;
    }

    if (!der2 && !der3)
    {
        return;
    }

    // Compute second derivative.
    Vector2<Real> XDer2 = Vector2<Real>::ZERO;
    Real wDer2 = (Real)0;
    for (i = imin; i <= imax; ++i)
    {
        tmp = mBasis.GetD2(i)*mCtrlWeight[i];
        XDer2 += tmp*mCtrlPoint[i];
        wDer2 += tmp;
    }
    Vector2<Real> PDer2 = invW*(XDer2 - ((Real)2)*wDer1*PDer1 - wDer2*P);
    if (der2)
    {
        *der2 = PDer2;
    }

    if (!der3)
    {
        return;
    }

    // Compute third derivative.
    Vector2<Real> XDer3 = Vector2<Real>::ZERO;
    Real wDer3 = (Real)0;
    for (i = imin; i <= imax; i++)
    {
        tmp = mBasis.GetD3(i)*mCtrlWeight[i];
        XDer3 += tmp*mCtrlPoint[i];
        wDer3 += tmp;
    }
    if (der3)
    {
        *der3 = invW*(XDer3 - ((Real)3)*wDer1*PDer2 -
            ((Real)3)*wDer2*PDer1 - wDer3*P);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
BSplineBasis<Real>& NURBSCurve2<Real>::GetBasis ()
{
    return mBasis;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> NURBSCurve2<Real>::GetPosition (Real t) const
{
    Vector2<Real> pos;
    Get(t, &pos, 0, 0, 0);
    return pos;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> NURBSCurve2<Real>::GetFirstDerivative (Real t) const
{
    Vector2<Real> der1;
    Get(t, 0, &der1, 0, 0);
    return der1;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> NURBSCurve2<Real>::GetSecondDerivative (Real t) const
{
    Vector2<Real> der2;
    Get(t, 0, 0, &der2, 0);
    return der2;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> NURBSCurve2<Real>::GetThirdDerivative (Real t) const
{
    Vector2<Real> der3;
    Get(t, 0, 0, 0, &der3);
    return der3;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class NURBSCurve2<float>;

template WM5_MATHEMATICS_ITEM
class NURBSCurve2<double>;
//----------------------------------------------------------------------------
}
