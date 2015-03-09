// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5BezierCurve3.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
BezierCurve3<Real>::BezierCurve3 (int degree, Vector3<Real>* ctrlPoint)
    :
    SingleCurve3<Real>((Real)0, (Real)1)
{
    assertion(degree >= 2, "The degree must be three or larger\n");

    int i, j;

    mDegree = degree;
    mNumCtrlPoints = mDegree + 1;
    mCtrlPoint = ctrlPoint;

    // Compute first-order differences.
    mDer1CtrlPoint = new1<Vector3<Real> >(mNumCtrlPoints - 1);
    for (i = 0; i < mNumCtrlPoints - 1; ++i)
    {
        mDer1CtrlPoint[i] = mCtrlPoint[i + 1] - mCtrlPoint[i];
    }

    // Compute second-order differences.
    mDer2CtrlPoint = new1<Vector3<Real> >(mNumCtrlPoints - 2);
    for (i = 0; i < mNumCtrlPoints - 2; ++i)
    {
        mDer2CtrlPoint[i] = mDer1CtrlPoint[i + 1] - mDer1CtrlPoint[i];
    }

    // Compute third-order differences.
    if (degree >= 3)
    {
        mDer3CtrlPoint = new1<Vector3<Real> >(mNumCtrlPoints - 3);
        for (i = 0; i < mNumCtrlPoints - 3; ++i)
        {
            mDer3CtrlPoint[i] = mDer2CtrlPoint[i + 1] - mDer2CtrlPoint[i];
        }
    }
    else
    {
        mDer3CtrlPoint = 0;
    }

    // Compute combinatorial values Choose(N,K), store in mChoose[N][K].
    // The values mChoose[r][c] are invalid for r < c (use only the
    // entries for r >= c).
    mChoose = new2<Real>(mNumCtrlPoints, mNumCtrlPoints);

    mChoose[0][0] = (Real)1;
    mChoose[1][0] = (Real)1;
    mChoose[1][1] = (Real)1;
    for (i = 2; i <= mDegree; ++i)
    {
        mChoose[i][0] = (Real)1;
        mChoose[i][i] = (Real)1;
        for (j = 1; j < i; ++j)
        {
            mChoose[i][j] = mChoose[i-1][j-1] + mChoose[i-1][j];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
BezierCurve3<Real>::~BezierCurve3 ()
{
    delete2(mChoose);
    delete1(mDer3CtrlPoint);
    delete1(mDer2CtrlPoint);
    delete1(mDer1CtrlPoint);
    delete1(mCtrlPoint);
}
//----------------------------------------------------------------------------
template <typename Real>
int BezierCurve3<Real>::GetDegree () const
{
    return mDegree;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>* BezierCurve3<Real>::GetControlPoints () const
{
    return mCtrlPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BezierCurve3<Real>::GetPosition (Real t) const
{
    Real oneMinusT = (Real)1 - t;
    Real powT = t;
    Vector3<Real> result = oneMinusT*mCtrlPoint[0];

    for (int i = 1; i < mDegree; ++i)
    {
        Real coeff = mChoose[mDegree][i]*powT;
        result = (result+coeff*mCtrlPoint[i])*oneMinusT;
        powT *= t;
    }

    result += powT*mCtrlPoint[mDegree];

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BezierCurve3<Real>::GetFirstDerivative (Real t) const
{
    Real oneMinusT = (Real)1 - t;
    Real powT = t;
    Vector3<Real> result = oneMinusT*mDer1CtrlPoint[0];

    int degreeM1 = mDegree - 1;
    for (int i = 1; i < degreeM1; ++i)
    {
        Real coeff = mChoose[degreeM1][i]*powT;
        result = (result+coeff*mDer1CtrlPoint[i])*oneMinusT;
        powT *= t;
    }

    result += powT*mDer1CtrlPoint[degreeM1];
    result *= Real(mDegree);

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BezierCurve3<Real>::GetSecondDerivative (Real t) const
{
    Real oneMinusT = (Real)1 - t;
    Real powT = t;
    Vector3<Real> result = oneMinusT*mDer2CtrlPoint[0];

    int degreeM2 = mDegree - 2;
    for (int i = 1; i < degreeM2; ++i)
    {
        Real coeff = mChoose[degreeM2][i]*powT;
        result = (result+coeff*mDer2CtrlPoint[i])*oneMinusT;
        powT *= t;
    }

    result += powT*mDer2CtrlPoint[degreeM2];
    result *= Real(mDegree*(mDegree-1));

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BezierCurve3<Real>::GetThirdDerivative (Real t) const
{
    if (mDegree < 3)
    {
        return Vector3<Real>::ZERO;
    }

    Real oneMinusT = (Real)1 - t;
    Real powT = t;
    Vector3<Real> result = oneMinusT*mDer3CtrlPoint[0];

    int degreeM3 = mDegree - 3;
    for (int i = 1; i < degreeM3; ++i)
    {
        Real coeff = mChoose[degreeM3][i]*powT;
        result = (result+coeff*mDer3CtrlPoint[i])*oneMinusT;
        powT *= t;
    }

    result += powT*mDer3CtrlPoint[degreeM3];
    result *= Real(mDegree*(mDegree-1)*(mDegree-2));

    return result;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class BezierCurve3<float>;

template WM5_MATHEMATICS_ITEM
class BezierCurve3<double>;
//----------------------------------------------------------------------------
}
