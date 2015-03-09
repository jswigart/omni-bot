// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5APPRELLIPSEBYARCS2_H
#define WM5APPRELLIPSEBYARCS2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector2.h"

namespace Wm5
{

// The ellipse is (x/a)^2 + (y/b)^2 = 1, but only the portion in the first
// quadrant (x >= 0 and y >= 0) is approximated.  Generate numArcs >= 2 arcs
// by constructing points corresponding to the weighted averages of the
// curvatures at the ellipse points (a,0) and (0,b).  The returned input point
// array has numArcs+1 elements and the returned input center and radius
// arrays each have numArc elements.  The arc associated with points[i] and
// points[i+1] has center centers[i] and radius radii[i].

template <typename Real> WM5_MATHEMATICS_ITEM
void ApproximateEllipseByArcs (Real a, Real b, int numArcs,
    Vector2<Real>*& points, Vector2<Real>*& centers, Real*& radii);

}

#endif
