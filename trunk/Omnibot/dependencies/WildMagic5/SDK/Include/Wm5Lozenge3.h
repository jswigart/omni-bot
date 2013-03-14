// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5LOZENGE3_H
#define WM5LOZENGE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Rectangle3.h"

namespace Wm5
{

template <typename Real>
class Lozenge3
{
public:
    // Construction and destruction.  A lozenge is the set of points that are
    // equidistant from a rectangle, the common distance called the radius.
    Lozenge3 ();  // uninitialized
    ~Lozenge3 ();

    Lozenge3 (const Rectangle3<Real>& rectangle, Real radius);

    Rectangle3<Real> Rectangle;
    Real Radius;
};

#include "Wm5Lozenge3.inl"

typedef Lozenge3<float> Lozenge3f;
typedef Lozenge3<double> Lozenge3d;

}

#endif
