// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprPolynomialFit2.h"
#include "Wm5LinearSystem.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
PolynomialFit2<Real>::PolynomialFit2 (int numSamples, const Real* xSamples,
    const Real* wSamples, int numPowers, const int* powers)
    :
    mNumPowers(numPowers)
{
    InitializePowers(numPowers, powers);

    mPowers = new1<int>(mNumPowers);
    memcpy(mPowers, powers, mNumPowers*sizeof(int));
    mCoefficients = new1<Real>(mNumPowers);
    GMatrix<Real> mat(mNumPowers, mNumPowers);  // initially zero
    GVector<Real> rhs(mNumPowers);  // initially zero
    GVector<Real> xPower(mNumPowers);

    int i, row, col;
    for (i = 0; i < numSamples; ++i)
    {
        // Compute relevant powers of x.  TODO: Build minimum tree for
        // powers of x?
        Real x = xSamples[i];
        Real w = wSamples[i];
        for (int j = 0; j < mNumPowers; ++j)
        {
            xPower[j] = Math<Real>::Pow(x, (Real)mPowers[j]);
        }

        for (row = 0; row < mNumPowers; ++row)
        {
            // Update the upper-triangular portion of the symmetric matrix.
            for (col = row; col < mNumPowers; ++col)
            {
                mat[row][col] += xPower[mPowers[row] + mPowers[col]];
            }

            // Update the right-hand side of the system.
            rhs[row] += w*xPower[mPowers[row]];
        }
    }

    // Copy the upper-triangular portion of the symmetric matrix to the
    // lower-triangular portion.
    for (row = 0; row < mNumPowers; ++row)
    {
        for (col = 0; col < row; ++col)
        {
            mat[row][col] = mat[col][row];
        }
    }

    // Precondition by normalizing the sums.
    Real invNumSamples = ((Real)1)/(Real)numSamples;
    for (row = 0; row < mNumPowers; ++row)
    {
        for (col = 0; col < mNumPowers; ++col)
        {
            mat[row][col] *= invNumSamples;
        }
        rhs[row] *= invNumSamples;
    }

    if (LinearSystem<Real>().Solve(mat, rhs, mCoefficients))
    {
        mSolved = true;
    }
    else
    {
        memset(mCoefficients, 0, mNumPowers*sizeof(Real));
        mSolved = false;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
PolynomialFit2<Real>::~PolynomialFit2 ()
{
    delete1(mPowers);
    delete1(mXPowers);
    delete1(mCoefficients);
}
//----------------------------------------------------------------------------
template <typename Real>
PolynomialFit2<Real>::operator bool () const
{
    return mSolved;
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialFit2<Real>::GetXMin () const
{
    return mMin[0];
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialFit2<Real>::GetXMax () const
{
    return mMax[0];
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialFit2<Real>::GetWMin () const
{
    return mMin[1];
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialFit2<Real>::GetWMax () const
{
    return mMax[1];
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialFit2<Real>::operator() (Real x) const
{
    // Transform x from the original space to [-1,1].
    x = (Real)-1 + ((Real)2)*(x - mMin[0])*mScale[0];

    // Compute relevant powers of x.
    for (int j = 1; j <= mMaxXPower; ++j)
    {
        mXPowers[j] = mXPowers[j-1] * x;
    }

    Real w = (Real)0;
    for (int i = 0; i < mNumPowers; ++i)
    {
        Real xp = mXPowers[mPowers[i]];
        w += mCoefficients[i] * xp;
    }

    // Transform w from [-1,1] back to the original space.
    w = (w + (Real)1)*mInvTwoWScale + mMin[1];
    return w;
}
//----------------------------------------------------------------------------
template <typename Real>
void PolynomialFit2<Real>::InitializePowers (int numPowers, const int* powers)
{
    // Copy the powers for use in evaluation of the fitted polynomial.
    mNumPowers = numPowers;
    mPowers = new1<int>(mNumPowers);
    memcpy(mPowers, powers, mNumPowers*sizeof(int));

    // Determine the maximum power.  Powers of x are computed up to twice the
    // powers when constructing the fitted polynomial.  Powers of x are
    // computed up to the powers for the evaluation of the fitted polynomial.
    mMaxXPower = mPowers[0];
    int i;
    for (i = 1; i < mNumPowers; ++i)
    {
        if (mPowers[i] > mMaxXPower)
        {
            mMaxXPower = mPowers[i];
        }
    }
    mXPowers = new1<Real>(2*mMaxXPower + 1);
    mXPowers[0] = (Real)1;
}
//----------------------------------------------------------------------------
template <typename Real>
void PolynomialFit2<Real>::TransformToUnit (int numSamples,
    const Real* srcSamples[2], Real* trgSamples[2])
{
    // Transform the data to [-1,1]^2 for numerical robustness.
    for (int j = 0; j < 2; ++j)
    {
        mMin[j] = srcSamples[j][0];
        mMax[j] = mMin[j];
        int i;
        for (i = 1; i < numSamples; ++i)
        {
            Real value = srcSamples[j][i];
            if (value < mMin[j])
            {
                mMin[j] = value;
            }
            else if (value > mMax[j])
            {
                mMax[j] = value;
            }
        }
        mScale[j] = (Real)1/(mMax[j] - mMin[j]);

        trgSamples[j] = new1<Real>(numSamples);
        for (i = 0; i < numSamples; ++i)
        {
            trgSamples[j][i] = (Real)-1 +
                ((Real)2)*(srcSamples[j][i] - mMin[j])*mScale[j];
        }
    }

    mInvTwoWScale = ((Real)0.5)/mScale[1];
}
//----------------------------------------------------------------------------
template <typename Real>
void PolynomialFit2<Real>::DoLeastSquaresFit (int numSamples,
    Real* trgSamples[2])
{
    // The matrix and vector for a linear system that determines the
    // coefficients of the fitted polynomial.
    GMatrix<Real> mat(mNumPowers, mNumPowers);  // initially zero
    GVector<Real> rhs(mNumPowers);  // initially zero
    mCoefficients = new1<Real>(mNumPowers);

    int row, col;
    for (int i = 0; i < numSamples; ++i)
    {
        // Compute relevant powers of x.
        Real x = trgSamples[0][i];
        Real w = trgSamples[1][i];
        for (int j = 1; j <= 2*mMaxXPower; ++j)
        {
            mXPowers[j] = mXPowers[j-1] * x;
        }

        for (row = 0; row < mNumPowers; ++row)
        {
            // Update the upper-triangular portion of the symmetric matrix.
            Real xp;
            for (col = row; col < mNumPowers; ++col)
            {
                xp = mXPowers[mPowers[row] + mPowers[col]];
                mat[row][col] += xp;
            }

            // Update the right-hand side of the system.
            xp = mXPowers[mPowers[row]];
            rhs[row] += xp * w;
        }
    }

    // Copy the upper-triangular portion of the symmetric matrix to the
    // lower-triangular portion.
    for (row = 0; row < mNumPowers; ++row)
    {
        for (col = 0; col < row; ++col)
        {
            mat[row][col] = mat[col][row];
        }
    }

    // Precondition by normalizing the sums.
    Real invNumSamples = ((Real)1)/(Real)numSamples;
    for (row = 0; row < mNumPowers; ++row)
    {
        for (col = 0; col < mNumPowers; ++col)
        {
            mat[row][col] *= invNumSamples;
        }
        rhs[row] *= invNumSamples;
    }

    if (LinearSystem<Real>().Solve(mat, rhs, mCoefficients))
    {
        mSolved = true;
    }
    else
    {
        memset(mCoefficients, 0, mNumPowers*sizeof(Real));
        mSolved = false;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class PolynomialFit2<float>;

template WM5_MATHEMATICS_ITEM
class PolynomialFit2<double>;
//----------------------------------------------------------------------------
}
