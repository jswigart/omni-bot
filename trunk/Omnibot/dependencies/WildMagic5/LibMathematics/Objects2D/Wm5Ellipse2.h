// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5ELLIPSE2_H
#define WM5ELLIPSE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5EigenDecomposition.h"

namespace Wm5
{

template <typename Real>
class Ellipse2
{
public:
    // An ellipse has center K, axis directions U[0] and U[1] (both
    // unit-length vectors), and extents e[0] and e[1] (both positive
    // numbers).  A point X = K+y[0]*U[0]+y[1]*U[1] is on the ellipse whenever
    // (y[0]/e[0])^2+(y[1]/e[1])^2 = 1.  The test for a point inside the
    // ellipse uses "<=" instead of "=" in the previous expression.  An
    // algebraic representation for the ellipse is
    //   1 = (X-K)^T * (U[0]*U[0]^T/e[0]^2 + U[1]*U[1]^T/e[1]^2) * (X-K)
    //     = (X-K)^T * M * (X-K)
    // where the superscript T denotes transpose.  Observe that U[i]*U[i]^T
    // is a matrix, not a scalar dot product.  The matrix M is symmetric.
    // The ellipse is also represented by a quadratic equation
    //   0 = a0 + a1*x[0] + a2*x[1] + a3*x[0]^2 + a4*x[0]*x[1] + a5*x[1]^2
    //     = a0 + [a1 a2]*X + X^T*[a3   a4/2]*X
    //                            [a4/2 a5  ]
    //     = C + B^T*X + X^T*A*X
    // where X = (x[0],x[1]).  This equation can be factored to the form
    // (X-K)^T*M*(X-K) = 1, where K = -A^{-1}*B/2, M = A/(B^T*A^{-1}*B/4-C).
    // To be an ellipse, M must have all positive eigenvalues.

    // Construction and destruction.
    Ellipse2 ();  // uninitialized
    ~Ellipse2 ();

    Ellipse2 (const Vector2<Real>& center, const Vector2<Real> axis[2],
        const Real extent[2]);

    Ellipse2 (const Vector2<Real>& center, const Vector2<Real>& axis0,
        const Vector2<Real>& axis1, const Real extent0, const Real extent1);

    // Compute M = sum_{i=0}^1 U[i]*U[i]^T/e[i]^2.
    void GetM (Matrix2<Real>& M) const;

    // Compute M^{-1} = sum_{i=0}^1 U[i]*U[i]^T*e[i]^2.
    void GetMInverse (Matrix2<Real>& MInverse) const;

    // Construct the coefficients in the quadratic equation that represents
    // the ellipse.  'coeff' stores a0 through a5.  'A', 'B', and 'C' are as
    // described in the comments before the constructors.
    void ToCoefficients (Real coeff[6]) const;
    void ToCoefficients (Matrix2<Real>& A, Vector2<Real>& B,
        Real& C) const;

    // Construct C, U[i], and e[i] from the quadratic equation.  The return
    // value is 'true' if and only if the input coefficients represent an
    // ellipse.  If the function returns 'false', the ellipse data members
    // are undefined.  'coeff' stores a0 through a5.  'A', 'B', and 'C' are as
    // described in the comments before the constructors.
    bool FromCoefficients (const Real coeff[6]);
    bool FromCoefficients (const Matrix2<Real>& A,
        const Vector2<Real>& B, Real C);

    // Evaluate the quadratic function Q(X) = (X-K)^T * M * (X-K) - 1.
    Real Evaluate (const Vector2<Real>& point) const;

    // Test whether the input point is inside or on the ellipse.  The point
    // is contained when Q(X) <= 0, where Q(X) is the function in the comment
    // before the function Evaluate().
    bool Contains (const Vector2<Real>& point) const;

    Vector2<Real> Center;
    Vector2<Real> Axis[2];
    Real Extent[2];

private:
    static void Convert (const Real coeff[6], Matrix2<Real>& A,
        Vector2<Real>& B, Real& C);

    static void Convert (const Matrix2<Real>& A, const Vector2<Real>& B,
        Real C, Real coeff[6]);
};

#include "Wm5Ellipse2.inl"

typedef Ellipse2<float> Ellipse2f;
typedef Ellipse2<double> Ellipse2d;

}

#endif
