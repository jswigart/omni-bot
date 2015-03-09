// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5EIGENDECOMPOSITION_H
#define WM5EIGENDECOMPOSITION_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Matrix2.h"
#include "Wm5Matrix3.h"
#include "Wm5Matrix4.h"
#include "Wm5GMatrix.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM EigenDecomposition
{
public:
    // Construction and destruction.  The matrix of an eigensystem must be
    // symmetric.
    EigenDecomposition (int size);
    EigenDecomposition (const Matrix2<Real>& mat);
    EigenDecomposition (const Matrix3<Real>& mat);
    EigenDecomposition (const GMatrix<Real>& mat);
    ~EigenDecomposition ();

    // Set the matrix for the eigensystem.
    Real& operator() (int row, int column);
    EigenDecomposition& operator= (const Matrix2<Real>& mat);
    EigenDecomposition& operator= (const Matrix3<Real>& mat);
    EigenDecomposition& operator= (const GMatrix<Real>& mat);

    // Solve the eigensystem.  Set 'increasingSort' to 'true' when you want
    // the eigenvalues to be sorted in increasing order; otherwise, the
    // eigenvalues are sorted in decreasing order.
    void Solve (bool increasingSort);

    // Get the results.  The calls to GetEigenvector2, GetEigenvectors2,
    // GetEigenvector3, and GetEigenvector3 should be made only if you know
    // that the eigensystem is of the corresponding size.
    Real GetEigenvalue (int i) const;
    const Real* GetEigenvalues () const;
    Vector2<Real> GetEigenvector2 (int i) const;
    Matrix2<Real> GetEigenvectors2 () const;
    Vector3<Real> GetEigenvector3 (int i) const;
    Matrix3<Real> GetEigenvectors3 () const;
    GVector<Real> GetEigenvector (int i) const;
    const GMatrix<Real>& GetEigenvectors () const;

private:
    int mSize;
    GMatrix<Real> mMatrix;
    Real* mDiagonal;
    Real* mSubdiagonal;

    // For odd size matrices, the Householder reduction involves an odd
    // number of reflections.  The product of these is a reflection.  The
    // QL algorithm uses rotations for further reductions.  The final
    // orthogonal matrix whose columns are the eigenvectors is a reflection,
    // so its determinant is -1.  For even size matrices, the Householder
    // reduction involves an even number of reflections whose product is a
    // rotation.  The final orthogonal matrix has determinant +1.  Many
    // algorithms that need an eigendecomposition want a rotation matrix.
    // We want to guarantee this is the case, so mRotation keeps track of
    // this.  The DecrSort and IncrSort further complicate the issue since
    // they swap columns of the orthogonal matrix, causing the matrix to
    // toggle between rotation and reflection.  The value mRotation must
    // be toggled accordingly.
    bool mIsRotation;
    void GuaranteeRotation ();

    // Householder reduction to tridiagonal form.
    void Tridiagonal2 ();
    void Tridiagonal3 ();
    void TridiagonalN ();

    // QL algorithm with implicit shifting.  This function is called for
    // tridiagonal matrices.
    bool QLAlgorithm ();

    // Sort eigenvalues from largest to smallest.
    void DecreasingSort ();

    // Sort eigenvalues from smallest to largest.
    void IncreasingSort ();
};

typedef EigenDecomposition<float> EigenDecompositionf;
typedef EigenDecomposition<double> EigenDecompositiond;

}

#endif
