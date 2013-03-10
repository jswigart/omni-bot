// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5CAPSULE3_H
#define WM5CAPSULE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Segment3.h"

namespace Wm5
{

template <typename Real>
class Capsule3
{
public:
    // Construction and destruction.  A capsule is the set of points that are
    // equidistant from a segment, the common distance called the radius.
    Capsule3 ();  // uninitialized
    ~Capsule3 ();

    Capsule3 (const Segment3<Real>& segment, Real radius);

    Segment3<Real> Segment;
    Real Radius;
};

#include "Wm5Capsule3.inl"

typedef Capsule3<float> Capsule3f;
typedef Capsule3<double> Capsule3d;

}

#endif
