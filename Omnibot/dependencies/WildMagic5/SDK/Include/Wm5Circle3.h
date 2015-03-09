// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5CIRCLE3_H
#define WM5CIRCLE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class Circle3
{
public:
    // The plane containing the circle is Dot(N,X-C) = 0, where X is any point
    // in the plane.  Vectors U, V, and N form an orthonormal right-handed set
    // (matrix [U V N] is orthonormal and has determinant 1).  The circle
    // within the plane is parameterized by X = C + R*(cos(t)*U + sin(t)*V),
    // where t is an angle in [-pi,pi).

    // Construction and destruction.
    Circle3 ();  // uninitialized
    ~Circle3 ();

    // U is the input 'direction0', V is the input 'direction1, and N is the
    // input 'normal'.
    Circle3 (const Vector3<Real>& center, const Vector3<Real>& direction0,
        const Vector3<Real>& direction1, const Vector3<Real>& normal,
        const Real radius);

    Vector3<Real> Center, Direction0, Direction1, Normal;
    Real Radius;
};

#include "Wm5Circle3.inl"

typedef Circle3<float> Circle3f;
typedef Circle3<double> Circle3d;

}

#endif
