// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONTSEPARATEPOINTS3_H
#define WM5CONTSEPARATEPOINTS3_H

// Separate two point sets, if possible, by computing a plane for which the
// point sets lie on opposite sides.  The algorithm computes the convex hull
// of the point sets, then uses the method of separating axes to determine if
// the two convex polyhedra are disjoint.  The convex hull calculation is
// O(n*log(n)).  There is a randomized linear approach that takes O(n), but
// I have not yet implemented it.
//
// The return value of the function is 'true' if and only if there is a
// separation.  If 'true', the returned plane is a separating plane.

#include "Wm5MathematicsLIB.h"
#include "Wm5Plane3.h"

namespace Wm5
{

// Assumes that both sets have at least 4 noncoplanar points.
template <typename Real>
class WM5_MATHEMATICS_ITEM SeparatePoints3
{
public:
    SeparatePoints3 (int numPoints0, const Vector3<Real>* points0,
        int numPoints1, const Vector3<Real>* points1,
        Plane3<Real>& separatingPlane);

    // Return the result of the constructor call.  If 'true', the output
    // plane 'separatingPlane' is valid.
    operator bool ();

private:
    static int OnSameSide (const Plane3<Real>& plane, int numTriangles,
        const int* indices, const Vector3<Real>* points);

    static int WhichSide (const Plane3<Real>& plane, int numTriangles,
        const int* indices, const Vector3<Real>* points);

    bool mSeparated;
};

typedef SeparatePoints3<float> SeparatePoints3f;
typedef SeparatePoints3<double> SeparatePoints3d;

}

#endif
