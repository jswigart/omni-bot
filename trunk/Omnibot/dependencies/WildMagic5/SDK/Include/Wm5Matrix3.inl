// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2012/07/29)

//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>::Matrix3 (bool makeZero)
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
Matrix3<Real>::Matrix3 (const Matrix3& mat)
{
    mEntry[0] = mat.mEntry[0];
    mEntry[1] = mat.mEntry[1];
    mEntry[2] = mat.mEntry[2];
    mEntry[3] = mat.mEntry[3];
    mEntry[4] = mat.mEntry[4];
    mEntry[5] = mat.mEntry[5];
    mEntry[6] = mat.mEntry[6];
    mEntry[7] = mat.mEntry[7];
    mEntry[8] = mat.mEntry[8];
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>::Matrix3 (Real m00, Real m01, Real m02, Real m10, Real m11,
    Real m12, Real m20, Real m21, Real m22)
{
    mEntry[0] = m00;
    mEntry[1] = m01;
    mEntry[2] = m02;
    mEntry[3] = m10;
    mEntry[4] = m11;
    mEntry[5] = m12;
    mEntry[6] = m20;
    mEntry[7] = m21;
    mEntry[8] = m22;
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>::Matrix3 (const Real entry[9], bool rowMajor)
{
    if (rowMajor)
    {
        mEntry[0] = entry[0];
        mEntry[1] = entry[1];
        mEntry[2] = entry[2];
        mEntry[3] = entry[3];
        mEntry[4] = entry[4];
        mEntry[5] = entry[5];
        mEntry[6] = entry[6];
        mEntry[7] = entry[7];
        mEntry[8] = entry[8];
    }
    else
    {
        mEntry[0] = entry[0];
        mEntry[1] = entry[3];
        mEntry[2] = entry[6];
        mEntry[3] = entry[1];
        mEntry[4] = entry[4];
        mEntry[5] = entry[7];
        mEntry[6] = entry[2];
        mEntry[7] = entry[5];
        mEntry[8] = entry[8];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>::Matrix3 (const Vector3<Real>& u, const Vector3<Real>& v,
    const Vector3<Real>& w, bool columns)
{
    if (columns)
    {
        mEntry[0] = u[0];
        mEntry[1] = v[0];
        mEntry[2] = w[0];
        mEntry[3] = u[1];
        mEntry[4] = v[1];
        mEntry[5] = w[1];
        mEntry[6] = u[2];
        mEntry[7] = v[2];
        mEntry[8] = w[2];
    }
    else
    {
        mEntry[0] = u[0];
        mEntry[1] = u[1];
        mEntry[2] = u[2];
        mEntry[3] = v[0];
        mEntry[4] = v[1];
        mEntry[5] = v[2];
        mEntry[6] = w[0];
        mEntry[7] = w[1];
        mEntry[8] = w[2];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>::Matrix3 (const Vector3<Real>* v, bool columns)
{
    if (columns)
    {
        mEntry[0] = v[0][0];
        mEntry[1] = v[1][0];
        mEntry[2] = v[2][0];
        mEntry[3] = v[0][1];
        mEntry[4] = v[1][1];
        mEntry[5] = v[2][1];
        mEntry[6] = v[0][2];
        mEntry[7] = v[1][2];
        mEntry[8] = v[2][2];
    }
    else
    {
        mEntry[0] = v[0][0];
        mEntry[1] = v[0][1];
        mEntry[2] = v[0][2];
        mEntry[3] = v[1][0];
        mEntry[4] = v[1][1];
        mEntry[5] = v[1][2];
        mEntry[6] = v[2][0];
        mEntry[7] = v[2][1];
        mEntry[8] = v[2][2];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>::Matrix3 (Real m00, Real m11, Real m22)
{
    MakeDiagonal(m00, m11, m22);
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>::Matrix3 (const Vector3<Real>& axis, Real angle)
{
    MakeRotation(axis, angle);
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>::Matrix3 (const Vector3<Real>& u, const Vector3<Real>& v)
{
    MakeTensorProduct(u, v);
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>& Matrix3<Real>::operator= (const Matrix3& mat)
{
    mEntry[0] = mat.mEntry[0];
    mEntry[1] = mat.mEntry[1];
    mEntry[2] = mat.mEntry[2];
    mEntry[3] = mat.mEntry[3];
    mEntry[4] = mat.mEntry[4];
    mEntry[5] = mat.mEntry[5];
    mEntry[6] = mat.mEntry[6];
    mEntry[7] = mat.mEntry[7];
    mEntry[8] = mat.mEntry[8];
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>& Matrix3<Real>::MakeZero ()
{
    mEntry[0] = (Real)0;
    mEntry[1] = (Real)0;
    mEntry[2] = (Real)0;
    mEntry[3] = (Real)0;
    mEntry[4] = (Real)0;
    mEntry[5] = (Real)0;
    mEntry[6] = (Real)0;
    mEntry[7] = (Real)0;
    mEntry[8] = (Real)0;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>& Matrix3<Real>::MakeIdentity ()
{
    mEntry[0] = (Real)1;
    mEntry[1] = (Real)0;
    mEntry[2] = (Real)0;
    mEntry[3] = (Real)0;
    mEntry[4] = (Real)1;
    mEntry[5] = (Real)0;
    mEntry[6] = (Real)0;
    mEntry[7] = (Real)0;
    mEntry[8] = (Real)1;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>& Matrix3<Real>::MakeDiagonal (Real m00, Real m11, Real m22)
{
    mEntry[0] = m00;
    mEntry[1] = (Real)0;
    mEntry[2] = (Real)0;
    mEntry[3] = (Real)0;
    mEntry[4] = m11;
    mEntry[5] = (Real)0;
    mEntry[6] = (Real)0;
    mEntry[7] = (Real)0;
    mEntry[8] = m22;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>& Matrix3<Real>::MakeRotation (const Vector3<Real>& axis,
    Real angle)
{
    Real cs = Math<Real>::Cos(angle);
    Real sn = Math<Real>::Sin(angle);
    Real oneMinusCos = ((Real)1) - cs;
    Real x2 = axis[0]*axis[0];
    Real y2 = axis[1]*axis[1];
    Real z2 = axis[2]*axis[2];
    Real xym = axis[0]*axis[1]*oneMinusCos;
    Real xzm = axis[0]*axis[2]*oneMinusCos;
    Real yzm = axis[1]*axis[2]*oneMinusCos;
    Real xSin = axis[0]*sn;
    Real ySin = axis[1]*sn;
    Real zSin = axis[2]*sn;
    
    mEntry[0] = x2*oneMinusCos + cs;
    mEntry[1] = xym - zSin;
    mEntry[2] = xzm + ySin;
    mEntry[3] = xym + zSin;
    mEntry[4] = y2*oneMinusCos + cs;
    mEntry[5] = yzm - xSin;
    mEntry[6] = xzm - ySin;
    mEntry[7] = yzm + xSin;
    mEntry[8] = z2*oneMinusCos + cs;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>& Matrix3<Real>::MakeTensorProduct (const Vector3<Real>& u,
    const Vector3<Real>& v)
{
    mEntry[0] = u[0]*v[0];
    mEntry[1] = u[0]*v[1];
    mEntry[2] = u[0]*v[2];
    mEntry[3] = u[1]*v[0];
    mEntry[4] = u[1]*v[1];
    mEntry[5] = u[1]*v[2];
    mEntry[6] = u[2]*v[0];
    mEntry[7] = u[2]*v[1];
    mEntry[8] = u[2]*v[2];
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> Matrix3<Real>::operator+ (const Matrix3& mat) const
{
    return Matrix3<Real>
    (
        mEntry[0] + mat.mEntry[0],
        mEntry[1] + mat.mEntry[1],
        mEntry[2] + mat.mEntry[2],
        mEntry[3] + mat.mEntry[3],
        mEntry[4] + mat.mEntry[4],
        mEntry[5] + mat.mEntry[5],
        mEntry[6] + mat.mEntry[6],
        mEntry[7] + mat.mEntry[7],
        mEntry[8] + mat.mEntry[8]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> Matrix3<Real>::operator- (const Matrix3& mat) const
{
    return Matrix3<Real>
    (
        mEntry[0] - mat.mEntry[0],
        mEntry[1] - mat.mEntry[1],
        mEntry[2] - mat.mEntry[2],
        mEntry[3] - mat.mEntry[3],
        mEntry[4] - mat.mEntry[4],
        mEntry[5] - mat.mEntry[5],
        mEntry[6] - mat.mEntry[6],
        mEntry[7] - mat.mEntry[7],
        mEntry[8] - mat.mEntry[8]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> Matrix3<Real>::operator* (Real scalar) const
{
    return Matrix3<Real>
    (
        scalar*mEntry[0],
        scalar*mEntry[1],
        scalar*mEntry[2],
        scalar*mEntry[3],
        scalar*mEntry[4],
        scalar*mEntry[5],
        scalar*mEntry[6],
        scalar*mEntry[7],
        scalar*mEntry[8]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> Matrix3<Real>::operator/ (Real scalar) const
{
    if (scalar != (Real)0)
    {
        Real invScalar = ((Real)1)/scalar;
        return Matrix3<Real>
        (
            invScalar*mEntry[0],
            invScalar*mEntry[1],
            invScalar*mEntry[2],
            invScalar*mEntry[3],
            invScalar*mEntry[4],
            invScalar*mEntry[5],
            invScalar*mEntry[6],
            invScalar*mEntry[7],
            invScalar*mEntry[8]
        );
    }
    else
    {
        return Matrix3<Real>
        (
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
template <typename Real>
Matrix3<Real> Matrix3<Real>::operator- () const
{
    return Matrix3<Real>
    (
        -mEntry[0],
        -mEntry[1],
        -mEntry[2],
        -mEntry[3],
        -mEntry[4],
        -mEntry[5],
        -mEntry[6],
        -mEntry[7],
        -mEntry[8]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>& Matrix3<Real>::operator+= (const Matrix3& mat)
{
    mEntry[0] += mat.mEntry[0];
    mEntry[1] += mat.mEntry[1];
    mEntry[2] += mat.mEntry[2];
    mEntry[3] += mat.mEntry[3];
    mEntry[4] += mat.mEntry[4];
    mEntry[5] += mat.mEntry[5];
    mEntry[6] += mat.mEntry[6];
    mEntry[7] += mat.mEntry[7];
    mEntry[8] += mat.mEntry[8];
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>& Matrix3<Real>::operator-= (const Matrix3& mat)
{
    mEntry[0] -= mat.mEntry[0];
    mEntry[1] -= mat.mEntry[1];
    mEntry[2] -= mat.mEntry[2];
    mEntry[3] -= mat.mEntry[3];
    mEntry[4] -= mat.mEntry[4];
    mEntry[5] -= mat.mEntry[5];
    mEntry[6] -= mat.mEntry[6];
    mEntry[7] -= mat.mEntry[7];
    mEntry[8] -= mat.mEntry[8];
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>& Matrix3<Real>::operator*= (Real scalar)
{
    mEntry[0] *= scalar;
    mEntry[1] *= scalar;
    mEntry[2] *= scalar;
    mEntry[3] *= scalar;
    mEntry[4] *= scalar;
    mEntry[5] *= scalar;
    mEntry[6] *= scalar;
    mEntry[7] *= scalar;
    mEntry[8] *= scalar;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>& Matrix3<Real>::operator/= (Real scalar)
{
    if (scalar != (Real)0)
    {
        Real invScalar = ((Real)1)/scalar;
        mEntry[0] *= invScalar;
        mEntry[1] *= invScalar;
        mEntry[2] *= invScalar;
        mEntry[3] *= invScalar;
        mEntry[4] *= invScalar;
        mEntry[5] *= invScalar;
        mEntry[6] *= invScalar;
        mEntry[7] *= invScalar;
        mEntry[8] *= invScalar;
    }
    else
    {
        mEntry[0] = Math<Real>::MAX_REAL;
        mEntry[1] = Math<Real>::MAX_REAL;
        mEntry[2] = Math<Real>::MAX_REAL;
        mEntry[3] = Math<Real>::MAX_REAL;
        mEntry[4] = Math<Real>::MAX_REAL;
        mEntry[5] = Math<Real>::MAX_REAL;
        mEntry[6] = Math<Real>::MAX_REAL;
        mEntry[7] = Math<Real>::MAX_REAL;
        mEntry[8] = Math<Real>::MAX_REAL;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Matrix3<Real>::operator* (const Vector3<Real>& vec) const
{
    return Vector3<Real>
    (
        mEntry[0]*vec[0] + mEntry[1]*vec[1] + mEntry[2]*vec[2],
        mEntry[3]*vec[0] + mEntry[4]*vec[1] + mEntry[5]*vec[2],
        mEntry[6]*vec[0] + mEntry[7]*vec[1] + mEntry[8]*vec[2]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
Real Matrix3<Real>::QForm (const Vector3<Real>& u, const Vector3<Real>& v)
    const
{
    return u.Dot((*this)*v);
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> Matrix3<Real>::Transpose () const
{
    return Matrix3<Real>
    (
        mEntry[0],
        mEntry[3],
        mEntry[6],
        mEntry[1],
        mEntry[4],
        mEntry[7],
        mEntry[2],
        mEntry[5],
        mEntry[8]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> Matrix3<Real>::operator* (const Matrix3& mat) const
{
    // A*B
    return Matrix3<Real>
    (
        mEntry[0]*mat.mEntry[0] +
        mEntry[1]*mat.mEntry[3] +
        mEntry[2]*mat.mEntry[6],

        mEntry[0]*mat.mEntry[1] +
        mEntry[1]*mat.mEntry[4] +
        mEntry[2]*mat.mEntry[7],

        mEntry[0]*mat.mEntry[2] +
        mEntry[1]*mat.mEntry[5] +
        mEntry[2]*mat.mEntry[8],

        mEntry[3]*mat.mEntry[0] +
        mEntry[4]*mat.mEntry[3] +
        mEntry[5]*mat.mEntry[6],

        mEntry[3]*mat.mEntry[1] +
        mEntry[4]*mat.mEntry[4] +
        mEntry[5]*mat.mEntry[7],

        mEntry[3]*mat.mEntry[2] +
        mEntry[4]*mat.mEntry[5] +
        mEntry[5]*mat.mEntry[8],

        mEntry[6]*mat.mEntry[0] +
        mEntry[7]*mat.mEntry[3] +
        mEntry[8]*mat.mEntry[6],

        mEntry[6]*mat.mEntry[1] +
        mEntry[7]*mat.mEntry[4] +
        mEntry[8]*mat.mEntry[7],

        mEntry[6]*mat.mEntry[2] +
        mEntry[7]*mat.mEntry[5] +
        mEntry[8]*mat.mEntry[8]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> Matrix3<Real>::TransposeTimes (const Matrix3& mat) const
{
    // A^T*B
    return Matrix3<Real>
    (
        mEntry[0]*mat.mEntry[0] +
        mEntry[3]*mat.mEntry[3] +
        mEntry[6]*mat.mEntry[6],

        mEntry[0]*mat.mEntry[1] +
        mEntry[3]*mat.mEntry[4] +
        mEntry[6]*mat.mEntry[7],

        mEntry[0]*mat.mEntry[2] +
        mEntry[3]*mat.mEntry[5] +
        mEntry[6]*mat.mEntry[8],

        mEntry[1]*mat.mEntry[0] +
        mEntry[4]*mat.mEntry[3] +
        mEntry[7]*mat.mEntry[6],

        mEntry[1]*mat.mEntry[1] +
        mEntry[4]*mat.mEntry[4] +
        mEntry[7]*mat.mEntry[7],

        mEntry[1]*mat.mEntry[2] +
        mEntry[4]*mat.mEntry[5] +
        mEntry[7]*mat.mEntry[8],

        mEntry[2]*mat.mEntry[0] +
        mEntry[5]*mat.mEntry[3] +
        mEntry[8]*mat.mEntry[6],

        mEntry[2]*mat.mEntry[1] +
        mEntry[5]*mat.mEntry[4] +
        mEntry[8]*mat.mEntry[7],

        mEntry[2]*mat.mEntry[2] +
        mEntry[5]*mat.mEntry[5] +
        mEntry[8]*mat.mEntry[8]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> Matrix3<Real>::TimesTranspose (const Matrix3& mat) const
{
    // A*B^T
    return Matrix3<Real>
    (
        mEntry[0]*mat.mEntry[0] +
        mEntry[1]*mat.mEntry[1] +
        mEntry[2]*mat.mEntry[2],

        mEntry[0]*mat.mEntry[3] +
        mEntry[1]*mat.mEntry[4] +
        mEntry[2]*mat.mEntry[5],

        mEntry[0]*mat.mEntry[6] +
        mEntry[1]*mat.mEntry[7] +
        mEntry[2]*mat.mEntry[8],

        mEntry[3]*mat.mEntry[0] +
        mEntry[4]*mat.mEntry[1] +
        mEntry[5]*mat.mEntry[2],

        mEntry[3]*mat.mEntry[3] +
        mEntry[4]*mat.mEntry[4] +
        mEntry[5]*mat.mEntry[5],

        mEntry[3]*mat.mEntry[6] +
        mEntry[4]*mat.mEntry[7] +
        mEntry[5]*mat.mEntry[8],

        mEntry[6]*mat.mEntry[0] +
        mEntry[7]*mat.mEntry[1] +
        mEntry[8]*mat.mEntry[2],

        mEntry[6]*mat.mEntry[3] +
        mEntry[7]*mat.mEntry[4] +
        mEntry[8]*mat.mEntry[5],

        mEntry[6]*mat.mEntry[6] +
        mEntry[7]*mat.mEntry[7] +
        mEntry[8]*mat.mEntry[8]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> Matrix3<Real>::TransposeTimesTranspose (const Matrix3& mat)
    const
{
    // A^T*B^T
    return Matrix3<Real>
    (
        mEntry[0]*mat.mEntry[0] +
        mEntry[3]*mat.mEntry[1] +
        mEntry[6]*mat.mEntry[2],

        mEntry[0]*mat.mEntry[3] +
        mEntry[3]*mat.mEntry[4] +
        mEntry[6]*mat.mEntry[5],

        mEntry[0]*mat.mEntry[6] +
        mEntry[3]*mat.mEntry[7] +
        mEntry[6]*mat.mEntry[8],

        mEntry[1]*mat.mEntry[0] +
        mEntry[4]*mat.mEntry[1] +
        mEntry[7]*mat.mEntry[2],

        mEntry[1]*mat.mEntry[3] +
        mEntry[4]*mat.mEntry[4] +
        mEntry[7]*mat.mEntry[5],

        mEntry[1]*mat.mEntry[6] +
        mEntry[4]*mat.mEntry[7] +
        mEntry[7]*mat.mEntry[8],

        mEntry[2]*mat.mEntry[0] +
        mEntry[5]*mat.mEntry[1] +
        mEntry[8]*mat.mEntry[2],

        mEntry[2]*mat.mEntry[3] +
        mEntry[5]*mat.mEntry[4] +
        mEntry[8]*mat.mEntry[5],

        mEntry[2]*mat.mEntry[6] +
        mEntry[5]*mat.mEntry[7] +
        mEntry[8]*mat.mEntry[8]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> Matrix3<Real>::TimesDiagonal (const Vector3<Real>& diag) const
{
    return Matrix3<Real>
    (
        mEntry[0]*diag[0],
        mEntry[1]*diag[1],
        mEntry[2]*diag[2],
        mEntry[3]*diag[0],
        mEntry[4]*diag[1],
        mEntry[5]*diag[2],
        mEntry[6]*diag[0],
        mEntry[7]*diag[1],
        mEntry[8]*diag[2]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> Matrix3<Real>::DiagonalTimes (const Vector3<Real>& diag) const
{
    return Matrix3<Real>
    (
        diag[0]*mEntry[0],
        diag[0]*mEntry[1],
        diag[0]*mEntry[2],
        diag[1]*mEntry[3],
        diag[1]*mEntry[4],
        diag[1]*mEntry[5],
        diag[2]*mEntry[6],
        diag[2]*mEntry[7],
        diag[2]*mEntry[8]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> Matrix3<Real>::Inverse (const Real epsilon) const
{
    // Invert a 3x3 using cofactors.  This is faster than using a generic
    // Gaussian elimination because of the loop overhead of such a method.

    Matrix3 inverse;

    // Compute the adjoint.
    inverse.mEntry[0] = mEntry[4]*mEntry[8] - mEntry[5]*mEntry[7];
    inverse.mEntry[1] = mEntry[2]*mEntry[7] - mEntry[1]*mEntry[8];
    inverse.mEntry[2] = mEntry[1]*mEntry[5] - mEntry[2]*mEntry[4];
    inverse.mEntry[3] = mEntry[5]*mEntry[6] - mEntry[3]*mEntry[8];
    inverse.mEntry[4] = mEntry[0]*mEntry[8] - mEntry[2]*mEntry[6];
    inverse.mEntry[5] = mEntry[2]*mEntry[3] - mEntry[0]*mEntry[5];
    inverse.mEntry[6] = mEntry[3]*mEntry[7] - mEntry[4]*mEntry[6];
    inverse.mEntry[7] = mEntry[1]*mEntry[6] - mEntry[0]*mEntry[7];
    inverse.mEntry[8] = mEntry[0]*mEntry[4] - mEntry[1]*mEntry[3];

    Real det = mEntry[0]*inverse.mEntry[0] + mEntry[1]*inverse.mEntry[3] +
        mEntry[2]*inverse.mEntry[6];

    if (Math<Real>::FAbs(det) > epsilon)
    {
        Real invDet = ((Real)1)/det;
        inverse.mEntry[0] *= invDet;
        inverse.mEntry[1] *= invDet;
        inverse.mEntry[2] *= invDet;
        inverse.mEntry[3] *= invDet;
        inverse.mEntry[4] *= invDet;
        inverse.mEntry[5] *= invDet;
        inverse.mEntry[6] *= invDet;
        inverse.mEntry[7] *= invDet;
        inverse.mEntry[8] *= invDet;
        return inverse;
    }

    return ZERO;
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real> Matrix3<Real>::Adjoint () const
{
    return Matrix3<Real>
    (
        mEntry[4]*mEntry[8] - mEntry[5]*mEntry[7],
        mEntry[2]*mEntry[7] - mEntry[1]*mEntry[8],
        mEntry[1]*mEntry[5] - mEntry[2]*mEntry[4],
        mEntry[5]*mEntry[6] - mEntry[3]*mEntry[8],
        mEntry[0]*mEntry[8] - mEntry[2]*mEntry[6],
        mEntry[2]*mEntry[3] - mEntry[0]*mEntry[5],
        mEntry[3]*mEntry[7] - mEntry[4]*mEntry[6],
        mEntry[1]*mEntry[6] - mEntry[0]*mEntry[7],
        mEntry[0]*mEntry[4] - mEntry[1]*mEntry[3]
    );
}
//----------------------------------------------------------------------------
template <typename Real>
Real Matrix3<Real>::Determinant () const
{
    Real co00 = mEntry[4]*mEntry[8] - mEntry[5]*mEntry[7];
    Real co10 = mEntry[5]*mEntry[6] - mEntry[3]*mEntry[8];
    Real co20 = mEntry[3]*mEntry[7] - mEntry[4]*mEntry[6];
    Real det = mEntry[0]*co00 + mEntry[1]*co10 + mEntry[2]*co20;
    return det;
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::ExtractAxisAngle (Vector3<Real>& axis, Real& angle) const
{
    // Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
    // The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
    // I is the identity and
    //
    //       +-        -+
    //   P = |  0 -z +y |
    //       | +z  0 -x |
    //       | -y +x  0 |
    //       +-        -+
    //
    // If A > 0, R represents a counterclockwise rotation about the axis in
    // the sense of looking from the tip of the axis vector towards the
    // origin.  Some algebra will show that
    //
    //   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P
    //
    // In the event that A = pi, R-R^t = 0 which prevents us from extracting
    // the axis through P.  Instead note that R = I+2*P^2 when A = pi, so
    // P^2 = (R-I)/2.  The diagonal entries of P^2 are x^2-1, y^2-1, and
    // z^2-1.  We can solve these for axis (x,y,z).  Because the angle is pi,
    // it does not matter which sign you choose on the square roots.

    Real trace = mEntry[0] + mEntry[4] + mEntry[8];
    Real cs = ((Real)0.5)*(trace - (Real)1);
    angle = Math<Real>::ACos(cs);  // in [0,PI]

    if (angle > (Real)0)
    {
        if (angle < Math<Real>::PI)
        {
            axis[0] = mEntry[7] - mEntry[5];
            axis[1] = mEntry[2] - mEntry[6];
            axis[2] = mEntry[3] - mEntry[1];
            axis.Normalize();
        }
        else
        {
            // angle is PI
            Real halfInverse;
            if (mEntry[0] >= mEntry[4])
            {
                // r00 >= r11
                if (mEntry[0] >= mEntry[8])
                {
                    // r00 is maximum diagonal term
                    axis[0] = ((Real)0.5)*Math<Real>::Sqrt((Real)1
                        + mEntry[0] - mEntry[4] - mEntry[8]);
                    halfInverse = ((Real)0.5)/axis[0];
                    axis[1] = halfInverse*mEntry[1];
                    axis[2] = halfInverse*mEntry[2];
                }
                else
                {
                    // r22 is maximum diagonal term
                    axis[2] = ((Real)0.5)*Math<Real>::Sqrt((Real)1
                        + mEntry[8] - mEntry[0] - mEntry[4]);
                    halfInverse = ((Real)0.5)/axis[2];
                    axis[0] = halfInverse*mEntry[2];
                    axis[1] = halfInverse*mEntry[5];
                }
            }
            else
            {
                // r11 > r00
                if (mEntry[4] >= mEntry[8])
                {
                    // r11 is maximum diagonal term
                    axis[1] = ((Real)0.5)*Math<Real>::Sqrt((Real)1
                        + mEntry[4] - mEntry[0] - mEntry[8]);
                    halfInverse  = ((Real)0.5)/axis[1];
                    axis[0] = halfInverse*mEntry[1];
                    axis[2] = halfInverse*mEntry[5];
                }
                else
                {
                    // r22 is maximum diagonal term
                    axis[2] = ((Real)0.5)*Math<Real>::Sqrt((Real)1
                        + mEntry[8] - mEntry[0] - mEntry[4]);
                    halfInverse = ((Real)0.5)/axis[2];
                    axis[0] = halfInverse*mEntry[2];
                    axis[1] = halfInverse*mEntry[5];
                }
            }
        }
    }
    else
    {
        // The angle is 0 and the matrix is the identity.  Any axis will
        // work, so just use the x-axis.
        axis[0] = (Real)1;
        axis[1] = (Real)0;
        axis[2] = (Real)0;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::Orthonormalize ()
{
    // Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
    // M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.

    // Compute q0.
    Real invLength = Math<Real>::InvSqrt(mEntry[0]*mEntry[0] +
        mEntry[3]*mEntry[3] + mEntry[6]*mEntry[6]);

    mEntry[0] *= invLength;
    mEntry[3] *= invLength;
    mEntry[6] *= invLength;

    // Compute q1.
    Real dot0 = mEntry[0]*mEntry[1] + mEntry[3]*mEntry[4] +
        mEntry[6]*mEntry[7];

    mEntry[1] -= dot0*mEntry[0];
    mEntry[4] -= dot0*mEntry[3];
    mEntry[7] -= dot0*mEntry[6];

    invLength = Math<Real>::InvSqrt(mEntry[1]*mEntry[1] +
        mEntry[4]*mEntry[4] + mEntry[7]*mEntry[7]);

    mEntry[1] *= invLength;
    mEntry[4] *= invLength;
    mEntry[7] *= invLength;

    // compute q2
    Real dot1 = mEntry[1]*mEntry[2] + mEntry[4]*mEntry[5] +
        mEntry[7]*mEntry[8];

    dot0 = mEntry[0]*mEntry[2] + mEntry[3]*mEntry[5] +
        mEntry[6]*mEntry[8];

    mEntry[2] -= dot0*mEntry[0] + dot1*mEntry[1];
    mEntry[5] -= dot0*mEntry[3] + dot1*mEntry[4];
    mEntry[8] -= dot0*mEntry[6] + dot1*mEntry[7];

    invLength = Math<Real>::InvSqrt(mEntry[2]*mEntry[2] +
        mEntry[5]*mEntry[5] + mEntry[8]*mEntry[8]);

    mEntry[2] *= invLength;
    mEntry[5] *= invLength;
    mEntry[8] *= invLength;
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::EigenDecomposition (Matrix3& rot, Matrix3& diag) const
{
    // Factor M = R*D*R^T.  The columns of R are the eigenvectors.  The
    // diagonal entries of D are the corresponding eigenvalues.
    Real diagonal[3], subdiagonal[2];
    rot = *this;
    bool reflection = rot.Tridiagonalize(diagonal, subdiagonal);
    bool converged = rot.QLAlgorithm(diagonal, subdiagonal);
    assertion(converged, "QLAlgorithm failed to converge\n");
    WM5_UNUSED(converged);

    // Sort the eigenvalues in increasing order, d0 <= d1 <= d2.  This is an
    // insertion sort.
    int i;
    Real save;

    if (diagonal[1] < diagonal[0])
    {
        // Swap d0 and d1.
        save = diagonal[0];
        diagonal[0] = diagonal[1];
        diagonal[1] = save;

        // Swap V0 and V1.
        for (i = 0; i < 3; ++i)
        {
            save = rot[i][0];
            rot[i][0] = rot[i][1];
            rot[i][1] = save;
        }
        reflection = !reflection;
    }

    if (diagonal[2] < diagonal[1])
    {
        // Swap d1 and d2.
        save = diagonal[1];
        diagonal[1] = diagonal[2];
        diagonal[2] = save;

        // Swap V1 and V2.
        for (i = 0; i < 3; ++i)
        {
            save = rot[i][1];
            rot[i][1] = rot[i][2];
            rot[i][2] = save;
        }
        reflection = !reflection;
    }

    if (diagonal[1] < diagonal[0])
    {
        // Swap d0 and d1.
        save = diagonal[0];
        diagonal[0] = diagonal[1];
        diagonal[1] = save;

        // Swap V0 and V1.
        for (i = 0; i < 3; ++i)
        {
            save = rot[i][0];
            rot[i][0] = rot[i][1];
            rot[i][1] = save;
        }
        reflection = !reflection;
    }

    diag.MakeDiagonal(diagonal[0], diagonal[1], diagonal[2]);

    if (reflection)
    {
        // The orthogonal transformation that diagonalizes M is a reflection.
        // Make the eigenvectors a right-handed system by changing sign on
        // the last column.
        rot[0][2] = -rot[0][2];
        rot[1][2] = -rot[1][2];
        rot[2][2] = -rot[2][2];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::MakeEulerXYZ (Real xAngle, Real yAngle, Real zAngle)
{
    Real cs, sn;

    cs = Math<Real>::Cos(xAngle);
    sn = Math<Real>::Sin(xAngle);
    Matrix3 xMat(
        (Real)1, (Real)0, (Real)0,
        (Real)0, cs,      -sn,
        (Real)0, sn,      cs);

    cs = Math<Real>::Cos(yAngle);
    sn = Math<Real>::Sin(yAngle);
    Matrix3 yMat(
        cs,      (Real)0, sn,
        (Real)0, (Real)1, (Real)0,
        -sn,     (Real)0, cs);

    cs = Math<Real>::Cos(zAngle);
    sn = Math<Real>::Sin(zAngle);
    Matrix3 zMat(
        cs,      -sn,     (Real)0,
        sn,      cs,      (Real)0,
        (Real)0, (Real)0, (Real)1);

    *this = xMat*(yMat*zMat);
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::MakeEulerXZY (Real xAngle, Real zAngle, Real yAngle)
{
    Real cs, sn;

    cs = Math<Real>::Cos(xAngle);
    sn = Math<Real>::Sin(xAngle);
    Matrix3 xMat(
        (Real)1, (Real)0, (Real)0,
        (Real)0, cs,      -sn,
        (Real)0, sn,      cs);

    cs = Math<Real>::Cos(zAngle);
    sn = Math<Real>::Sin(zAngle);
    Matrix3 zMat(
        cs,      -sn,     (Real)0,
        sn,      cs,      (Real)0,
        (Real)0, (Real)0, (Real)1);

    cs = Math<Real>::Cos(yAngle);
    sn = Math<Real>::Sin(yAngle);
    Matrix3 yMat(
        cs,      (Real)0, sn,
        (Real)0, (Real)1, (Real)0,
        -sn,     (Real)0, cs);

    *this = xMat*(zMat*yMat);
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::MakeEulerYXZ (Real yAngle, Real xAngle, Real zAngle)
{
    Real cs, sn;

    cs = Math<Real>::Cos(yAngle);
    sn = Math<Real>::Sin(yAngle);
    Matrix3 yMat(
        cs,      (Real)0, sn,
        (Real)0, (Real)1, (Real)0,
        -sn,     (Real)0, cs);

    cs = Math<Real>::Cos(xAngle);
    sn = Math<Real>::Sin(xAngle);
    Matrix3 xMat(
        (Real)1, (Real)0, (Real)0,
        (Real)0, cs,      -sn,
        (Real)0, sn,      cs);

    cs = Math<Real>::Cos(zAngle);
    sn = Math<Real>::Sin(zAngle);
    Matrix3 zMat(
        cs,      -sn,     (Real)0,
        sn,      cs,      (Real)0,
        (Real)0, (Real)0, (Real)1);

    *this = yMat*(xMat*zMat);
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::MakeEulerYZX (Real yAngle, Real zAngle, Real xAngle)
{
    Real cs, sn;

    cs = Math<Real>::Cos(yAngle);
    sn = Math<Real>::Sin(yAngle);
    Matrix3 yMat(
        cs,      (Real)0, sn,
        (Real)0, (Real)1, (Real)0,
        -sn,     (Real)0, cs);

    cs = Math<Real>::Cos(zAngle);
    sn = Math<Real>::Sin(zAngle);
    Matrix3 zMat(
        cs,      -sn,     (Real)0,
        sn,      cs,      (Real)0,
        (Real)0, (Real)0, (Real)1);

    cs = Math<Real>::Cos(xAngle);
    sn = Math<Real>::Sin(xAngle);
    Matrix3 xMat(
        (Real)1,(Real)0,(Real)0,
        (Real)0,cs,-sn,
        (Real)0,sn,cs);

    *this = yMat*(zMat*xMat);
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::MakeEulerZXY (Real zAngle, Real xAngle, Real yAngle)
{
    Real cs, sn;

    cs = Math<Real>::Cos(zAngle);
    sn = Math<Real>::Sin(zAngle);
    Matrix3 zMat(
        cs,      -sn,     (Real)0,
        sn,      cs,      (Real)0,
        (Real)0, (Real)0, (Real)1);

    cs = Math<Real>::Cos(xAngle);
    sn = Math<Real>::Sin(xAngle);
    Matrix3 xMat(
        (Real)1, (Real)0, (Real)0,
        (Real)0, cs,      -sn,
        (Real)0, sn,      cs);

    cs = Math<Real>::Cos(yAngle);
    sn = Math<Real>::Sin(yAngle);
    Matrix3 yMat(
        cs,      (Real)0, sn,
        (Real)0, (Real)1, (Real)0,
        -sn,     (Real)0, cs);

    *this = zMat*(xMat*yMat);
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::MakeEulerZYX (Real zAngle, Real yAngle, Real xAngle)
{
    Real cs, sn;

    cs = Math<Real>::Cos(zAngle);
    sn = Math<Real>::Sin(zAngle);
    Matrix3 zMat(
        cs,      -sn,     (Real)0,
        sn,      cs,      (Real)0,
        (Real)0, (Real)0, (Real)1);

    cs = Math<Real>::Cos(yAngle);
    sn = Math<Real>::Sin(yAngle);
    Matrix3 yMat(
        cs,      (Real)0, sn,
        (Real)0, (Real)1, (Real)0,
        -sn,     (Real)0, cs);

    cs = Math<Real>::Cos(xAngle);
    sn = Math<Real>::Sin(xAngle);
    Matrix3 xMat(
        (Real)1, (Real)0, (Real)0,
        (Real)0, cs,      -sn,
        (Real)0, sn,      cs);

    *this = zMat*(yMat*xMat);
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::MakeEulerXYX (Real x0Angle, Real yAngle, Real x1Angle)
{
    Real cs, sn;

    cs = Math<Real>::Cos(x0Angle);
    sn = Math<Real>::Sin(x0Angle);
    Matrix3 x0Mat(
        (Real)1, (Real)0, (Real)0,
        (Real)0, cs,      -sn,
        (Real)0, sn,      cs);

    cs = Math<Real>::Cos(yAngle);
    sn = Math<Real>::Sin(yAngle);
    Matrix3 yMat(
        cs,      (Real)0, sn,
        (Real)0, (Real)1, (Real)0,
        -sn,     (Real)0, cs);

    cs = Math<Real>::Cos(x1Angle);
    sn = Math<Real>::Sin(x1Angle);
    Matrix3 x1Mat(
        (Real)1, (Real)0, (Real)0,
        (Real)0, cs,      -sn,
        (Real)0, sn,      cs);

    *this = x0Mat*(yMat*x1Mat);
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::MakeEulerXZX (Real x0Angle, Real zAngle, Real x1Angle)
{
    Real cs, sn;

    cs = Math<Real>::Cos(x0Angle);
    sn = Math<Real>::Sin(x0Angle);
    Matrix3 x0Mat(
        (Real)1, (Real)0, (Real)0,
        (Real)0, cs,      -sn,
        (Real)0, sn,      cs);

    cs = Math<Real>::Cos(zAngle);
    sn = Math<Real>::Sin(zAngle);
    Matrix3 zMat(
        cs,      -sn,     (Real)0,
        sn,      cs,      (Real)0,
        (Real)0, (Real)0, (Real)1);

    cs = Math<Real>::Cos(x1Angle);
    sn = Math<Real>::Sin(x1Angle);
    Matrix3 x1Mat(
        (Real)1, (Real)0, (Real)0,
        (Real)0, cs,      -sn,
        (Real)0, sn,      cs);

    *this = x0Mat*(zMat*x1Mat);
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::MakeEulerYXY (Real y0Angle, Real xAngle, Real y1Angle)
{
    Real cs, sn;

    cs = Math<Real>::Cos(y0Angle);
    sn = Math<Real>::Sin(y0Angle);
    Matrix3 y0Mat(
        cs,      (Real)0, sn,
        (Real)0, (Real)1, (Real)0,
        -sn,     (Real)0, cs);

    cs = Math<Real>::Cos(xAngle);
    sn = Math<Real>::Sin(xAngle);
    Matrix3 xMat(
        (Real)1, (Real)0, (Real)0,
        (Real)0, cs,      -sn,
        (Real)0, sn,      cs);

    cs = Math<Real>::Cos(y1Angle);
    sn = Math<Real>::Sin(y1Angle);
    Matrix3 y1Mat(
        cs,      (Real)0, sn,
        (Real)0, (Real)1, (Real)0,
        -sn,     (Real)0, cs);

    *this = y0Mat*(xMat*y1Mat);
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::MakeEulerYZY (Real y0Angle, Real zAngle, Real y1Angle)
{
    Real cs, sn;

    cs = Math<Real>::Cos(y0Angle);
    sn = Math<Real>::Sin(y0Angle);
    Matrix3 y0Mat(
        cs,      (Real)0, sn,
        (Real)0, (Real)1, (Real)0,
        -sn,     (Real)0, cs);

    cs = Math<Real>::Cos(zAngle);
    sn = Math<Real>::Sin(zAngle);
    Matrix3 zMat(
        cs,      -sn,     (Real)0,
        sn,      cs,      (Real)0,
        (Real)0, (Real)0, (Real)1);

    cs = Math<Real>::Cos(y1Angle);
    sn = Math<Real>::Sin(y1Angle);
    Matrix3 y1Mat(
        cs,      (Real)0, sn,
        (Real)0, (Real)1, (Real)0,
        -sn,     (Real)0, cs);

    *this = y0Mat*(zMat*y1Mat);
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::MakeEulerZXZ (Real z0Angle, Real xAngle, Real z1Angle)
{
    Real cs, sn;

    cs = Math<Real>::Cos(z0Angle);
    sn = Math<Real>::Sin(z0Angle);
    Matrix3 z0Mat(
        cs,      -sn,     (Real)0,
        sn,      cs,      (Real)0,
        (Real)0, (Real)0, (Real)1);

    cs = Math<Real>::Cos(xAngle);
    sn = Math<Real>::Sin(xAngle);
    Matrix3 xMat(
        (Real)1, (Real)0, (Real)0,
        (Real)0, cs,      -sn,
        (Real)0, sn,      cs);

    cs = Math<Real>::Cos(z1Angle);
    sn = Math<Real>::Sin(z1Angle);
    Matrix3 z1Mat(
        cs,      -sn,     (Real)0,
        sn,      cs,      (Real)0,
        (Real)0, (Real)0, (Real)1);

    *this = z0Mat*(xMat*z1Mat);
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::MakeEulerZYZ (Real z0Angle, Real yAngle, Real z1Angle)
{
    Real cs, sn;

    cs = Math<Real>::Cos(z0Angle);
    sn = Math<Real>::Sin(z0Angle);
    Matrix3 z0Mat(
        cs,      -sn,     (Real)0,
        sn,      cs,      (Real)0,
        (Real)0, (Real)0, (Real)1);

    cs = Math<Real>::Cos(yAngle);
    sn = Math<Real>::Sin(yAngle);
    Matrix3 yMat(
        cs,      (Real)0, sn,
        (Real)0, (Real)1, (Real)0,
        -sn,     (Real)0, cs);

    cs = Math<Real>::Cos(z1Angle);
    sn = Math<Real>::Sin(z1Angle);
    Matrix3 z1Mat(
        cs,      -sn,     (Real)0,
        sn,      cs,      (Real)0,
        (Real)0, (Real)0, (Real)1);

    *this = z0Mat*(yMat*z1Mat);
}
//----------------------------------------------------------------------------
template <typename Real>
typename Matrix3<Real>::EulerResult Matrix3<Real>::ExtractEulerXYZ (
    Real& xAngle, Real& yAngle, Real& zAngle) const
{
    // +-           -+   +-                                        -+
    // | r00 r01 r02 |   |  cy*cz           -cy*sz            sy    |
    // | r10 r11 r12 | = |  cz*sx*sy+cx*sz   cx*cz-sx*sy*sz  -cy*sx |
    // | r20 r21 r22 |   | -cx*cz*sy+sx*sz   cz*sx+cx*sy*sz   cx*cy |
    // +-           -+   +-                                        -+

    if (mEntry[2] < (Real)1)
    {
        if (mEntry[2] > -(Real)1)
        {
            // y_angle = asin(r02)
            // x_angle = atan2(-r12,r22)
            // z_angle = atan2(-r01,r00)
            yAngle = (Real)asin((double)mEntry[2]);
            xAngle = Math<Real>::ATan2(-mEntry[5], mEntry[8]);
            zAngle = Math<Real>::ATan2(-mEntry[1], mEntry[0]);
            return EA_UNIQUE;
        }
        else
        {
            // y_angle = -pi/2
            // z_angle - x_angle = atan2(r10,r11)
            // WARNING.  The solution is not unique.  Choosing z_angle = 0.
            yAngle = -Math<Real>::HALF_PI;
            xAngle = -Math<Real>::ATan2(mEntry[3], mEntry[4]);
            zAngle = (Real)0;
            return EA_NOT_UNIQUE_DIF;
        }
    }
    else
    {
        // y_angle = +pi/2
        // z_angle + x_angle = atan2(r10,r11)
        // WARNING.  The solutions is not unique.  Choosing z_angle = 0.
        yAngle = Math<Real>::HALF_PI;
        xAngle = Math<Real>::ATan2(mEntry[3], mEntry[4]);
        zAngle = (Real)0;
        return EA_NOT_UNIQUE_SUM;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
typename Matrix3<Real>::EulerResult Matrix3<Real>::ExtractEulerXZY (
    Real& xAngle, Real& zAngle, Real& yAngle) const
{
    // +-           -+   +-                                        -+
    // | r00 r01 r02 |   |  cy*cz           -sz      cz*sy          |
    // | r10 r11 r12 | = |  sx*sy+cx*cy*sz   cx*cz  -cy*sx+cx*sy*sz |
    // | r20 r21 r22 |   | -cx*sy+cy*sx*sz   cz*sx   cx*cy+sx*sy*sz |
    // +-           -+   +-                                        -+

    if (mEntry[1] < (Real)1)
    {
        if (mEntry[1] > -(Real)1)
        {
            // z_angle = asin(-r01)
            // x_angle = atan2(r21,r11)
            // y_angle = atan2(r02,r00)
            zAngle = (Real)asin(-(double)mEntry[1]);
            xAngle = Math<Real>::ATan2(mEntry[7], mEntry[4]);
            yAngle = Math<Real>::ATan2(mEntry[2], mEntry[0]);
            return EA_UNIQUE;
        }
        else
        {
            // z_angle = +pi/2
            // y_angle - x_angle = atan2(-r20,r22)
            // WARNING.  The solution is not unique.  Choosing y_angle = 0.
            zAngle = Math<Real>::HALF_PI;
            xAngle = -Math<Real>::ATan2(-mEntry[6] ,mEntry[8]);
            yAngle = (Real)0;
            return EA_NOT_UNIQUE_DIF;
        }
    }
    else
    {
        // z_angle = -pi/2
        // y_angle + x_angle = atan2(-r20,r22)
        // WARNING.  The solution is not unique.  Choosing y_angle = 0.
        zAngle = -Math<Real>::HALF_PI;
        xAngle = Math<Real>::ATan2(-mEntry[6], mEntry[8]);
        yAngle = (Real)0;
        return EA_NOT_UNIQUE_SUM;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
typename Matrix3<Real>::EulerResult Matrix3<Real>::ExtractEulerYXZ (
    Real& yAngle, Real& xAngle, Real& zAngle) const
{
    // +-           -+   +-                                       -+
    // | r00 r01 r02 |   |  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz   cx*sy |
    // | r10 r11 r12 | = |  cx*sz           cx*cz           -sx    |
    // | r20 r21 r22 |   | -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz   cx*cy |
    // +-           -+   +-                                       -+

    if (mEntry[5] < (Real)1)
    {
        if (mEntry[5] > -(Real)1)
        {
            // x_angle = asin(-r12)
            // y_angle = atan2(r02,r22)
            // z_angle = atan2(r10,r11)
            xAngle = (Real)asin(-(double)mEntry[5]);
            yAngle = Math<Real>::ATan2(mEntry[2], mEntry[8]);
            zAngle = Math<Real>::ATan2(mEntry[3], mEntry[4]);
            return EA_UNIQUE;
        }
        else
        {
            // x_angle = +pi/2
            // z_angle - y_angle = atan2(-r01,r00)
            // WARNING.  The solution is not unique.  Choosing z_angle = 0.
            xAngle = Math<Real>::HALF_PI;
            yAngle = -Math<Real>::ATan2(-mEntry[1], mEntry[0]);
            zAngle = (Real)0;
            return EA_NOT_UNIQUE_DIF;
        }
    }
    else
    {
        // x_angle = -pi/2
        // z_angle + y_angle = atan2(-r01,r00)
        // WARNING.  The solution is not unique.  Choosing z_angle = 0.
        xAngle = -Math<Real>::HALF_PI;
        yAngle = Math<Real>::ATan2(-mEntry[1], mEntry[0]);
        zAngle = (Real)0;
        return EA_NOT_UNIQUE_SUM;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
typename Matrix3<Real>::EulerResult Matrix3<Real>::ExtractEulerYZX (
    Real& yAngle, Real& zAngle, Real& xAngle) const
{
    // +-           -+   +-                                       -+
    // | r00 r01 r02 |   |  cy*cz  sx*sy-cx*cy*sz   cx*sy+cy*sx*sz |
    // | r10 r11 r12 | = |  sz     cx*cz           -cz*sx          |
    // | r20 r21 r22 |   | -cz*sy  cy*sx+cx*sy*sz   cx*cy-sx*sy*sz |
    // +-           -+   +-                                       -+

    if (mEntry[3] < (Real)1)
    {
        if (mEntry[3] > -(Real)1)
        {
            // z_angle = asin(r10)
            // y_angle = atan2(-r20,r00)
            // x_angle = atan2(-r12,r11)
            zAngle = (Real)asin((double)mEntry[3]);
            yAngle = Math<Real>::ATan2(-mEntry[6], mEntry[0]);
            xAngle = Math<Real>::ATan2(-mEntry[5], mEntry[4]);
            return EA_UNIQUE;
        }
        else
        {
            // z_angle = -pi/2
            // x_angle - y_angle = atan2(r21,r22)
            // WARNING.  The solution is not unique.  Choosing x_angle = 0.
            zAngle = -Math<Real>::HALF_PI;
            yAngle = -Math<Real>::ATan2(mEntry[7], mEntry[8]);
            xAngle = (Real)0;
            return EA_NOT_UNIQUE_DIF;
        }
    }
    else
    {
        // z_angle = +pi/2
        // x_angle + y_angle = atan2(r21,r22)
        // WARNING.  The solution is not unique.  Choosing x_angle = 0.
        zAngle = Math<Real>::HALF_PI;
        yAngle = Math<Real>::ATan2(mEntry[7], mEntry[8]);
        xAngle = (Real)0;
        return EA_NOT_UNIQUE_SUM;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
typename Matrix3<Real>::EulerResult Matrix3<Real>::ExtractEulerZXY (
    Real& zAngle, Real& xAngle, Real& yAngle) const
{
    // +-           -+   +-                                        -+
    // | r00 r01 r02 |   |  cy*cz-sx*sy*sz  -cx*sz   cz*sy+cy*sx*sz |
    // | r10 r11 r12 | = |  cz*sx*sy+cy*sz   cx*cz  -cy*cz*sx+sy*sz |
    // | r20 r21 r22 |   | -cx*sy            sx      cx*cy          |
    // +-           -+   +-                                        -+

    if (mEntry[7] < (Real)1)
    {
        if (mEntry[7] > -(Real)1)
        {
            // x_angle = asin(r21)
            // z_angle = atan2(-r01,r11)
            // y_angle = atan2(-r20,r22)
            xAngle = (Real)asin((double)mEntry[7]);
            zAngle = Math<Real>::ATan2(-mEntry[1], mEntry[4]);
            yAngle = Math<Real>::ATan2(-mEntry[6], mEntry[8]);
            return EA_UNIQUE;
        }
        else
        {
            // x_angle = -pi/2
            // y_angle - z_angle = atan2(r02,r00)
            // WARNING.  The solution is not unique.  Choosing y_angle = 0.
            xAngle = -Math<Real>::HALF_PI;
            zAngle = -Math<Real>::ATan2(mEntry[2], mEntry[0]);
            yAngle = (Real)0;
            return EA_NOT_UNIQUE_DIF;
        }
    }
    else
    {
        // x_angle = +pi/2
        // y_angle + z_angle = atan2(r02,r00)
        // WARNING.  The solution is not unique.  Choosing y_angle = 0.
        xAngle = Math<Real>::HALF_PI;
        zAngle = Math<Real>::ATan2(mEntry[2], mEntry[0]);
        yAngle = (Real)0;
        return EA_NOT_UNIQUE_SUM;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
typename Matrix3<Real>::EulerResult Matrix3<Real>::ExtractEulerZYX (
    Real& zAngle, Real& yAngle, Real& xAngle) const
{
    // +-           -+   +-                                      -+
    // | r00 r01 r02 |   |  cy*cz  cz*sx*sy-cx*sz  cx*cz*sy+sx*sz |
    // | r10 r11 r12 | = |  cy*sz  cx*cz+sx*sy*sz -cz*sx+cx*sy*sz |
    // | r20 r21 r22 |   | -sy     cy*sx           cx*cy          |
    // +-           -+   +-                                      -+

    if (mEntry[6] < (Real)1)
    {
        if (mEntry[6] > -(Real)1)
        {
            // y_angle = asin(-r20)
            // z_angle = atan2(r10,r00)
            // x_angle = atan2(r21,r22)
            yAngle = (Real)asin(-(double)mEntry[6]);
            zAngle = Math<Real>::ATan2(mEntry[3], mEntry[0]);
            xAngle = Math<Real>::ATan2(mEntry[7], mEntry[8]);
            return EA_UNIQUE;
        }
        else
        {
            // y_angle = +pi/2
            // x_angle - z_angle = atan2(r01,r02)
            // WARNING.  The solution is not unique.  Choosing x_angle = 0.
            yAngle = Math<Real>::HALF_PI;
            zAngle = -Math<Real>::ATan2(mEntry[1], mEntry[2]);
            xAngle = (Real)0;
            return EA_NOT_UNIQUE_DIF;
        }
    }
    else
    {
        // y_angle = -pi/2
        // x_angle + z_angle = atan2(-r01,-r02)
        // WARNING.  The solution is not unique.  Choosing x_angle = 0;
        yAngle = -Math<Real>::HALF_PI;
        zAngle = Math<Real>::ATan2(-mEntry[1], -mEntry[2]);
        xAngle = (Real)0;
        return EA_NOT_UNIQUE_SUM;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
typename Matrix3<Real>::EulerResult Matrix3<Real>::ExtractEulerXYX (
    Real& x0Angle, Real& yAngle, Real& x1Angle) const
{
    // +-           -+   +-                                                -+
    // | r00 r01 r02 |   |  cy      sy*sx1               sy*cx1             |
    // | r10 r11 r12 | = |  sy*sx0  cx0*cx1-cy*sx0*sx1  -cy*cx1*sx0-cx0*sx1 |
    // | r20 r21 r22 |   | -sy*cx0  cx1*sx0+cy*cx0*sx1   cy*cx0*cx1-sx0*sx1 |
    // +-           -+   +-                                                -+

    if (mEntry[0] < (Real)1)
    {
        if (mEntry[0] > -(Real)1)
        {
            // y_angle  = acos(r00)
            // x0_angle = atan2(r10,-r20)
            // x1_angle = atan2(r01,r02)
            yAngle = (Real)acos((double)mEntry[0]);
            x0Angle = Math<Real>::ATan2(mEntry[3], -mEntry[6]);
            x1Angle = Math<Real>::ATan2(mEntry[1], mEntry[2]);
            return EA_UNIQUE;
        }
        else
        {
            // Not a unique solution:  x1_angle - x0_angle = atan2(-r12,r11)
            yAngle = Math<Real>::PI;
            x0Angle = -Math<Real>::ATan2(-mEntry[5], mEntry[4]);
            x1Angle = (Real)0;
            return EA_NOT_UNIQUE_DIF;
        }
    }
    else
    {
        // Not a unique solution:  x1_angle + x0_angle = atan2(-r12,r11)
        yAngle = (Real)0;
        x0Angle = Math<Real>::ATan2(-mEntry[5], mEntry[4]);
        x1Angle = (Real)0;
        return EA_NOT_UNIQUE_SUM;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
typename Matrix3<Real>::EulerResult Matrix3<Real>::ExtractEulerXZX (
    Real& x0Angle, Real& zAngle, Real& x1Angle) const
{
    // +-           -+   +-                                                -+
    // | r00 r01 r02 |   | cz      -sz*cx1               sz*sx1             |
    // | r10 r11 r12 | = | sz*cx0   cz*cx0*cx1-sx0*sx1  -cx1*sx0-cz*cx0*sx1 |
    // | r20 r21 r22 |   | sz*sx0   cz*cx1*sx0+cx0*sx1   cx0*cx1-cz*sx0*sx1 |
    // +-           -+   +-                                                -+

    if (mEntry[0] < (Real)1)
    {
        if (mEntry[0] > -(Real)1)
        {
            // z_angle  = acos(r00)
            // x0_angle = atan2(r20,r10)
            // x1_angle = atan2(r02,-r01)
            zAngle = (Real)acos((double)mEntry[0]);
            x0Angle = Math<Real>::ATan2(mEntry[6], mEntry[3]);
            x1Angle = Math<Real>::ATan2(mEntry[2], -mEntry[1]);
            return EA_UNIQUE;
        }
        else
        {
            // Not a unique solution:  x1_angle - x0_angle = atan2(r21,r22)
            zAngle = Math<Real>::PI;
            x0Angle = -Math<Real>::ATan2(mEntry[7], mEntry[8]);
            x1Angle = (Real)0;
            return EA_NOT_UNIQUE_DIF;
        }
    }
    else
    {
        // Not a unique solution:  x1_angle + x0_angle = atan2(r21,r22)
        zAngle = (Real)0;
        x0Angle = Math<Real>::ATan2(mEntry[7], mEntry[8]);
        x1Angle = (Real)0;
        return EA_NOT_UNIQUE_SUM;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
typename Matrix3<Real>::EulerResult Matrix3<Real>::ExtractEulerYXY (
    Real& y0Angle, Real& xAngle, Real& y1Angle) const
{
    // +-           -+   +-                                                -+
    // | r00 r01 r02 |   |  cy0*cy1-cx*sy0*sy1  sx*sy0   cx*cy1*sy0+cy0*sy1 |
    // | r10 r11 r12 | = |  sx*sy1              cx      -sx*cy1             |
    // | r20 r21 r22 |   | -cy1*sy0-cx*cy0*sy1  sx*cy0   cx*cy0*cy1-sy0*sy1 |
    // +-           -+   +-                                                -+

    if (mEntry[4] < (Real)1)
    {
        if (mEntry[4] > -(Real)1)
        {
            // x_angle  = acos(r11)
            // y0_angle = atan2(r01,r21)
            // y1_angle = atan2(r10,-r12)
            xAngle = (Real)acos((double)mEntry[4]);
            y0Angle = Math<Real>::ATan2(mEntry[1], mEntry[7]);
            y1Angle = Math<Real>::ATan2(mEntry[3], -mEntry[5]);
            return EA_UNIQUE;
        }
        else
        {
            // Not a unique solution:  y1_angle - y0_angle = atan2(r02,r00)
            xAngle = Math<Real>::PI;
            y0Angle = -Math<Real>::ATan2(mEntry[2], mEntry[0]);
            y1Angle = (Real)0;
            return EA_NOT_UNIQUE_DIF;
        }
    }
    else
    {
        // Not a unique solution:  y1_angle + y0_angle = atan2(r02,r00)
        xAngle = (Real)0;
        y0Angle = Math<Real>::ATan2(mEntry[2], mEntry[0]);
        y1Angle = (Real)0;
        return EA_NOT_UNIQUE_SUM;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
typename Matrix3<Real>::EulerResult Matrix3<Real>::ExtractEulerYZY (
    Real& y0Angle, Real& zAngle, Real& y1Angle) const
{
    // +-           -+   +-                                                -+
    // | r00 r01 r02 |   |  cz*cy0*cy1-sy0*sy1  -sz*cy0  cy1*sy0+cz*cy0*sy1 |
    // | r10 r11 r12 | = |  sz*cy1               cz      sz*sy1             |
    // | r20 r21 r22 |   | -cz*cy1*sy0-cy0*sy1   sz*sy0  cy0*cy1-cz*sy0*sy1 |
    // +-           -+   +-                                                -+

    if (mEntry[4] < (Real)1)
    {
        if (mEntry[4] > -(Real)1)
        {
            // z_angle  = acos(r11)
            // y0_angle = atan2(r21,-r01)
            // y1_angle = atan2(r12,r10)
            zAngle = (Real)acos((double)mEntry[4]);
            y0Angle = Math<Real>::ATan2(mEntry[7], -mEntry[1]);
            y1Angle = Math<Real>::ATan2(mEntry[5], mEntry[3]);
            return EA_UNIQUE;
        }
        else
        {
            // Not a unique solution:  y1_angle - y0_angle = atan2(-r20,r22)
            zAngle = Math<Real>::PI;
            y0Angle = -Math<Real>::ATan2(-mEntry[6], mEntry[8]);
            y1Angle = (Real)0;
            return EA_NOT_UNIQUE_DIF;
        }
    }
    else
    {
        // Not a unique solution:  y1_angle + y0_angle = atan2(-r20,r22)
        zAngle = (Real)0;
        y0Angle = Math<Real>::ATan2(-mEntry[6], mEntry[8]);
        y1Angle = (Real)0;
        return EA_NOT_UNIQUE_SUM;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
typename Matrix3<Real>::EulerResult Matrix3<Real>::ExtractEulerZXZ (
    Real& z0Angle, Real& xAngle, Real& z1Angle) const
{
    // +-           -+   +-                                                -+
    // | r00 r01 r02 |   | cz0*cz1-cx*sz0*sz1  -cx*cz1*sz0-cz0*sz1   sx*sz0 |
    // | r10 r11 r12 | = | cz1*sz0+cx*cz0*sz1   cx*cz0*cz1-sz0*sz1  -sz*cz0 |
    // | r20 r21 r22 |   | sx*sz1               sx*cz1               cx     |
    // +-           -+   +-                                                -+

    if (mEntry[8] < (Real)1)
    {
        if (mEntry[8] > -(Real)1)
        {
            // x_angle  = acos(r22)
            // z0_angle = atan2(r02,-r12)
            // z1_angle = atan2(r20,r21)
            xAngle = (Real)acos((double)mEntry[8]);
            z0Angle = Math<Real>::ATan2(mEntry[2], -mEntry[5]);
            z1Angle = Math<Real>::ATan2(mEntry[6], mEntry[7]);
            return EA_UNIQUE;
        }
        else
        {
            // Not a unique solution:  z1_angle - z0_angle = atan2(-r01,r00)
            xAngle = Math<Real>::PI;
            z0Angle = -Math<Real>::ATan2(-mEntry[1], mEntry[0]);
            z1Angle = (Real)0;
            return EA_NOT_UNIQUE_DIF;
        }
    }
    else
    {
        // Not a unique solution:  z1_angle + z0_angle = atan2(-r01,r00)
        xAngle = (Real)0;
        z0Angle = Math<Real>::ATan2(-mEntry[1], mEntry[0]);
        z1Angle = (Real)0;
        return EA_NOT_UNIQUE_SUM;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
typename Matrix3<Real>::EulerResult Matrix3<Real>::ExtractEulerZYZ (
    Real& z0Angle, Real& yAngle, Real& z1Angle) const
{
    // +-           -+   +-                                                -+
    // | r00 r01 r02 |   |  cy*cz0*cz1-sz0*sz1  -cz1*sz0-cy*cz0*sz1  sy*cz0 |
    // | r10 r11 r12 | = |  cy*cz1*sz0+cz0*sz1   cz0*cz1-cy*sz0*sz1  sy*sz0 |
    // | r20 r21 r22 |   | -sy*cz1               sy*sz1              cy     |
    // +-           -+   +-                                                -+

    if (mEntry[8] < (Real)1)
    {
        if (mEntry[8] > -(Real)1)
        {
            // y_angle  = acos(r22)
            // z0_angle = atan2(r12,r02)
            // z1_angle = atan2(r21,-r20)
            yAngle = (Real)acos((double)mEntry[8]);
            z0Angle = Math<Real>::ATan2(mEntry[5], mEntry[2]);
            z1Angle = Math<Real>::ATan2(mEntry[7], -mEntry[6]);
            return EA_UNIQUE;
        }
        else // r22 = -1
        {
            // Not a unique solution:  z1_angle - z0_angle = atan2(r10,r11)
            yAngle = Math<Real>::PI;
            z0Angle = -Math<Real>::ATan2(mEntry[3], mEntry[4]);
            z1Angle = (Real)0;
            return EA_NOT_UNIQUE_DIF;
        }
    }
    else // r22 = +1
    {
        // Not a unique solution:  z1_angle + z0_angle = atan2(r10,r11)
        yAngle = (Real)0;
        z0Angle = Math<Real>::ATan2(mEntry[3], mEntry[4]);
        z1Angle = (Real)0;
        return EA_NOT_UNIQUE_SUM;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Matrix3<Real>& Matrix3<Real>::Slerp (Real t, const Matrix3& rot0,
    const Matrix3& rot1)
{
    Vector3<Real> axis;
    Real angle;
    Matrix3 prod = rot0.TransposeTimes(rot1);
    prod.ExtractAxisAngle(axis, angle);
    MakeRotation(axis, t*angle);
    *this = rot0*(*this);
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::SingularValueDecomposition (Matrix3& left,
    Matrix3& diag, Matrix3& rightTranspose) const
{
    // TODO.  Replace by a call to EigenDecomposition and a QR factorization
    // that is specialized for 3x3.  The QDUDecomposition appears to assume
    // the input matrix is invertible, but a general QR factorization has to
    // deal with non-full rank.

    GMatrix<Real> M(3, 3);
    memcpy(M.GetElements(), mEntry, 9*sizeof(Real));

    GMatrix<Real> tmpL(3, 3), tmpD(3, 3), tmpRTranspose(3, 3);
    Wm5::SingularValueDecomposition<Real>(M, tmpL, tmpD, tmpRTranspose);

    memcpy(left.mEntry, tmpL.GetElements(), 9*sizeof(Real));
    memcpy(diag.mEntry, tmpD.GetElements(), 9*sizeof(Real));
    memcpy(rightTranspose.mEntry, tmpRTranspose.GetElements(), 9*sizeof(Real));
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::PolarDecomposition (Matrix3& qMat, Matrix3& sMat)
{
    // Decompose M = L*D*R^T.
    Matrix3 left, diag, rightTranspose;
    SingularValueDecomposition(left, diag, rightTranspose);

    // Compute Q = L*R^T.
    qMat = left*rightTranspose;

    // Compute S = R*D*R^T.
    sMat = rightTranspose.TransposeTimes(diag*rightTranspose);

    // Numerical round-off errors can cause S not to be symmetric in the
    // sense that S[i][j] and S[j][i] are slightly different for i != j.
    // Correct this by averaging S = (S + S^T)/2.
    sMat[0][1] = ((Real)0.5)*(sMat[0][1] + sMat[1][0]);
    sMat[1][0] = sMat[0][1];
    sMat[0][2] = ((Real)0.5)*(sMat[0][2] + sMat[2][0]);
    sMat[2][0] = sMat[0][2];
    sMat[1][2] = ((Real)0.5)*(sMat[1][2] + sMat[2][1]);
    sMat[2][1] = sMat[1][2];
}
//----------------------------------------------------------------------------
template <typename Real>
void Matrix3<Real>::QDUDecomposition (Matrix3& qMat, Matrix3& diag,
    Matrix3& uMat) const
{
    // Factor M = QR = QDU where Q is orthogonal (rotation), D is diagonal
    // (scaling),  and U is upper triangular with ones on its diagonal
    // (shear).  Algorithm uses Gram-Schmidt orthogonalization (the QR
    // algorithm).
    //
    // If M = [ m0 | m1 | m2 ] and Q = [ q0 | q1 | q2 ], then
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.  The matrix R has entries
    //
    //   r00 = q0*m0  r01 = q0*m1  r02 = q0*m2
    //   r10 = 0      r11 = q1*m1  r12 = q1*m2
    //   r20 = 0      r21 = 0      r22 = q2*m2
    //
    // so D = diag(r00,r11,r22) and U has entries u01 = r01/r00,
    // u02 = r02/r00, and u12 = r12/r11.

    // Build orthogonal matrix Q.
    Real invLength = Math<Real>::InvSqrt(mEntry[0]*mEntry[0] +
        mEntry[3]*mEntry[3] + mEntry[6]*mEntry[6]);
    qMat[0][0] = mEntry[0]*invLength;
    qMat[1][0] = mEntry[3]*invLength;
    qMat[2][0] = mEntry[6]*invLength;

    Real fDot = qMat[0][0]*mEntry[1] + qMat[1][0]*mEntry[4] +
        qMat[2][0]*mEntry[7];
    qMat[0][1] = mEntry[1]-fDot*qMat[0][0];
    qMat[1][1] = mEntry[4]-fDot*qMat[1][0];
    qMat[2][1] = mEntry[7]-fDot*qMat[2][0];
    invLength = Math<Real>::InvSqrt(qMat[0][1]*qMat[0][1] +
        qMat[1][1]*qMat[1][1] + qMat[2][1]*qMat[2][1]);
    qMat[0][1] *= invLength;
    qMat[1][1] *= invLength;
    qMat[2][1] *= invLength;

    fDot = qMat[0][0]*mEntry[2] + qMat[1][0]*mEntry[5] +
        qMat[2][0]*mEntry[8];
    qMat[0][2] = mEntry[2]-fDot*qMat[0][0];
    qMat[1][2] = mEntry[5]-fDot*qMat[1][0];
    qMat[2][2] = mEntry[8]-fDot*qMat[2][0];
    fDot = qMat[0][1]*mEntry[2] + qMat[1][1]*mEntry[5] +
        qMat[2][1]*mEntry[8];
    qMat[0][2] -= fDot*qMat[0][1];
    qMat[1][2] -= fDot*qMat[1][1];
    qMat[2][2] -= fDot*qMat[2][1];
    invLength = Math<Real>::InvSqrt(qMat[0][2]*qMat[0][2] +
        qMat[1][2]*qMat[1][2] + qMat[2][2]*qMat[2][2]);
    qMat[0][2] *= invLength;
    qMat[1][2] *= invLength;
    qMat[2][2] *= invLength;

    // Guarantee that orthogonal matrix has determinant 1 (no reflections).
    Real det =
        qMat[0][0]*qMat[1][1]*qMat[2][2] + qMat[0][1]*qMat[1][2]*qMat[2][0] +
        qMat[0][2]*qMat[1][0]*qMat[2][1] - qMat[0][2]*qMat[1][1]*qMat[2][0] -
        qMat[0][1]*qMat[1][0]*qMat[2][2] - qMat[0][0]*qMat[1][2]*qMat[2][1];

    if (det < (Real)0)
    {
        for (int row = 0; row < 3; ++row)
        {
            for (int col = 0; col < 3; ++col)
            {
                qMat[row][col] = -qMat[row][col];
            }
        }
    }

    // Build "right" matrix R.
    Matrix3 right;
    right[0][0] = qMat[0][0]*mEntry[0] + qMat[1][0]*mEntry[3] +
        qMat[2][0]*mEntry[6];
    right[0][1] = qMat[0][0]*mEntry[1] + qMat[1][0]*mEntry[4] +
        qMat[2][0]*mEntry[7];
    right[1][1] = qMat[0][1]*mEntry[1] + qMat[1][1]*mEntry[4] +
        qMat[2][1]*mEntry[7];
    right[0][2] = qMat[0][0]*mEntry[2] + qMat[1][0]*mEntry[5] +
        qMat[2][0]*mEntry[8];
    right[1][2] = qMat[0][1]*mEntry[2] + qMat[1][1]*mEntry[5] +
        qMat[2][1]*mEntry[8];
    right[2][2] = qMat[0][2]*mEntry[2] + qMat[1][2]*mEntry[5] +
        qMat[2][2]*mEntry[8];

    // The scaling component.
    diag.MakeDiagonal(right[0][0], right[1][1], right[2][2]);

    // the shear component
    Real invD00 = ((Real)1)/diag[0][0];
    uMat[0][0] = (Real)1;
    uMat[0][1] = right[0][1]*invD00;
    uMat[0][2] = right[0][2]*invD00;
    uMat[1][0] = (Real)0;
    uMat[1][1] = (Real)1;
    uMat[1][2] = right[1][2]/diag[1][1];
    uMat[2][0] = (Real)0;
    uMat[2][1] = (Real)0;
    uMat[2][2] = (Real)1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Matrix3<Real>::Tridiagonalize (Real diagonal[3], Real subdiagonal[2])
{
    // Householder reduction T = Q^t M Q
    //   Input:   
    //     mat, symmetric 3x3 matrix M
    //   Output:  
    //     mat, orthogonal matrix Q (a reflection)
    //     diag, diagonal entries of T
    //     subd, subdiagonal entries of T (T is symmetric)

    Real m00 = mEntry[0];
    Real m01 = mEntry[1];
    Real m02 = mEntry[2];
    Real m11 = mEntry[4];
    Real m12 = mEntry[5];
    Real m22 = mEntry[8];

    diagonal[0] = m00;
    if (Math<Real>::FAbs(m02) >= Math<Real>::ZERO_TOLERANCE)
    {
        subdiagonal[0] = Math<Real>::Sqrt(m01*m01 + m02*m02);
        Real invLength = ((Real)1)/subdiagonal[0];
        m01 *= invLength;
        m02 *= invLength;
        Real tmp = ((Real)2)*m01*m12 + m02*(m22 - m11);
        diagonal[1] = m11 + m02*tmp;
        diagonal[2] = m22 - m02*tmp;
        subdiagonal[1] = m12 - m01*tmp;

        mEntry[0] = (Real)1;
        mEntry[1] = (Real)0;
        mEntry[2] = (Real)0;
        mEntry[3] = (Real)0;
        mEntry[4] = m01;
        mEntry[5] = m02;
        mEntry[6] = (Real)0;
        mEntry[7] = m02;
        mEntry[8] = -m01;
        return true;
    }
    else
    {
        diagonal[1] = m11;
        diagonal[2] = m22;
        subdiagonal[0] = m01;
        subdiagonal[1] = m12;

        mEntry[0] = (Real)1;
        mEntry[1] = (Real)0;
        mEntry[2] = (Real)0;
        mEntry[3] = (Real)0;
        mEntry[4] = (Real)1;
        mEntry[5] = (Real)0;
        mEntry[6] = (Real)0;
        mEntry[7] = (Real)0;
        mEntry[8] = (Real)1;
        return false;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool Matrix3<Real>::QLAlgorithm (Real diagonal[3], Real subdiagonal[2])
{
    // This is an implementation of the symmetric QR algorithm from the book
    // "Matrix Computations" by Gene H. Golub and Charles F. Van Loan,
    // second edition.  The algorithm is 8.2.3.  The implementation has a
    // slight variation to actually make it a QL algorithm, and it traps the
    // case when either of the subdiagonal terms s0 or s1 is zero and reduces
    // the 2-by-2 subblock directly.

    const int imax = 32;
    for (int i = 0; i < imax; ++i)
    {
        Real sum, diff, discr, eigVal0, eigVal1, cs, sn, tmp;
        int row;

        sum = Math<Real>::FAbs(diagonal[0]) + Math<Real>::FAbs(diagonal[1]);
        if (Math<Real>::FAbs(subdiagonal[0]) + sum == sum)
        {
            // The matrix is effectively
            //       +-        -+
            //   M = | d0  0  0 |
            //       | 0  d1 s1 |
            //       | 0  s1 d2 |
            //       +-        -+

            // Test whether M is diagonal (within numerical round-off).
            sum = Math<Real>::FAbs(diagonal[1]) +
                Math<Real>::FAbs(diagonal[2]);
            if (Math<Real>::FAbs(subdiagonal[1]) + sum == sum)
            {
                return true;
            }

            // Compute the eigenvalues as roots of a quadratic equation.
            sum = diagonal[1] + diagonal[2];
            diff = diagonal[1] - diagonal[2];
            discr = Math<Real>::Sqrt(diff*diff +
                ((Real)4)*subdiagonal[1]*subdiagonal[1]);
            eigVal0 = ((Real)0.5)*(sum - discr);
            eigVal1 = ((Real)0.5)*(sum + discr);

            // Compute the Givens rotation.
            if (diff >= (Real)0)
            {
                cs = subdiagonal[1];
                sn = diagonal[1] - eigVal0;
            }
            else
            {
                cs = diagonal[2] - eigVal0;
                sn = subdiagonal[1];
            }
            tmp = Math<Real>::InvSqrt(cs*cs + sn*sn);
            cs *= tmp;
            sn *= tmp;

            // Postmultiply the current orthogonal matrix with the Givens
            // rotation.
            for (row = 0; row < 3; ++row)
            {
                tmp = mEntry[2+3*row];
                mEntry[2+3*row] = sn*mEntry[1+3*row] + cs*tmp;
                mEntry[1+3*row] = cs*mEntry[1+3*row] - sn*tmp;
            }

            // Update the tridiagonal matrix.
            diagonal[1] = eigVal0;
            diagonal[2] = eigVal1;
            subdiagonal[0] = (Real)0;
            subdiagonal[1] = (Real)0;
            return true;
        }

        sum = Math<Real>::FAbs(diagonal[1]) + Math<Real>::FAbs(diagonal[2]);
        if (Math<Real>::FAbs(subdiagonal[1]) + sum == sum)
        {
            // The matrix is effectively
            //       +-         -+
            //   M = | d0  s0  0 |
            //       | s0  d1  0 |
            //       | 0   0  d2 |
            //       +-         -+

            // Test whether M is diagonal (within numerical round-off).
            sum = Math<Real>::FAbs(diagonal[0]) +
                Math<Real>::FAbs(diagonal[1]);
            if (Math<Real>::FAbs(subdiagonal[0]) + sum == sum)
            {
                return true;
            }

            // Compute the eigenvalues as roots of a quadratic equation.
            sum = diagonal[0] + diagonal[1];
            diff = diagonal[0] - diagonal[1];
            discr = Math<Real>::Sqrt(diff*diff +
                ((Real)4.0)*subdiagonal[0]*subdiagonal[0]);
            eigVal0 = ((Real)0.5)*(sum - discr);
            eigVal1 = ((Real)0.5)*(sum + discr);

            // Compute the Givens rotation.
            if (diff >= (Real)0)
            {
                cs = subdiagonal[0];
                sn = diagonal[0] - eigVal0;
            }
            else
            {
                cs = diagonal[1] - eigVal0;
                sn = subdiagonal[0];
            }
            tmp = Math<Real>::InvSqrt(cs*cs + sn*sn);
            cs *= tmp;
            sn *= tmp;

            // Postmultiply the current orthogonal matrix with the Givens
            // rotation.
            for (row = 0; row < 3; ++row)
            {
                tmp = mEntry[1+3*row];
                mEntry[1+3*row] = sn*mEntry[0+3*row] + cs*tmp;
                mEntry[0+3*row] = cs*mEntry[0+3*row] - sn*tmp;
            }

            // Update the tridiagonal matrix.
            diagonal[0] = eigVal0;
            diagonal[1] = eigVal1;
            subdiagonal[0] = (Real)0;
            subdiagonal[1] = (Real)0;
            return true;
        }

        // The matrix is
        //       +-        -+
        //   M = | d0 s0  0 |
        //       | s0 d1 s1 |
        //       | 0  s1 d2 |
        //       +-        -+

        // Set up the parameters for the first pass of the QL step.  The
        // value for A is the difference between diagonal term D[2] and the
        // implicit shift suggested by Wilkinson.
        Real ratio = (diagonal[1] - diagonal[0])/(((Real)2)*subdiagonal[0]);
        Real root = Math<Real>::Sqrt((Real)1 + ratio*ratio);
        Real b = subdiagonal[1];
        Real a = diagonal[2] - diagonal[0];
        if (ratio >= (Real)0)
        {
            a += subdiagonal[0]/(ratio + root);
        }
        else
        {
            a += subdiagonal[0]/(ratio - root);
        }

        // Compute the Givens rotation for the first pass.
        if (Math<Real>::FAbs(b) >= Math<Real>::FAbs(a))
        {
            ratio = a/b;
            sn = Math<Real>::InvSqrt((Real)1 + ratio*ratio);
            cs = ratio*sn;
        }
        else
        {
            ratio = b/a;
            cs = Math<Real>::InvSqrt((Real)1 + ratio*ratio);
            sn = ratio*cs;
        }

        // Postmultiply the current orthogonal matrix with the Givens
        // rotation.
        for (row = 0; row < 3; ++row)
        {
            tmp = mEntry[2+3*row];
            mEntry[2+3*row] = sn*mEntry[1+3*row] + cs*tmp;
            mEntry[1+3*row] = cs*mEntry[1+3*row] - sn*tmp;
        }

        // Set up the parameters for the second pass of the QL step.  The
        // values tmp0 and tmp1 are required to fully update the tridiagonal
        // matrix at the end of the second pass.
        Real tmp0 = (diagonal[1] - diagonal[2])*sn +
            ((Real)2)*subdiagonal[1]*cs;
        Real tmp1 = cs*subdiagonal[0];
        b = sn*subdiagonal[0];
        a = cs*tmp0 - subdiagonal[1];
        tmp0 *= sn;

        // Compute the Givens rotation for the second pass.  The subdiagonal
        // term S[1] in the tridiagonal matrix is updated at this time.
        if (Math<Real>::FAbs(b) >= Math<Real>::FAbs(a))
        {
            ratio = a/b;
            root = Math<Real>::Sqrt((Real)1 + ratio*ratio);
            subdiagonal[1] = b*root;
            sn = ((Real)1)/root;
            cs = ratio*sn;
        }
        else
        {
            ratio = b/a;
            root = Math<Real>::Sqrt((Real)1 + ratio*ratio);
            subdiagonal[1] = a*root;
            cs = ((Real)1)/root;
            sn = ratio*cs;
        }

        // Postmultiply the current orthogonal matrix with the Givens
        // rotation.
        for (row = 0; row < 3; ++row)
        {
            tmp = mEntry[1+3*row];
            mEntry[1+3*row] = sn*mEntry[0+3*row] + cs*tmp;
            mEntry[0+3*row] = cs*mEntry[0+3*row] - sn*tmp;
        }

        // Complete the update of the tridiagonal matrix.
        Real tmp2 = diagonal[1] - tmp0;
        diagonal[2] += tmp0;
        tmp0 = (diagonal[0] - tmp2)*sn + ((Real)2)*tmp1*cs;
        subdiagonal[0] = cs*tmp0 - tmp1;
        tmp0 *= sn;
        diagonal[1] = tmp2 + tmp0;
        diagonal[0] -= tmp0;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Matrix3<Real> operator* (Real scalar, const Matrix3<Real>& mat)
{
    return mat*scalar;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector3<Real> operator* (const Vector3<Real>& vec,
    const Matrix3<Real>& mat)
{
    return Vector3<Real>
    (
        vec[0]*mat[0][0] + vec[1]*mat[1][0] + vec[2]*mat[2][0],
        vec[0]*mat[0][1] + vec[1]*mat[1][1] + vec[2]*mat[2][1],
        vec[0]*mat[0][2] + vec[1]*mat[1][2] + vec[2]*mat[2][2]
    );
}
//----------------------------------------------------------------------------
