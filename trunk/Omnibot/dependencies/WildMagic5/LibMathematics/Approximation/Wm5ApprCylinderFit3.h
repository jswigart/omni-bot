// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5APPRCYLINDERFIT3_H
#define WM5APPRCYLINDERFIT3_H

// Least-squares fit of a finite cylinder to (x,y,z) data.  The cylinder has
// center C, unit-length axis direction U, radius r, and height h.  The end
// disks of the cylinder are at C+(h/2)*U and C-(h/2)*U.  A point X is on the
// cylinder wall if (X-C)^T*(I-U*U^T)*(X-C) = r^2.  A point X is on the end
// disk at C+(h/2)*U if Dot(U,X-C) = h/2 and (X-C)^T*(I-U*U^T)*(X-C) <= r^2.
// A point X is on the end disk at C-(h/2)*U if Dot(U,X-C) = -h/2 and
// (X-C)^T*(I-U*U^T)*(X-C) <= r^2.
  
// The inputs are the quantity of points and the point array.  The outputs
// are the center C, unit-length axis direction U, radius R, and height H.
// You can supply initial guesses for C and U.  In this case you need to set
// bInputsAreInitialGuess to 'true'.  Otherwise set it to 'false' and the
// function will select C and U by first fitting the data with a least-squares
// line.  The return function value is the error for the least-squares fit,
// e >= 0.  If all the points lie exactly on a cylinder, then e = 0.
//
// You can examine the error e and iterate the calls yourself.  The outputs
// C, U, R, and H can be fed back into the function call as initial guesses.
//
// Real error0 = CylinderFit3<Real>(numPoints,points,C,U,R,H,false);
// for (i = 1; i <= imax; i++)
// {
//     Real error1 = CylinderFit3<Real>(numPoints,points,C,U,R,H,true);
//     if (eError1 not changed much from error0)
//         break;
// }

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM CylinderFit3
{
public:
    CylinderFit3 (int numPoints, const Vector3<Real>* points,
        Vector3<Real>& center, Vector3<Real>& axis, Real& radius,
        Real& height, bool inputsAreInitialGuess);

    // Return the error value.
    operator Real ();

private:
    static Real UpdateInvRSqr (int numPoints, const Vector3<Real>* points,
        const Vector3<Real>& center, const Vector3<Real>& axis,
        Real& invRadiusSqr);

    static Real UpdateDirection (int numPoints, const Vector3<Real>* points,
        const Vector3<Real>& center, Vector3<Real>& axis,
        Real& invRadiusSqr);

    static Real UpdateCenter (int numPoints, const Vector3<Real>* points,
        Vector3<Real>& center, const Vector3<Real>& axis,
        const Real& invRadiusSqr);

    Real mError;
};

typedef CylinderFit3<float> CylinderFit3f;
typedef CylinderFit3<double> CylinderFit3d;

}

#endif
