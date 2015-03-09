// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprEllipseByArcs2.h"
#include "Wm5ContScribeCircle2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
void ApproximateEllipseByArcs (Real a, Real b, int numArcs,
    Vector2<Real>*& points, Vector2<Real>*& centers, Real*& radii)
{
    // Allocate arrays.
    assertion(numArcs >= 2, "Must specify at least two arcs\n");
    if (numArcs < 2)
    {
        points = 0;
        centers = 0;
        radii = 0;
        return;
    }

    points = new1<Vector2<Real> >(numArcs + 1);
    centers = new1<Vector2<Real> >(numArcs);
    radii = new1<Real>(numArcs);

    // Intermediate ellipse quantities.
    Real a2 = a*a, b2 = b*b, ab = a*b;
    Real invB2mA2 = ((Real)1)/(b2 - a2);

    // End points of ellipse in first quadrant.  Points are generated in
    // counterclockwise order.
    points[0] = Vector2<Real>(a, (Real)0);
    points[numArcs] = Vector2<Real>((Real)0, b);

    // Curvature at end points, store curvature for computing arcs.
    Real curv0 = a/b2;
    Real curv1 = b/a2;

    // Select ellipse points based on curvature properties.
    Real invNumArcs = ((Real)1)/numArcs;
    int i;
    for (i = 1; i < numArcs; ++i)
    {
        // Curvature at new point is weighted average of curvature at ends.
        Real weight1 = i*invNumArcs;
        Real weight0 = (Real)1 - weight1;
        Real curv = weight0*curv0 + weight1*curv1;

        // Compute point having this curvature.
        Real tmp = Math<Real>::Pow(ab/curv, (Real)2/(Real)3);
        points[i][0] = a*Math<Real>::Sqrt(
            Math<Real>::FAbs((tmp - a2)*invB2mA2));
        points[i][1] = b*Math<Real>::Sqrt(
            Math<Real>::FAbs((tmp - b2)*invB2mA2));
    }

    // Compute arc at (a,0).
    Circle2<Real> circle;
    Circumscribe<Real>(Vector2<Real>(points[1][0], -points[1][1]),
        points[0], points[1], circle);
    centers[0] = circle.Center;
    radii[0] = circle.Radius;

    // Compute arc at (0,b).
    int last = numArcs - 1;
    Circumscribe(Vector2<Real>(-points[last][0], points[last][1]),
        points[numArcs], points[last], circle);
    centers[last] = circle.Center;
    radii[last] = circle.Radius;

    // Compute arcs at intermediate points between (a,0) and (0,b).
    int iM, iP;
    for (iM = 0, i = 1, iP = 2; i < last; ++iM, ++i, ++iP)
    {
        Circumscribe<Real>(points[iM], points[i], points[iP], circle);
        centers[i] = circle.Center;
        radii[i] = circle.Radius;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
void ApproximateEllipseByArcs<float> (float, float, int,
    Vector2<float>*&, Vector2<float>*&, float*&);

template WM5_MATHEMATICS_ITEM
void ApproximateEllipseByArcs<double> (double, double, int,
    Vector2<double>*&, Vector2<double>*&, double*&);
//----------------------------------------------------------------------------
}
