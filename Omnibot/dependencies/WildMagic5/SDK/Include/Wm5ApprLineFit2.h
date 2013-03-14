// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5APPRLINEFIT2_H
#define WM5APPRLINEFIT2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Line2.h"

namespace Wm5
{

// Least-squares fit of a line to (x,f(x)) data by using distance
// measurements in the y-direction.  The resulting line is represented by
// y = A*x + B.  The return value is 'false' if the 2x2 coefficient matrix
// in the linear system that defines A and B is (nearly) singular.  In this
// case, A and B are returned as MAX_REAL.
template <typename Real> WM5_MATHEMATICS_ITEM
bool HeightLineFit2 (int numPoints, const Vector2<Real>* points,
    Real& coeffA, Real& coeffB);

// Least-squares fit of a line to (x,y,z) data by using distance measurements
// orthogonal to the proposed line.
template <typename Real> WM5_MATHEMATICS_ITEM
Line2<Real> OrthogonalLineFit2 (int numPoints, const Vector2<Real>* points);

}

#endif
