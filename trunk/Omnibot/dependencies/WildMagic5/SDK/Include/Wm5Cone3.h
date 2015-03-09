// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5CONE3_H
#define WM5CONE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class Cone3
{
public:
    // An acute cone is Dot(A,X-V) = |X-V| cos(t) where V is the vertex, A
    // is the unit-length direction of the axis of the cone, and T is the
    // cone angle with 0 < t < pi/2.  The cone interior is defined by the
    // inequality Dot(A,X-V) >= |X-V| cos(t).  Since cos(t) > 0, we can avoid
    // computing square roots.  The solid cone is defined by the inequality
    // Dot(A,X-V)^2 >= Dot(X-V,X-V) cos(t)^2.  This is an infinite,
    // single-sided cone.
    //
    // The cone may be truncated by a plane perpendicular to its axis at a
    // height h from the vertex (distance from the vertex to the intersection
    // of the plane and the axis).  The infinite cone has h = infinity.  The
    // finite cone has a disk of intersection between the plane and infinite
    // cone.  The radius r of the disk is r = h*tan(t).

    // Construction and destruction.
    Cone3 ();  // uninitialized
    ~Cone3 ();

    // The axis must be unit-length and the angle must be in (0,pi/2).  For
    // an infinite cone, set 'height' to Math<Real>::MAX_FLOAT.
    Cone3 (const Vector3<Real>& vertex, const Vector3<Real>& axis,
        const Real angle, const Real height);

    // The axis must be unit-length.  The pair (cosAngle,sinAngle) must be
    // strictly in the first quadrant of the plane to guarantee that the
    // cone angle is in (0,pi/2).  For an infinite cone, set 'height' to
    // Math<Real>::MAX_FLOAT.
    Cone3 (const Vector3<Real>& vertex, const Vector3<Real>& axis,
        const Real cosAngle, const Real sinAngle, const Real height);

    Vector3<Real> Vertex, Axis;
    Real CosAngle, SinAngle, Height;
};

#include "Wm5Cone3.inl"

typedef Cone3<float> Cone3f;
typedef Cone3<double> Cone3d;

}

#endif
