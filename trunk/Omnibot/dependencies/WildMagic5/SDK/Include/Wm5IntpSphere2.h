// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTPSPHERE2_H
#define WM5INTPSPHERE2_H

// Interpolation of a scalar-valued function defined on a sphere.  Although
// the sphere lives in 3D, the interpolation is a 2D method whose input
// points are angles (theta,phi) from spherical coordinates.  The domains of
// the angles are -PI <= theta <= PI and 0 <= phi <= PI.

#include "Wm5MathematicsLIB.h"
#include "Wm5IntpQdrNonuniform2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntpSphere2
{
public:
    // Construction and destruction.  If you want IntpSphere2 to delete the
    // input arrays during destruction, set owner to 'true'.  Otherwise, you
    // own the arrays and must delete them yourself.
    //
    // For complete spherical coverage, include the two antipodal (theta,phi)
    // points (-PI,0,F(-PI,0)) and (-PI,PI,F(-PI,PI)) in the input data.
    // These correspond to the sphere poles x = 0, y = 0, and |z| = 1.
    //
    // The computation type is for the Delaunay triangulation and should be
    // one of Query::{QT_INT64,QT_INTEGER,QT_RATIONAL,QT_REAL}.
    IntpSphere2 (int quantity, Real* theta, Real* phi, Real* F,
        bool owner, Query::Type queryType);

    ~IntpSphere2 ();

    // Spherical coordinates are
    //   x = cos(theta)*sin(phi)
    //   y = sin(theta)*sin(phi)
    //   z = cos(phi)
    // for -PI <= theta <= PI, 0 <= phi <= PI.  The application can use this
    // function to convert unit length vectors (x,y,z) to (theta,phi).
    static void GetSphericalCoords (Real x, Real y, Real z,
        Real& theta, Real& phi);

    bool Evaluate (Real theta, Real phi, Real& F);

private:
    Delaunay2<Real>* mDT;
    IntpQdrNonuniform2<Real>* mInterp;
};

typedef IntpSphere2<float> IntpSphere2f;
typedef IntpSphere2<double> IntpSphere2d;

}

#endif
