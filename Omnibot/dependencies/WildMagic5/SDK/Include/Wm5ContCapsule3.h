// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONTCAPSULE3_H
#define WM5CONTCAPSULE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Capsule3.h"
#include "Wm5Sphere3.h"

namespace Wm5
{

// Compute axis of capsule segment using least-squares fit.  Radius is
// maximum distance from points to axis.  Hemispherical caps are chosen
// as close together as possible.
template <typename Real> WM5_MATHEMATICS_ITEM
Capsule3<Real> ContCapsule (int numPoints, const Vector3<Real>* points);

// Test for containment of a point by a capsule.
template <typename Real> WM5_MATHEMATICS_ITEM
bool InCapsule (const Vector3<Real>& point, const Capsule3<Real>& capsule);

// Test for containment of a sphere by a capsule.
template <typename Real> WM5_MATHEMATICS_ITEM
bool InCapsule (const Sphere3<Real>& sphere, const Capsule3<Real>& capsule);

// Test for containment of a capsule by a capsule.
template <typename Real> WM5_MATHEMATICS_ITEM
bool InCapsule (const Capsule3<Real>& testCapsule,
    const Capsule3<Real>& capsule);

// Compute a capsule that contains the input capsules.  The returned capsule
// is not necessarily the one of smallest volume that contains the inputs.
template <typename Real> WM5_MATHEMATICS_ITEM
Capsule3<Real> MergeCapsules (const Capsule3<Real>& capsule0,
    const Capsule3<Real>& capsule1);

}

#endif
