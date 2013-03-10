// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5RECTANGLE3_H
#define WM5RECTANGLE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class Rectangle3
{
public:
    // Points are R(s,t) = C+s0*U0+s1*U1, where C is the center of the
    // rectangle, U0 and U1 are unit-length and perpendicular axes.  The
    // parameters s0 and s1 are constrained by |s0| <= e0 and |s1| <= e1,
    // where e0 > 0 and e1 > 0 are called the extents of the rectangle.

    // Construction and destruction.
    Rectangle3 ();  // uninitialized
    ~Rectangle3 ();

    Rectangle3 (const Vector3<Real>& center, const Vector3<Real> axis[2],
        const Real extent[2]);

    Rectangle3 (const Vector3<Real>& center, const Vector3<Real>& axis0,
        const Vector3<Real>& axis1, Real extent0, Real extent1);

    void ComputeVertices (Vector3<Real> vertex[4]) const;

    // Get the rectangle corners.
    Vector3<Real> GetPPCorner () const;  // C + e0*A0 + e1*A1
    Vector3<Real> GetPMCorner () const;  // C + e0*A0 - e1*A1
    Vector3<Real> GetMPCorner () const;  // C - e0*A0 + e1*A1
    Vector3<Real> GetMMCorner () const;  // C - e0*A0 - e1*A1

    Vector3<Real> Center;
    Vector3<Real> Axis[2];
    Real Extent[2];
};

#include "Wm5Rectangle3.inl"

typedef Rectangle3<float> Rectangle3f;
typedef Rectangle3<double> Rectangle3d;

}

#endif
