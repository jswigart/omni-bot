// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5RAY2_H
#define WM5RAY2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector2.h"

namespace Wm5
{

template <typename Real>
class Ray2
{
public:
    // The ray is represented as P+t*D, where P is the ray origin, D is a
    // unit-length direction vector, and t >= 0.  The user must ensure that D
    // is indeed unit length.

    // Construction and destruction.
    Ray2 ();  // uninitialized
    ~Ray2 ();

    Ray2 (const Vector2<Real>& origin, const Vector2<Real>& direction);

    Vector2<Real> Origin, Direction;
};

#include "Wm5Ray2.inl"

typedef Ray2<float> Ray2f;
typedef Ray2<double> Ray2d;

}

#endif
