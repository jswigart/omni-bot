// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5TRIANGLE3_H
#define WM5TRIANGLE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class Triangle3
{
public:
    // The triangle is represented as an array of three vertices:
    // V0, V1, and V2.

    // Construction and destruction.
    Triangle3 ();  // uninitialized
    ~Triangle3 ();

    Triangle3 (const Vector3<Real>& v0, const Vector3<Real>& v1,
        const Vector3<Real>& v2);

    Triangle3 (const Vector3<Real> vertex[3]);

    // Distance from the point Q to the triangle.  TODO:  Move this
    // to the physics library distance code.
    Real DistanceTo (const Vector3<Real>& q) const;

    Vector3<Real> V[3];
};

#include "Wm5Triangle3.inl"

typedef Triangle3<float> Triangle3f;
typedef Triangle3<double> Triangle3d;

}

#endif
