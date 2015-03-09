// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprSphereFit3.h"
#include "Wm5ApprQuadraticFit3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
bool SphereFit3 (int numPoints, const Vector3<Real>* points,
    int maxIterations, Sphere3<Real>& sphere,
    bool initialCenterIsAverage)
{
    // Compute the average of the data points.
    Vector3<Real> average = points[0];
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
        sphere.Center = average;
    }
    else
    {
        QuadraticSphereFit3<Real>(numPoints, points, sphere.Center,
            sphere.Radius);
    }

    int i1;
    for (i1 = 0; i1 < maxIterations; ++i1)
    {
        // Update the iterates.
        Vector3<Real> current = sphere.Center;

        // Compute average L, dL/da, dL/db, dL/dc.
        Real lenAverage = (Real)0;
        Vector3<Real> derLenAverage = Vector3<Real>::ZERO;
        for (i0 = 0; i0 < numPoints; i0++)
        {
            Vector3<Real> diff = points[i0] - sphere.Center;
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

        sphere.Center = average + lenAverage*derLenAverage;
        sphere.Radius = lenAverage;

        Vector3<Real> diff = sphere.Center - current;
        if (Math<Real>::FAbs(diff[0]) <= Math<Real>::ZERO_TOLERANCE
        &&  Math<Real>::FAbs(diff[1]) <= Math<Real>::ZERO_TOLERANCE
        &&  Math<Real>::FAbs(diff[2]) <= Math<Real>::ZERO_TOLERANCE)
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
bool SphereFit3<float> (int, const Vector3<float>*, int, Sphere3<float>&,
    bool);

template WM5_MATHEMATICS_ITEM
bool SphereFit3<double> (int, const Vector3<double>*, int, Sphere3<double>&,
    bool);
//----------------------------------------------------------------------------
}
