// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5MATRIX2_H
#define WM5MATRIX2_H

// Rotation matrices are of the form
//   R = cos(t) -sin(t)
//       sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the xy-plane.

#include "Wm5MathematicsLIB.h"
#include "Wm5Table.h"
#include "Wm5Vector2.h"

namespace Wm5
{

template <typename Real>
class Matrix2 : public Table<2,2,Real>
{
public:
    // If makeZero is 'true', create the zero matrix; otherwise, create the
    // identity matrix.
    Matrix2 (bool makeZero = true);

    // Copy constructor.
    Matrix2 (const Matrix2& mat);

    // Input mrc is in row r, column c.
    Matrix2 (Real m00, Real m01, Real m10, Real m11);

    // Create a matrix from an array of numbers.  The input array is
    // interpreted based on the bool input as
    //   true:  entry[0..3] = {m00,m01,m10,m11}  [row major]
    //   false: entry[0..3] = {m00,m10,m01,m11}  [column major]
    Matrix2 (const Real entry[4], bool rowMajor);

    // Create matrices based on vector input.  The bool is interpreted as
    //   true: vectors are columns of the matrix
    //   false: vectors are rows of the matrix
    Matrix2 (const Vector2<Real>& u, const Vector2<Real>& v,
        bool columns);
    Matrix2 (const Vector2<Real>* vectors, bool columns);

    // Create a diagonal matrix, m01 = m10 = 0.
    Matrix2 (Real m00, Real m11);

    // Create a rotation matrix (positive angle -> counterclockwise).
    Matrix2 (Real angle);

    // Create a tensor product U*V^T.
    Matrix2 (const Vector2<Real>& u, const Vector2<Real>& v);

    // Assignment.
    Matrix2& operator= (const Matrix2& mat);

    // Create various matrices.
    void MakeZero ();
    void MakeIdentity ();
    void MakeDiagonal (Real m00, Real m11);
    void MakeRotation (Real angle);
    void MakeTensorProduct (const Vector2<Real>& u, const Vector2<Real>& v);

    // Arithmetic operations.
    Matrix2 operator+ (const Matrix2& mat) const;
    Matrix2 operator- (const Matrix2& mat) const;
    Matrix2 operator* (Real scalar) const;
    Matrix2 operator/ (Real scalar) const;
    Matrix2 operator- () const;

    // Arithmetic updates.
    Matrix2& operator+= (const Matrix2& mat);
    Matrix2& operator-= (const Matrix2& mat);
    Matrix2& operator*= (Real scalar);
    Matrix2& operator/= (Real scalar);

    // M*vec
    Vector2<Real> operator* (const Vector2<Real>& vec) const;

    // u^T*M*v
    Real QForm (const Vector2<Real>& u, const Vector2<Real>& v) const;

    // M^T
    Matrix2 Transpose () const;

    // M*mat
    Matrix2 operator* (const Matrix2& mat) const;

    // M^T*mat
    Matrix2 TransposeTimes (const Matrix2& mat) const;

    // M*mat^T
    Matrix2 TimesTranspose (const Matrix2& mat) const;

    // M^T*mat^T
    Matrix2 TransposeTimesTranspose (const Matrix2& mat) const;

    // Other operations.
    Matrix2 Inverse (const Real epsilon = (Real)0) const;
    Matrix2 Adjoint () const;
    Real Determinant () const;

    // The matrix must be a rotation for these functions to be valid.  The
    // last function uses Gram-Schmidt orthonormalization applied to the
    // columns of the rotation matrix.  The angle must be in radians, not
    // degrees.
    void ExtractAngle (Real& angle) const;
    void Orthonormalize ();

    // The matrix must be symmetric.  Factor M = R * D * R^T where
    // R = [u0|u1] is a rotation matrix with columns u0 and u1 and
    // D = diag(d0,d1) is a diagonal matrix whose diagonal entries are d0
    // and d1.  The eigenvector u[i] corresponds to eigenvector d[i].  The
    // eigenvalues are ordered as d0 <= d1.
    void EigenDecomposition (Matrix2& rot, Matrix2& diag) const;

    // Special matrices.
    WM5_MATHEMATICS_ITEM static const Matrix2 ZERO;
    WM5_MATHEMATICS_ITEM static const Matrix2 IDENTITY;

protected:
    using Table<2,2,Real>::mEntry;
};

// c * M
template <typename Real>
inline Matrix2<Real> operator* (Real scalar, const Matrix2<Real>& mat);

// v^T * M
template <typename Real>
inline Vector2<Real> operator* (const Vector2<Real>& vec,
    const Matrix2<Real>& mat);

#include "Wm5Matrix2.inl"

typedef Matrix2<float> Matrix2f;
typedef Matrix2<double> Matrix2d;

}

#endif
