// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/23)

#include "Wm5MathematicsPCH.h"
#include "Wm5NoniterativeEigen3x3.h"
#include "Wm5Assert.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
NoniterativeEigen3x3<Real>::NoniterativeEigen3x3 (const Matrix3<Real>& A)
{
    // Scale the matrix so its entries are in [-1,1].  The scaling is applied
    // only when at least one matrix entry has magnitude larger than 1.
    Matrix3<Real> AScaled = A;
    Real* scaledEntry = (Real*)AScaled;
    Real maxValue = Math<Real>::FAbs(scaledEntry[0]);
    Real absValue = Math<Real>::FAbs(scaledEntry[1]);
    if (absValue > maxValue)
    {
        maxValue = absValue;
    }
    absValue = Math<Real>::FAbs(scaledEntry[2]);
    if (absValue > maxValue)
    {
        maxValue = absValue;
    }
    absValue = Math<Real>::FAbs(scaledEntry[4]);
    if (absValue > maxValue)
    {
        maxValue = absValue;
    }
    absValue = Math<Real>::FAbs(scaledEntry[5]);
    if (absValue > maxValue)
    {
        maxValue = absValue;
    }
    absValue = Math<Real>::FAbs(scaledEntry[8]);
    if (absValue > maxValue)
    {
        maxValue = absValue;
    }

    int i;
    if (maxValue > (Real)1)
    {
        Real invMaxValue = ((Real)1)/maxValue;
        for (i = 0; i < 9; ++i)
        {
            scaledEntry[i] *= invMaxValue;
        }
    }

    // Compute the eigenvalues using double-precision arithmetic.
    double root[3];
    ComputeRoots(AScaled,root);
    mEigenvalue[0] = (Real)root[0];
    mEigenvalue[1] = (Real)root[1];
    mEigenvalue[2] = (Real)root[2];

    Real maxEntry[3];
    Vector3<Real> maxRow[3];
    for (i = 0; i < 3; ++i)
    {
        Matrix3<Real> M = AScaled;
        M[0][0] -= mEigenvalue[i];
        M[1][1] -= mEigenvalue[i];
        M[2][2] -= mEigenvalue[i];
        if (!PositiveRank(M, maxEntry[i], maxRow[i]))
        {
            // Rescale back to the original size.
            if (maxValue > (Real)1)
            {
                for (int j = 0; j < 3; ++j)
                {
                    mEigenvalue[j] *= maxValue;
                }
            }

            mEigenvector[0] = Vector3<Real>::UNIT_X;
            mEigenvector[1] = Vector3<Real>::UNIT_Y;
            mEigenvector[2] = Vector3<Real>::UNIT_Z;
            return;
        }
    }

    Real totalMax = maxEntry[0];
    i = 0;
    if (maxEntry[1] > totalMax)
    {
        totalMax = maxEntry[1];
        i = 1;
    }
    if (maxEntry[2] > totalMax)
    {
        i = 2;
    }

    if (i == 0)
    {
        maxRow[0].Normalize();
        ComputeVectors(AScaled, maxRow[0], 1, 2, 0);
    }
    else if (i == 1)
    {
        maxRow[1].Normalize();
        ComputeVectors(AScaled, maxRow[1], 2, 0, 1);
    }
    else
    {
        maxRow[2].Normalize();
        ComputeVectors(AScaled, maxRow[2], 0, 1, 2);
    }

    // Rescale back to the original size.
    if (maxValue > (Real)1)
    {
        for (i = 0; i < 3; ++i)
        {
            mEigenvalue[i] *= maxValue;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
NoniterativeEigen3x3<Real>::~NoniterativeEigen3x3 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Real NoniterativeEigen3x3<Real>::GetEigenvalue (int i) const
{
    assertion(0 <= i && i < 3, "Invalid index\n");
    return mEigenvalue[i];
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* NoniterativeEigen3x3<Real>::GetEigenvalues () const
{
    return mEigenvalue;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& NoniterativeEigen3x3<Real>::GetEigenvector(
    int i) const
{
    assertion(0 <= i && i < 3, "Invalid index\n");
    return mEigenvector[i];
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>* NoniterativeEigen3x3<Real>::GetEigenvectors () const
{
    return mEigenvector;
}
//----------------------------------------------------------------------------
template <typename Real>
void NoniterativeEigen3x3<Real>::ComputeRoots (const Matrix3<Real>& A,
    double root[3])
{
    // Convert the unique matrix entries to double precision.
    double a00 = (double)A[0][0];
    double a01 = (double)A[0][1];
    double a02 = (double)A[0][2];
    double a11 = (double)A[1][1];
    double a12 = (double)A[1][2];
    double a22 = (double)A[2][2];

    // The characteristic equation is x^3 - c2*x^2 + c1*x - c0 = 0.  The
    // eigenvalues are the roots to this equation, all guaranteed to be
    // real-valued, because the matrix is symmetric.
    double c0 = a00*a11*a22 + 2.0*a01*a02*a12 - a00*a12*a12 -
        a11*a02*a02 - a22*a01*a01;

    double c1 = a00*a11 - a01*a01 + a00*a22 - a02*a02 +
        a11*a22 - a12*a12;

    double c2 = a00 + a11 + a22;

    // Construct the parameters used in classifying the roots of the equation
    // and in solving the equation for the roots in closed form.
    double c2Div3 = c2*msInv3;
    double aDiv3 = (c1 - c2*c2Div3)*msInv3;
    if (aDiv3 > 0.0)
    {
        aDiv3 = 0.0;
    }

    double halfMB = 0.5*(c0 + c2Div3*(2.0*c2Div3*c2Div3 - c1));

    double q = halfMB*halfMB + aDiv3*aDiv3*aDiv3;
    if (q > 0.0)
    {
        q = 0.0;
    }

    // Compute the eigenvalues by solving for the roots of the polynomial.
    double magnitude = Mathd::Sqrt(-aDiv3);
    double angle = Mathd::ATan2(Mathd::Sqrt(-q), halfMB)*msInv3;
    double cs = Mathd::Cos(angle);
    double sn = Mathd::Sin(angle);
    double root0 = c2Div3 + 2.0*magnitude*cs;
    double root1 = c2Div3 - magnitude*(cs + msRoot3*sn);
    double root2 = c2Div3 - magnitude*(cs - msRoot3*sn);

    // Sort in increasing order.
    if (root1 >= root0)
    {
        root[0] = root0;
        root[1] = root1;
    }
    else
    {
        root[0] = root1;
        root[1] = root0;
    }

    if (root2 >= root[1])
    {
        root[2] = root2;
    }
    else
    {
        root[2] = root[1];
        if (root2 >= root[0])
        {
            root[1] = root2;
        }
        else
        {
            root[1] = root[0];
            root[0] = root2;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool NoniterativeEigen3x3<Real>::PositiveRank (Matrix3<Real>& M,
    Real& maxEntry, Vector3<Real>& maxRow) const
{
    // Locate the maximum-magnitude entry of the matrix.
    maxEntry = (Real)-1;
    int row, maxRowIndex = -1;
    for (row = 0; row < 3; ++row)
    {
        for (int col = row; col < 3; ++col)
        {
            Real absValue = Math<Real>::FAbs(M[row][col]);
            if (absValue > maxEntry)
            {
                maxEntry = absValue;
                maxRowIndex = row;
            }
        }
    }

    // Return the row containing the maximum, to be used for eigenvector
    // construction.
    maxRow = M.GetRow(maxRowIndex);

    return maxEntry >= Math<Real>::ZERO_TOLERANCE;
}
//----------------------------------------------------------------------------
template <typename Real>
void NoniterativeEigen3x3<Real>::ComputeVectors (const Matrix3<Real>& A,
    Vector3<Real>& U2, int i0, int i1, int i2)
{
    Vector3<Real> U0, U1;
    Vector3<Real>::GenerateComplementBasis(U0, U1, U2);

    // V[i2] = c0*U0 + c1*U1,  c0^2 + c1^2=1
    // e2*V[i2] = c0*A*U0 + c1*A*U1
    // e2*c0 = c0*U0.Dot(A*U0) + c1*U0.Dot(A*U1) = d00*c0 + d01*c1
    // e2*c1 = c0*U1.Dot(A*U0) + c1*U1.Dot(A*U1) = d01*c0 + d11*c1
    Vector3<Real> tmp = A*U0;
    Real p00 = mEigenvalue[i2] - U0.Dot(tmp);
    Real p01 = U1.Dot(tmp);
    Real p11 = mEigenvalue[i2] - U1.Dot(A*U1);
    Real invLength;
    Real maxValue = Math<Real>::FAbs(p00);
    int row = 0;
    Real absValue = Math<Real>::FAbs(p01);
    if (absValue > maxValue)
    {
        maxValue = absValue;
    }
    absValue = Math<Real>::FAbs(p11);
    if (absValue > maxValue)
    {
        maxValue = absValue;
        row = 1;
    }

    if (maxValue >= Math<Real>::ZERO_TOLERANCE)
    {
        if (row == 0)
        {
            invLength = Math<Real>::InvSqrt(p00*p00 + p01*p01);
            p00 *= invLength;
            p01 *= invLength;
            mEigenvector[i2] = p01*U0 + p00*U1;
        }
        else
        {
            invLength = Math<Real>::InvSqrt(p11*p11 + p01*p01);
            p11 *= invLength;
            p01 *= invLength;
            mEigenvector[i2] = p11*U0 + p01*U1;
        }
    }
    else
    {
        if (row == 0)
        {
            mEigenvector[i2] = U1;
        }
        else
        {
            mEigenvector[i2] = U0;
        }
    }

    // V[i0] = c0*U2 + c1*Cross(U2,V[i2]) = c0*R + c1*S
    // e0*V[i0] = c0*A*R + c1*A*S
    // e0*c0 = c0*R.Dot(A*R) + c1*R.Dot(A*S) = d00*c0 + d01*c1
    // e0*c1 = c0*S.Dot(A*R) + c1*S.Dot(A*S) = d01*c0 + d11*c1
    Vector3<Real> S = U2.Cross(mEigenvector[i2]);
    tmp = A*U2;
    p00 = mEigenvalue[i0] - U2.Dot(tmp);
    p01 = S.Dot(tmp);
    p11 = mEigenvalue[i0] - S.Dot(A*S);
    maxValue = Math<Real>::FAbs(p00);
    row = 0;
    absValue = Math<Real>::FAbs(p01);
    if (absValue > maxValue)
    {
        maxValue = absValue;
    }
    absValue = Math<Real>::FAbs(p11);
    if (absValue > maxValue)
    {
        maxValue = absValue;
        row = 1;
    }

    if (maxValue >= Math<Real>::ZERO_TOLERANCE)
    {
        if (row == 0)
        {
            invLength = Math<Real>::InvSqrt(p00*p00 + p01*p01);
            p00 *= invLength;
            p01 *= invLength;
            mEigenvector[i0] = p01*U2 + p00*S;
        }
        else
        {
            invLength = Math<Real>::InvSqrt(p11*p11 + p01*p01);
            p11 *= invLength;
            p01 *= invLength;
            mEigenvector[i0] = p11*U2 + p01*S;
        }
    }
    else
    {
        if (row == 0)
        {
            mEigenvector[i0] = S;
        }
        else
        {
            mEigenvector[i0] = U2;
        }
    }

    // V[i1] = Cross(V[i2],V[i0])
    mEigenvector[i1] = mEigenvector[i2].Cross(mEigenvector[i0]);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template<> const double NoniterativeEigen3x3<float>::msInv3 = 1.0/3.0;
template<> const double NoniterativeEigen3x3<float>::msRoot3 =
    Mathd::Sqrt(3.0);

template WM5_MATHEMATICS_ITEM
class NoniterativeEigen3x3<float>;

template<> const double NoniterativeEigen3x3<double>::msInv3 = 1.0/3.0;
template<> const double NoniterativeEigen3x3<double>::msRoot3 =
    Mathd::Sqrt(3.0);

template WM5_MATHEMATICS_ITEM
class NoniterativeEigen3x3<double>;
//----------------------------------------------------------------------------
}
