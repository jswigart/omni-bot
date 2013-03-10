// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5BSplineCurveFit.h"
#include "Wm5Math.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
BSplineCurveFit<Real>::BSplineCurveFit (int dimension, int numSamples,
    const Real* sampleData, int degree, int numControls)
    :
    mBasis(numControls, degree)
{
    assertion(dimension >= 1, "Invalid input\n");
    assertion(1 <= degree && degree < numControls, "Invalid input\n");
    assertion(numControls <= numSamples, "Invalid input\n");

    mDimension = dimension;
    mNumSamples = numSamples;
    mSampleData = sampleData;
    mDegree = degree;
    mNumControls = numControls;
    mControlData = new1<Real>(mDimension*numControls);

    // The double-precision basis functions are used to help with the
    // numerical round-off errors.
    BSplineFitBasisd dBasis(mNumControls,mDegree);
    double tMultiplier = 1.0/(double)(mNumSamples - 1);

    // Fit the data points with a B-spline curve using a least-squares error
    // metric.  The problem is of the form A^T*A*Q = A^T*P, where A^T*A is a
    // banded matrix, P contains the sample data, and Q is the unknown vector
    // of control points.

    double t;
    int i0, i1, i2, imin, imax, j;

    // Construct the matrix A^T*A.
    BandedMatrixd* ATAMat = new0 BandedMatrixd(mNumControls,
        mDegree + 1, mDegree + 1);

    for (i0 = 0; i0 < mNumControls; ++i0)
    {
        for (i1 = 0; i1 < i0; ++i1)
        {
            (*ATAMat)(i0, i1) = (*ATAMat)(i1, i0);
        }

        int i1Max = i0 + mDegree;
        if (i1Max >= mNumControls)
        {
            i1Max = mNumControls - 1;
        }

        for (i1 = i0; i1 <= i1Max; ++i1)
        {
            double value = 0.0;
            for (i2 = 0; i2 < mNumSamples; ++i2)
            {
                t = tMultiplier*(double)i2;
                dBasis.Compute(t, imin, imax);
                if (imin <= i0 && i0 <= imax && imin <= i1 && i1 <= imax)
                {
                    double dB0 = dBasis.GetValue(i0 - imin);
                    double dB1 = dBasis.GetValue(i1 - imin);
                    value += dB0*dB1;
                }
            }
            (*ATAMat)(i0, i1) = value;
        }
    }

    // Construct the matrix A^T.
    double** ATMat = new2<double>(mNumSamples, mNumControls);
    memset(ATMat[0], 0, mNumControls*mNumSamples*sizeof(double));
    for (i0 = 0; i0 < mNumControls; ++i0)
    {
        for (i1 = 0; i1 < mNumSamples; ++i1)
        {
            t = tMultiplier*(double)i1;
            dBasis.Compute(t, imin, imax);
            if (imin <= i0 && i0 <= imax)
            {
                ATMat[i0][i1] = dBasis.GetValue(i0 - imin);
            }
        }
    }

    // Compute X0 = (A^T*A)^{-1}*A^T by solving the linear system
    // A^T*A*X = A^T.
    bool solved = ATAMat->SolveSystem(ATMat,mNumSamples);
    assertion(solved, "Failed to solve linear system\n");
    WM5_UNUSED(solved);

    // The control points for the fitted curve are stored in the vector
    // Q = X0*P, where P is the vector of sample data.
    memset(mControlData, 0, mNumControls*mDimension*sizeof(Real));
    for (i0 = 0; i0 < mNumControls; ++i0)
    {
        Real* Q = mControlData + i0*mDimension;
        for (i1 = 0; i1 < mNumSamples; ++i1)
        {
            const Real* P = mSampleData + i1*mDimension;
            Real xValue = (Real)ATMat[i0][i1];
            for (j = 0; j < mDimension; j++)
            {
                Q[j] += xValue*P[j];
            }
        }
    }

    // Set the first and last output control points to match the first and
    // last input samples.  This supports the application of fitting keyframe
    // data with B-spline curves.  The user expects that the curve passes
    // through the first and last positions in order to support matching two
    // consecutive keyframe sequences.
    Real* cEnd0 = mControlData;
    const Real* sEnd0 = mSampleData;
    Real* cEnd1 = &mControlData[mDimension*(mNumControls-1)];
    const Real* sEnd1 = &mSampleData[mDimension*(mNumSamples-1)];
    for (j = 0; j < mDimension; j++)
    {
        *cEnd0++ = *sEnd0++;
        *cEnd1++ = *sEnd1++;
    }

    delete2(ATMat);
    delete0(ATAMat);
}
//----------------------------------------------------------------------------
template <typename Real>
BSplineCurveFit<Real>::~BSplineCurveFit ()
{
    delete1(mControlData);
}
//----------------------------------------------------------------------------
template <typename Real>
int BSplineCurveFit<Real>::GetDimension () const
{
    return mDimension;
}
//----------------------------------------------------------------------------
template <typename Real>
int BSplineCurveFit<Real>::GetSampleQuantity () const
{
    return mNumSamples;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* BSplineCurveFit<Real>::GetSampleData () const
{
    return mSampleData;
}
//----------------------------------------------------------------------------
template <typename Real>
int BSplineCurveFit<Real>::GetDegree () const
{
    return mDegree;
}
//----------------------------------------------------------------------------
template <typename Real>
int BSplineCurveFit<Real>::GetControlQuantity () const
{
    return mNumControls;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* BSplineCurveFit<Real>::GetControlData () const
{
    return mControlData;
}
//----------------------------------------------------------------------------
template <typename Real>
const BSplineFitBasis<Real>& BSplineCurveFit<Real>::GetBasis () const
{
    return mBasis;
}
//----------------------------------------------------------------------------
template <typename Real>
void BSplineCurveFit<Real>::GetPosition (Real t, Real* position) const
{
    int imin, imax;
    mBasis.Compute(t, imin, imax);

    Real* source = &mControlData[mDimension*imin];
    Real basisValue = mBasis.GetValue(0);
    int j;
    for (j = 0; j < mDimension; ++j)
    {
        position[j] = basisValue*(*source++);
    }

    for (int i = imin+1, index = 1; i <= imax; ++i, ++index)
    {
        basisValue = mBasis.GetValue(index);
        for (j = 0; j < mDimension; ++j)
        {
            position[j] += basisValue*(*source++);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class BSplineCurveFit<float>;

template WM5_MATHEMATICS_ITEM
class BSplineCurveFit<double>;
//----------------------------------------------------------------------------
}
