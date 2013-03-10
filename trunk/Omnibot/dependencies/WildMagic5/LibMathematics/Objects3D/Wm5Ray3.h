// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5RAY3_H
#define WM5RAY3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class Ray3
{
public:
    // The ray is represented as P+t*D, where P is the ray origin, D is a
    // unit-length direction vector, and t >= 0.  The user must ensure that D
    // is indeed unit length.

    // Construction and destruction.
    Ray3 ();  // uninitialized
    ~Ray3 ();

    Ray3 (const Vector3<Real>& origin, const Vector3<Real>& direction);

    Vector3<Real> Origin, Direction;
};

#include "Wm5Ray3.inl"

typedef Ray3<float> Ray3f;
typedef Ray3<double> Ray3d;

}

#endif
