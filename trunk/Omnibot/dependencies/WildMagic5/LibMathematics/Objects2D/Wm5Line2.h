// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5LINE2_H
#define WM5LINE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector2.h"

namespace Wm5
{

template <typename Real>
class Line2
{
public:
    // The line is represented as P+t*D where P is the line origin, D is a
    // unit-length direction vector, and t is any real number.  The user must
    // ensure that D is indeed unit length.

    // Construction and destruction.
    Line2 ();  // uninitialized
    ~Line2 ();

    Line2 (const Vector2<Real>& origin, const Vector2<Real>& direction);

    Vector2<Real> Origin, Direction;
};

#include "Wm5Line2.inl"

typedef Line2<float> Line2f;
typedef Line2<double> Line2d;

}

#endif
