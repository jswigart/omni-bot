// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5APPRLINEFIT3_H
#define WM5APPRLINEFIT3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Line3.h"

namespace Wm5
{

// Least-squares fit of a line to (x,y,z) data by using distance measurements
// orthogonal to the proposed line.
template <typename Real> WM5_MATHEMATICS_ITEM
Line3<Real> OrthogonalLineFit3 (int numPoints, const Vector3<Real>* points);

}

#endif
