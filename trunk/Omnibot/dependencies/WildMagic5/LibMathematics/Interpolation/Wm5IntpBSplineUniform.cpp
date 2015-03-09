// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntpBSplineUniform.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntpBSplineUniform<Real>::IntpBSplineUniform (int dims, int degree,
    const int* dim, Real* data)
{
    // Get input data.
    assertion(dims > 0 && degree > 0 && dim && data, "Invalid input\n");
    int i;
    for (i = 0; i < dims; ++i)
    {
        assertion(dim[i] > degree+1,  "Invalid input\n");
    }

    mDims = dims;
    mDegree = degree;
    mDim = new1<int>(mDims);
    memcpy(mDim, dim, mDims*sizeof(int));
    mData = data;

    // Setup degree constants.
    mDp1 = mDegree + 1;
    mDp1ToN = 1;
    for (i = 0; i < mDims; ++i)
    {
        mDp1ToN *= mDp1;
    }
    mDp1To2N = mDp1ToN*mDp1ToN;

    // Compute domain [min,max] for B-spline.
    mDomMin = new1<Real>(mDims);
    mDomMax = new1<Real>(mDims);
    for (i = 0; i < mDims; ++i)
    {
        Real domSup = (Real)(mDim[i] - mDegree + 1);
        Real next = ((Real)0.5)*((Real)1 + domSup);
        do
        {
            mDomMax[i] = next;
            next = ((Real)0.5)*(next + domSup);
        }
        while (next < domSup);
        mDomMin[i] = (Real)1;
    }

    // Initialize grid extremes.
    mGridMin = new1<int>(mDims);
    mGridMax = new1<int>(mDims);
    for (i = 0; i < mDims; ++i)
    {
        mGridMin[i] = -1;
        mGridMax[i] = -1;
    }

    // Initialize base indices.
    mBase = new1<int>(mDims);
    mOldBase = new1<int>(mDims);
    for (i = 0; i < mDims; ++i)
    {
        mOldBase[i] = -1;
    }

    // Generate spline blending matrix.
    mMatrix = BlendMatrix(mDegree);

    // Cache for optimizing ComputeIntermediate().
    mCache = new1<Real>(mDp1ToN);

    // Storage for intermediate tensor product.
    mInter = new1<Real>(mDp1ToN);

    // Polynomial allocations.
    mPoly = new1<Real*>(mDims);
    for (i = 0; i < mDims; ++i)
    {
        mPoly[i] = new1<Real>(mDp1);
    }

    // Coefficients for polynomial calculations.
    mCoeff = new1<Real*>(mDp1);
    for (int row = 0; row <= mDegree; ++row)
    {
        mCoeff[row] = new1<Real>(mDp1);
        for (int col = row; col <= mDegree; ++col)
        {
            mCoeff[row][col] = (Real)1;
            for (i = 0; i <= row-1; ++i)
            {
                mCoeff[row][col] *= (Real)(col - i);
            }
        }
    }

    // Tensor product of m with itself N times.
    mProduct = new1<Real>(mDp1To2N);
    mSkip = new1<int>(mDp1To2N);
    int* coord = new1<int>(2*mDims);  // for address decoding
    int j;
    for (j = 0; j < mDp1To2N; ++j)
    {
        int temp = j;
        for (i = 0; i < 2*mDims; ++i)
        {
            coord[i] = temp % mDp1;
            temp /= mDp1;
        }

        mProduct[j] = (Real)1;
        for (i = 0; i < mDims; ++i)
        {
            mProduct[j] *= mMatrix[coord[i]][coord[i+mDims]];
        }

        mSkip[j] = 1;
    }
    delete1(coord);

    // Compute increments to skip zero values of mtensor.
    for (i = 0; i < mDp1To2N; /**/)
    {
        for (j = i+1; j < mDp1To2N && mProduct[j] == (Real)0; ++j)
        {
            mSkip[i]++;
        }
        i = j;
    }

    mEvaluateCallback = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
IntpBSplineUniform<Real>::~IntpBSplineUniform ()
{
    delete1(mDim);
    delete1(mDomMin);
    delete1(mDomMax);
    delete1(mGridMin);
    delete1(mGridMax);
    delete1(mBase);
    delete1(mOldBase);
    delete1(mCache);
    delete1(mInter);
    delete1(mProduct);
    delete1(mSkip);

    int i;
    for (i = 0; i < mDims; ++i)
    {
        delete1(mPoly[i]);
    }
    delete1(mPoly);

    for (i = 0; i <= mDegree; ++i)
    {
        delete1(mMatrix[i]);
        delete1(mCoeff[i]);
    }
    delete1(mMatrix);
    delete1(mCoeff);
}
//----------------------------------------------------------------------------
template <typename Real>
int IntpBSplineUniform<Real>::GetDimension () const
{
    return mDims;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntpBSplineUniform<Real>::GetDegree () const
{
    return mDegree;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpBSplineUniform<Real>::GetDomainMin (int i) const
{
    return mDomMin[i];
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpBSplineUniform<Real>::GetDomainMax (int i) const
{
    return mDomMax[i];
}
//----------------------------------------------------------------------------
template <typename Real>
int IntpBSplineUniform<Real>::GetGridMin (int i) const
{
    return mGridMin[i];
}
//----------------------------------------------------------------------------
template <typename Real>
int IntpBSplineUniform<Real>::GetGridMax (int i) const
{
    return mGridMax[i];
}
//----------------------------------------------------------------------------
template <typename Real>
void IntpBSplineUniform<Real>::SetPolynomial (int order, Real diff,
    Real* poly)
{
    Real diffPower = (Real)1;
    for (int i = order; i <= mDegree; ++i)
    {
        poly[i] = mCoeff[order][i]*diffPower;
        diffPower *= diff;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
int IntpBSplineUniform<Real>::Choose (int n, int k)
{
    // Computes combination "n choose k".
    if (n <= 1 || k >= n)
    {
        return 1;
    }

    int result = 1;
    int i;
    for (i = 0; i < k; ++i)
    {
        result *= n-i;
    }
    for (i = 1; i <= k; ++i)
    {
        result /= i;
    }

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Real** IntpBSplineUniform<Real>::BlendMatrix (int degree)
{
    int degP1 = degree + 1;
    int row, col, i, j, k;

    // Allocate triple arrays.
    int*** AMat = new1<int**>(degP1);
    int*** BMat = new1<int**>(degP1);
    for (k = 0; k <= degree; ++k)
    {
        AMat[k] = new1<int*>(degP1);
        BMat[k] = new1<int*>(degP1);
        for (row = 0; row <= degree; ++row)
        {
            AMat[k][row] = new1<int>(degP1);
            BMat[k][row] = new1<int>(degP1);
            for (col = 0; col <= degree; ++col)
            {
                AMat[k][row][col] = 0;
                BMat[k][row][col] = 0;
            }
        }
    }

    AMat[0][0][0] = 1;
    BMat[0][0][0] = 1;

    for (k = 1; k <= degree; ++k)
    {
        // Compute A[].
        for (row = 0; row <= k; ++row)
        {
            for (col = 0; col <= k; ++col)
            {
                AMat[k][row][col] = 0;
                if (col >= 1)
                {
                    AMat[k][row][col] += AMat[k-1][row][col-1];
                    if (row >= 1)
                    {
                        AMat[k][row][col] -= BMat[k-1][row-1][col-1];
                    }
                }
                if (row >= 1)
                {
                    AMat[k][row][col] += (k+1)*BMat[k-1][row-1][col];
                }
            }
        }

        // Compute B[].
        for (row = 0; row <= k; ++row)
        {
            for (col = 0; col <= k; ++col)
            {
                BMat[k][row][col]= 0;
                for (i = col; i <= k; ++i)
                {
                    if ((i-col) % 2)
                    {
                        BMat[k][row][col] -= Choose(i, col)*AMat[k][row][i];
                    }
                    else
                    {
                        BMat[k][row][col] += Choose(i, col)*AMat[k][row][i];
                    }
                }
            }
        }
    }

    Real** CMat = new1<Real*>(degP1);
    for (row = 0; row <= degree; ++row)
    {
        CMat[row] = new1<Real>(degP1);
        for (col = 0; col <= degree; ++col)
        {
            CMat[row][col]= 0;
            for (i = col; i <= degree; ++i)
            {
                int prod = 1;
                for (j = 1; j <= i-col; ++j)
                {
                    prod *= degree-row;
                }
                CMat[row][col] += prod*Choose(i, col) *
                    AMat[degree][degree-row][i];
            }
        }
    }

    Real factorial = 1;
    for (k = 1; k <= degree; ++k)
    {
        factorial *= k;
    }
    Real invFactorial = ((Real)1)/factorial;
    Real** matrix = new1<Real*>(degP1);
    for (row = 0; row <= degree; ++row)
    {
        matrix[row] = new1<Real>(degP1);
        for (col = 0; col <= degree; ++col)
        {
            matrix[row][col] = CMat[row][col]*invFactorial;
        }
    }

    // Deallocate triple arrays.
    for (k = 0; k <= degree; ++k)
    {
        for (row = 0; row <= degree; ++row)
        {
            delete1(BMat[k][row]);
            delete1(AMat[k][row]);
        }
        delete1(BMat[k]);
        delete1(AMat[k]);
    }
    delete1(BMat);
    delete1(AMat);

    // Deallocate integer matrix.
    for (k = 0; k <= degree; ++k)
    {
        delete1(CMat[k]);
    }
    delete1(CMat);

    return matrix;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntpBSplineUniform<float>;

template WM5_MATHEMATICS_ITEM
class IntpBSplineUniform<double>;
//----------------------------------------------------------------------------
}
