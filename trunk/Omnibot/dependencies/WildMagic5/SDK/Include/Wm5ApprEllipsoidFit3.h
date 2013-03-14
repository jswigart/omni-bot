// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5APPRELLIPSOIDFIT3_H
#define WM5APPRELLIPSOIDFIT3_H

// The ellipsoid in general form is  X^t A X + B^t X + C = 0 where
// A is a positive definite 3x3 matrix, B is a 3x1 vector, C is a
// scalar, and X is a 3x1 vector.  Completing the square,
// (X-U)^t A (X-U) = U^t A U - C where U = -0.5 A^{-1} B.  Define
// M = A/(U^t A U - C).  The ellipsoid is (X-U)^t M (X-U) = 1.  Factor
// M = R^t D R where R is orthonormal and D is diagonal with positive
// diagonal terms.  The ellipsoid in factored form is
//     (X-U)^t R^t D^t R (X-U) = 1
//
// Find the least squares fit of a set of N points P[0] through P[N-1].
// The error return value is the least-squares energy function at (U,R,D).
//   Real error = EllipseFit3<Real>(numPoints,points,U,R,D);

#include "Wm5MathematicsLIB.h"
#include "Wm5Matrix3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM EllipsoidFit3
{
public:
    EllipsoidFit3 (int numPoints, const Vector3<Real>* points,
        Vector3<Real>& center, Matrix3<Real>& rotate, Real diag[3]);

    // Return the error value.
    operator Real ();

private:
    static void InitialGuess (int numPoints, const Vector3<Real>* points,
        Vector3<Real>& center, Matrix3<Real>& rotate, Real diag[3]);

    static Real Energy (const Real* input, void* userData);

    static void MatrixToAngles (const Matrix3<Real>& rotate, Real* angle);
    static void AnglesToMatrix (const Real* angle, Matrix3<Real>& rotate);

    int mNumPoints;
    const Vector3<Real>* mPoints;
    Vector3<Real>* mTemp;

    Real mError;
};

typedef EllipsoidFit3<float> EllipsoidFit3f;
typedef EllipsoidFit3<double> EllipsoidFit3d;

}

#endif
