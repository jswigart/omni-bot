// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5LINE3_H
#define WM5LINE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class Line3
{
public:
    // The line is represented as P+t*D where P is the line origin, D is a
    // unit-length direction vector, and t is any real number.  The user must
    // ensure that D is indeed unit length.

    // Construction and destruction.
    Line3 ();  // uninitialized
    ~Line3 ();

    Line3 (const Vector3<Real>& origin, const Vector3<Real>& direction);

    Vector3<Real> Origin, Direction;
};

#include "Wm5Line3.inl"

typedef Line3<float> Line3f;
typedef Line3<double> Line3d;

}

#endif
