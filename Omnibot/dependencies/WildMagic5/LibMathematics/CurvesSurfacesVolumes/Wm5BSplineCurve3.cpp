// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5BSplineCurve3.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
BSplineCurve3<Real>::BSplineCurve3 (int numCtrlPoints,
    const Vector3<Real>* ctrlPoint, int degree, bool loop, bool open)
    :
    SingleCurve3<Real>((Real)0, (Real)1),
    mLoop(loop)
{
    assertion(numCtrlPoints >= 2, "Invalid input\n");
    assertion(1 <= degree && degree <= numCtrlPoints-1, "Invalid input\n");

    mNumCtrlPoints = numCtrlPoints;
    mReplicate = (loop ? (open ? 1 : degree) : 0);
    CreateControl(ctrlPoint);
    mBasis.Create(mNumCtrlPoints + mReplicate, degree, open);
}
//----------------------------------------------------------------------------
template <typename Real>
BSplineCurve3<Real>::BSplineCurve3 (int numCtrlPoints,
    const Vector3<Real>* ctrlPoint, int degree, bool loop,
    const Real* knot)
    :
    SingleCurve3<Real>((Real)0, (Real)1),
    mLoop(loop)
{
    assertion(numCtrlPoints >= 2, "Invalid input\n");
    assertion(1 <= degree && degree <= numCtrlPoints-1, "Invalid input\n");

    mNumCtrlPoints = numCtrlPoints;
    mReplicate = (loop ? 1 : 0);
    CreateControl(ctrlPoint);
    mBasis.Create(mNumCtrlPoints + mReplicate, degree, knot);
}
//----------------------------------------------------------------------------
template <typename Real>
BSplineCurve3<Real>::~BSplineCurve3 ()
{
    delete1(mCtrlPoint);
}
//----------------------------------------------------------------------------
template <typename Real>
void BSplineCurve3<Real>::CreateControl (const Vector3<Real>* ctrlPoint)
{
    int newNumCtrlPoints = mNumCtrlPoints + mReplicate;

    mCtrlPoint = new1<Vector3<Real> >(newNumCtrlPoints);
    memcpy(mCtrlPoint, ctrlPoint, mNumCtrlPoints*sizeof(Vector3<Real>));

    for (int i = 0; i < mReplicate; ++i)
    {
        mCtrlPoint[mNumCtrlPoints + i] = ctrlPoint[i];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
int BSplineCurve3<Real>::GetNumCtrlPoints () const
{
    return mNumCtrlPoints;
}
//----------------------------------------------------------------------------
template <typename Real>
int BSplineCurve3<Real>::GetDegree () const
{
    return mBasis.GetDegree();
}
//----------------------------------------------------------------------------
template <typename Real>
bool BSplineCurve3<Real>::IsOpen () const
{
    return mBasis.IsOpen();
}
//----------------------------------------------------------------------------
template <typename Real>
bool BSplineCurve3<Real>::IsUniform () const
{
    return mBasis.IsUniform();
}
//----------------------------------------------------------------------------
template <typename Real>
bool BSplineCurve3<Real>::IsLoop () const
{
    return mLoop;
}
//----------------------------------------------------------------------------
template <typename Real>
void BSplineCurve3<Real>::SetControlPoint (int i, const Vector3<Real>& ctrl)
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
Vector3<Real> BSplineCurve3<Real>::GetControlPoint (int i) const
{
    if (0 <= i && i < mNumCtrlPoints)
    {
        return mCtrlPoint[i];
    }

    return Vector3<Real>(Math<Real>::MAX_REAL, Math<Real>::MAX_REAL,
        Math<Real>::MAX_REAL);
}
//----------------------------------------------------------------------------
template <typename Real>
void BSplineCurve3<Real>::SetKnot (int i, Real knot)
{
    mBasis.SetKnot(i, knot);
}
//----------------------------------------------------------------------------
template <typename Real>
Real BSplineCurve3<Real>::GetKnot (int i) const
{
    return mBasis.GetKnot(i);
}
//----------------------------------------------------------------------------
template <typename Real>
void BSplineCurve3<Real>::Get (Real t, Vector3<Real>* pos,
    Vector3<Real>* der1, Vector3<Real>* der2, Vector3<Real>* der3) const
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
    else
    {
        mBasis.Compute(t, 0, imin, imax);
    }

    if (pos)
    {
        *pos = Vector3<Real>::ZERO;
        for (i = imin; i <= imax; ++i)
        {
            *pos += mBasis.GetD0(i)*mCtrlPoint[i];
        }
    }

    if (der1)
    {
        *der1 = Vector3<Real>::ZERO;
        for (i = imin; i <= imax; ++i)
        {
            *der1 += mBasis.GetD1(i)*mCtrlPoint[i];
        }
    }

    if (der2)
    {
        *der2 = Vector3<Real>::ZERO;
        for (i = imin; i <= imax; ++i)
        {
            *der2 += mBasis.GetD2(i)*mCtrlPoint[i];
        }
    }

    if (der3)
    {
        *der3 = Vector3<Real>::ZERO;
        for (i = imin; i <= imax; ++i)
        {
            *der3 += mBasis.GetD3(i)*mCtrlPoint[i];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
BSplineBasis<Real>& BSplineCurve3<Real>::GetBasis ()
{
    return mBasis;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineCurve3<Real>::GetPosition (Real t) const
{
    Vector3<Real> pos;
    Get(t, &pos, 0, 0, 0);
    return pos;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineCurve3<Real>::GetFirstDerivative (Real t) const
{
    Vector3<Real> der1;
    Get(t, 0, &der1, 0, 0);
    return der1;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineCurve3<Real>::GetSecondDerivative (Real t) const
{
    Vector3<Real> der2;
    Get(t, 0, 0, &der2, 0);
    return der2;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineCurve3<Real>::GetThirdDerivative (Real t) const
{
    Vector3<Real> der3;
    Get(t, 0, 0, 0, &der3);
    return der3;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class BSplineCurve3<float>;

template WM5_MATHEMATICS_ITEM
class BSplineCurve3<double>;
//----------------------------------------------------------------------------
}
