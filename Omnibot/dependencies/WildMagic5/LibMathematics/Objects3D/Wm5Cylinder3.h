// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5CYLINDER3_H
#define WM5CYLINDER3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Line3.h"

namespace Wm5
{

template <typename Real>
class Cylinder3
{
public:
    // Construction and destruction.  The cylinder axis is a line.  The origin
    // of the cylinder is chosen to be the line origin.  The cylinder wall is
    // at a distance R units from the axis.  An infinite cylinder has infinite
    // height.  A finite cylinder has center C at the line origin and has a
    // finite height H.  The segment for the finite cylinder has endpoints
    // C-(H/2)*D and C+(H/2)*D where D is a unit-length direction of the line.
    Cylinder3 ();  // uninitialized
    ~Cylinder3 ();
    Cylinder3 (const Line3<Real>& axis, Real radius, Real height);

    Line3<Real> Axis;
    Real Radius, Height;
};

#include "Wm5Cylinder3.inl"

typedef Cylinder3<float> Cylinder3f;
typedef Cylinder3<double> Cylinder3d;

}

#endif
