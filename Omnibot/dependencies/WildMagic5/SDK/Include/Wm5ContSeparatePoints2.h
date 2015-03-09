// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONTSEPARATEPOINTS2_H
#define WM5CONTSEPARATEPOINTS2_H

// Separate two point sets, if possible, by computing a line for which the
// point sets lie on opposite sides.  The algorithm computes the convex hull
// of the point sets, then uses the method of separating axes to determine if
// the two convex polygons are disjoint.  The convex hull calculation is
// O(n*log(n)).  There is a randomized linear approach that takes O(n), but
// I have not yet implemented it.
//
// The return value of the function is 'true' if and only if there is a
// separation.  If 'true', the returned line is a separating line.

#include "Wm5MathematicsLIB.h"
#include "Wm5Line2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM SeparatePoints2
{
public:
    SeparatePoints2 (int numPoints0, const Vector2<Real>* points0,
        int numPoints1, const Vector2<Real>* points1,
        Line2<Real>& separatingLine);

    // Return the result of the constructor call.  If 'true', the output
    // line 'separatingLine' is valid.
    operator bool ();

private:
    static int OnSameSide (const Vector2<Real>& lineNormal,
        Real lineConstant, int numEdges, const int* edges,
        const Vector2<Real>* points);

    static int WhichSide (const Vector2<Real>& lineNormal,
        Real lineConstant, int numEdges, const int* edges,
        const Vector2<Real>* points);

    bool mSeparated;
};

typedef SeparatePoints2<float> SeparatePoints2f;
typedef SeparatePoints2<double> SeparatePoints2d;

}

#endif
