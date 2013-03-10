// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntpBSplineUniform2.h"
#include "Wm5Math.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntpBSplineUniform2<Real>::IntpBSplineUniform2 (int degree, const int* dim,
    Real* data)
    :
    IntpBSplineUniform<Real>(2, degree, dim, data)
{
}
//----------------------------------------------------------------------------
template <typename Real>
int IntpBSplineUniform2<Real>::Index (int ix, int iy) const
{
    return ix + mDim[0]*iy;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpBSplineUniform2<Real>::operator() (Real* X)
{
    return (*this)(X[0], X[1]);
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpBSplineUniform2<Real>::operator() (int* dx, Real* X)
{
    return (*this)(dx[0], dx[1], X[0], X[1]);
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpBSplineUniform2<Real>::operator() (Real x, Real y)
{
    mBase[0] = (int)Math<Real>::Floor(x);
    mBase[1] = (int)Math<Real>::Floor(y);
    for (int dim = 0; dim < 2; ++dim)
    {
        if (mOldBase[dim] != mBase[dim])
        {
            // Switch to new local grid.
            for (int k = 0; k < 2; ++k)
            {
                mOldBase[k] = mBase[k];
                mGridMin[k] = mBase[k] - 1;
                mGridMax[k] = mGridMin[k] + mDegree;
            }

            // Fill in missing grid data if necessary.
            if (mEvaluateCallback)
            {
                EvaluateUnknownData();
            }

            ComputeIntermediate();
            break;
        }
    }

    SetPolynomial(0, x - mBase[0], mPoly[0]);
    SetPolynomial(0, y - mBase[1], mPoly[1]);

    int index[2] = { 0, 0 };
    Real result = (Real)0;
    for (int k = index[0] + mDp1*index[1]; k < mDp1ToN; ++k)
    {
        result += mPoly[0][index[0]]*mPoly[1][index[1]]*mInter[k];

        if (++index[0] <= mDegree)
        {
            continue;
        }
        index[0] = 0;

        index[1]++;
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpBSplineUniform2<Real>::operator() (int dx, int dy, Real x, Real y)
{
    mBase[0] = (int)Math<Real>::Floor(x);
    mBase[1] = (int)Math<Real>::Floor(y);
    for (int dim = 0; dim < 2; ++dim)
    {
        if (mOldBase[dim] != mBase[dim])
        {
            // Switch to new local grid.
            for (int k = 0; k < 2; ++k)
            {
                mOldBase[k] = mBase[k];
                mGridMin[k] = mBase[k] - 1;
                mGridMax[k] = mGridMin[k] + mDegree;
            }

            // Fill in missing grid data if necessary.
            if (mEvaluateCallback)
            {
                EvaluateUnknownData();
            }

            ComputeIntermediate();
            break;
        }
    }

    SetPolynomial(dx, x - mBase[0], mPoly[0]);
    SetPolynomial(dy, y - mBase[1], mPoly[1]);

    int index[2] = { dx, dy };
    int incr1 = dx;
    Real result = (Real)0.0;
    for (int k = index[0] + mDp1*index[1]; k < mDp1ToN; ++k)
    {
        result += mPoly[0][index[0]]*mPoly[1][index[1]]*mInter[k];

        if (++index[0] <= mDegree)
        {
            continue;
        }
        index[0] = dx;
        k += incr1;

        index[1]++;
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
void IntpBSplineUniform2<Real>::EvaluateUnknownData ()
{
    for (int k1 = mGridMin[1]; k1 <= mGridMax[1]; ++k1)
    {
        for (int k0 = mGridMin[0]; k0 <= mGridMax[0]; ++k0)
        {
            int index = Index(k0, k1);
            if (mData[index] == Math<Real>::MAX_REAL)
            {
                mData[index] = mEvaluateCallback(index);
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void IntpBSplineUniform2<Real>::ComputeIntermediate ()
{
    // Fetch subblock of data to cache.
    int delta0 = mDim[0] - mDp1;
    int loop[2];
    for (int dim = 0; dim < 2; ++dim)
    {
        loop[dim] = mGridMin[dim];
    }
    int index = Index(loop[0], loop[1]);
    int k;
    for (k = 0; k < mDp1ToN; ++k, ++index)
    {
        mCache[k] = mData[index];

        if (++loop[0] <= mGridMax[0])
        {
            continue;
        }
        loop[0] = mGridMin[0];
        index += delta0;

        loop[1]++;
    }

    // Compute and save the intermediate product.
    for (int i = 0, j = 0; i < mDp1ToN; ++i)
    {
        Real sum = (Real)0;
        for (k = 0; k < mDp1ToN; k += mSkip[j], j += mSkip[j])
        {
            sum += mProduct[j]*mCache[k];
        }
        mInter[i] = sum;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntpBSplineUniform2<float>;

template WM5_MATHEMATICS_ITEM
class IntpBSplineUniform2<double>;
//----------------------------------------------------------------------------
}
