// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5CIRCLE2_H
#define WM5CIRCLE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector2.h"

namespace Wm5
{

template <typename Real>
class Circle2
{
public:
    // Construction and destruction.
    Circle2 ();  // uninitialized
    ~Circle2 ();

    Circle2 (const Vector2<Real>& center, Real radius);

    Vector2<Real> Center;
    Real Radius;
};

#include "Wm5Circle2.inl"

typedef Circle2<float> Circle2f;
typedef Circle2<double> Circle2d;

}

#endif
