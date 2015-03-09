// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5TORUS_H
#define WM5TORUS_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class Torus3
{
public:
    // Implementation of torus centered at (0,0,0) with z-axis as the
    // axis of symmetry (axis about which circle is rotated to generate
    // the torus).
    //
    // Algebraic form:
    //      Ro > 0 is radius from center of torus (outer radius)
    //      Ri > 0 is radius of tube of torus (inner radius)
    //      p^2 = x^2 + y^2 + z^2
    //      p^4 - 2*(Ro^2 + Ri^2)*p^2 + 4*Ro^2*z^2 + (Ro^2 - Ri^2)^2 = 0
    //
    // Parametric form:
    //      0 <= s <= 1, 0 <= t <= 1
    //      Rc = Ro + Ri*cos(2*PI*t)
    //      x = Rc*cos(2*PI*s)
    //      y = Rc*sin(2*PI*s)
    //      z = Ri*sin(2*PI*t)

    // Construction and destruction.
    Torus3 ();  // uninitialized
    ~Torus3 ();
    Torus3 (Real outerRadius, Real innerRadius);

    Vector3<Real> GetPosition (Real s, Real t) const;
    Vector3<Real> GetNormal (Real s, Real t) const;
    void GetParameters (const Vector3<Real>& position, Real& s,
        Real& t) const;

    Real OuterRadius, InnerRadius;
};

#include "Wm5Torus3.inl"

typedef Torus3<float> Torus3f;
typedef Torus3<double> Torus3d;

}

#endif
