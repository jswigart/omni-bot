// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5BSplineSurfaceFit.h"
#include "Wm5Math.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
BSplineSurfaceFit<Real>::BSplineSurfaceFit (int degree0, int numControls0,
    int numSamples0, int degree1, int numControls1, int numSamples1,
    Vector3<Real>** samples)
{
    assertion(1 <= degree0 && degree0 < numControls0, "Invalid input\n");
    assertion(numControls0 <= numSamples0, "Invalid input\n");
    assertion(1 <= degree1 && degree1 < numControls1, "Invalid input\n");
    assertion(numControls1 <= numSamples1, "Invalid input\n");

    mDegree[0] = degree0;
    mNumSamples[0] = numSamples0;
    mNumControls[0] = numControls0;
    mDegree[1] = degree1;
    mNumSamples[1] = numSamples1;
    mNumControls[1] = numControls1;
    mSamples = samples;
    mControls = new2<Vector3<Real> >(numControls0, numControls1);

    // The double-precision basis functions are used to help with the
    // numerical round-off errors.
    BSplineFitBasisd* dBasis[2];
    double tMultiplier[2];
    int dim;
    for (dim = 0; dim < 2; ++dim)
    {
        mBasis[dim] = new0 BSplineFitBasis<Real>(mNumControls[dim],
            mDegree[dim]);

        dBasis[dim] = new0 BSplineFitBasisd(mNumControls[dim],
            mDegree[dim]);

        tMultiplier[dim] = 1.0/(double)(mNumSamples[dim] - 1);
    }

    // Fit the data points with a B-spline surface using a least-squares error
    // metric.  The problem is of the form A0^T*A0*Q*A1^T*A1 = A0^T*P*A1, where
    // A0^T*A0 and A1^T*A1 are banded matrices, P contains the sample data, and
    // Q is the unknown matrix of control points.

    double t;
    int i0, i1, i2, imin, imax;

    // Construct the matrices A0^T*A0 and A1^T*A1.
    BandedMatrixd* ATAMat[2];
    for (dim = 0; dim < 2; ++dim)
    {
        ATAMat[dim] = new0 BandedMatrixd(mNumControls[dim],
            mDegree[dim] + 1, mDegree[dim] + 1);

        for (i0 = 0; i0 < mNumControls[dim]; ++i0)
        {
            for (i1 = 0; i1 < i0; ++i1)
            {
                (*ATAMat[dim])(i0, i1) = (*ATAMat[dim])(i1, i0);
            }

            int i1Max = i0 + mDegree[dim];
            if (i1Max >= mNumControls[dim])
            {
                i1Max = mNumControls[dim] - 1;
            }

            for (i1 = i0; i1 <= i1Max; ++i1)
            {
                double value = 0.0;
                for (i2 = 0; i2 < mNumSamples[dim]; ++i2)
                {
                    t = tMultiplier[dim]*(double)i2;
                    dBasis[dim]->Compute(t, imin, imax);
                    if (imin <= i0 && i0 <= imax && imin <= i1 && i1 <= imax)
                    {
                        double b0 = dBasis[dim]->GetValue(i0 - imin);
                        double b1 = dBasis[dim]->GetValue(i1 - imin);
                        value += b0*b1;
                    }
                }
                (*ATAMat[dim])(i0, i1) = value;
            }
        }
    }

    // Construct the matrices A0^T and A1^T.
    double** ATMat[2];
    for (dim = 0; dim < 2; dim++)
    {
        ATMat[dim] = new2<double>(mNumSamples[dim], mNumControls[dim]);
        memset(ATMat[dim][0], 0,
            mNumControls[dim]*mNumSamples[dim]*sizeof(double));

        for (i0 = 0; i0 < mNumControls[dim]; ++i0)
        {
            for (i1 = 0; i1 < mNumSamples[dim]; ++i1)
            {
                t = tMultiplier[dim]*(double)i1;
                dBasis[dim]->Compute(t, imin, imax);
                if (imin <= i0 && i0 <= imax)
                {
                    ATMat[dim][i0][i1] = dBasis[dim]->GetValue(i0 - imin);
                }
            }
        }
    }

    // Compute X0 = (A0^T*A0)^{-1}*A0^T and X1 = (A1^T*A1)^{-1}*A1^T by
    // solving the linear systems A0^T*A0*X0 = A0^T and A1^T*A1*X1 = A1^T.
    for (dim = 0; dim < 2; ++dim)
    {
        bool solved = ATAMat[dim]->SolveSystem(ATMat[dim], mNumSamples[dim]);
        assertion(solved, "Failed to solve linear system\n");
        WM5_UNUSED(solved);
    }

    // The control points for the fitted surface are stored in the matrix
    // Q = X0*P*X1^T, where P is the matrix of sample data.
    for (i0 = 0; i0 < mNumControls[0]; ++i0)
    {
        for (i1 = 0; i1 < mNumControls[1]; ++i1)
        {
            Vector3<Real> sum = Vector3<Real>::ZERO;
            for (int j0 = 0; j0 < mNumSamples[0]; ++j0)
            {
                Real x0Value = (Real)ATMat[0][i0][j0];
                for (int j1 = 0; j1 < mNumSamples[1]; ++j1)
                {
                    Real x1Value = (Real)ATMat[1][i1][j1];
                    sum += (x0Value*x1Value)*mSamples[j0][j1];
                }
            }
            mControls[i0][i1] = sum;
        }
    }

    for (dim = 0; dim < 2; ++dim)
    {
        delete0(dBasis[dim]);
        delete0(ATAMat[dim]);
        delete2(ATMat[dim]);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
BSplineSurfaceFit<Real>::~BSplineSurfaceFit ()
{
    delete0(mBasis[0]);
    delete0(mBasis[1]);
    delete2(mControls);
}
//----------------------------------------------------------------------------
template <typename Real>
int BSplineSurfaceFit<Real>::GetSampleQuantity (int i) const
{
    return mNumSamples[i];
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real>** BSplineSurfaceFit<Real>::GetSamplePoints () const
{
    return mSamples;
}
//----------------------------------------------------------------------------
template <typename Real>
int BSplineSurfaceFit<Real>::GetDegree (int i) const
{
    return mDegree[i];
}
//----------------------------------------------------------------------------
template <typename Real>
int BSplineSurfaceFit<Real>::GetControlQuantity (int i) const
{
    return mNumControls[i];
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real>** BSplineSurfaceFit<Real>::GetControlPoints () const
{
    return mControls;
}
//----------------------------------------------------------------------------
template <typename Real>
const BSplineFitBasis<Real>& BSplineSurfaceFit<Real>::GetBasis (int i) const
{
    return *mBasis[i];
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> BSplineSurfaceFit<Real>::GetPosition (Real u, Real v) const
{
    int iumin, iumax, ivmin, ivmax;
    mBasis[0]->Compute(u, iumin, iumax);
    mBasis[1]->Compute(v, ivmin, ivmax);

    Vector3<Real> pos = Vector3<Real>::ZERO;
    for (int iu = iumin, i = 0; iu <= iumax; ++iu, ++i)
    {
        Real value0 = mBasis[0]->GetValue(i);
        for (int iv = ivmin, j = 0; iv <= ivmax; ++iv, ++j)
        {
            Real value1 = mBasis[1]->GetValue(j);
            pos += (value0*value1)*mControls[iu][iv];
        }
    }
    return pos;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class BSplineSurfaceFit<float>;

template WM5_MATHEMATICS_ITEM
class BSplineSurfaceFit<double>;
//----------------------------------------------------------------------------
}
