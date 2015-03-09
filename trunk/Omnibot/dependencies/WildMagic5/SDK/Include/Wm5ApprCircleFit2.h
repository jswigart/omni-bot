// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5APPRCIRCLEFIT2_H
#define WM5APPRCIRCLEFIT2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Circle2.h"

namespace Wm5
{

// Least-squares fit of a circle to a set of points.  A successful fit is
// indicated by the return value of 'true'.  If the return value is 'false',
// the number of iterations was exceeded.  Try increasing the maximum number
// of iterations.
//
// If initialCenterIsAverage is set to 'true', the initial guess for the
// circle center is the average of the data points.  If the data points are
// clustered along a small arc, CircleFit2 is very slow to converge.  If
// initialCenterIsAverage is set to 'false', the initial guess for the
// circle center is computed using a least-squares estimate of the
// coefficients for a quadratic equation that represents a circle.  This
// approach tends to converge rapidly.

template <typename Real> WM5_MATHEMATICS_ITEM
bool CircleFit2 (int numPoints, const Vector2<Real>* points,
    int maxIterations, Circle2<Real>& circle, bool initialCenterIsAverage);

}

#endif
