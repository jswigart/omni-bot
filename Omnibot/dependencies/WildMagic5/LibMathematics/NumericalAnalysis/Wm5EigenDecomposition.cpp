// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/09)

#include "Wm5MathematicsPCH.h"
#include "Wm5EigenDecomposition.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
EigenDecomposition<Real>::EigenDecomposition (int size)
    :
    mMatrix(size, size)
{
    assertion(size >= 2, "Invalid size in Eigendecomposition constructor\n");

    mSize = size;
    mDiagonal = new1<Real>(mSize);
    mSubdiagonal = new1<Real>(mSize);
    mIsRotation = false;
}
//----------------------------------------------------------------------------
template <typename Real>
EigenDecomposition<Real>::EigenDecomposition (const Matrix2<Real>& mat)
    :
    mMatrix(2, 2, &mat[0][0])
{
    mSize = 2;
    mDiagonal = new1<Real>(mSize);
    mSubdiagonal = new1<Real>(mSize);
    mIsRotation = false;
}
//----------------------------------------------------------------------------
template <typename Real>
EigenDecomposition<Real>::EigenDecomposition (const Matrix3<Real>& mat)
    :
    mMatrix(3, 3, &mat[0][0])
{
    mSize = 3;
    mDiagonal = new1<Real>(mSize);
    mSubdiagonal = new1<Real>(mSize);
    mIsRotation = false;
}
//----------------------------------------------------------------------------
template <typename Real>
EigenDecomposition<Real>::EigenDecomposition (const GMatrix<Real>& mat)
    :
    mMatrix(mat)
{
    mSize = mat.GetNumRows();
    assertion(mSize >= 2 && (mat.GetNumColumns() == mSize),
        "Square matrix required in EigenDecomposition constructor\n");

    mDiagonal = new1<Real>(mSize);
    mSubdiagonal = new1<Real>(mSize);
    mIsRotation = false;
}
//----------------------------------------------------------------------------
template <typename Real>
EigenDecomposition<Real>::~EigenDecomposition ()
{
    delete1(mDiagonal);
    delete1(mSubdiagonal);
}
//----------------------------------------------------------------------------
template <typename Real>
Real& EigenDecomposition<Real>::operator() (int row, int column)
{
    return mMatrix[row][column];
}
//----------------------------------------------------------------------------
template <typename Real>
EigenDecomposition<Real>& EigenDecomposition<Real>::operator= (
    const Matrix2<Real>& mat)
{
    mMatrix.SetMatrix(2, 2, &mat[0][0]);
    mSize = 2;
    delete1(mDiagonal);
    delete1(mSubdiagonal);
    mDiagonal = new1<Real>(mSize);
    mSubdiagonal = new1<Real>(mSize);
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
EigenDecomposition<Real>& EigenDecomposition<Real>::operator= (
    const Matrix3<Real>& mat)
{
    mMatrix.SetMatrix(3, 3, &mat[0][0]);
    mSize = 3;
    delete1(mDiagonal);
    delete1(mSubdiagonal);
    mDiagonal = new1<Real>(mSize);
    mSubdiagonal = new1<Real>(mSize);
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
EigenDecomposition<Real>& EigenDecomposition<Real>::operator= (
    const GMatrix<Real>& mat)
{
    mMatrix = mat;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
void EigenDecomposition<Real>::Solve (bool increasingSort)
{
    if (mSize == 2)
    {
        Tridiagonal2();
    }
    else if (mSize == 3)
    {
        Tridiagonal3();
    }
    else
    {
        TridiagonalN();
    }

    QLAlgorithm();

    if (increasingSort)
    {
        IncreasingSort();
    }
    else
    {
        DecreasingSort();
    }

    GuaranteeRotation();
}
//----------------------------------------------------------------------------
template <typename Real>
Real EigenDecomposition<Real>::GetEigenvalue (int i) const
{
    assertion(0 <= i && i < mSize, "Invalid index in GetEigenvalue\n");
    return mDiagonal[i];
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* EigenDecomposition<Real>::GetEigenvalues () const
{
    return mDiagonal;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> EigenDecomposition<Real>::GetEigenvector2 (int i) const
{
    assertion(mSize == 2, "Mismatched dimension in GetEigenvector2\n");

    if (mSize == 2)
    {
        Vector2<Real> eigenvector;
        for (int row = 0; row < mSize; ++row)
        {
            eigenvector[row] = mMatrix[row][i];
        }
        return eigenvector;
    }
    return Vector2<Real>::ZERO;
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix2<Real> EigenDecomposition<Real>::GetEigenvectors2 () const
{
    assertion(mSize == 2, "Mismatched dimension in GetEigenvectors2\n");

    Matrix2<Real> eigenvectors;
    for (int row = 0; row < 2; ++row)
    {
        for (int column = 0; column < 2; ++column)
        {
            eigenvectors[row][column] = mMatrix[row][column];
        }
    }
    return eigenvectors;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> EigenDecomposition<Real>::GetEigenvector3 (int i) const
{
    assertion(mSize == 3, "Mismatched dimension in GetEigenvector3\n");

    if (mSize == 3)
    {
        Vector3<Real> eigenvector;
        for (int row = 0; row < mSize; ++row)
        {
            eigenvector[row] = mMatrix[row][i];
        }
        return eigenvector;
    }
    return Vector3<Real>::ZERO;
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> EigenDecomposition<Real>::GetEigenvectors3 () const
{
    assertion(mSize == 3, "Mismatched dimension in GetEigenvectors2\n");

    Matrix3<Real> eigenvectors;
    for (int row = 0; row < 3; ++row)
    {
        for (int column = 0; column < 3; ++column)
        {
            eigenvectors[row][column] = mMatrix[row][column];
        }
    }
    return eigenvectors;
}
//----------------------------------------------------------------------------
template <typename Real>
GVector<Real> EigenDecomposition<Real>::GetEigenvector (int i) const
{
    return mMatrix.GetColumn(i);
}
//----------------------------------------------------------------------------
template <typename Real>
const GMatrix<Real>& EigenDecomposition<Real>::GetEigenvectors () const
{
    return mMatrix;
}
//----------------------------------------------------------------------------
template <typename Real>
void EigenDecomposition<Real>::Tridiagonal2 ()
{
    // The matrix is already tridiagonal.
    mDiagonal[0] = mMatrix[0][0];
    mDiagonal[1] = mMatrix[1][1];
    mSubdiagonal[0] = mMatrix[0][1];
    mSubdiagonal[1] = (Real)0;
    mMatrix[0][0] = (Real)1;
    mMatrix[0][1] = (Real)0;
    mMatrix[1][0] = (Real)0;
    mMatrix[1][1] = (Real)1;

    mIsRotation = true;
}
//----------------------------------------------------------------------------
template <typename Real>
void EigenDecomposition<Real>::Tridiagonal3 ()
{
    Real m00 = mMatrix[0][0];
    Real m01 = mMatrix[0][1];
    Real m02 = mMatrix[0][2];
    Real m11 = mMatrix[1][1];
    Real m12 = mMatrix[1][2];
    Real m22 = mMatrix[2][2];

    mDiagonal[0] = m00;
    mSubdiagonal[2] = (Real)0;
    if (Math<Real>::FAbs(m02) > Math<Real>::ZERO_TOLERANCE)
    {
        Real length = Math<Real>::Sqrt(m01*m01 + m02*m02);
        Real invLength = ((Real)1)/length;
        m01 *= invLength;
        m02 *= invLength;
        Real q = ((Real)2)*m01*m12 + m02*(m22 - m11);
        mDiagonal[1] = m11 + m02*q;
        mDiagonal[2] = m22 - m02*q;
        mSubdiagonal[0] = length;
        mSubdiagonal[1] = m12 - m01*q;
        mMatrix[0][0] = (Real)1;
        mMatrix[0][1] = (Real)0;
        mMatrix[0][2] = (Real)0;
        mMatrix[1][0] = (Real)0;
        mMatrix[1][1] = m01;
        mMatrix[1][2] = m02;
        mMatrix[2][0] = (Real)0;
        mMatrix[2][1] = m02;
        mMatrix[2][2] = -m01;
        mIsRotation = false;
    }
    else
    {
        mDiagonal[1] = m11;
        mDiagonal[2] = m22;
        mSubdiagonal[0] = m01;
        mSubdiagonal[1] = m12;
        mMatrix[0][0] = (Real)1;
        mMatrix[0][1] = (Real)0;
        mMatrix[0][2] = (Real)0;
        mMatrix[1][0] = (Real)0;
        mMatrix[1][1] = (Real)1;
        mMatrix[1][2] = (Real)0;
        mMatrix[2][0] = (Real)0;
        mMatrix[2][1] = (Real)0;
        mMatrix[2][2] = (Real)1;
        mIsRotation = true;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void EigenDecomposition<Real>::TridiagonalN ()
{
    // TODO:  This is Numerical Recipes in C code.  Rewrite to avoid
    // copyright issues.

    int i0, i1, i2, i3;

    for (i0 = mSize - 1, i3 = mSize - 2; i0 >= 1; --i0, --i3)
    {
        Real value0 = (Real)0;
        Real scale = (Real)0;

        if (i3 > 0)
        {
            for (i2 = 0; i2 <= i3; ++i2)
            {
                scale += Math<Real>::FAbs(mMatrix[i0][i2]);
            }
            if (scale == (Real)0)
            {
                mSubdiagonal[i0] = mMatrix[i0][i3];
            }
            else
            {
                Real invScale = ((Real)1)/scale;
                for (i2 = 0; i2 <= i3; ++i2)
                {
                    mMatrix[i0][i2] *= invScale;
                    value0 += mMatrix[i0][i2]*mMatrix[i0][i2];
                }

                Real value1 = mMatrix[i0][i3];
                Real value2 = Math<Real>::Sqrt(value0);
                if (value1 > (Real)0)
                {
                    value2 = -value2;
                }

                mSubdiagonal[i0] = scale*value2;
                value0 -= value1*value2;
                mMatrix[i0][i3] = value1-value2;
                value1 = (Real)0;
                Real invValue0 = ((Real)1)/value0;
                for (i1 = 0; i1 <= i3; ++i1)
                {
                    mMatrix[i1][i0] = mMatrix[i0][i1]*invValue0;
                    value2 = (Real)0;
                    for (i2 = 0; i2 <= i1; ++i2)
                    {
                        value2 += mMatrix[i1][i2]*mMatrix[i0][i2];
                    }
                    for (i2 = i1+1; i2 <= i3; ++i2)
                    {
                        value2 += mMatrix[i2][i1]*mMatrix[i0][i2];
                    }
                    mSubdiagonal[i1] = value2*invValue0;
                    value1 += mSubdiagonal[i1]*mMatrix[i0][i1];
                }

                Real value3 = ((Real)0.5)*value1*invValue0;
                for (i1 = 0; i1 <= i3; ++i1)
                {
                    value1 = mMatrix[i0][i1];
                    value2 = mSubdiagonal[i1] - value3*value1;
                    mSubdiagonal[i1] = value2;
                    for (i2 = 0; i2 <= i1; i2++)
                    {
                        mMatrix[i1][i2] -=
                            value1*mSubdiagonal[i2] + value2*mMatrix[i0][i2];
                    }
                }
            }
        }
        else
        {
            mSubdiagonal[i0] = mMatrix[i0][i3];
        }

        mDiagonal[i0] = value0;
    }

    mDiagonal[0] = (Real)0;
    mSubdiagonal[0] = (Real)0;
    for (i0 = 0, i3 = -1; i0 <= mSize - 1; ++i0, ++i3)
    {
        if (mDiagonal[i0] != (Real)0)
        {
            for (i1 = 0; i1 <= i3; ++i1)
            {
                Real sum = (Real)0;
                for (i2 = 0; i2 <= i3; ++i2)
                {
                    sum += mMatrix[i0][i2]*mMatrix[i2][i1];
                }
                for (i2 = 0; i2 <= i3; ++i2)
                {
                    mMatrix[i2][i1] -= sum*mMatrix[i2][i0];
                }
            }
        }
        mDiagonal[i0] = mMatrix[i0][i0];
        mMatrix[i0][i0] = (Real)1;
        for (i1 = 0; i1 <= i3; ++i1)
        {
            mMatrix[i1][i0] = (Real)0;
            mMatrix[i0][i1] = (Real)0;
        }
    }

    // Reordering needed by EigenDecomposition::QLAlgorithm.
    for (i0 = 1, i3 = 0; i0 < mSize; ++i0, ++i3)
    {
        mSubdiagonal[i3] = mSubdiagonal[i0];
    }
    mSubdiagonal[mSize-1] = (Real)0;

    mIsRotation = ((mSize % 2) == 0);
}
//----------------------------------------------------------------------------
template <typename Real>
bool EigenDecomposition<Real>::QLAlgorithm ()
{
    // TODO:  This is Numerical Recipes in C code.  Rewrite to avoid
    // copyright issues.

    const int iMaxIter = 32;

    for (int i0 = 0; i0 < mSize; ++i0)
    {
        int i1;
        for (i1 = 0; i1 < iMaxIter; ++i1)
        {
            int i2;
            for (i2 = i0; i2 <= mSize - 2; ++i2)
            {
                Real tmp = Math<Real>::FAbs(mDiagonal[i2]) +
                    Math<Real>::FAbs(mDiagonal[i2+1]);

                if (Math<Real>::FAbs(mSubdiagonal[i2]) + tmp == tmp)
                {
                    break;
                }
            }
            if (i2 == i0)
            {
                break;
            }

            Real value0 = (mDiagonal[i0 + 1]
                - mDiagonal[i0])/(((Real)2)*mSubdiagonal[i0]);
            Real value1 = Math<Real>::Sqrt(value0*value0 + (Real)1);
            if (value0 < (Real)0)
            {
                value0 = mDiagonal[i2] - mDiagonal[i0]
                    + mSubdiagonal[i0]/(value0 - value1);
            }
            else
            {
                value0 = mDiagonal[i2] - mDiagonal[i0]
                    + mSubdiagonal[i0]/(value0 + value1);
            }

            Real sn = (Real)1, cs = (Real)1, value2 = (Real)0;
            for (int i3 = i2 - 1; i3 >= i0; --i3)
            {
                Real value3 = sn*mSubdiagonal[i3];
                Real value4 = cs*mSubdiagonal[i3];
                if (Math<Real>::FAbs(value3) >= Math<Real>::FAbs(value0))
                {
                    cs = value0/value3;
                    value1 = Math<Real>::Sqrt(cs*cs + (Real)1);
                    mSubdiagonal[i3 + 1] = value3*value1;
                    sn = ((Real)1)/value1;
                    cs *= sn;
                }
                else
                {
                    sn = value3/value0;
                    value1 = Math<Real>::Sqrt(sn*sn + (Real)1);
                    mSubdiagonal[i3 + 1] = value0*value1;
                    cs = ((Real)1)/value1;
                    sn *= cs;
                }
                value0 = mDiagonal[i3 + 1] - value2;
                value1 = (mDiagonal[i3] - value0)*sn + ((Real)2)*value4*cs;
                value2 = sn*value1;
                mDiagonal[i3 + 1] = value0 + value2;
                value0 = cs*value1 - value4;

                for (int i4 = 0; i4 < mSize; ++i4)
                {
                    value3 = mMatrix[i4][i3 + 1];
                    mMatrix[i4][i3 + 1] = sn*mMatrix[i4][i3] + cs*value3;
                    mMatrix[i4][i3] = cs*mMatrix[i4][i3] - sn*value3;
                }
            }
            mDiagonal[i0] -= value2;
            mSubdiagonal[i0] = value0;
            mSubdiagonal[i2] = (Real)0;
        }
        if (i1 == iMaxIter)
        {
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
void EigenDecomposition<Real>::DecreasingSort ()
{
    // Sort the eigenvalues in decreasing order, e[0] >= ... >= e[mSize-1]
    for (int i0 = 0, i1; i0 <= mSize - 2; ++i0)
    {
        // Locate the maximum eigenvalue.
        i1 = i0;
        Real maxValue = mDiagonal[i1];
        int i2;
        for (i2 = i0 + 1; i2 < mSize; ++i2)
        {
            if (mDiagonal[i2] > maxValue)
            {
                i1 = i2;
                maxValue = mDiagonal[i1];
            }
        }

        if (i1 != i0)
        {
            // Swap the eigenvalues.
            mDiagonal[i1] = mDiagonal[i0];
            mDiagonal[i0] = maxValue;

            // Swap the eigenvectors corresponding to the eigenvalues.
            for (i2 = 0; i2 < mSize; ++i2)
            {
                Real tmp = mMatrix[i2][i0];
                mMatrix[i2][i0] = mMatrix[i2][i1];
                mMatrix[i2][i1] = tmp;
                mIsRotation = !mIsRotation;
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void EigenDecomposition<Real>::IncreasingSort ()
{
    // Sort the eigenvalues in increasing order, e[0] <= ... <= e[mSize-1]
    for (int i0 = 0, i1; i0 <= mSize - 2; ++i0)
    { 
        // Locate the minimum eigenvalue.
        i1 = i0;
        Real minValue = mDiagonal[i1];
        int i2;
        for (i2 = i0 + 1; i2 < mSize; ++i2)
        {
            if (mDiagonal[i2] < minValue)
            {
                i1 = i2;
                minValue = mDiagonal[i1];
            }
        }

        if (i1 != i0)
        {
            // Swap the eigenvalues.
            mDiagonal[i1] = mDiagonal[i0];
            mDiagonal[i0] = minValue;

            // Swap the eigenvectors corresponding to the eigenvalues.
            for (i2 = 0; i2 < mSize; ++i2)
            {
                Real tmp = mMatrix[i2][i0];
                mMatrix[i2][i0] = mMatrix[i2][i1];
                mMatrix[i2][i1] = tmp;
                mIsRotation = !mIsRotation;
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void EigenDecomposition<Real>::GuaranteeRotation ()
{
    if (!mIsRotation)
    {
        // Change sign on the first column.
        for (int row = 0; row < mSize; ++row)
        {
            mMatrix[row][0] = -mMatrix[row][0];
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class EigenDecomposition<float>;

template WM5_MATHEMATICS_ITEM
class EigenDecomposition<double>;
}
//----------------------------------------------------------------------------
