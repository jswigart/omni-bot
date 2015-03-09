// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
Matrix4<Real>::Matrix4 (bool makeZero)
{
    if (makeZero)
    {
        MakeZero();
    }
    else
    {
        MakeIdentity();
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix4<Real>::Matrix4 (const Matrix4& mat)
{
    mEntry[ 0] = mat.mEntry[ 0];
    mEntry[ 1] = mat.mEntry[ 1];
    mEntry[ 2] = mat.mEntry[ 2];
    mEntry[ 3] = mat.mEntry[ 3];
    mEntry[ 4] = mat.mEntry[ 4];
    mEntry[ 5] = mat.mEntry[ 5];
    mEntry[ 6] = mat.mEntry[ 6];
    mEntry[ 7] = mat.mEntry[ 7];
    mEntry[ 8] = mat.mEntry[ 8];
    mEntry[ 9] = mat.mEntry[ 9];
    mEntry[10] = mat.mEntry[10];
    mEntry[11] = mat.mEntry[11];
    mEntry[12] = mat.mEntry[12];
    mEntry[13] = mat.mEntry[13];
    mEntry[14] = mat.mEntry[14];
    mEntry[15] = mat.mEntry[15];
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix4<Real>::Matrix4 (Real m00, Real m01, Real m02, Real m03, Real m10,
    Real m11, Real m12, Real m13, Real m20, Real m21, Real m22, Real m23,
    Real m30, Real m31, Real m32, Real m33)
{
    mEntry[ 0] = m00;
    mEntry[ 1] = m01;
    mEntry[ 2] = m02;
    mEntry[ 3] = m03;
    mEntry[ 4] = m10;
    mEntry[ 5] = m11;
    mEntry[ 6] = m12;
    mEntry[ 7] = m13;
    mEntry[ 8] = m20;
    mEntry[ 9] = m21;
    mEntry[10] = m22;
    mEntry[11] = m23;
    mEntry[12] = m30;
    mEntry[13] = m31;
    mEntry[14] = m32;
    mEntry[15] = m33;
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix4<Real>::Matrix4 (const Real entry[16], bool rowMajor)
{
    if (rowMajor)
    {
        mEntry[ 0] = entry[ 0];
        mEntry[ 1] = entry[ 1];
        mEntry[ 2] = entry[ 2];
        mEntry[ 3] = entry[ 3];
        mEntry[ 4] = entry[ 4];
        mEntry[ 5] = entry[ 5];
        mEntry[ 6] = entry[ 6];
        mEntry[ 7] = entry[ 7];
        mEntry[ 8] = entry[ 8];
        mEntry[ 9] = entry[ 9];
        mEntry[10] = entry[10];
        mEntry[11] = entry[11];
        mEntry[12] = entry[12];
        mEntry[13] = entry[13];
        mEntry[14] = entry[14];
        mEntry[15] = entry[15];
    }
    else
    {
        mEntry[ 0] = entry[ 0];
        mEntry[ 1] = entry[ 4];
        mEntry[ 2] = entry[ 8];
        mEntry[ 3] = entry[12];
        mEntry[ 4] = entry[ 1];
        mEntry[ 5] = entry[ 5];
        mEntry[ 6] = entry[ 9];
        mEntry[ 7] = entry[13];
        mEntry[ 8] = entry[ 2];
        mEntry[ 9] = entry[ 6];
        mEntry[10] = entry[10];
        mEntry[11] = entry[14];
        mEntry[12] = entry[ 3];
        mEntry[13] = entry[ 7];
        mEntry[14] = entry[11];
        mEntry[15] = entry[15];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix4<Real>& Matrix4<Real>::operator= (const Matrix4& mat)
{
    mEntry[ 0] = mat.mEntry[ 0];
    mEntry[ 1] = mat.mEntry[ 1];
    mEntry[ 2] = mat.mEntry[ 2];
    mEntry[ 3] = mat.mEntry[ 3];
    mEntry[ 4] = mat.mEntry[ 4];
    mEntry[ 5] = mat.mEntry[ 5];
    mEntry[ 6] = mat.mEntry[ 6];
    mEntry[ 7] = mat.mEntry[ 7];
    mEntry[ 8] = mat.mEntry[ 8];
    mEntry[ 9] = mat.mEntry[ 9];
    mEntry[10] = mat.mEntry[10];
    mEntry[11] = mat.mEntry[11];
    mEntry[12] = mat.mEntry[12];
    mEntry[13] = mat.mEntry[13];
    mEntry[14] = mat.mEntry[14];
    mEntry[15] = mat.mEntry[15];
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix4<Real>::MakeZero ()
{
    mEntry[ 0] = (Real)0;
    mEntry[ 1] = (Real)0;
    mEntry[ 2] = (Real)0;
    mEntry[ 3] = (Real)0;
    mEntry[ 4] = (Real)0;
    mEntry[ 5] = (Real)0;
    mEntry[ 6] = (Real)0;
    mEntry[ 7] = (Real)0;
    mEntry[ 8] = (Real)0;
    mEntry[ 9] = (Real)0;
    mEntry[10] = (Real)0;
    mEntry[11] = (Real)0;
    mEntry[12] = (Real)0;
    mEntry[13] = (Real)0;
    mEntry[14] = (Real)0;
    mEntry[15] = (Real)0;
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix4<Real>::MakeIdentity ()
{
    mEntry[ 0] = (Real)1;
    mEntry[ 1] = (Real)0;
    mEntry[ 2] = (Real)0;
    mEntry[ 3] = (Real)0;
    mEntry[ 4] = (Real)0;
    mEntry[ 5] = (Real)1;
    mEntry[ 6] = (Real)0;
    mEntry[ 7] = (Real)0;
    mEntry[ 8] = (Real)0;
    mEntry[ 9] = (Real)0;
    mEntry[10] = (Real)1;
    mEntry[11] = (Real)0;
    mEntry[12] = (Real)0;
    mEntry[13] = (Real)0;
    mEntry[14] = (Real)0;
    mEntry[15] = (Real)1;
}
//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real> Matrix4<Real>::operator+ (const Matrix4& mat) const
{
    return Matrix4<Real>
    (
        mEntry[ 0] + mat.mEntry[ 0],
        mEntry[ 1] + mat.mEntry[ 1],
        mEntry[ 2] + mat.mEntry[ 2],
        mEntry[ 3] + mat.mEntry[ 3],
        mEntry[ 4] + mat.mEntry[ 4],
        mEntry[ 5] + mat.mEntry[ 5],
        mEntry[ 6] + mat.mEntry[ 6],
        mEntry[ 7] + mat.mEntry[ 7],
        mEntry[ 8] + mat.mEntry[ 8],
        mEntry[ 9] + mat.mEntry[ 9],
        mEntry[10] + mat.mEntry[10],
        mEntry[11] + mat.mEntry[11],
        mEntry[12] + mat.mEntry[12],
        mEntry[13] + mat.mEntry[13],
        mEntry[14] + mat.mEntry[14],
        mEntry[15] + mat.mEntry[15]
    );
}
//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real> Matrix4<Real>::operator- (const Matrix4& mat) const
{
    return Matrix4<Real>
    (
        mEntry[ 0] - mat.mEntry[ 0],
        mEntry[ 1] - mat.mEntry[ 1],
        mEntry[ 2] - mat.mEntry[ 2],
        mEntry[ 3] - mat.mEntry[ 3],
        mEntry[ 4] - mat.mEntry[ 4],
        mEntry[ 5] - mat.mEntry[ 5],
        mEntry[ 6] - mat.mEntry[ 6],
        mEntry[ 7] - mat.mEntry[ 7],
        mEntry[ 8] - mat.mEntry[ 8],
        mEntry[ 9] - mat.mEntry[ 9],
        mEntry[10] - mat.mEntry[10],
        mEntry[11] - mat.mEntry[11],
        mEntry[12] - mat.mEntry[12],
        mEntry[13] - mat.mEntry[13],
        mEntry[14] - mat.mEntry[14],
        mEntry[15] - mat.mEntry[15]
    );
}
//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real> Matrix4<Real>::operator* (Real scalar) const
{
    return Matrix4<Real>
    (
        scalar*mEntry[ 0],
        scalar*mEntry[ 1],
        scalar*mEntry[ 2],
        scalar*mEntry[ 3],
        scalar*mEntry[ 4],
        scalar*mEntry[ 5],
        scalar*mEntry[ 6],
        scalar*mEntry[ 7],
        scalar*mEntry[ 8],
        scalar*mEntry[ 9],
        scalar*mEntry[10],
        scalar*mEntry[11],
        scalar*mEntry[12],
        scalar*mEntry[13],
        scalar*mEntry[14],
        scalar*mEntry[15]
    );
}
//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real> Matrix4<Real>::operator/ (Real scalar) const
{
    if (scalar != (Real)0)
    {
        Real invScalar = ((Real)1)/scalar;
        return Matrix4<Real>
        (
            invScalar*mEntry[ 0],
            invScalar*mEntry[ 1],
            invScalar*mEntry[ 2],
            invScalar*mEntry[ 3],
            invScalar*mEntry[ 4],
            invScalar*mEntry[ 5],
            invScalar*mEntry[ 6],
            invScalar*mEntry[ 7],
            invScalar*mEntry[ 8],
            invScalar*mEntry[ 9],
            invScalar*mEntry[10],
            invScalar*mEntry[11],
            invScalar*mEntry[12],
            invScalar*mEntry[13],
            invScalar*mEntry[14],
            invScalar*mEntry[15]
        );
    }
    else
    {
        return Matrix4<Real>
        (
            Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL
        );
    }
}
//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real> Matrix4<Real>::operator- () const
{
    return Matrix4<Real>
    (
        -mEntry[ 0],
        -mEntry[ 1],
        -mEntry[ 2],
        -mEntry[ 3],
        -mEntry[ 4],
        -mEntry[ 5],
        -mEntry[ 6],
        -mEntry[ 7],
        -mEntry[ 8],
        -mEntry[ 9],
        -mEntry[10],
        -mEntry[11],
        -mEntry[12],
        -mEntry[13],
        -mEntry[14],
        -mEntry[15]
    );
}
//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real>& Matrix4<Real>::operator+= (const Matrix4& mat)
{
    mEntry[ 0] += mat.mEntry[ 0];
    mEntry[ 1] += mat.mEntry[ 1];
    mEntry[ 2] += mat.mEntry[ 2];
    mEntry[ 3] += mat.mEntry[ 3];
    mEntry[ 4] += mat.mEntry[ 4];
    mEntry[ 5] += mat.mEntry[ 5];
    mEntry[ 6] += mat.mEntry[ 6];
    mEntry[ 7] += mat.mEntry[ 7];
    mEntry[ 8] += mat.mEntry[ 8];
    mEntry[ 9] += mat.mEntry[ 9];
    mEntry[10] += mat.mEntry[10];
    mEntry[11] += mat.mEntry[11];
    mEntry[12] += mat.mEntry[12];
    mEntry[13] += mat.mEntry[13];
    mEntry[14] += mat.mEntry[14];
    mEntry[15] += mat.mEntry[15];
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real>& Matrix4<Real>::operator-= (const Matrix4& mat)
{
    mEntry[ 0] -= mat.mEntry[ 0];
    mEntry[ 1] -= mat.mEntry[ 1];
    mEntry[ 2] -= mat.mEntry[ 2];
    mEntry[ 3] -= mat.mEntry[ 3];
    mEntry[ 4] -= mat.mEntry[ 4];
    mEntry[ 5] -= mat.mEntry[ 5];
    mEntry[ 6] -= mat.mEntry[ 6];
    mEntry[ 7] -= mat.mEntry[ 7];
    mEntry[ 8] -= mat.mEntry[ 8];
    mEntry[ 9] -= mat.mEntry[ 9];
    mEntry[10] -= mat.mEntry[10];
    mEntry[11] -= mat.mEntry[11];
    mEntry[12] -= mat.mEntry[12];
    mEntry[13] -= mat.mEntry[13];
    mEntry[14] -= mat.mEntry[14];
    mEntry[15] -= mat.mEntry[15];
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real>& Matrix4<Real>::operator*= (Real scalar)
{
    mEntry[ 0] *= scalar;
    mEntry[ 1] *= scalar;
    mEntry[ 2] *= scalar;
    mEntry[ 3] *= scalar;
    mEntry[ 4] *= scalar;
    mEntry[ 5] *= scalar;
    mEntry[ 6] *= scalar;
    mEntry[ 7] *= scalar;
    mEntry[ 8] *= scalar;
    mEntry[ 9] *= scalar;
    mEntry[10] *= scalar;
    mEntry[11] *= scalar;
    mEntry[12] *= scalar;
    mEntry[13] *= scalar;
    mEntry[14] *= scalar;
    mEntry[15] *= scalar;
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real>& Matrix4<Real>::operator/= (Real scalar)
{
    if (scalar != (Real)0)
    {
        Real invScalar = ((Real)1)/scalar;
        mEntry[ 0] *= invScalar;
        mEntry[ 1] *= invScalar;
        mEntry[ 2] *= invScalar;
        mEntry[ 3] *= invScalar;
        mEntry[ 4] *= invScalar;
        mEntry[ 5] *= invScalar;
        mEntry[ 6] *= invScalar;
        mEntry[ 7] *= invScalar;
        mEntry[ 8] *= invScalar;
        mEntry[ 9] *= invScalar;
        mEntry[10] *= invScalar;
        mEntry[11] *= invScalar;
        mEntry[12] *= invScalar;
        mEntry[13] *= invScalar;
        mEntry[14] *= invScalar;
        mEntry[15] *= invScalar;
    }
    else
    {
        mEntry[ 0] = Math<Real>::MAX_REAL;
        mEntry[ 1] = Math<Real>::MAX_REAL;
        mEntry[ 2] = Math<Real>::MAX_REAL;
        mEntry[ 3] = Math<Real>::MAX_REAL;
        mEntry[ 4] = Math<Real>::MAX_REAL;
        mEntry[ 5] = Math<Real>::MAX_REAL;
        mEntry[ 6] = Math<Real>::MAX_REAL;
        mEntry[ 7] = Math<Real>::MAX_REAL;
        mEntry[ 8] = Math<Real>::MAX_REAL;
        mEntry[ 9] = Math<Real>::MAX_REAL;
        mEntry[10] = Math<Real>::MAX_REAL;
        mEntry[11] = Math<Real>::MAX_REAL;
        mEntry[12] = Math<Real>::MAX_REAL;
        mEntry[13] = Math<Real>::MAX_REAL;
        mEntry[14] = Math<Real>::MAX_REAL;
        mEntry[15] = Math<Real>::MAX_REAL;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real> Matrix4<Real>::operator* (const Vector4<Real>& vec) const
{
    return Vector4<Real>
    (
        mEntry[ 0]*vec[0] +
        mEntry[ 1]*vec[1] +
        mEntry[ 2]*vec[2] +
        mEntry[ 3]*vec[3],

        mEntry[ 4]*vec[0] +
        mEntry[ 5]*vec[1] +
        mEntry[ 6]*vec[2] +
        mEntry[ 7]*vec[3],

        mEntry[ 8]*vec[0] +
        mEntry[ 9]*vec[1] +
        mEntry[10]*vec[2] +
        mEntry[11]*vec[3],

        mEntry[12]*vec[0] +
        mEntry[13]*vec[1] +
        mEntry[14]*vec[2] +
        mEntry[15]*vec[3]
    );
}
//----------------------------------------------------------------------------
template <class Real>
Matrix4<Real> Matrix4<Real>::Transpose () const
{
    return Matrix4<Real>
    (
        mEntry[ 0],
        mEntry[ 4],
        mEntry[ 8],
        mEntry[12],
        mEntry[ 1],
        mEntry[ 5],
        mEntry[ 9],
        mEntry[13],
        mEntry[ 2],
        mEntry[ 6],
        mEntry[10],
        mEntry[14],
        mEntry[ 3],
        mEntry[ 7],
        mEntry[11],
        mEntry[15]
    );
}
//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real> Matrix4<Real>::operator* (const Matrix4& mat) const
{
    // A*B
    return Matrix4<Real>
    (
        mEntry[ 0]*mat.mEntry[ 0] +
        mEntry[ 1]*mat.mEntry[ 4] +
        mEntry[ 2]*mat.mEntry[ 8] +
        mEntry[ 3]*mat.mEntry[12],

        mEntry[ 0]*mat.mEntry[ 1] +
        mEntry[ 1]*mat.mEntry[ 5] +
        mEntry[ 2]*mat.mEntry[ 9] +
        mEntry[ 3]*mat.mEntry[13],

        mEntry[ 0]*mat.mEntry[ 2] +
        mEntry[ 1]*mat.mEntry[ 6] +
        mEntry[ 2]*mat.mEntry[10] +
        mEntry[ 3]*mat.mEntry[14],

        mEntry[ 0]*mat.mEntry[ 3] +
        mEntry[ 1]*mat.mEntry[ 7] +
        mEntry[ 2]*mat.mEntry[11] +
        mEntry[ 3]*mat.mEntry[15],

        mEntry[ 4]*mat.mEntry[ 0] +
        mEntry[ 5]*mat.mEntry[ 4] +
        mEntry[ 6]*mat.mEntry[ 8] +
        mEntry[ 7]*mat.mEntry[12],

        mEntry[ 4]*mat.mEntry[ 1] +
        mEntry[ 5]*mat.mEntry[ 5] +
        mEntry[ 6]*mat.mEntry[ 9] +
        mEntry[ 7]*mat.mEntry[13],

        mEntry[ 4]*mat.mEntry[ 2] +
        mEntry[ 5]*mat.mEntry[ 6] +
        mEntry[ 6]*mat.mEntry[10] +
        mEntry[ 7]*mat.mEntry[14],

        mEntry[ 4]*mat.mEntry[ 3] +
        mEntry[ 5]*mat.mEntry[ 7] +
        mEntry[ 6]*mat.mEntry[11] +
        mEntry[ 7]*mat.mEntry[15],

        mEntry[ 8]*mat.mEntry[ 0] +
        mEntry[ 9]*mat.mEntry[ 4] +
        mEntry[10]*mat.mEntry[ 8] +
        mEntry[11]*mat.mEntry[12],

        mEntry[ 8]*mat.mEntry[ 1] +
        mEntry[ 9]*mat.mEntry[ 5] +
        mEntry[10]*mat.mEntry[ 9] +
        mEntry[11]*mat.mEntry[13],

        mEntry[ 8]*mat.mEntry[ 2] +
        mEntry[ 9]*mat.mEntry[ 6] +
        mEntry[10]*mat.mEntry[10] +
        mEntry[11]*mat.mEntry[14],

        mEntry[ 8]*mat.mEntry[ 3] +
        mEntry[ 9]*mat.mEntry[ 7] +
        mEntry[10]*mat.mEntry[11] +
        mEntry[11]*mat.mEntry[15],

        mEntry[12]*mat.mEntry[ 0] +
        mEntry[13]*mat.mEntry[ 4] +
        mEntry[14]*mat.mEntry[ 8] +
        mEntry[15]*mat.mEntry[12],

        mEntry[12]*mat.mEntry[ 1] +
        mEntry[13]*mat.mEntry[ 5] +
        mEntry[14]*mat.mEntry[ 9] +
        mEntry[15]*mat.mEntry[13],

        mEntry[12]*mat.mEntry[ 2] +
        mEntry[13]*mat.mEntry[ 6] +
        mEntry[14]*mat.mEntry[10] +
        mEntry[15]*mat.mEntry[14],

        mEntry[12]*mat.mEntry[ 3] +
        mEntry[13]*mat.mEntry[ 7] +
        mEntry[14]*mat.mEntry[11] +
        mEntry[15]*mat.mEntry[15]
    );
}
//----------------------------------------------------------------------------
template <class Real>
Matrix4<Real> Matrix4<Real>::TransposeTimes (const Matrix4& mat) const
{
    // A^T*B
    return Matrix4<Real>
    (
        mEntry[ 0]*mat.mEntry[ 0] +
        mEntry[ 4]*mat.mEntry[ 4] +
        mEntry[ 8]*mat.mEntry[ 8] +
        mEntry[12]*mat.mEntry[12],

        mEntry[ 0]*mat.mEntry[ 1] +
        mEntry[ 4]*mat.mEntry[ 5] +
        mEntry[ 8]*mat.mEntry[ 9] +
        mEntry[12]*mat.mEntry[13],

        mEntry[ 0]*mat.mEntry[ 2] +
        mEntry[ 4]*mat.mEntry[ 6] +
        mEntry[ 8]*mat.mEntry[10] +
        mEntry[12]*mat.mEntry[14],

        mEntry[ 0]*mat.mEntry[ 3] +
        mEntry[ 4]*mat.mEntry[ 7] +
        mEntry[ 8]*mat.mEntry[11] +
        mEntry[12]*mat.mEntry[15],

        mEntry[ 1]*mat.mEntry[ 0] +
        mEntry[ 5]*mat.mEntry[ 4] +
        mEntry[ 9]*mat.mEntry[ 8] +
        mEntry[13]*mat.mEntry[12],

        mEntry[ 1]*mat.mEntry[ 1] +
        mEntry[ 5]*mat.mEntry[ 5] +
        mEntry[ 9]*mat.mEntry[ 9] +
        mEntry[13]*mat.mEntry[13],

        mEntry[ 1]*mat.mEntry[ 2] +
        mEntry[ 5]*mat.mEntry[ 6] +
        mEntry[ 9]*mat.mEntry[10] +
        mEntry[13]*mat.mEntry[14],

        mEntry[ 1]*mat.mEntry[ 3] +
        mEntry[ 5]*mat.mEntry[ 7] +
        mEntry[ 9]*mat.mEntry[11] +
        mEntry[13]*mat.mEntry[15],

        mEntry[ 2]*mat.mEntry[ 0] +
        mEntry[ 6]*mat.mEntry[ 4] +
        mEntry[10]*mat.mEntry[ 8] +
        mEntry[14]*mat.mEntry[12],

        mEntry[ 2]*mat.mEntry[ 1] +
        mEntry[ 6]*mat.mEntry[ 5] +
        mEntry[10]*mat.mEntry[ 9] +
        mEntry[14]*mat.mEntry[13],

        mEntry[ 2]*mat.mEntry[ 2] +
        mEntry[ 6]*mat.mEntry[ 6] +
        mEntry[10]*mat.mEntry[10] +
        mEntry[14]*mat.mEntry[14],

        mEntry[ 2]*mat.mEntry[ 3] +
        mEntry[ 6]*mat.mEntry[ 7] +
        mEntry[10]*mat.mEntry[11] +
        mEntry[14]*mat.mEntry[15],

        mEntry[ 3]*mat.mEntry[ 0] +
        mEntry[ 7]*mat.mEntry[ 4] +
        mEntry[11]*mat.mEntry[ 8] +
        mEntry[15]*mat.mEntry[12],

        mEntry[ 3]*mat.mEntry[ 1] +
        mEntry[ 7]*mat.mEntry[ 5] +
        mEntry[11]*mat.mEntry[ 9] +
        mEntry[15]*mat.mEntry[13],

        mEntry[ 3]*mat.mEntry[ 2] +
        mEntry[ 7]*mat.mEntry[ 6] +
        mEntry[11]*mat.mEntry[10] +
        mEntry[15]*mat.mEntry[14],

        mEntry[ 3]*mat.mEntry[ 3] +
        mEntry[ 7]*mat.mEntry[ 7] +
        mEntry[11]*mat.mEntry[11] +
        mEntry[15]*mat.mEntry[15]
    );
}
//----------------------------------------------------------------------------
template <class Real>
Matrix4<Real> Matrix4<Real>::TimesTranspose (const Matrix4& mat) const
{
    // A*B^T
    return Matrix4<Real>
    (
        mEntry[ 0]*mat.mEntry[ 0] +
        mEntry[ 1]*mat.mEntry[ 1] +
        mEntry[ 2]*mat.mEntry[ 2] +
        mEntry[ 3]*mat.mEntry[ 3],

        mEntry[ 0]*mat.mEntry[ 4] +
        mEntry[ 1]*mat.mEntry[ 5] +
        mEntry[ 2]*mat.mEntry[ 6] +
        mEntry[ 3]*mat.mEntry[ 7],

        mEntry[ 0]*mat.mEntry[ 8] +
        mEntry[ 1]*mat.mEntry[ 9] +
        mEntry[ 2]*mat.mEntry[10] +
        mEntry[ 3]*mat.mEntry[11],

        mEntry[ 0]*mat.mEntry[12] +
        mEntry[ 1]*mat.mEntry[13] +
        mEntry[ 2]*mat.mEntry[14] +
        mEntry[ 3]*mat.mEntry[15],

        mEntry[ 4]*mat.mEntry[ 0] +
        mEntry[ 5]*mat.mEntry[ 1] +
        mEntry[ 6]*mat.mEntry[ 2] +
        mEntry[ 7]*mat.mEntry[ 3],

        mEntry[ 4]*mat.mEntry[ 4] +
        mEntry[ 5]*mat.mEntry[ 5] +
        mEntry[ 6]*mat.mEntry[ 6] +
        mEntry[ 7]*mat.mEntry[ 7],

        mEntry[ 4]*mat.mEntry[ 8] +
        mEntry[ 5]*mat.mEntry[ 9] +
        mEntry[ 6]*mat.mEntry[10] +
        mEntry[ 7]*mat.mEntry[11],

        mEntry[ 4]*mat.mEntry[12] +
        mEntry[ 5]*mat.mEntry[13] +
        mEntry[ 6]*mat.mEntry[14] +
        mEntry[ 7]*mat.mEntry[15],

        mEntry[ 8]*mat.mEntry[ 0] +
        mEntry[ 9]*mat.mEntry[ 1] +
        mEntry[10]*mat.mEntry[ 2] +
        mEntry[11]*mat.mEntry[ 3],

        mEntry[ 8]*mat.mEntry[ 4] +
        mEntry[ 9]*mat.mEntry[ 5] +
        mEntry[10]*mat.mEntry[ 6] +
        mEntry[11]*mat.mEntry[ 7],

        mEntry[ 8]*mat.mEntry[ 8] +
        mEntry[ 9]*mat.mEntry[ 9] +
        mEntry[10]*mat.mEntry[10] +
        mEntry[11]*mat.mEntry[11],

        mEntry[ 8]*mat.mEntry[12] +
        mEntry[ 9]*mat.mEntry[13] +
        mEntry[10]*mat.mEntry[14] +
        mEntry[11]*mat.mEntry[15],

        mEntry[12]*mat.mEntry[ 0] +
        mEntry[13]*mat.mEntry[ 1] +
        mEntry[14]*mat.mEntry[ 2] +
        mEntry[15]*mat.mEntry[ 3],

        mEntry[12]*mat.mEntry[ 4] +
        mEntry[13]*mat.mEntry[ 5] +
        mEntry[14]*mat.mEntry[ 6] +
        mEntry[15]*mat.mEntry[ 7],

        mEntry[12]*mat.mEntry[ 8] +
        mEntry[13]*mat.mEntry[ 9] +
        mEntry[14]*mat.mEntry[10] +
        mEntry[15]*mat.mEntry[11],

        mEntry[12]*mat.mEntry[12] +
        mEntry[13]*mat.mEntry[13] +
        mEntry[14]*mat.mEntry[14] +
        mEntry[15]*mat.mEntry[15]
    );
}
//----------------------------------------------------------------------------
template <class Real>
Matrix4<Real> Matrix4<Real>::TransposeTimesTranspose (const Matrix4& mat)
    const
{
    // A^T*B^T
    return Matrix4<Real>
    (
        mEntry[ 0]*mat.mEntry[ 0] +
        mEntry[ 4]*mat.mEntry[ 1] +
        mEntry[ 8]*mat.mEntry[ 2] +
        mEntry[12]*mat.mEntry[ 3],

        mEntry[ 0]*mat.mEntry[ 4] +
        mEntry[ 4]*mat.mEntry[ 5] +
        mEntry[ 8]*mat.mEntry[ 6] +
        mEntry[12]*mat.mEntry[ 7],

        mEntry[ 0]*mat.mEntry[ 8] +
        mEntry[ 4]*mat.mEntry[ 9] +
        mEntry[ 8]*mat.mEntry[10] +
        mEntry[12]*mat.mEntry[11],

        mEntry[ 0]*mat.mEntry[12] +
        mEntry[ 4]*mat.mEntry[13] +
        mEntry[ 8]*mat.mEntry[14] +
        mEntry[12]*mat.mEntry[15],

        mEntry[ 1]*mat.mEntry[ 0] +
        mEntry[ 5]*mat.mEntry[ 1] +
        mEntry[ 9]*mat.mEntry[ 2] +
        mEntry[13]*mat.mEntry[ 3],

        mEntry[ 1]*mat.mEntry[ 4] +
        mEntry[ 5]*mat.mEntry[ 5] +
        mEntry[ 9]*mat.mEntry[ 6] +
        mEntry[13]*mat.mEntry[ 7],

        mEntry[ 1]*mat.mEntry[ 8] +
        mEntry[ 5]*mat.mEntry[ 9] +
        mEntry[ 9]*mat.mEntry[10] +
        mEntry[13]*mat.mEntry[11],

        mEntry[ 1]*mat.mEntry[12] +
        mEntry[ 5]*mat.mEntry[13] +
        mEntry[ 9]*mat.mEntry[14] +
        mEntry[13]*mat.mEntry[15],

        mEntry[ 2]*mat.mEntry[ 0] +
        mEntry[ 6]*mat.mEntry[ 1] +
        mEntry[10]*mat.mEntry[ 2] +
        mEntry[14]*mat.mEntry[ 3],

        mEntry[ 2]*mat.mEntry[ 4] +
        mEntry[ 6]*mat.mEntry[ 5] +
        mEntry[10]*mat.mEntry[ 6] +
        mEntry[14]*mat.mEntry[ 7],

        mEntry[ 2]*mat.mEntry[ 8] +
        mEntry[ 6]*mat.mEntry[ 9] +
        mEntry[10]*mat.mEntry[10] +
        mEntry[14]*mat.mEntry[11],

        mEntry[ 2]*mat.mEntry[12] +
        mEntry[ 6]*mat.mEntry[13] +
        mEntry[10]*mat.mEntry[14] +
        mEntry[14]*mat.mEntry[15],

        mEntry[ 3]*mat.mEntry[ 0] +
        mEntry[ 7]*mat.mEntry[ 1] +
        mEntry[11]*mat.mEntry[ 2] +
        mEntry[15]*mat.mEntry[ 3],

        mEntry[ 3]*mat.mEntry[ 4] +
        mEntry[ 7]*mat.mEntry[ 5] +
        mEntry[11]*mat.mEntry[ 6] +
        mEntry[15]*mat.mEntry[ 7],

        mEntry[ 3]*mat.mEntry[ 8] +
        mEntry[ 7]*mat.mEntry[ 9] +
        mEntry[11]*mat.mEntry[10] +
        mEntry[15]*mat.mEntry[11],

        mEntry[ 3]*mat.mEntry[12] +
        mEntry[ 7]*mat.mEntry[13] +
        mEntry[11]*mat.mEntry[14] +
        mEntry[15]*mat.mEntry[15]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix4<Real> Matrix4<Real>::Inverse (const Real epsilon) const
{
    Real a0 = mEntry[ 0]*mEntry[ 5] - mEntry[ 1]*mEntry[ 4];
    Real a1 = mEntry[ 0]*mEntry[ 6] - mEntry[ 2]*mEntry[ 4];
    Real a2 = mEntry[ 0]*mEntry[ 7] - mEntry[ 3]*mEntry[ 4];
    Real a3 = mEntry[ 1]*mEntry[ 6] - mEntry[ 2]*mEntry[ 5];
    Real a4 = mEntry[ 1]*mEntry[ 7] - mEntry[ 3]*mEntry[ 5];
    Real a5 = mEntry[ 2]*mEntry[ 7] - mEntry[ 3]*mEntry[ 6];
    Real b0 = mEntry[ 8]*mEntry[13] - mEntry[ 9]*mEntry[12];
    Real b1 = mEntry[ 8]*mEntry[14] - mEntry[10]*mEntry[12];
    Real b2 = mEntry[ 8]*mEntry[15] - mEntry[11]*mEntry[12];
    Real b3 = mEntry[ 9]*mEntry[14] - mEntry[10]*mEntry[13];
    Real b4 = mEntry[ 9]*mEntry[15] - mEntry[11]*mEntry[13];
    Real b5 = mEntry[10]*mEntry[15] - mEntry[11]*mEntry[14];

    Real det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
    if (Math<Real>::FAbs(det) > epsilon)
    {
        Matrix4 inverse;
        inverse.mEntry[ 0] = + mEntry[ 5]*b5 - mEntry[ 6]*b4 + mEntry[ 7]*b3;
        inverse.mEntry[ 4] = - mEntry[ 4]*b5 + mEntry[ 6]*b2 - mEntry[ 7]*b1;
        inverse.mEntry[ 8] = + mEntry[ 4]*b4 - mEntry[ 5]*b2 + mEntry[ 7]*b0;
        inverse.mEntry[12] = - mEntry[ 4]*b3 + mEntry[ 5]*b1 - mEntry[ 6]*b0;
        inverse.mEntry[ 1] = - mEntry[ 1]*b5 + mEntry[ 2]*b4 - mEntry[ 3]*b3;
        inverse.mEntry[ 5] = + mEntry[ 0]*b5 - mEntry[ 2]*b2 + mEntry[ 3]*b1;
        inverse.mEntry[ 9] = - mEntry[ 0]*b4 + mEntry[ 1]*b2 - mEntry[ 3]*b0;
        inverse.mEntry[13] = + mEntry[ 0]*b3 - mEntry[ 1]*b1 + mEntry[ 2]*b0;
        inverse.mEntry[ 2] = + mEntry[13]*a5 - mEntry[14]*a4 + mEntry[15]*a3;
        inverse.mEntry[ 6] = - mEntry[12]*a5 + mEntry[14]*a2 - mEntry[15]*a1;
        inverse.mEntry[10] = + mEntry[12]*a4 - mEntry[13]*a2 + mEntry[15]*a0;
        inverse.mEntry[14] = - mEntry[12]*a3 + mEntry[13]*a1 - mEntry[14]*a0;
        inverse.mEntry[ 3] = - mEntry[ 9]*a5 + mEntry[10]*a4 - mEntry[11]*a3;
        inverse.mEntry[ 7] = + mEntry[ 8]*a5 - mEntry[10]*a2 + mEntry[11]*a1;
        inverse.mEntry[11] = - mEntry[ 8]*a4 + mEntry[ 9]*a2 - mEntry[11]*a0;
        inverse.mEntry[15] = + mEntry[ 8]*a3 - mEntry[ 9]*a1 + mEntry[10]*a0;

        Real invDet = ((Real)1)/det;
        inverse.mEntry[ 0] *= invDet;
        inverse.mEntry[ 1] *= invDet;
        inverse.mEntry[ 2] *= invDet;
        inverse.mEntry[ 3] *= invDet;
        inverse.mEntry[ 4] *= invDet;
        inverse.mEntry[ 5] *= invDet;
        inverse.mEntry[ 6] *= invDet;
        inverse.mEntry[ 7] *= invDet;
        inverse.mEntry[ 8] *= invDet;
        inverse.mEntry[ 9] *= invDet;
        inverse.mEntry[10] *= invDet;
        inverse.mEntry[11] *= invDet;
        inverse.mEntry[12] *= invDet;
        inverse.mEntry[13] *= invDet;
        inverse.mEntry[14] *= invDet;
        inverse.mEntry[15] *= invDet;

        return inverse;
    }

    return ZERO;
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix4<Real> Matrix4<Real>::Adjoint () const
{
    Real a0 = mEntry[ 0]*mEntry[ 5] - mEntry[ 1]*mEntry[ 4];
    Real a1 = mEntry[ 0]*mEntry[ 6] - mEntry[ 2]*mEntry[ 4];
    Real a2 = mEntry[ 0]*mEntry[ 7] - mEntry[ 3]*mEntry[ 4];
    Real a3 = mEntry[ 1]*mEntry[ 6] - mEntry[ 2]*mEntry[ 5];
    Real a4 = mEntry[ 1]*mEntry[ 7] - mEntry[ 3]*mEntry[ 5];
    Real a5 = mEntry[ 2]*mEntry[ 7] - mEntry[ 3]*mEntry[ 6];
    Real b0 = mEntry[ 8]*mEntry[13] - mEntry[ 9]*mEntry[12];
    Real b1 = mEntry[ 8]*mEntry[14] - mEntry[10]*mEntry[12];
    Real b2 = mEntry[ 8]*mEntry[15] - mEntry[11]*mEntry[12];
    Real b3 = mEntry[ 9]*mEntry[14] - mEntry[10]*mEntry[13];
    Real b4 = mEntry[ 9]*mEntry[15] - mEntry[11]*mEntry[13];
    Real b5 = mEntry[10]*mEntry[15] - mEntry[11]*mEntry[14];

    return Matrix4<Real>
    (
        + mEntry[ 5]*b5 - mEntry[ 6]*b4 + mEntry[ 7]*b3,
        - mEntry[ 1]*b5 + mEntry[ 2]*b4 - mEntry[ 3]*b3,
        + mEntry[13]*a5 - mEntry[14]*a4 + mEntry[15]*a3,
        - mEntry[ 9]*a5 + mEntry[10]*a4 - mEntry[11]*a3,
        - mEntry[ 4]*b5 + mEntry[ 6]*b2 - mEntry[ 7]*b1,
        + mEntry[ 0]*b5 - mEntry[ 2]*b2 + mEntry[ 3]*b1,
        - mEntry[12]*a5 + mEntry[14]*a2 - mEntry[15]*a1,
        + mEntry[ 8]*a5 - mEntry[10]*a2 + mEntry[11]*a1,
        + mEntry[ 4]*b4 - mEntry[ 5]*b2 + mEntry[ 7]*b0,
        - mEntry[ 0]*b4 + mEntry[ 1]*b2 - mEntry[ 3]*b0,
        + mEntry[12]*a4 - mEntry[13]*a2 + mEntry[15]*a0,
        - mEntry[ 8]*a4 + mEntry[ 9]*a2 - mEntry[11]*a0,
        - mEntry[ 4]*b3 + mEntry[ 5]*b1 - mEntry[ 6]*b0,
        + mEntry[ 0]*b3 - mEntry[ 1]*b1 + mEntry[ 2]*b0,
        - mEntry[12]*a3 + mEntry[13]*a1 - mEntry[14]*a0,
        + mEntry[ 8]*a3 - mEntry[ 9]*a1 + mEntry[10]*a0
    );
}
//----------------------------------------------------------------------------
template <typename Real>
Real Matrix4<Real>::Determinant () const
{
    Real a0 = mEntry[ 0]*mEntry[ 5] - mEntry[ 1]*mEntry[ 4];
    Real a1 = mEntry[ 0]*mEntry[ 6] - mEntry[ 2]*mEntry[ 4];
    Real a2 = mEntry[ 0]*mEntry[ 7] - mEntry[ 3]*mEntry[ 4];
    Real a3 = mEntry[ 1]*mEntry[ 6] - mEntry[ 2]*mEntry[ 5];
    Real a4 = mEntry[ 1]*mEntry[ 7] - mEntry[ 3]*mEntry[ 5];
    Real a5 = mEntry[ 2]*mEntry[ 7] - mEntry[ 3]*mEntry[ 6];
    Real b0 = mEntry[ 8]*mEntry[13] - mEntry[ 9]*mEntry[12];
    Real b1 = mEntry[ 8]*mEntry[14] - mEntry[10]*mEntry[12];
    Real b2 = mEntry[ 8]*mEntry[15] - mEntry[11]*mEntry[12];
    Real b3 = mEntry[ 9]*mEntry[14] - mEntry[10]*mEntry[13];
    Real b4 = mEntry[ 9]*mEntry[15] - mEntry[11]*mEntry[13];
    Real b5 = mEntry[10]*mEntry[15] - mEntry[11]*mEntry[14];
    Real det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
    return det;
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix4<Real>::MakeObliqueProjection (const Vector3<Real>& normal,
    const Vector3<Real>& origin, const Vector3<Real>& direction)
{
    // The projection plane is Dot(N,X-P) = 0 where N is a 3-by-1 unit-length
    // normal vector and P is a 3-by-1 point on the plane.  The projection
    // is oblique to the plane, in the direction of the 3-by-1 vector D.
    // Necessarily Dot(N,D) is not zero for this projection to make sense.
    // Given a 3-by-1 point U, compute the intersection of the line U+t*D
    // with the plane to obtain t = -Dot(N,U-P)/Dot(N,D).  Then
    //
    //   projection(U) = P + [I - D*N^T/Dot(N,D)]*(U-P)
    //
    // A 4-by-4 homogeneous transformation representing the projection is
    //
    //       +-                               -+
    //   M = | D*N^T - Dot(N,D)*I   -Dot(N,P)D |
    //       |          0^T          -Dot(N,D) |
    //       +-                               -+
    //
    // where M applies to [U^T 1]^T by M*[U^T 1]^T.  The matrix is chosen so
    // that M[3][3] > 0 whenever Dot(N,D) < 0 (projection is onto the
    // "positive side" of the plane).

    Real dotND = normal.Dot(direction);
    Real dotNO = normal.Dot(origin);
    mEntry[ 0] = direction[0]*normal[0] - dotND;
    mEntry[ 1] = direction[0]*normal[1];
    mEntry[ 2] = direction[0]*normal[2];
    mEntry[ 3] = -dotNO*direction[0];
    mEntry[ 4] = direction[1]*normal[0];
    mEntry[ 5] = direction[1]*normal[1] - dotND;
    mEntry[ 6] = direction[1]*normal[2];
    mEntry[ 7] = -dotNO*direction[1];
    mEntry[ 8] = direction[2]*normal[0];
    mEntry[ 9] = direction[2]*normal[1];
    mEntry[10] = direction[2]*normal[2] - dotND;
    mEntry[11] = -dotNO*direction[2];
    mEntry[12] = 0.0f;
    mEntry[13] = 0.0f;
    mEntry[14] = 0.0f;
    mEntry[15] = -dotND;
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix4<Real>::MakePerspectiveProjection (
    const Vector3<Real>& normal, const Vector3<Real>& origin,
    const Vector3<Real>& eye)
{
    //     +-                                                 -+
    // M = | Dot(N,E-P)*I - E*N^T    -(Dot(N,E-P)*I - E*N^T)*E |
    //     |        -N^t                      Dot(N,E)         |
    //     +-                                                 -+
    //
    // where E is the eye point, P is a point on the plane, and N is a
    // unit-length plane normal.

    Real dotND = normal.Dot(eye - origin);
    mEntry[ 0] = dotND - eye[0]*normal[0];
    mEntry[ 1] = -eye[0]*normal[1];
    mEntry[ 2] = -eye[0]*normal[2];
    mEntry[ 3] = -(mEntry[0]*eye[0] + mEntry[1]*eye[1] + mEntry[2]*eye[2]);
    mEntry[ 4] = -eye[1]*normal[0];
    mEntry[ 5] = dotND - eye[1]*normal[1];
    mEntry[ 6] = -eye[1]*normal[2];
    mEntry[ 7] = -(mEntry[4]*eye[0] + mEntry[5]*eye[1] + mEntry[6]*eye[2]);
    mEntry[ 8] = -eye[2]*normal[0];
    mEntry[ 9] = -eye[2]*normal[1];
    mEntry[10] = dotND- eye[2]*normal[2];
    mEntry[11] = -(mEntry[8]*eye[0] + mEntry[9]*eye[1] + mEntry[10]*eye[2]);
    mEntry[12] = -normal[0];
    mEntry[13] = -normal[1];
    mEntry[14] = -normal[2];
    mEntry[15] = normal.Dot(eye);
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix4<Real>::MakeReflection (const Vector3<Real>& normal,
    const Vector3<Real>& origin)
{
    //     +-                         -+
    // M = | I-2*N*N^T    2*Dot(N,P)*N |
    //     |     0^T            1      |
    //     +-                         -+
    //
    // where P is a point on the plane and N is a unit-length plane normal.

    Real twoDotNO = ((Real)2)*(normal.Dot(origin));
    mEntry[ 0] = (Real)1 - ((Real)2)*normal[0]*normal[0];
    mEntry[ 1] = -((Real)2)*normal[0]*normal[1];
    mEntry[ 2] = -((Real)2)*normal[0]*normal[2];
    mEntry[ 3] = twoDotNO*normal[0];
    mEntry[ 4] = -((Real)2)*normal[1]*normal[0];
    mEntry[ 5] = (Real)1 - ((Real)2)*normal[1]*normal[1];
    mEntry[ 6] = -((Real)2)*normal[1]*normal[2];
    mEntry[ 7] = twoDotNO*normal[1];
    mEntry[ 8] = -((Real)2)*normal[2]*normal[0];
    mEntry[ 9] = -((Real)2)*normal[2]*normal[1];
    mEntry[10] = (Real)1 - ((Real)2)*normal[2]*normal[2];
    mEntry[11] = twoDotNO*normal[2];
    mEntry[12] = (Real)0;
    mEntry[13] = (Real)0;
    mEntry[14] = (Real)0;
    mEntry[15] = (Real)1;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Matrix4<Real> operator* (Real scalar, const Matrix4<Real>& mat)
{
    return mat*scalar;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector4<Real> operator* (const Vector4<Real>& vec,
    const Matrix4<Real>& mat)
{
    return Vector4<Real>(
        vec[0]*mat[0][0]+vec[1]*mat[1][0]+vec[2]*mat[2][0]+vec[3]*mat[3][0],
        vec[0]*mat[0][1]+vec[1]*mat[1][1]+vec[2]*mat[2][1]+vec[3]*mat[3][1],
        vec[0]*mat[0][2]+vec[1]*mat[1][2]+vec[2]*mat[2][2]+vec[3]*mat[3][2],
        vec[0]*mat[0][3]+vec[1]*mat[1][3]+vec[2]*mat[2][3]+vec[3]*mat[3][3]);
}
//----------------------------------------------------------------------------
