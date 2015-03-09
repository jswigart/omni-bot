// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntpBSplineUniform1.h"
#include "Wm5Math.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntpBSplineUniform1<Real>::IntpBSplineUniform1 (int degree, int dim,
    Real* data)
    :
    IntpBSplineUniform<Real>(1, degree, &dim, data)
{
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpBSplineUniform1<Real>::operator() (Real* X)
{
    return (*this)(*X);
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpBSplineUniform1<Real>::operator() (int* dx, Real* X)
{
    return (*this)(*dx, *X);
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpBSplineUniform1<Real>::operator() (Real x)
{
    mBase[0] = (int)Math<Real>::Floor(x);
    if (mOldBase[0] != mBase[0])
    {
        // Switch to new local grid.
        mOldBase[0] = mBase[0];
        mGridMin[0] = mBase[0] - 1;
        mGridMax[0] = mGridMin[0] + mDegree;

        // Fill in missing grid data if necessary.
        if (mEvaluateCallback)
        {
            EvaluateUnknownData();
        }

        ComputeIntermediate();
    }

    SetPolynomial(0, x - mBase[0], mPoly[0]);

    Real result = (Real)0;
    for (int k = 0; k <= mDegree; ++k)
    {
        result += mPoly[0][k]*mInter[k];
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpBSplineUniform1<Real>::operator() (int dx, Real x)
{
    mBase[0] = (int)Math<Real>::Floor(x);
    if (mOldBase[0] != mBase[0])
    {
        // Switch to new local grid.
        mOldBase[0] = mBase[0];
        mGridMin[0] = mBase[0] - 1;
        mGridMax[0] = mGridMin[0] + mDegree;

        // Fill in missing grid data if necessary.
        if (mEvaluateCallback)
        {
            EvaluateUnknownData();
        }

        ComputeIntermediate();
    }

    SetPolynomial(dx, x - mBase[0], mPoly[0]);

    Real result = (Real)0;
    for (int k = dx; k <= mDegree; ++k)
    {
        result += mPoly[0][k]*mInter[k];
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
void IntpBSplineUniform1<Real>::EvaluateUnknownData ()
{
    for (int k = mGridMin[0]; k <= mGridMax[0]; ++k)
    {
        if (mData[k] == Math<Real>::MAX_REAL)
        {
            mData[k] = mEvaluateCallback(k);
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void IntpBSplineUniform1<Real>::ComputeIntermediate ()
{
    for (int k = 0; k <= mDegree; ++k)
    {
        mInter[k] = (Real)0;
        for (int i = 0, j = mBase[0]-1; i <= mDegree; ++i, ++j)
        {
            mInter[k] += mData[j]*mMatrix[i][k];
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntpBSplineUniform1<float>;

template WM5_MATHEMATICS_ITEM
class IntpBSplineUniform1<double>;
//----------------------------------------------------------------------------
}
