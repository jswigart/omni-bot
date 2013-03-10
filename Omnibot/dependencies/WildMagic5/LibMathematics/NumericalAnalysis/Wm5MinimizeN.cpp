// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/03/09)

#include "Wm5MathematicsPCH.h"
#include "Wm5MinimizeN.h"
#include "Wm5Math.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
MinimizeN<Real>::MinimizeN (int dimensions, Function function,
    int maxLevel, int maxBracket, int maxIterations, void* userData)
    :
    mDimensions(dimensions),
    mFunction(function),
    mMaxIterations(maxIterations),
    mUserData(userData),
    mMinimizer(LineFunction, maxLevel, maxBracket)
{
    assertion(mDimensions >= 1 && mFunction, "Invalid inputs\n");

    mTCurr = new1<Real>(mDimensions);
    mTSave = new1<Real>(mDimensions);
    mDirectionStorage = new1<Real>(mDimensions*(mDimensions + 1));
    mDirection = new1<Real*>(mDimensions + 1);
    for (int i = 0; i <= mDimensions; ++i)
    {
        mDirection[i] = &mDirectionStorage[i*mDimensions];
    }
    mDConj = mDirection[mDimensions];

    mLineArg = new1<Real>(mDimensions);
}
//----------------------------------------------------------------------------
template <typename Real>
MinimizeN<Real>::~MinimizeN ()
{
    delete1(mTCurr);
    delete1(mTSave);
    delete1(mDirectionStorage);
    delete1(mDirection);
    delete1(mLineArg);
}
//----------------------------------------------------------------------------
template <typename Real>
void MinimizeN<Real>::GetMinimum (const Real* t0, const Real* t1,
    const Real* tInitial, Real* tMin, Real& fMin)
{
    // For 1D function callback.
    mMinimizer.SetUserData(this);

    // The initial guess.
    size_t numBytes = mDimensions*sizeof(Real);
    mFCurr = mFunction(tInitial, mUserData);
    memcpy(mTSave, tInitial, numBytes);
    memcpy(mTCurr, tInitial, numBytes);

    // Initialize the direction set to the standard Euclidean basis.
    size_t numBasisBytes = numBytes*(mDimensions + 1);
    memset(mDirectionStorage, 0, numBasisBytes);
    int i;
    for (i = 0; i < mDimensions; ++i)
    {
        mDirection[i][i] = (Real)1;
    }

    Real ell0, ell1, ellMin;
    for (int iter = 0; iter < mMaxIterations; iter++)
    {
        // Find minimum in each direction and update current location.
        for (i = 0; i < mDimensions; ++i)
        {
            mDCurr = mDirection[i];
            ComputeDomain(t0, t1, ell0, ell1);
            mMinimizer.GetMinimum(ell0, ell1, (Real)0, ellMin, mFCurr);
            for (int j = 0; j < mDimensions; ++j)
            {
                mTCurr[j] += ellMin*mDCurr[j];
            }
        }

        // Estimate a unit-length conjugate direction.
        Real length = (Real)0;
        for (i = 0; i < mDimensions; ++i)
        {
            mDConj[i] = mTCurr[i] - mTSave[i];
            length += mDConj[i]*mDConj[i];
        }

        const Real epsilon = (Real)1e-06;
        length = Math<Real>::Sqrt(length);
        if (length < epsilon)
        {
            // New position did not change significantly from old one.
            // Should there be a better convergence criterion here?
            break;
        }

        Real invlength = ((Real)1)/length;
        for (i = 0; i < mDimensions; ++i)
        {
            mDConj[i] *= invlength;
        }

        // Minimize in conjugate direction.
        mDCurr = mDConj;
        ComputeDomain(t0, t1, ell0, ell1);
        mMinimizer.GetMinimum(ell0, ell1, (Real)0, ellMin, mFCurr);
        for (i = 0; i < mDimensions; ++i)
        {
            mTCurr[i] += ellMin*mDCurr[i];
        }

        // Cycle the directions and add conjugate direction to set.
        mDConj = mDirection[0];
        for (i = 0; i < mDimensions; ++i)
        {
            mDirection[i] = mDirection[i+1];
        }

        // Set parameters for next pass.
        memcpy(mTSave, mTCurr, numBytes);
    }

    memcpy(tMin, mTCurr, numBytes);
    fMin = mFCurr;
}
//----------------------------------------------------------------------------
template <typename Real>
void MinimizeN<Real>::ComputeDomain (const Real* t0, const Real* t1,
    Real& ell0, Real& ell1)
{
    ell0 = -Math<Real>::MAX_REAL;
    ell1 = +Math<Real>::MAX_REAL;

    for (int i = 0; i < mDimensions; ++i)
    {
        Real b0 = t0[i] - mTCurr[i];
        Real b1 = t1[i] - mTCurr[i];
        Real inv;
        if (mDCurr[i] > (Real)0)
        {
            // The valid t-interval is [b0,b1].
            inv = ((Real)1)/mDCurr[i];
            b0 *= inv;
            if (b0 > ell0)
            {
                ell0 = b0;
            }
            b1 *= inv;
            if (b1 < ell1)
            {
                ell1 = b1;
            }
        }
        else if (mDCurr[i] < (Real)0)
        {
            // The valid t-interval is [b1,b0].
            inv = ((Real)1)/mDCurr[i];
            b0 *= inv;
            if (b0 < ell1)
            {
                ell1 = b0;
            }
            b1 *= inv;
            if (b1 > ell0)
            {
                ell0 = b1;
            }
        }
    }

    // Correction if numerical errors lead to values nearly zero.
    if (ell0 > (Real)0)
    {
        ell0 = (Real)0;
    }
    if (ell1 < (Real)0)
    {
        ell1 = (Real)0;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Real MinimizeN<Real>::LineFunction (Real t, void* userData)
{
    MinimizeN& self = *(MinimizeN*)userData;

    for (int i = 0; i < self.mDimensions; ++i)
    {
        self.mLineArg[i] = self.mTCurr[i] + t*self.mDCurr[i];
    }

    Real result = self.mFunction(self.mLineArg, self.mUserData);
    return result;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class MinimizeN<float>;

template WM5_MATHEMATICS_ITEM
class MinimizeN<double>;
//----------------------------------------------------------------------------
}
