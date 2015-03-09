// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONTSCRIBECIRCLE3SPHERE3_H
#define WM5CONTSCRIBECIRCLE3SPHERE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Circle3.h"
#include "Wm5Sphere3.h"

namespace Wm5
{

// All functions return 'true' if circle/sphere has been constructed,
// 'false' otherwise (input points are linearly dependent).

// Circle containing three 3D points.
template <typename Real> WM5_MATHEMATICS_ITEM
bool Circumscribe (const Vector3<Real>& v0, const Vector3<Real>& v1,
    const Vector3<Real>& v2, Circle3<Real>& circle);

// Sphere containing four 3D points.
template <typename Real> WM5_MATHEMATICS_ITEM
bool Circumscribe (const Vector3<Real>& v0, const Vector3<Real>& v1,
    const Vector3<Real>& v2, const Vector3<Real>& v3,
    Sphere3<Real>& sphere);

// Circle inscribing triangle of three 3D points.
template <typename Real> WM5_MATHEMATICS_ITEM
bool Inscribe (const Vector3<Real>& v0, const Vector3<Real>& v1,
    const Vector3<Real>& v2, Circle3<Real>& circle);

// Sphere inscribing tetrahedron of four 3D points.
template <typename Real> WM5_MATHEMATICS_ITEM
bool Inscribe (const Vector3<Real>& v0, const Vector3<Real>& v1,
    const Vector3<Real>& v2, const Vector3<Real>& v3,
    Sphere3<Real>& sphere);

}

#endif
