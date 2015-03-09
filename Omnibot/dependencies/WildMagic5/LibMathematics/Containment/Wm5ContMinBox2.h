// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5MINAREABOX2_H
#define WM5MINAREABOX2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Box2.h"
#include "Wm5Query.h"

namespace Wm5
{
// Compute a minimum-area oriented box containing the specified points.  The
// algorithm uses the rotating calipers method.  If the input points represent
// a counterclockwise-ordered polygon, set 'isConvexPolygon' to 'true';
// otherwise, set 'isConvexPolygon' to 'false'.
//
// This is a function class.  Use it as follows:
//   Box2<Real> minBox = MinBox2(numPoints, points, epsilon, queryType,
//       isConvexPolygon);

template <typename Real>
class WM5_MATHEMATICS_ITEM MinBox2
{
public:
    MinBox2 (int numPoints, const Vector2<Real>* points, Real epsilon,
        Query::Type queryType, bool isConvexPolygon);

    operator Box2<Real> () const;

private:
    // Flags for the rotating calipers algorithm.
    enum { F_NONE, F_LEFT, F_RIGHT, F_BOTTOM, F_TOP };

    void UpdateBox (const Vector2<Real>& LPoint,
        const Vector2<Real>& RPoint, const Vector2<Real>& BPoint,
        const Vector2<Real>& TPoint, const Vector2<Real>& U,
        const Vector2<Real>& V, Real& minAreaDiv4);

    Box2<Real> mMinBox;
};

}

#endif
