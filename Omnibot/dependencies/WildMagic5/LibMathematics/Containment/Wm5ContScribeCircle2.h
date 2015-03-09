// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONTSCRIBECIRCLE2_H
#define WM5CONTSCRIBECIRCLE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Circle2.h"
#include "Wm5LinearSystem.h"

namespace Wm5
{

// All functions return 'true' if circle has been constructed, 'false'
// otherwise (input points are linearly dependent).

// Circle containing three 2D points.
template <typename Real> WM5_MATHEMATICS_ITEM
bool Circumscribe (const Vector2<Real>& v0, const Vector2<Real>& v1,
    const Vector2<Real>& v2, Circle2<Real>& circle);

// Circle inscribing triangle of three 2D points.
template <typename Real> WM5_MATHEMATICS_ITEM
bool Inscribe (const Vector2<Real>& v0, const Vector2<Real>& v1,
    const Vector2<Real>& v2, Circle2<Real>& circle);

}

#endif
