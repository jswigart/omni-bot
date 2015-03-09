// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprCircleFit2.h"
#include "Wm5ApprQuadraticFit2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
bool CircleFit2 (int numPoints, const Vector2<Real>* points,
    int maxIterations, Circle2<Real>& circle, bool initialCenterIsAverage)
{
    // Compute the average of the data points.
    Vector2<Real> average = points[0];
    int i0;
    for (i0 = 1; i0 < numPoints; ++i0)
    {
        average += points[i0];
    }
    Real invNumPoints = ((Real)1)/(Real)numPoints;
    average *= invNumPoints;

    // The initial guess for the center.
    if (initialCenterIsAverage)
    {
        circle.Center = average;
    }
    else
    {
        QuadraticCircleFit2<Real>(numPoints, points, circle.Center,
            circle.Radius);
    }

    int i1;
    for (i1 = 0; i1 < maxIterations; ++i1)
    {
        // Update the iterates.
        Vector2<Real> current = circle.Center;

        // Compute average L, dL/da, dL/db.
        Real lenAverage = (Real)0;
        Vector2<Real> derLenAverage = Vector2<Real>::ZERO;
        for (i0 = 0; i0 < numPoints; ++i0)
        {
            Vector2<Real> diff = points[i0] - circle.Center;
            Real length = diff.Length();
            if (length > Math<Real>::ZERO_TOLERANCE)
            {
                lenAverage += length;
                Real invLength = ((Real)1)/length;
                derLenAverage -= invLength*diff;
            }
        }
        lenAverage *= invNumPoints;
        derLenAverage *= invNumPoints;

        circle.Center = average + lenAverage*derLenAverage;
        circle.Radius = lenAverage;

        Vector2<Real> diff = circle.Center - current;
        if (Math<Real>::FAbs(diff[0]) <= Math<Real>::ZERO_TOLERANCE
        &&  Math<Real>::FAbs(diff[1]) <= Math<Real>::ZERO_TOLERANCE)
        {
            break;
        }
    }

    return i1 < maxIterations;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
bool CircleFit2<float> (int, const Vector2<float>*, int, Circle2<float>&,
    bool);

template WM5_MATHEMATICS_ITEM
bool CircleFit2<double> (int, const Vector2<double>*, int, Circle2<double>&,
    bool);
//----------------------------------------------------------------------------
}
