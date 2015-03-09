// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntpBSplineUniform4.h"
#include "Wm5Math.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntpBSplineUniform4<Real>::IntpBSplineUniform4 (int degree, const int* dim,
    Real* data)
    :
    IntpBSplineUniform<Real>(4, degree, dim, data)
{
}
//----------------------------------------------------------------------------
template <typename Real>
int IntpBSplineUniform4<Real>::Index (int ix, int iy, int iz, int iw) const
{
    return ix + mDim[0]*(iy + mDim[1]*(iz + mDim[2]*iw));
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpBSplineUniform4<Real>::operator() (Real* X)
{
    return (*this)(X[0], X[1], X[2], X[3]);
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpBSplineUniform4<Real>::operator() (int* dx, Real* X)
{
    return (*this)(dx[0], dx[1], dx[2], dx[3], X[0], X[1], X[2], X[3]);
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpBSplineUniform4<Real>::operator() (Real x, Real y, Real z, Real w)
{
    mBase[0] = (int)Math<Real>::Floor(x);
    mBase[1] = (int)Math<Real>::Floor(y);
    mBase[2] = (int)Math<Real>::Floor(z);
    mBase[3] = (int)Math<Real>::Floor(w);
    for (int dim = 0; dim < 4; dim++)
    {
        if (mOldBase[dim] != mBase[dim])
        {
            // Switch to new local grid.
            for (int k = 0; k < 4; ++k)
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
    SetPolynomial(0, z - mBase[2], mPoly[2]);
    SetPolynomial(0, w - mBase[3], mPoly[3]);

    int index[4] = { 0, 0, 0, 0 };
    Real result = (Real)0;
    for (int k = 0; k < mDp1ToN; ++k)
    {
        result += mPoly[0][index[0]]*mPoly[1][index[1]]*
            mPoly[2][index[2]]*mPoly[3][index[3]]*mInter[k];

        if (++index[0] <= mDegree)
        {
            continue;
        }
        index[0] = 0;

        if (++index[1] <= mDegree)
        {
            continue;
        }
        index[1] = 0;

        if (++index[2] <= mDegree)
        {
            continue;
        }
        index[2] = 0;

        index[3]++;
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpBSplineUniform4<Real>::operator() (int dx, int dy, int dz, int dw,
    Real x, Real y, Real z, Real w)
{
    mBase[0] = (int)Math<Real>::Floor(x);
    mBase[1] = (int)Math<Real>::Floor(y);
    mBase[2] = (int)Math<Real>::Floor(z);
    mBase[3] = (int)Math<Real>::Floor(w);
    for (int dim = 0; dim < 4; ++dim)
    {
        if (mOldBase[dim] != mBase[dim])
        {
            // Switch to new local grid.
            for (int k = 0; k < 4; ++k)
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
    SetPolynomial(dz, z - mBase[2], mPoly[2]);
    SetPolynomial(dw, w - mBase[3], mPoly[3]);

    int index[4] = { dx, dy, dz, dw };
    int delta[3] =  { dx, mDp1*dy, mDp1*mDp1*dz };
    Real result = (Real)0;
    int k = index[0] + mDp1*(index[1] + mDp1*(index[2] + mDp1*index[3]));
    for (/**/; k < mDp1ToN; ++k)
    {
        result += mPoly[0][index[0]]*mPoly[1][index[1]]*
            mPoly[2][index[2]]*mPoly[3][index[3]]*mInter[k];

        if (++index[0] <= mDegree)
        {
            continue;
        }
        index[0] = dx;
        k += delta[0];

        if (++index[1] <= mDegree)
        {
            continue;
        }
        index[1] = dy;
        k += delta[1];

        if (++index[2] <= mDegree)
        {
            continue;
        }
        index[2] = dz;
        k += delta[2];

        index[3]++;
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
void IntpBSplineUniform4<Real>::EvaluateUnknownData ()
{
    for (int k3 = mGridMin[3]; k3 <= mGridMax[3]; ++k3)
    {
        for (int k2 = mGridMin[2]; k2 <= mGridMax[2]; ++k2)
        {
            for (int k1 = mGridMin[1]; k1 <= mGridMax[1]; ++k1)
            {
                for (int k0 = mGridMin[0]; k0 <= mGridMax[0]; ++k0)
                {
                    int index = Index(k0,k1,k2,k3);
                    if (mData[index] == Math<Real>::MAX_REAL)
                    {
                        mData[index] = mEvaluateCallback(index);
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void IntpBSplineUniform4<Real>::ComputeIntermediate ()
{
    // Fetch subblock of data to cache.
    int delta[3] =
    {
        mDim[0] - mDp1,
        mDim[0]*(mDim[1] - mDp1),
        mDim[0]*mDim[1]*(mDim[2] - mDp1)
    };
    int loop[4];
    for (int dim = 0; dim < 4; ++dim)
    {
        loop[dim] = mGridMin[dim];
    }
    int index = Index(loop[0], loop[1], loop[2], loop[3]);
    int k;
    for (k = 0; k < mDp1ToN; ++k, ++index)
    {
        mCache[k] = mData[index];

        if (++loop[0] <= mGridMax[0])
        {
            continue;
        }
        loop[0] = mGridMin[0];
        index += delta[0];

        if (++loop[1] <= mGridMax[1])
        {
            continue;
        }
        loop[1] = mGridMin[1];
        index += delta[1];

        if (++loop[2] <= mGridMax[2])
        {
            continue;
        }
        loop[2] = mGridMin[2];
        index += delta[2];

        loop[3]++;
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
class IntpBSplineUniform4<float>;

template WM5_MATHEMATICS_ITEM
class IntpBSplineUniform4<double>;
//----------------------------------------------------------------------------
}
