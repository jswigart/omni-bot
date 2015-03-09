// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONTCYLINDER3_H
#define WM5CONTCYLINDER3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Cylinder3.h"

namespace Wm5
{

// Compute the cylinder axis segment using least-squares fit.  The radius is
// the maximum distance from points to the axis.  The height is determined by
// projection of points onto the axis and determining the containing interval.
template <typename Real> WM5_MATHEMATICS_ITEM
Cylinder3<Real> ContCylinder (int numPoints, const Vector3<Real>* points);

}

#endif
