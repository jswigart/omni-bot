// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprPolynomialFit4.h"
#include "Wm5LinearSystem.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
PolynomialFit4<Real>::PolynomialFit4 (int numSamples, const Real* xSamples,
    const Real* ySamples, const Real* zSamples, const Real* wSamples,
    int numPowers, const Tuple<3,int>* powers)
{
    InitializePowers(numPowers, powers);

    const Real* srcSamples[4] = { xSamples, ySamples, zSamples, wSamples };
    Real* trgSamples[4] = { 0, 0, 0, 0 };
    TransformToUnit(numSamples, srcSamples, trgSamples);

    DoLeastSquaresFit(numSamples, trgSamples);

    delete1(trgSamples[0]);
    delete1(trgSamples[1]);
    delete1(trgSamples[2]);
    delete1(trgSamples[3]);
}
//----------------------------------------------------------------------------
template <typename Real>
PolynomialFit4<Real>::PolynomialFit4 (int numXSamples, int numYSamples,
    int numZSamples, const Real* xSamples, const Real* ySamples,
    const Real* zSamples, const Real* wSamples, int numPowers,
    const Tuple<3,int>* powers)
{
    InitializePowers(numPowers, powers);

    // Repackage the input into samples of the form (x[i],y[i],w[i],z[i]).
    int numSamples = numXSamples*numYSamples*numZSamples;
    Real* xRepackage = new1<Real>(numSamples);
    Real* yRepackage = new1<Real>(numSamples);
    Real* zRepackage = new1<Real>(numSamples);
    for (int z = 0, i = 0; z < numZSamples; ++z)
    {
        Real zInput = zSamples[z];
        for (int y = 0; y < numYSamples; ++y)
        {
            Real yInput = ySamples[y];
            for (int x = 0; x < numXSamples; ++x, ++i)
            {
                xRepackage[i] = xSamples[x];
                yRepackage[i] = yInput;
                zRepackage[i] = zInput;
            }
        }
    }

    const Real* srcSamples[4] =
        { xRepackage, yRepackage, zRepackage, wSamples };
    Real* trgSamples[4] = { 0, 0, 0, 0 };
    TransformToUnit(numSamples, srcSamples, trgSamples);

    DoLeastSquaresFit(numSamples, trgSamples);

    delete1(trgSamples[0]);
    delete1(trgSamples[1]);
    delete1(trgSamples[2]);
    delete1(trgSamples[3]);
    delete1(xRepackage);
    delete1(yRepackage);
    delete1(zRepackage);
}
//----------------------------------------------------------------------------
template <typename Real>
PolynomialFit4<Real>::~PolynomialFit4 ()
{
    delete1(mPowers);
    delete1(mXPowers);
    delete1(mYPowers);
    delete1(mZPowers);
    delete1(mCoefficients);
}
//----------------------------------------------------------------------------
template <typename Real>
PolynomialFit4<Real>::operator bool () const
{
    return mSolved;
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialFit4<Real>::GetXMin () const
{
    return mMin[0];
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialFit4<Real>::GetXMax () const
{
    return mMax[0];
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialFit4<Real>::GetYMin () const
{
    return mMin[1];
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialFit4<Real>::GetYMax () const
{
    return mMax[1];
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialFit4<Real>::GetZMin () const
{
    return mMin[2];
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialFit4<Real>::GetZMax () const
{
    return mMax[2];
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialFit4<Real>::GetWMin () const
{
    return mMin[3];
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialFit4<Real>::GetWMax () const
{
    return mMax[3];
}
//----------------------------------------------------------------------------
template <typename Real>
Real PolynomialFit4<Real>::operator() (Real x, Real y, Real z) const
{
    // Transform (x,y,z) from the original space to [-1,1]^3.
    x = (Real)-1 + ((Real)2)*(x - mMin[0])*mScale[0];
    y = (Real)-1 + ((Real)2)*(y - mMin[1])*mScale[1];
    z = (Real)-1 + ((Real)2)*(z - mMin[2])*mScale[2];

    // Compute relevant powers of x, y, and z.
    int j;
    for (j = 1; j <= mMaxXPower; ++j)
    {
        mXPowers[j] = mXPowers[j-1] * x;
    }
    for (j = 1; j <= mMaxYPower; ++j)
    {
        mYPowers[j] = mYPowers[j-1] * y;
    }
    for (j = 1; j <= mMaxZPower; ++j)
    {
        mZPowers[j] = mZPowers[j-1] * z;
    }

    Real w = (Real)0;
    for (int i = 0; i < mNumPowers; ++i)
    {
        Real xp = mXPowers[mPowers[i][0]];
        Real yp = mYPowers[mPowers[i][1]];
        Real zp = mYPowers[mPowers[i][2]];
        w += mCoefficients[i] * xp * yp * zp;
    }

    // Transform w from [-1,1] back to the original space.
    w = (w + (Real)1)*mInvTwoWScale + mMin[3];
    return w;
}
//----------------------------------------------------------------------------
template <typename Real>
void PolynomialFit4<Real>::InitializePowers (int numPowers,
    const Tuple<3,int>* powers)
{
    // Copy the powers for use in evaluation of the fitted polynomial.
    mNumPowers = numPowers;
    mPowers = new1<Tuple<3,int> >(mNumPowers);
    memcpy(mPowers, powers, mNumPowers*sizeof(Tuple<3,int>));

    // Determine the maximum power per dimension.  Powers of x, y, and z are
    // computed up to twice the powers when constructing the fitted polynomial.
    // Powers of x, y, and z are computed up to the powers for the evaluation
    // of the fitted polynomial.
    mMaxXPower = mPowers[0][0];
    mMaxYPower = mPowers[0][1];
    mMaxZPower = mPowers[0][2];
    int i;
    for (i = 1; i < mNumPowers; ++i)
    {
        if (mPowers[i][0] > mMaxXPower)
        {
            mMaxXPower = mPowers[i][0];
        }
        if (mPowers[i][1] > mMaxYPower)
        {
            mMaxYPower = mPowers[i][1];
        }
        if (mPowers[i][2] > mMaxZPower)
        {
            mMaxZPower = mPowers[i][2];
        }
    }
    mXPowers = new1<Real>(2*mMaxXPower + 1);
    mYPowers = new1<Real>(2*mMaxYPower + 1);
    mZPowers = new1<Real>(2*mMaxZPower + 1);
    mXPowers[0] = (Real)1;
    mYPowers[0] = (Real)1;
    mZPowers[0] = (Real)1;
}
//----------------------------------------------------------------------------
template <typename Real>
void PolynomialFit4<Real>::TransformToUnit (int numSamples,
    const Real* srcSamples[4], Real* trgSamples[4])
{
    // Transform the data to [-1,1]^4 for numerical robustness.
    for (int j = 0; j < 4; ++j)
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

    mInvTwoWScale = ((Real)0.5)/mScale[3];
}
//----------------------------------------------------------------------------
template <typename Real>
void PolynomialFit4<Real>::DoLeastSquaresFit (int numSamples,
    Real* trgSamples[4])
{
    // The matrix and vector for a linear system that determines the
    // coefficients of the fitted polynomial.
    GMatrix<Real> mat(mNumPowers, mNumPowers);  // initially zero
    GVector<Real> rhs(mNumPowers);  // initially zero
    mCoefficients = new1<Real>(mNumPowers);

    int row, col;
    for (int i = 0; i < numSamples; ++i)
    {
        // Compute relevant powers of x and y.
        Real x = trgSamples[0][i];
        Real y = trgSamples[1][i];
        Real z = trgSamples[2][i];
        Real w = trgSamples[3][i];
        int j;
        for (j = 1; j <= 2*mMaxXPower; ++j)
        {
            mXPowers[j] = mXPowers[j-1] * x;
        }
        for (j = 1; j <= 2*mMaxYPower; ++j)
        {
            mYPowers[j] = mYPowers[j-1] * y;
        }
        for (j = 1; j <= 2*mMaxZPower; ++j)
        {
            mZPowers[j] = mZPowers[j-1] * z;
        }

        for (row = 0; row < mNumPowers; ++row)
        {
            // Update the upper-triangular portion of the symmetric matrix.
            Real xp, yp, zp;
            for (col = row; col < mNumPowers; ++col)
            {
                xp = mXPowers[mPowers[row][0] + mPowers[col][0]];
                yp = mYPowers[mPowers[row][1] + mPowers[col][1]];
                zp = mYPowers[mPowers[row][2] + mPowers[col][2]];
                mat[row][col] += xp * yp * zp;
            }

            // Update the right-hand side of the system.
            xp = mXPowers[mPowers[row][0]];
            yp = mYPowers[mPowers[row][1]];
            zp = mYPowers[mPowers[row][2]];
            rhs[row] += xp * yp * zp * w;
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
class PolynomialFit4<float>;

template WM5_MATHEMATICS_ITEM
class PolynomialFit4<double>;
//----------------------------------------------------------------------------
}
