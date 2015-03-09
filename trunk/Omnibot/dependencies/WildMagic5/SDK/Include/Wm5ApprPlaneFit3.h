// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5APPRPLANEFIT3_H
#define WM5APPRPLANEFIT3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Plane3.h"

namespace Wm5
{

// Least-squares fit of a plane to (x,y,f(x,y)) data by using distance
// measurements in the z-direction.  The resulting plane is represented by
// z = A*x + B*y + C.  The return value is 'false' if the 3x3 coefficient
// matrix in the linear system that defines A, B, and C is (nearly) singular.
// In this case, A, B, and C are returned as MAX_REAL.
template <typename Real> WM5_MATHEMATICS_ITEM
bool HeightPlaneFit3 (int numPoints, const Vector3<Real>* points,
    Real& a, Real& b, Real& c);

// Least-squares fit of a plane to (x,y,z) data by using distance measurements
// orthogonal to the proposed plane.
template <typename Real> WM5_MATHEMATICS_ITEM
Plane3<Real> OrthogonalPlaneFit3 (int numPoints,
    const Vector3<Real>* points);

}

#endif
