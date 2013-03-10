// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprGaussPointsFit2.h"
#include "Wm5EigenDecomposition.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Box2<Real> GaussPointsFit2 (int numPoints, const Vector2<Real>* points)
{
    Box2<Real> box(Vector2<Real>::ZERO, Vector2<Real>::UNIT_X,
        Vector2<Real>::UNIT_Y, (Real)1, (Real)1);

    // Compute the mean of the points.
    box.Center = points[0];
    int i;
    for (i = 1; i < numPoints; ++i)
    {
        box.Center += points[i];
    }
    Real invNumPoints = ((Real)1)/numPoints;
    box.Center *= invNumPoints;

    // Compute the covariance matrix of the points.
    Real sumXX = (Real)0, sumXY = (Real)0, sumYY = (Real)0;
    for (i = 0; i < numPoints; ++i)
    {
        Vector2<Real> diff = points[i] - box.Center;
        sumXX += diff[0]*diff[0];
        sumXY += diff[0]*diff[1];
        sumYY += diff[1]*diff[1];
    }

    sumXX *= invNumPoints;
    sumXY *= invNumPoints;
    sumYY *= invNumPoints;

    // Setup the eigensolver.
    EigenDecomposition<Real> esystem(2);
    esystem(0,0) = sumXX;
    esystem(0,1) = sumXY;
    esystem(1,0) = sumXY;
    esystem(1,1) = sumYY;
    esystem.Solve(true);

    for (i = 0; i < 2; ++i)
    {
        box.Extent[i] = esystem.GetEigenvalue(i);
        box.Axis[i] = esystem.GetEigenvector2(i);
    }

    return box;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
Box2<float> GaussPointsFit2<float> (int, const Vector2<float>*);

template WM5_MATHEMATICS_ITEM
Box2<double> GaussPointsFit2<double> (int, const Vector2<double>*);
//----------------------------------------------------------------------------
}
