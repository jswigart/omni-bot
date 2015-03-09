// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5HMatrix.h"
using namespace Wm5;

const HMatrix HMatrix::ZERO(true);
const HMatrix HMatrix::IDENTITY(false);

//----------------------------------------------------------------------------
HMatrix::HMatrix ()
{
}
//----------------------------------------------------------------------------
HMatrix::HMatrix (const HMatrix& mat)
{
    for (int i = 0; i < 16; ++i)
    {
        mEntry[i] = mat.mEntry[i];
    }
}
//----------------------------------------------------------------------------
HMatrix::HMatrix (const Matrix3f& mat)
{
    mEntry[ 0] = mat[0][0];
    mEntry[ 1] = mat[0][1];
    mEntry[ 2] = mat[0][2];
    mEntry[ 3] = 0.0f;
    mEntry[ 4] = mat[1][0];
    mEntry[ 5] = mat[1][1];
    mEntry[ 6] = mat[1][2];
    mEntry[ 7] = 0.0f;
    mEntry[ 8] = mat[2][0];
    mEntry[ 9] = mat[2][1];
    mEntry[10] = mat[2][2];
    mEntry[11] = 0.0f;
    mEntry[12] = 0.0f;
    mEntry[13] = 0.0f;
    mEntry[14] = 0.0f;
    mEntry[15] = 1.0f;
}
//----------------------------------------------------------------------------
HMatrix::HMatrix (bool makeZero)
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
HMatrix::HMatrix (
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33)
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
HMatrix::HMatrix (const float entry[16], bool rowMajor)
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
HMatrix::HMatrix (const float* tuple0, const float* tuple1,
    const float* tuple2, const float* tuple3, bool columns)
{
    if (columns)
    {
        mEntry[ 0] = tuple0[0];
        mEntry[ 1] = tuple1[0];
        mEntry[ 2] = tuple2[0];
        mEntry[ 3] = tuple3[0];
        mEntry[ 4] = tuple0[1];
        mEntry[ 5] = tuple1[1];
        mEntry[ 6] = tuple2[1];
        mEntry[ 7] = tuple3[1];
        mEntry[ 8] = tuple0[2];
        mEntry[ 9] = tuple1[2];
        mEntry[10] = tuple2[2];
        mEntry[11] = tuple3[2];
        mEntry[12] = tuple0[3];
        mEntry[13] = tuple1[3];
        mEntry[14] = tuple2[3];
        mEntry[15] = tuple3[3];
    }
    else
    {
        mEntry[ 0] = tuple0[0];
        mEntry[ 1] = tuple0[1];
        mEntry[ 2] = tuple0[2];
        mEntry[ 3] = tuple0[3];
        mEntry[ 4] = tuple1[0];
        mEntry[ 5] = tuple1[1];
        mEntry[ 6] = tuple1[2];
        mEntry[ 7] = tuple1[3];
        mEntry[ 8] = tuple2[0];
        mEntry[ 9] = tuple2[1];
        mEntry[10] = tuple2[2];
        mEntry[11] = tuple2[3];
        mEntry[12] = tuple3[0];
        mEntry[13] = tuple3[1];
        mEntry[14] = tuple3[2];
        mEntry[15] = tuple3[3];
    }
}
//----------------------------------------------------------------------------
HMatrix::HMatrix (float m00, float m11, float m22)
{
    MakeDiagonal(m00, m11, m22);
}
//----------------------------------------------------------------------------
HMatrix::HMatrix (const AVector& axis, float angle)
{
    MakeRotation(axis, angle);
}
//----------------------------------------------------------------------------
HMatrix::~HMatrix ()
{
}
//----------------------------------------------------------------------------
HMatrix::operator Matrix3f () const
{
    return Matrix3f
    (
        mEntry[ 0], mEntry[ 1], mEntry[ 2],
        mEntry[ 4], mEntry[ 5], mEntry[ 6],
        mEntry[ 8], mEntry[ 9], mEntry[10]
    );
}
//----------------------------------------------------------------------------
void HMatrix::SetRow (int row, const HPoint& hpnt)
{
    int i = 4*row;
    float* entry = &mEntry[i];
    *entry = hpnt[0];  ++entry;
    *entry = hpnt[1];  ++entry;
    *entry = hpnt[2];  ++entry;
    *entry = hpnt[3];
}
//----------------------------------------------------------------------------
void HMatrix::GetRow (int row, HPoint& hpnt) const
{
    int i = 4*row;
    const float* entry = &mEntry[i];
    hpnt[0] = *entry;  ++entry;
    hpnt[1] = *entry;  ++entry;
    hpnt[2] = *entry;  ++entry;
    hpnt[3] = *entry;
}
//----------------------------------------------------------------------------
void HMatrix::SetColumn (int column, const HPoint& hpnt)
{
    int i = column;
    float* entry = &mEntry[i];
    *entry = hpnt[0];  entry += 4;
    *entry = hpnt[1];  entry += 4;
    *entry = hpnt[2];  entry += 4;
    *entry = hpnt[3];
}
//----------------------------------------------------------------------------
void HMatrix::GetColumn (int column, HPoint& hpnt) const
{
    int i = column;
    const float* entry = &mEntry[i];
    hpnt[0] = *entry;  entry += 4;
    hpnt[1] = *entry;  entry += 4;
    hpnt[2] = *entry;  entry += 4;
    hpnt[3] = *entry;
}
//----------------------------------------------------------------------------
void HMatrix::GetColumnMajor (float* columnMajor) const
{
    columnMajor[ 0] = mEntry[ 0];
    columnMajor[ 1] = mEntry[ 4];
    columnMajor[ 2] = mEntry[ 8];
    columnMajor[ 3] = mEntry[12];
    columnMajor[ 4] = mEntry[ 1];
    columnMajor[ 5] = mEntry[ 5];
    columnMajor[ 6] = mEntry[ 9];
    columnMajor[ 7] = mEntry[13];
    columnMajor[ 8] = mEntry[ 2];
    columnMajor[ 9] = mEntry[ 6];
    columnMajor[10] = mEntry[10];
    columnMajor[11] = mEntry[14];
    columnMajor[12] = mEntry[ 3];
    columnMajor[13] = mEntry[ 7];
    columnMajor[14] = mEntry[11];
    columnMajor[15] = mEntry[15];
}
//----------------------------------------------------------------------------
HMatrix& HMatrix::operator= (const HMatrix& mat)
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
HMatrix& HMatrix::operator= (const Matrix3f& mat)
{
    mEntry[ 0] = mat[0][0];
    mEntry[ 1] = mat[0][1];
    mEntry[ 2] = mat[0][2];
    mEntry[ 3] = 0.0f;
    mEntry[ 4] = mat[1][0];
    mEntry[ 5] = mat[1][1];
    mEntry[ 6] = mat[1][2];
    mEntry[ 7] = 0.0f;
    mEntry[ 8] = mat[2][0];
    mEntry[ 9] = mat[2][1];
    mEntry[10] = mat[2][2];
    mEntry[11] = 0.0f;
    mEntry[12] = 0.0f;
    mEntry[13] = 0.0f;
    mEntry[14] = 0.0f;
    mEntry[15] = 1.0f;
    return *this;
}
//----------------------------------------------------------------------------
bool HMatrix::operator== (const HMatrix& mat) const
{
    return memcmp(mEntry, mat.mEntry, 16*sizeof(float)) == 0;
}
//----------------------------------------------------------------------------
bool HMatrix::operator!= (const HMatrix& mat) const
{
    return memcmp(mEntry, mat.mEntry, 16*sizeof(float)) != 0;
}
//----------------------------------------------------------------------------
bool HMatrix::operator<  (const HMatrix& mat) const
{
    return memcmp(mEntry, mat.mEntry, 16*sizeof(float)) < 0;
}
//----------------------------------------------------------------------------
bool HMatrix::operator<= (const HMatrix& mat) const
{
    return memcmp(mEntry, mat.mEntry, 16*sizeof(float)) <= 0;
}
//----------------------------------------------------------------------------
bool HMatrix::operator>  (const HMatrix& mat) const
{
    return memcmp(mEntry, mat.mEntry, 16*sizeof(float)) > 0;
}
//----------------------------------------------------------------------------
bool HMatrix::operator>= (const HMatrix& mat) const
{
    return memcmp(mEntry, mat.mEntry, 16*sizeof(float)) >= 0;
}
//----------------------------------------------------------------------------
HMatrix HMatrix::operator+ (const HMatrix& mat) const
{
    HMatrix result;
    for (int i = 0; i < 16; ++i)
    {
        result.mEntry[i] = mEntry[i] + mat.mEntry[i];
    }
    return result;
}
//----------------------------------------------------------------------------
HMatrix HMatrix::operator- (const HMatrix& mat) const
{
    HMatrix result;
    for (int i = 0; i < 16; ++i)
    {
        result.mEntry[i] = mEntry[i] - mat.mEntry[i];
    }
    return result;
}
//----------------------------------------------------------------------------
HMatrix HMatrix::operator* (float scalar) const
{
    HMatrix result;
    for (int i = 0; i < 16; ++i)
    {
        result.mEntry[i] = scalar*mEntry[i];
    }
    return result;
}
//----------------------------------------------------------------------------
HMatrix HMatrix::operator/ (float scalar) const
{
    HMatrix result;
    int i;

    if (scalar != 0.0f)
    {
        float invScalar = 1.0f/scalar;
        for (i = 0; i < 16; ++i)
        {
            result.mEntry[i] = invScalar*mEntry[i];
        }
    }
    else
    {
        for (i = 0; i < 16; ++i)
        {
            result.mEntry[i] = FLT_MAX;
        }
    }

    return result;
}
//----------------------------------------------------------------------------
HMatrix HMatrix::operator- () const
{
    HMatrix result;
    for (int i = 0; i < 16; ++i)
    {
        result.mEntry[i] = -mEntry[i];
    }
    return result;
}
//----------------------------------------------------------------------------
HMatrix Wm5::operator* (float scalar, const HMatrix& mat)
{
    return mat*scalar;
}
//----------------------------------------------------------------------------
HMatrix& HMatrix::operator+= (const HMatrix& mat)
{
    for (int i = 0; i < 16; ++i)
    {
        mEntry[i] += mat.mEntry[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
HMatrix& HMatrix::operator-= (const HMatrix& mat)
{
    for (int i = 0; i < 16; ++i)
    {
        mEntry[i] -= mat.mEntry[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
HMatrix& HMatrix::operator*= (float scalar)
{
    for (int i = 0; i < 16; ++i)
    {
        mEntry[i] *= scalar;
    }
    return *this;
}
//----------------------------------------------------------------------------
HMatrix& HMatrix::operator/= (float scalar)
{
    int i;

    if (scalar != 0.0f)
    {
        float invScalar = 1.0f/scalar;
        for (i = 0; i < 16; ++i)
        {
            mEntry[i] *= invScalar;
        }
    }
    else
    {
        for (i = 0; i < 16; ++i)
        {
            mEntry[i] = FLT_MAX;
        }
    }

    return *this;
}
//----------------------------------------------------------------------------
void HMatrix::MakeZero ()
{
    mEntry[ 0] = 0.0f;
    mEntry[ 1] = 0.0f;
    mEntry[ 2] = 0.0f;
    mEntry[ 3] = 0.0f;
    mEntry[ 4] = 0.0f;
    mEntry[ 5] = 0.0f;
    mEntry[ 6] = 0.0f;
    mEntry[ 7] = 0.0f;
    mEntry[ 8] = 0.0f;
    mEntry[ 9] = 0.0f;
    mEntry[10] = 0.0f;
    mEntry[11] = 0.0f;
    mEntry[12] = 0.0f;
    mEntry[13] = 0.0f;
    mEntry[14] = 0.0f;
    mEntry[15] = 0.0f;
}
//----------------------------------------------------------------------------
void HMatrix::MakeIdentity ()
{
    mEntry[ 0] = 1.0f;
    mEntry[ 1] = 0.0f;
    mEntry[ 2] = 0.0f;
    mEntry[ 3] = 0.0f;
    mEntry[ 4] = 0.0f;
    mEntry[ 5] = 1.0f;
    mEntry[ 6] = 0.0f;
    mEntry[ 7] = 0.0f;
    mEntry[ 8] = 0.0f;
    mEntry[ 9] = 0.0f;
    mEntry[10] = 1.0f;
    mEntry[11] = 0.0f;
    mEntry[12] = 0.0f;
    mEntry[13] = 0.0f;
    mEntry[14] = 0.0f;
    mEntry[15] = 1.0f;
}
//----------------------------------------------------------------------------
void HMatrix::MakeDiagonal (float m00, float m11, float m22)
{
    mEntry[ 0] = m00;
    mEntry[ 1] = 0.0f;
    mEntry[ 2] = 0.0f;
    mEntry[ 3] = 0.0f;
    mEntry[ 4] = 0.0f;
    mEntry[ 5] = m11;
    mEntry[ 6] = 0.0f;
    mEntry[ 7] = 0.0f;
    mEntry[ 8] = 0.0f;
    mEntry[ 9] = 0.0f;
    mEntry[10] = m22;
    mEntry[11] = 0.0f;
    mEntry[12] = 0.0f;
    mEntry[13] = 0.0f;
    mEntry[14] = 0.0f;
    mEntry[15] = 1.0f;
}
//----------------------------------------------------------------------------
void HMatrix::MakeRotation (const AVector& axis, float angle)
{
    float cs = cosf(angle);
    float sn = sinf(angle);
    float oneMinusCos = 1.0f - cs;
    float x2 = axis[0]*axis[0];
    float y2 = axis[1]*axis[1];
    float z2 = axis[2]*axis[2];
    float xym = axis[0]*axis[1]*oneMinusCos;
    float xzm = axis[0]*axis[2]*oneMinusCos;
    float yzm = axis[1]*axis[2]*oneMinusCos;
    float xSin = axis[0]*sn;
    float ySin = axis[1]*sn;
    float zSin = axis[2]*sn;
    
    mEntry[ 0] = x2*oneMinusCos + cs;
    mEntry[ 1] = xym - zSin;
    mEntry[ 2] = xzm + ySin;
    mEntry[ 3] = 0.0f;
    mEntry[ 4] = xym + zSin;
    mEntry[ 5] = y2*oneMinusCos + cs;
    mEntry[ 6] = yzm - xSin;
    mEntry[ 7] = 0.0f;
    mEntry[ 8] = xzm - ySin;
    mEntry[ 9] = yzm + xSin;
    mEntry[10] = z2*oneMinusCos + cs;
    mEntry[11] = 0.0f;
    mEntry[12] = 0.0f;
    mEntry[13] = 0.0f;
    mEntry[14] = 0.0f;
    mEntry[15] = 1.0f;
}
//----------------------------------------------------------------------------
HMatrix HMatrix::Transpose () const
{
    HMatrix result;
    result.mEntry[ 0] = mEntry[ 0];
    result.mEntry[ 1] = mEntry[ 4];
    result.mEntry[ 2] = mEntry[ 8];
    result.mEntry[ 3] = mEntry[12];
    result.mEntry[ 4] = mEntry[ 1];
    result.mEntry[ 5] = mEntry[ 5];
    result.mEntry[ 6] = mEntry[ 9];
    result.mEntry[ 7] = mEntry[13];
    result.mEntry[ 8] = mEntry[ 2];
    result.mEntry[ 9] = mEntry[ 6];
    result.mEntry[10] = mEntry[10];
    result.mEntry[11] = mEntry[14];
    result.mEntry[12] = mEntry[ 3];
    result.mEntry[13] = mEntry[ 7];
    result.mEntry[14] = mEntry[11];
    result.mEntry[15] = mEntry[15];
    return result;
}
//----------------------------------------------------------------------------
HMatrix HMatrix::Inverse (const float epsilon) const
{
    float a0 = mEntry[ 0]*mEntry[ 5] - mEntry[ 1]*mEntry[ 4];
    float a1 = mEntry[ 0]*mEntry[ 6] - mEntry[ 2]*mEntry[ 4];
    float a2 = mEntry[ 0]*mEntry[ 7] - mEntry[ 3]*mEntry[ 4];
    float a3 = mEntry[ 1]*mEntry[ 6] - mEntry[ 2]*mEntry[ 5];
    float a4 = mEntry[ 1]*mEntry[ 7] - mEntry[ 3]*mEntry[ 5];
    float a5 = mEntry[ 2]*mEntry[ 7] - mEntry[ 3]*mEntry[ 6];
    float b0 = mEntry[ 8]*mEntry[13] - mEntry[ 9]*mEntry[12];
    float b1 = mEntry[ 8]*mEntry[14] - mEntry[10]*mEntry[12];
    float b2 = mEntry[ 8]*mEntry[15] - mEntry[11]*mEntry[12];
    float b3 = mEntry[ 9]*mEntry[14] - mEntry[10]*mEntry[13];
    float b4 = mEntry[ 9]*mEntry[15] - mEntry[11]*mEntry[13];
    float b5 = mEntry[10]*mEntry[15] - mEntry[11]*mEntry[14];

    float det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
    if (fabsf(det) <= epsilon)
    {
        return ZERO;
    }

    HMatrix inverse;
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

    float invDet = 1.0f/det;
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
//----------------------------------------------------------------------------
HMatrix HMatrix::Adjoint () const
{
    float a0 = mEntry[ 0]*mEntry[ 5] - mEntry[ 1]*mEntry[ 4];
    float a1 = mEntry[ 0]*mEntry[ 6] - mEntry[ 2]*mEntry[ 4];
    float a2 = mEntry[ 0]*mEntry[ 7] - mEntry[ 3]*mEntry[ 4];
    float a3 = mEntry[ 1]*mEntry[ 6] - mEntry[ 2]*mEntry[ 5];
    float a4 = mEntry[ 1]*mEntry[ 7] - mEntry[ 3]*mEntry[ 5];
    float a5 = mEntry[ 2]*mEntry[ 7] - mEntry[ 3]*mEntry[ 6];
    float b0 = mEntry[ 8]*mEntry[13] - mEntry[ 9]*mEntry[12];
    float b1 = mEntry[ 8]*mEntry[14] - mEntry[10]*mEntry[12];
    float b2 = mEntry[ 8]*mEntry[15] - mEntry[11]*mEntry[12];
    float b3 = mEntry[ 9]*mEntry[14] - mEntry[10]*mEntry[13];
    float b4 = mEntry[ 9]*mEntry[15] - mEntry[11]*mEntry[13];
    float b5 = mEntry[10]*mEntry[15] - mEntry[11]*mEntry[14];

    return HMatrix(
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
        + mEntry[ 8]*a3 - mEntry[ 9]*a1 + mEntry[10]*a0);
}
//----------------------------------------------------------------------------
float HMatrix::Determinant () const
{
    float a0 = mEntry[ 0]*mEntry[ 5] - mEntry[ 1]*mEntry[ 4];
    float a1 = mEntry[ 0]*mEntry[ 6] - mEntry[ 2]*mEntry[ 4];
    float a2 = mEntry[ 0]*mEntry[ 7] - mEntry[ 3]*mEntry[ 4];
    float a3 = mEntry[ 1]*mEntry[ 6] - mEntry[ 2]*mEntry[ 5];
    float a4 = mEntry[ 1]*mEntry[ 7] - mEntry[ 3]*mEntry[ 5];
    float a5 = mEntry[ 2]*mEntry[ 7] - mEntry[ 3]*mEntry[ 6];
    float b0 = mEntry[ 8]*mEntry[13] - mEntry[ 9]*mEntry[12];
    float b1 = mEntry[ 8]*mEntry[14] - mEntry[10]*mEntry[12];
    float b2 = mEntry[ 8]*mEntry[15] - mEntry[11]*mEntry[12];
    float b3 = mEntry[ 9]*mEntry[14] - mEntry[10]*mEntry[13];
    float b4 = mEntry[ 9]*mEntry[15] - mEntry[11]*mEntry[13];
    float b5 = mEntry[10]*mEntry[15] - mEntry[11]*mEntry[14];
    float det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
    return det;
}
//----------------------------------------------------------------------------
HMatrix HMatrix::operator* (const HMatrix& mat) const
{
    return HMatrix(
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
        mEntry[15]*mat.mEntry[15]);
}
//----------------------------------------------------------------------------
HMatrix HMatrix::TransposeTimes (const HMatrix& mat) const
{
    // P = A^T*B
    return HMatrix(
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
        mEntry[15]*mat.mEntry[15]);
}
//----------------------------------------------------------------------------
HMatrix HMatrix::TimesTranspose (const HMatrix& mat) const
{
    // P = A*B^T
    return HMatrix(
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
        mEntry[15]*mat.mEntry[15]);
}
//----------------------------------------------------------------------------
HMatrix HMatrix::TransposeTimesTranspose (const HMatrix& mat) const
{
    // P = A^T*B^T
    return HMatrix(
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
        mEntry[15]*mat.mEntry[15]);
}
//----------------------------------------------------------------------------
HMatrix HMatrix::TimesDiagonal (const APoint& diag) const
{
    return HMatrix(
        mEntry[ 0]*diag[0], mEntry[ 1]*diag[1], mEntry[ 2]*diag[2],
            mEntry[ 3],
        mEntry[ 4]*diag[0], mEntry[ 5]*diag[1], mEntry[ 6]*diag[2],
            mEntry[ 7],
        mEntry[ 8]*diag[0], mEntry[ 9]*diag[1], mEntry[10]*diag[2],
            mEntry[11],
        mEntry[12]*diag[0], mEntry[13]*diag[1], mEntry[14]*diag[2],
            mEntry[15]
    );
}
//----------------------------------------------------------------------------
HMatrix HMatrix::DiagonalTimes (const APoint& diag) const
{
    return HMatrix(
        diag[0]*mEntry[ 0], diag[0]*mEntry[ 1], diag[0]*mEntry[ 2],
            mEntry[ 3],
        diag[1]*mEntry[ 4], diag[1]*mEntry[ 5], diag[1]*mEntry[ 6],
            mEntry[ 7],
        diag[2]*mEntry[ 8], diag[2]*mEntry[ 9], diag[2]*mEntry[10],
            mEntry[11],
        mEntry[12], mEntry[13], mEntry[14], mEntry[15]
    );
}
//----------------------------------------------------------------------------
void HMatrix::Orthonormalize ()
{
    // Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix has
    // upper-left 3x3 block M = [m0|m1|m2], then the orthonormal output matrix
    // is Q = [q0|q1|q2],
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.

    // Compute q0.
    float invLength = 1.0f/sqrtf(mEntry[0]*mEntry[0] +
        mEntry[4]*mEntry[4] + mEntry[8]*mEntry[8]);

    mEntry[0] *= invLength;
    mEntry[4] *= invLength;
    mEntry[8] *= invLength;

    // Compute q1.
    float dot0 = mEntry[0]*mEntry[1] + mEntry[4]*mEntry[5] +
        mEntry[8]*mEntry[9];

    mEntry[1] -= dot0*mEntry[0];
    mEntry[5] -= dot0*mEntry[4];
    mEntry[9] -= dot0*mEntry[8];

    invLength = 1.0f/sqrtf(mEntry[1]*mEntry[1] +
        mEntry[5]*mEntry[5] + mEntry[9]*mEntry[9]);

    mEntry[1] *= invLength;
    mEntry[5] *= invLength;
    mEntry[9] *= invLength;

    // Compute q2.
    float dot1 = mEntry[1]*mEntry[2] + mEntry[5]*mEntry[6] +
        mEntry[9]*mEntry[10];

    dot0 = mEntry[0]*mEntry[2] + mEntry[4]*mEntry[6] +
        mEntry[8]*mEntry[10];

    mEntry[ 2] -= dot0*mEntry[0] + dot1*mEntry[1];
    mEntry[ 6] -= dot0*mEntry[4] + dot1*mEntry[5];
    mEntry[10] -= dot0*mEntry[8] + dot1*mEntry[9];

    invLength = 1.0f/sqrtf(mEntry[2]*mEntry[2] +
        mEntry[6]*mEntry[6] + mEntry[10]*mEntry[10]);

    mEntry[ 2] *= invLength;
    mEntry[ 6] *= invLength;
    mEntry[10] *= invLength;
}
//----------------------------------------------------------------------------
HPoint HMatrix::operator* (const HPoint& p) const
{
    return HPoint(
        mEntry[ 0]*p[0] +
        mEntry[ 1]*p[1] +
        mEntry[ 2]*p[2] +
        mEntry[ 3]*p[3],

        mEntry[ 4]*p[0] +
        mEntry[ 5]*p[1] +
        mEntry[ 6]*p[2] +
        mEntry[ 7]*p[3],

        mEntry[ 8]*p[0] +
        mEntry[ 9]*p[1] +
        mEntry[10]*p[2] +
        mEntry[11]*p[3],

        mEntry[12]*p[0] +
        mEntry[13]*p[1] +
        mEntry[14]*p[2] +
        mEntry[15]*p[3]);
}
//----------------------------------------------------------------------------
HPoint Wm5::operator* (const HPoint& p, const HMatrix& mat)
{
    return HPoint(
        p[0]*mat[0][0] + p[1]*mat[1][0] + p[2]*mat[2][0] + p[3]*mat[3][0],
        p[0]*mat[0][1] + p[1]*mat[1][1] + p[2]*mat[2][1] + p[3]*mat[3][1],
        p[0]*mat[0][2] + p[1]*mat[1][2] + p[2]*mat[2][2] + p[3]*mat[3][2],
        p[0]*mat[0][3] + p[1]*mat[1][3] + p[2]*mat[2][3] + p[3]*mat[3][3]);
}
//----------------------------------------------------------------------------
void HMatrix::BatchMultiply (int numPoints, const HPoint* input,
    HPoint* output) const
{
    const HPoint* src = input;
    HPoint* trg = output;
    for (int i = 0; i < numPoints; ++i, src++, trg++)
    {
        *trg = (*this)*(*src);
    }
}
//----------------------------------------------------------------------------
APoint HMatrix::operator* (const APoint& p) const
{
    return APoint(
        mEntry[ 0]*p[0] +
        mEntry[ 1]*p[1] +
        mEntry[ 2]*p[2] +
        mEntry[ 3],

        mEntry[ 4]*p[0] +
        mEntry[ 5]*p[1] +
        mEntry[ 6]*p[2] +
        mEntry[ 7],

        mEntry[ 8]*p[0] +
        mEntry[ 9]*p[1] +
        mEntry[10]*p[2] +
        mEntry[11]);
}
//----------------------------------------------------------------------------
void HMatrix::BatchMultiply (int numPoints, const APoint* input,
    APoint* output) const
{
    const APoint* src = input;
    APoint* trg = output;
    for (int i = 0; i < numPoints; ++i, src++, trg++)
    {
        *trg = (*this)*(*src);
    }
}
//----------------------------------------------------------------------------
AVector HMatrix::operator* (const AVector& p) const
{
    return AVector(
        mEntry[ 0]*p[0] +
        mEntry[ 1]*p[1] +
        mEntry[ 2]*p[2],

        mEntry[ 4]*p[0] +
        mEntry[ 5]*p[1] +
        mEntry[ 6]*p[2],

        mEntry[ 8]*p[0] +
        mEntry[ 9]*p[1] +
        mEntry[10]*p[2]);
}
//----------------------------------------------------------------------------
void HMatrix::BatchMultiply (int numPoints, const AVector* input,
    AVector* output) const
{
    const AVector* src = input;
    AVector* trg = output;
    for (int i = 0; i < numPoints; ++i, src++, trg++)
    {
        *trg = (*this)*(*src);
    }
}
//----------------------------------------------------------------------------
float HMatrix::QForm (const HPoint& p0, const HPoint& p1) const
{
    HPoint Mp1(
        mEntry[ 0]*p1[0] +
        mEntry[ 1]*p1[1] +
        mEntry[ 2]*p1[2] +
        mEntry[ 3]*p1[3],

        mEntry[ 4]*p1[0] +
        mEntry[ 5]*p1[1] +
        mEntry[ 6]*p1[2] +
        mEntry[ 7]*p1[3],

        mEntry[ 8]*p1[0] +
        mEntry[ 9]*p1[1] +
        mEntry[10]*p1[2] +
        mEntry[11]*p1[3],

        mEntry[12]*p1[0] +
        mEntry[13]*p1[1] +
        mEntry[14]*p1[2] +
        mEntry[15]*p1[3]);

    float dot = p0[0]*Mp1[0] + p0[1]*Mp1[1] + p0[2]*Mp1[2] + p0[3]*Mp1[3];
    return dot;
}
//----------------------------------------------------------------------------
void HMatrix::MakeObliqueProjection (const APoint& origin,
    const AVector& normal, const AVector& direction)
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

    float dotND = normal.Dot(direction);
    float dotNO = origin.Dot(normal);

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
void HMatrix::MakePerspectiveProjection (const APoint& origin,
    const AVector& normal, const APoint& eye)
{
    //     +-                                                 -+
    // M = | Dot(N,E-P)*I - E*N^T    -(Dot(N,E-P)*I - E*N^T)*E |
    //     |        -N^t                      Dot(N,E)         |
    //     +-                                                 -+
    //
    // where E is the eye point, P is a point on the plane, and N is a
    // unit-length plane normal.

    float dotND = normal.Dot(eye - origin);

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
    mEntry[15] = eye.Dot(normal);
}
//----------------------------------------------------------------------------
void HMatrix::MakeReflection (const APoint& origin, const AVector& normal)
{
    //     +-                         -+
    // M = | I-2*N*N^T    2*Dot(N,P)*N |
    //     |     0^T            1      |
    //     +-                         -+
    //
    // where P is a point on the plane and N is a unit-length plane normal.

    float twoDotNO = 2.0f*origin.Dot(normal);

    mEntry[ 0] = 1.0f - 2.0f*normal[0]*normal[0];
    mEntry[ 1] = -2.0f*normal[0]*normal[1];
    mEntry[ 2] = -2.0f*normal[0]*normal[2];
    mEntry[ 3] = twoDotNO*normal[0];
    mEntry[ 4] = -2.0f*normal[1]*normal[0];
    mEntry[ 5] = 1.0f - 2.0f*normal[1]*normal[1];
    mEntry[ 6] = -2.0f*normal[1]*normal[2];
    mEntry[ 7] = twoDotNO*normal[1];
    mEntry[ 8] = -2.0f*normal[2]*normal[0];
    mEntry[ 9] = -2.0f*normal[2]*normal[1];
    mEntry[10] = 1.0f - 2.0f*normal[2]*normal[2];
    mEntry[11] = twoDotNO*normal[2];
    mEntry[12] = 0.0f;
    mEntry[13] = 0.0f;
    mEntry[14] = 0.0f;
    mEntry[15] = 1.0f;
}
//----------------------------------------------------------------------------
