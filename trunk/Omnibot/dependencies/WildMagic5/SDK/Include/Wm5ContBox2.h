// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONTBOX2_H
#define WM5CONTBOX2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Box2.h"

namespace Wm5
{

// Compute the smallest axis-aligned bounding box of the points.
template <typename Real> WM5_MATHEMATICS_ITEM
Box2<Real> ContAlignedBox (int numPoints, const Vector2<Real>* points);

// Compute an oriented bounding box of the points.  The box center is the
// average of the points.  The box axes are the eigenvectors of the covariance
// matrix.
template <typename Real> WM5_MATHEMATICS_ITEM
Box2<Real> ContOrientedBox (int numPoints, const Vector2<Real>* points);

// Test for containment.  Let X = C + y0*U0 + y1*U1 where C is the box center
// and U0 and U1 are the orthonormal axes of the box.  X is in the box if
// |y_i| <= E_i for all i where E_i are the extents of the box.
template <typename Real> WM5_MATHEMATICS_ITEM
bool InBox (const Vector2<Real>& point, const Box2<Real>& box);

// Construct an oriented box that contains two other oriented boxes.  The
// result is not guaranteed to be the minimum volume box containing the
// input boxes.
template <typename Real> WM5_MATHEMATICS_ITEM
Box2<Real> MergeBoxes (const Box2<Real>& box0, const Box2<Real>& box1);

}

#endif
