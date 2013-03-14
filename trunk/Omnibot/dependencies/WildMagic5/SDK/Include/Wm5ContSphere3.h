// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONTSPHERE3_H
#define WM5CONTSPHERE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Sphere3.h"

namespace Wm5
{

// Compute the smallest axis-aligned bounding box of the points, then
// compute a sphere containing the box.
template <typename Real> WM5_MATHEMATICS_ITEM
Sphere3<Real> ContSphereOfAABB (int numPoints, const Vector3<Real>* points);

// Compute the smallest sphere whose center is the average of the input
// points.
template <typename Real> WM5_MATHEMATICS_ITEM
Sphere3<Real> ContSphereAverage (int numPoints, const Vector3<Real>* points);

// Test for containment of a point inside a sphere.
template <typename Real> WM5_MATHEMATICS_ITEM
bool InSphere (const Vector3<Real>& point, const Sphere3<Real>& sphere);

// Compute the smallest sphere that contains the input spheres.
template <typename Real> WM5_MATHEMATICS_ITEM
Sphere3<Real> MergeSpheres (const Sphere3<Real>& sphere0,
    const Sphere3<Real>& sphere1);

}

#endif
