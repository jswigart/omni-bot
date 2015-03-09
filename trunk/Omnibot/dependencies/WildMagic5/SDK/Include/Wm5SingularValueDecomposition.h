// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5SINGULARVALUEDECOMPOSITION
#define WM5SINGULARVALUEDECOMPOSITION

#include "Wm5MathematicsLIB.h"
#include "Wm5GMatrix.h"

namespace Wm5
{
template <typename Real>
class WM5_MATHEMATICS_ITEM SingularValueDecomposition
{
public:
    // Singular value decomposition, M = L*D*Transpose(R), where L and R are
    // orthogonal and D is a diagonal matrix whose diagonal entries are
    // nonnegative.  Observe that M is m-by-n with m >= n, L is m-by-m, R is
    // n-by-n, and D is m-by-n; that is, M and D are the same size and not
    // necessarily square.
    SingularValueDecomposition (const GMatrix<Real>& M, GMatrix<Real>& L,
        GMatrix<Real>& D, GMatrix<Real>& RTranspose);

    ~SingularValueDecomposition ();

private:
    // === Matrix Computations Algorithm 5.1.1 (house)
    // Compute V so that V[0] = 1, and (I-2*V*V^T/V^T*V)*X is zero in all
    // but the first component.  Return V.
    static GVector<Real> HouseholderVector (const GVector<Real>& X);

    // === Matrix Computations Algorithm 5.1.2 (row.house)
    // Overwrite A with (I-2*V*V^T/V^T*V)*A.
    static void HouseholderPremultiply (const GVector<Real>& V,
        GMatrix<Real>& A);

    // === Matrix Computations Algorithm 5.1.3 (col.house)
    // Overwrite A with A*(I-2*V*V^T/V^T*V).
    static void HouseholderPostmultiply (const GVector<Real>& V,
        GMatrix<Real>& A);

    // === Matrix Computations Algorithm 5.2.1
    // Factor A = Q*R with Q orthogonal and R upper triangular.
    static void HouseholderQR (const GMatrix<Real>& A,
        GMatrix<Real>& Q, GMatrix<Real>& R);
};

typedef SingularValueDecomposition<float> SingularValueDecompositionf;
typedef SingularValueDecomposition<double> SingularValueDecompositiond;

}

#endif
