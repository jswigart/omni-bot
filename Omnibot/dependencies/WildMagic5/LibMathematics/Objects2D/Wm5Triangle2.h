// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5TRIANGLE2_H
#define WM5TRIANGLE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector2.h"

namespace Wm5
{

template <typename Real>
class Triangle2
{
public:
    // The triangle is represented as an array of three vertices:
    // V0, V1, and V2.

    // Construction and destruction.
    Triangle2 ();  // uninitialized
    ~Triangle2 ();

    Triangle2 (const Vector2<Real>& v0, const Vector2<Real>& v1,
        const Vector2<Real>& v2);

    Triangle2 (const Vector2<Real> vertex[3]);

    // Distance from the point Q to the triangle.  TODO:  Move this
    // to the physics library distance code.
    Real DistanceTo (const Vector2<Real>& q) const;

    Vector2<Real> V[3];
};

#include "Wm5Triangle2.inl"

typedef Triangle2<float> Triangle2f;
typedef Triangle2<double> Triangle2d;

}

#endif
