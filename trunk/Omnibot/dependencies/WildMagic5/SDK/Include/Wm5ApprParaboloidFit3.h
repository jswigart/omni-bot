// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5APPRPARABOLOIDFIT3_H
#define WM5APPRPARABOLOIDFIT3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{

// Least-squares fit of a paraboloid to a set of point.  The paraboloid is
// of the form z = c0*x^2+c1*x*y+c2*y^2+c3*x+c4*y+c5.  A successful fit is
// indicated by return value of 'true'.
//
// Given a set of samples (x_i,y_i,z_i) for 0 <= i < N, and assuming
// that the true values lie on a paraboloid
//     z = p0*x*x + p1*x*y + p2*y*y + p3*x + p4*y + p5  = Dot(P,Q(x,y))
// where P = (p0,p1,p2,p3,p4,p5) and Q(x,y) = (x*x,x*y,y*y,x,y,1),
// select P to minimize the sum of squared errors
//     E(P) = sum_{i=0}^{N-1} [Dot(P,Q_i)-z_i]^2
// where Q_i = Q(x_i,y_i).
//
// The minimum occurs when the gradient of E is the zero vector,
//     grad(E) = 2 sum_{i=0}^{N-1} [Dot(P,Q_i)-z_i] Q_i = 0
// Some algebra converts this to a system of 6 equations in 6 unknowns:
//     [(sum_{i=0}^{N-1} Q_i Q_i^t] P = sum_{i=0}^{N-1} z_i Q_i
// The product Q_i Q_i^t is a product of the 6x1 matrix Q_i with the
// 1x6 matrix Q_i^t, the result being a 6x6 matrix.
//
// Define the 6x6 symmetric matrix A = sum_{i=0}^{N-1} Q_i Q_i^t and the 6x1
// vector B = sum_{i=0}^{N-1} z_i Q_i.  The choice for P is the solution to
// the linear system of equations A*P = B.  The entries of A and B indicate
// summations over the appropriate product of variables.  For example,
// s(x^3 y) = sum_{i=0}^{N-1} x_i^3 y_i.
//
// +-                                                     -++  +   +-      -+
// | s(x^4) s(x^3 y)   s(x^2 y^2) s(x^3)   s(x^2 y) s(x^2) ||p0|   |s(z x^2)|
// |        s(x^2 y^2) s(x y^3)   s(x^2 y) s(x y^2) s(x y) ||p1|   |s(z x y)|
// |                   s(y^4)     s(x y^2) s(y^3)   s(y^2) ||p2| = |s(z y^2)|
// |                              s(x^2)   s(x y)   s(x)   ||p3|   |s(z x)  |
// |                                       s(y^2)   s(y)   ||p4|   |s(z y)  |
// |                                                s(1)   ||p5|   |s(z)    |
// +-                                                     -++  +   +-      -+

template <typename Real> WM5_MATHEMATICS_ITEM
bool ParaboloidFit3 (int numPoints, const Vector3<Real>* points,
    Real coeff[6]);

}

#endif
