// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5APPRQUADRATICFIT3_H
#define WM5APPRQUADRATICFIT3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{

// The quadratic fit is
//
//   0 = C[0] + C[1]*X + C[2]*Y + C[3]*Z + C[4]*X^2 + C[5]*Y^2
//       + C[6]*Z^2 + C[7]*X*Y + C[8]*X*Z + C[9]*Y*Z
//
// subject to Length(C) = 1.  Minimize E(C) = C^t M C with Length(C) = 1
// and M = (sum_i V_i)(sum_i V_i)^t where
//
//   V = (1, X, Y, Z, X^2, Y^2, Z^2, X*Y, X*Z, Y*Z)
//         
// The minimum value is the smallest eigenvalue of M and C is a corresponding
// unit length eigenvector.
//
// Input:
//   n = number of points to fit
//   p[0..n-1] = array of points to fit
//
// Output:
//   c[0..9] = coefficients of quadratic fit (the eigenvector)
//   return value of function is nonnegative and a measure of the fit
//   (the minimum eigenvalue; 0 = exact fit, positive otherwise)

// Canonical forms.  The quadratic equation can be factored into
// P^T A P + B^T P + K = 0 where P = (X,Y,Z), K = C[0], B = (C[1],C[2],C[3]),
// and A is a 3x3 symmetric matrix with A00 = C[4], A11 = C[5], A22 = C[6],
// A01 = C[7]/2, A02 = C[8]/2, and A12 = C[9]/2.  Matrix A = R^T D R where
// R is orthogonal and D is diagonal (using an eigendecomposition).  Define
// V = R P = (v0,v1,v2), E = R B = (e0,e1,e2), D = diag(d0,d1,d2), and f = K
// to obtain
//
//   d0 v0^2 + d1 v1^2 + d2 v^2 + e0 v0 + e1 v1 + e2 v2 + f = 0
//
// The characterization depends on the signs of the d_i.
template <typename Real> WM5_MATHEMATICS_ITEM
Real QuadraticFit3 (int numPoints, const Vector3<Real>* points,
    Real coeff[10]);

// If you think your points are nearly spherical, use this.  Sphere is of form
// C'[0]+C'[1]*X+C'[2]*Y+C'[3]*Z+C'[4]*(X^2+Y^2+Z^2) where Length(C') = 1.
// Function returns C = (C'[0]/C'[4],C'[1]/C'[4],C'[2]/C'[4],C'[3]/C'[4]), so
// fitted sphere is C[0]+C[1]*X+C[2]*Y+C[3]*Z+X^2+Y^2+Z^2.  Center is
// (xc,yc,zc) = -0.5*(C[1],C[2],C[3]) and radius is rad =
// sqrt(xc*xc+yc*yc+zc*zc-C[0]).
template <typename Real> WM5_MATHEMATICS_ITEM
Real QuadraticSphereFit3 (int numPoints, const Vector3<Real>* points,
    Vector3<Real>& center, Real& radius);

}

#endif
