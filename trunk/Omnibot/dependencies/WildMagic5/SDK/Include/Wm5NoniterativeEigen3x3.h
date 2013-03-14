// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5NONITERATIVEEIGEN3X3_H
#define WM5NONITERATIVEEIGEN3X3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Matrix3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM NoniterativeEigen3x3
{
public:
    // Construction and destruction.  The input matrix A must be symmetric.
    NoniterativeEigen3x3 (const Matrix3<Real>& A);
    ~NoniterativeEigen3x3 ();

    // Get the eigenvalues and eigenvectors.  The eigenvalues are stored in
    // increasing order.
    const Real GetEigenvalue (int i) const;
    const Real* GetEigenvalues () const;
    const Vector3<Real>& GetEigenvector (int i) const;
    const Vector3<Real>* GetEigenvectors () const;

private:
    // Compute the roots of the cubic polynomial.  Double-precision arithmetic
    // is used to minimize the effects due to subtractive cancellation.  The
    // roots are returned in increasing order.
    void ComputeRoots (const Matrix3<Real>& A, double root[3]);

    // Determine if M has positive rank.  The maximum-magnitude entry of M is
    // returned.  The row in which it is contained is also returned.
    bool PositiveRank (Matrix3<Real>& M, Real& maxEntry,
        Vector3<Real>& maxRow) const;

    // Compute the eigenvectors.
    void ComputeVectors (const Matrix3<Real>& A, Vector3<Real>& U2,
        int i0, int i1, int i2);

    Real mEigenvalue[3];
    Vector3<Real> mEigenvector[3];

    // For use by ComputeRoots.
    static const double msInv3;
    static const double msRoot3;
};

typedef NoniterativeEigen3x3<float> NoniterativeEigen3x3f;
typedef NoniterativeEigen3x3<double> NoniterativeEigen3x3d;

}

#endif
