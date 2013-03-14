// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5PERSPPROJELLIPSOID_H
#define WM5PERSPPROJELLIPSOID_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Ellipse2.h"
#include "Wm5Ellipsoid3.h"
#include "Wm5Plane3.h"

namespace Wm5
{

// Input
//   ellipsoid:   specified by center, axis directions, and extents
//   eye point:   E
//   view plane:  Dot(N,X) = D, planar vectors U and V so that {U,V,N} is an
//                orthonormal set of vectors
// The ellipsoid should be between the eye point and the view plane in the
// sense that all rays from the eye point that intersect the ellipsoid must
// also intersect the view plane.
//
// Output
//   plane origin:       P = E + (D - Dot(N,E))*N
//   projected ellipse:  The projected ellipse coordinates Y = (y0,y1) are the
//                       view plane coordinates of the actual 3D ellipse
//                       points X = P + y0*U + y1*V.

template <typename Real> WM5_MATHEMATICS_ITEM
void PerspectiveProjectEllipsoid (const Ellipsoid3<Real>& ellipsoid,
    const Vector3<Real>& eye, const Plane3<Real>& plane,
    const Vector3<Real>& U, const Vector3<Real>& V, Vector3<Real>& P,
    Ellipse2<Real>& ellipse);

}

#endif
