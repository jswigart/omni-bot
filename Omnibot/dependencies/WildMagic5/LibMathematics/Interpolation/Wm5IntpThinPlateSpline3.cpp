// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/05/22)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntpThinPlateSpline3.h"
#include "Wm5LinearSystem.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntpThinPlateSpline3<Real>::IntpThinPlateSpline3 (int quantity,
    Real* X, Real* Y, Real* Z, Real* F, Real smooth, bool owner,
    bool transformToUnitCube)
    :
    mSmooth(smooth)
{
    assertion(quantity >= 4 && X && Y && Z && F && smooth >= (Real)0,
        "Invalid input\n");

    mInitialized = false;
    mQuantity = quantity;
    mX = new1<Real>(mQuantity);
    mY = new1<Real>(mQuantity);
    mZ = new1<Real>(mQuantity);
    mA = new1<Real>(mQuantity);

    int i, row, col;

    if (transformToUnitCube)
    {
        // Map input (x,y,z) to unit cube.  This is not part of the classical
        // thin-plate spline algorithm, because the interpolation is not
        // invariant to scalings.
        mXMin = X[0];
        mXMax = mXMin;
        for (i = 1; i < mQuantity; ++i)
        {
            if (X[i] < mXMin)
            {
                mXMin = X[i];
            }
            if (X[i] > mXMax)
            {
                mXMax = X[i];
            }
        }
        mXInvRange = ((Real)1)/(mXMax - mXMin);
        for (i = 0; i < mQuantity; ++i)
        {
            mX[i] = (X[i] - mXMin)*mXInvRange;
        }

        mYMin = Y[0];
        mYMax = mYMin;
        for (i = 1; i < mQuantity; ++i)
        {
            if (Y[i] < mYMin)
            {
                mYMin = Y[i];
            }
            if (Y[i] > mYMax)
            {
                mYMax = Y[i];
            }
        }
        mYInvRange = ((Real)1)/(mYMax - mYMin);
        for (i = 0; i < mQuantity; ++i)
        {
            mY[i] = (Y[i] - mYMin)*mYInvRange;
        }

        mZMin = Z[0];
        mZMax = mZMin;
        for (i = 1; i < mQuantity; ++i)
        {
            if (Z[i] < mZMin)
            {
                mZMin = Z[i];
            }
            if (Z[i] > mZMax)
            {
                mZMax = Z[i];
            }
        }
        mZInvRange = ((Real)1)/(mZMax - mZMin);
        for (i = 0; i < mQuantity; ++i)
        {
            mZ[i] = (Z[i] - mZMin)*mZInvRange;
        }
    }
    else
    {
        // The classical thin-plate spline uses the data as is.  The values
        // mXMax, mYMax, and mZMax are not used, but they are initialized
        // anyway (to irrelevant numbers).
        mXMin = (Real)0;
        mXMax = (Real)1;
        mXInvRange = (Real)1;
        mYMin = (Real)0;
        mYMax = (Real)1;
        mYInvRange = (Real)1;
        mZMin = (Real)0;
        mZMax = (Real)1;
        mZInvRange = (Real)1;
        memcpy(mX, X, mQuantity*sizeof(Real));
        memcpy(mY, Y, mQuantity*sizeof(Real));
        memcpy(mZ, Z, mQuantity*sizeof(Real));
    }

    // Compute matrix A = M + lambda*I [NxN matrix].
    GMatrix<Real> AMat(mQuantity, mQuantity);
    for (row = 0; row < mQuantity; ++row)
    {
        for (col = 0; col < mQuantity; ++col)
        {
            if (row == col)
            {
                AMat[row][col] = mSmooth;
            }
            else
            {
                Real dx = mX[row] - mX[col];
                Real dy = mY[row] - mY[col];
                Real dz = mZ[row] - mZ[col];
                Real t = Math<Real>::Sqrt(dx*dx + dy*dy + dz*dz);
                AMat[row][col] = Kernel(t);
            }
        }
    }

    // Compute matrix B [Nx4 matrix].
    GMatrix<Real> BMat(mQuantity, 4);
    for (row = 0; row < mQuantity; ++row)
    {
        BMat[row][0] = (Real)1;
        BMat[row][1] = mX[row];
        BMat[row][2] = mY[row];
        BMat[row][3] = mZ[row];
    }

    // Compute A^{-1}.
    GMatrix<Real> invAMat(mQuantity, mQuantity);
    if (!LinearSystem<Real>().Inverse(AMat, invAMat))
    {
        return;
    }

    // Compute P = B^t A^{-1} [4xN matrix].
    GMatrix<Real> PMat = BMat.TransposeTimes(invAMat);

    // Compute Q = P B = B^t A^{-1} B  [4x4 matrix].
    GMatrix<Real> QMat = PMat*BMat;

    // Compute Q^{-1}.
    GMatrix<Real> invQMat(4, 4);
    if (!LinearSystem<Real>().Inverse(QMat, invQMat))
    {
        return;
    }

    // Compute P*w.
    Real prod[4];
    for (row = 0; row < 4; ++row)
    {
        prod[row] = (Real)0;
        for (i = 0; i < mQuantity; ++i)
        {
            prod[row] += PMat[row][i]*F[i];
        }
    }

    // Compute 'b' vector for smooth thin plate spline.
    for (row = 0; row < 4; ++row)
    {
        mB[row] = (Real)0;
        for (i = 0; i < 4; ++i)
        {
            mB[row] += invQMat[row][i]*prod[i];
        }
    }

    // Compute w-B*b.
    Real* tmp = new1<Real>(mQuantity);
    for (row = 0; row < mQuantity; ++row)
    {
        tmp[row] = F[row];
        for (i = 0; i < 4; ++i)
        {
            tmp[row] -= BMat[row][i]*mB[i];
        }
    }

    // Compute 'a' vector for smooth thin plate spline.
    for (row = 0; row < mQuantity; ++row)
    {
        mA[row] = (Real)0;
        for (i = 0; i < mQuantity; ++i)
        {
            mA[row] += invAMat[row][i]*tmp[i];
        }
    }
    delete1(tmp);

    mInitialized = true;

    if (owner)
    {
        delete1(X);
        delete1(Y);
        delete1(Z);
        delete1(F);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
IntpThinPlateSpline3<Real>::~IntpThinPlateSpline3 ()
{
    delete1(mX);
    delete1(mY);
    delete1(mZ);
    delete1(mA);
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntpThinPlateSpline3<Real>::IsInitialized () const
{
    return mInitialized;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* IntpThinPlateSpline3<Real>::GetACoefficients () const
{
    return mA;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* IntpThinPlateSpline3<Real>::GetBCoefficients () const
{
    return mB;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpThinPlateSpline3<Real>::GetSmooth () const
{
    return mSmooth;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpThinPlateSpline3<Real>::operator() (Real x, Real y, Real z)
{
    if (mInitialized)
    {
        // Map (x,y,z) to the unit cube.
        x = (x - mXMin)*mXInvRange;
        y = (y - mYMin)*mYInvRange;
        z = (z - mZMin)*mZInvRange;

        Real result = mB[0] + mB[1]*x + mB[2]*y + mB[3]*z;
        for (int i = 0; i < mQuantity; ++i)
        {
            Real dx = x - mX[i];
            Real dy = y - mY[i];
            Real dz = z - mZ[i];
            Real t = Math<Real>::Sqrt(dx*dx + dy*dy + dz*dz);
            result += mA[i]*Kernel(t);
        }
        return result;
    }

    return Math<Real>::MAX_REAL;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpThinPlateSpline3<Real>::ComputeFunctional () const
{
    Real functional = (Real)0;
    for (int row = 0; row < mQuantity; ++row)
    {
        for (int col = 0; col < mQuantity; ++col)
        {
            if (row == col)
            {
                functional += mSmooth*mA[row]*mA[col];
            }
            else
            {
                Real dx = mX[row] - mX[col];
                Real dy = mY[row] - mY[col];
                Real dz = mZ[row] - mZ[col];
                Real t = Math<Real>::Sqrt(dx*dx + dy*dy + dz*dz);
                Real k = Kernel(t);
                functional += k*mA[row]*mA[col];
            }
        }
    }

    if (mSmooth > (Real)0)
    {
        functional *= mSmooth;
    }

    return functional;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpThinPlateSpline3<Real>::Kernel (Real t)
{
    return -Math<Real>::FAbs(t);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntpThinPlateSpline3<float>;

template WM5_MATHEMATICS_ITEM
class IntpThinPlateSpline3<double>;
//----------------------------------------------------------------------------
}
