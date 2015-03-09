// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/03/27)

#ifndef WM5HMATRIX_H
#define WM5HMATRIX_H

#include "Wm5MathematicsLIB.h"
#include "Wm5APoint.h"
#include "Wm5Matrix3.h"

namespace Wm5
{

class WM5_MATHEMATICS_ITEM HMatrix
{
public:
    // Construction and destruction.
    HMatrix ();  // uninitialized
    HMatrix (const HMatrix& mat);
    HMatrix (const Matrix3f& mat);

    // If makeZero is 'true', create the zero matrix; otherwise, create the
    // identity matrix.
    HMatrix (bool makeZero);

    // Input mrc is in row r, column c.
    HMatrix (
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33);

    // Create a matrix from an array of numbers.  The input array is
    // interpreted based on the bool input as
    //   true:  entry[0..15]={m00,m01,m02,m03,m10,m11,m12,m13,m20,m21,m22,
    //                        m23,m30,m31,m32,m33} [row major]
    //   false: entry[0..15]={m00,m10,m20,m30,m01,m11,m21,m31,m02,m12,m22,
    //                        m32,m03,m13,m23,m33} [col major]
    HMatrix (const float* entry, bool rowMajor);

    // Create matrices based on point and vector input.  The bool is
    // interpreted as
    //   true: inputs are columns of the matrix
    //   false: inputs are rows of the matrix
    HMatrix (const float* tuple0, const float* tuple1, const float* tuple2,
        const float* tuple3, bool columns);

    // Create a diagonal matrix.
    HMatrix (float m00, float m11, float m22);

    // Create a rotation matrix (positive angle -> counterclockwise).  The
    // angle must be in radians, not degrees.
    HMatrix (const AVector& axis, float angle);

    ~HMatrix ();

    // Implicit conversions.  The upper 3x3 block of HMatrix is copied to
    // the Matrix3f object.
    operator Matrix3f () const;

    // Coordinate access.
    inline operator const float* () const;
    inline operator float* ();
    inline const float* operator[] (int row) const;
    inline float* operator[] (int row);
    inline const float& operator() (int row, int column) const;
    inline float& operator() (int row, int column);
    void SetRow (int row, const HPoint& hpnt);
    void GetRow (int row, HPoint& hpnt) const;
    void SetColumn (int column, const HPoint& hpnt);
    void GetColumn (int column, HPoint& hpnt) const;

    // The matrix is stored in row-major order.  Store these in column-major
    // order in the specified array, which must have at least 16 slots.
    void GetColumnMajor (float* columnMajor) const;

    // Assignment.
    HMatrix& operator= (const HMatrix& mat);
    HMatrix& operator= (const Matrix3f& mat);

    // Comparison (for use by STL containers).
    bool operator== (const HMatrix& mat) const;
    bool operator!= (const HMatrix& mat) const;
    bool operator<  (const HMatrix& mat) const;
    bool operator<= (const HMatrix& mat) const;
    bool operator>  (const HMatrix& mat) const;
    bool operator>= (const HMatrix& mat) const;

    // Arithmetic operations.
    HMatrix operator+ (const HMatrix& mat) const;
    HMatrix operator- (const HMatrix& mat) const;
    HMatrix operator* (float scalar) const;
    HMatrix operator/ (float scalar) const;
    HMatrix operator- () const;

    WM5_MATHEMATICS_ITEM
    friend HMatrix operator* (float scalar, const HMatrix& mat);

    // Arithmetic updates.
    HMatrix& operator+= (const HMatrix& mat);
    HMatrix& operator-= (const HMatrix& mat);
    HMatrix& operator*= (float scalar);
    HMatrix& operator/= (float scalar);

    // Operations on matrices.
    void MakeZero ();  // Z
    void MakeIdentity ();  // I
    void MakeDiagonal (float m00, float m11, float m22);  // D
    void MakeRotation (const AVector& axis, float angle);  // R
    HMatrix Transpose () const;  // M^T
    HMatrix Inverse (const float epsilon = 0.0f) const;  // M^{-1}
    HMatrix Adjoint () const;  // M^{adj}
    float Determinant () const;  // det(M)
    HMatrix operator* (const HMatrix& mat) const;  // M*mat
    HMatrix TransposeTimes (const HMatrix& mat) const;  // M^T*mat
    HMatrix TimesTranspose (const HMatrix& mat) const;  // M*mat^T
    HMatrix TransposeTimesTranspose (const HMatrix& mat) const;  // M^T*mat^T
    HMatrix TimesDiagonal (const APoint& diag) const;  // M*D
    HMatrix DiagonalTimes (const APoint& diag) const;  // D*M
    void Orthonormalize (); // applies to 3x3 upper-left block of M

    // Operations between matrices and homogeneous points.  Both M and p
    // are general homogeneous objects (M not required to be affine, p
    // not required to have w = 1).
    HPoint operator* (const HPoint& p) const;  // M*p
    
    WM5_MATHEMATICS_ITEM
    friend HPoint operator* (const HPoint& p, const HMatrix& mat);  // p*M

    void BatchMultiply (int numPoints, const HPoint* input,
        HPoint* output) const;  // M*p[0], ..., M*p[n-1]

    // Operations between affine matrices and affine points.
    APoint operator* (const APoint& p) const;  // M*p
    void BatchMultiply (int numPoints, const APoint* input,
        APoint* output) const;  // M*p[0], ..., M*p[n-1]

    // Operations between affine matrices and affine vectors.
    AVector operator* (const AVector& p) const;  // M*v
    void BatchMultiply (int numPoints, const AVector* input,
        AVector* output) const;  // M*v[0], ..., M*v[n-1]

    // Compute a quadratic forms.
    float QForm (const HPoint& p0, const HPoint& p1) const;  // p0^T*M*p1

    // Set the transformation to an oblique projection matrix onto a
    // specified plane.  The plane has an 'origin' point and a unit-length
    // 'normal'.
    void MakeObliqueProjection (const APoint& origin, const AVector& normal,
        const AVector& direction);

    // Set the transformation to a perspective projection matrix onto a
    // specified plane.  The plane has an 'origin' point and a unit-length
    // 'normal'.  The 'eye' is the origin of projection.
    void MakePerspectiveProjection (const APoint& origin,
        const AVector& normal, const APoint& eye);

    // Set the transformation to a reflection matrix through a specified
    // plane.  The plane has an 'origin' point and a unit-length 'normal'.
    void MakeReflection (const APoint& origin, const AVector& normal);

    // Special matrices.
    static const HMatrix ZERO;
    static const HMatrix IDENTITY;

private:
    // The matrix is stored in row-major order.
    float mEntry[16];
};

#include "Wm5HMatrix.inl"

}

#endif
