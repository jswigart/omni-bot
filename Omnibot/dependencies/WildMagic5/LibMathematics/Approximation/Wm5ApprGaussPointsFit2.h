// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5APPRGAUSSPOINTSFIT2_H
#define WM5APPRGAUSSPOINTSFIT2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Box2.h"

namespace Wm5
{

// Fit points with a Gaussian distribution.  The center is the mean of the
// points, the axes are the eigenvectors of the covariance matrix, and the
// extents are the eigenvalues of the covariance matrix and are returned in
// increasing order.  The quantites are stored in a Box2<Real> just to have a
// single container.
template <typename Real> WM5_MATHEMATICS_ITEM
Box2<Real> GaussPointsFit2 (int numPoints, const Vector2<Real>* points);

}

#endif
