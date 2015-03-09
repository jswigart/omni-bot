// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONTLOZENGE3_H
#define WM5CONTLOZENGE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Lozenge3.h"

namespace Wm5
{

// Compute plane of lozenge rectangle using least-squares fit.  Parallel
// planes are chosen close enough together so that all the data points lie
// between them.  The radius is half the distance between the two planes.
// The half-cylinder and quarter-cylinder side pieces are chosen using a
// method similar to that used for fitting by capsules.
template <typename Real> WM5_MATHEMATICS_ITEM
Lozenge3<Real> ContLozenge (int numPoints, const Vector3<Real>* points);

// Test for containment of a point x by a lozenge.
template <typename Real> WM5_MATHEMATICS_ITEM
bool InLozenge (const Vector3<Real>& point, const Lozenge3<Real>& lozenge);

}

#endif
