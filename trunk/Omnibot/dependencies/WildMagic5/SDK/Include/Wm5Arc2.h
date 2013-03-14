// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5ARC2_H
#define WM5ARC2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector2.h"

namespace Wm5
{

template <typename Real>
class Arc2
{
public:
    // The arc is defined by two points End0 and End1 on the circle so that
    // End1 is obtained from End0 by traversing counterclockwise.  The
    // application is responsible for ensuring that End0 and End1 are on the
    // circle and that they are properly ordered.

    // Construction and destruction.
    Arc2 ();  // uninitialized
    ~Arc2 ();

    Arc2 (const Vector2<Real>& center, Real radius,
        const Vector2<Real>& end0, const Vector2<Real>& end1);

    // Test whether P is on the arc.  The application must ensure that P is on
    // the circle; that is, |P-C| = R.  This test works for any angle between
    // B-C and A-C, not just those between 0 and pi radians.
    bool Contains (const Vector2<Real>& p) const;

    Vector2<Real> Center;
    Real Radius;
    Vector2<Real> End0, End1;
};

#include "Wm5Arc2.inl"

typedef Arc2<float> Arc2f;
typedef Arc2<double> Arc2d;

}

#endif
