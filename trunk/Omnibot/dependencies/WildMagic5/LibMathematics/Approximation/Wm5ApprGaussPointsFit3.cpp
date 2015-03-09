// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprGaussPointsFit3.h"
#include "Wm5EigenDecomposition.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Box3<Real> GaussPointsFit3 (int numPoints, const Vector3<Real>* points)
{
    Box3<Real> box(Vector3<Real>::ZERO,Vector3<Real>::UNIT_X,
        Vector3<Real>::UNIT_Y,Vector3<Real>::UNIT_Z,(Real)1.0,(Real)1.0,
        (Real)1.0);

    // Compute the mean of the points.
    box.Center = points[0];
    int i;
    for (i = 1; i < numPoints; ++i)
    {
        box.Center += points[i];
    }
    Real invNumPoints = ((Real)1.0)/numPoints;
    box.Center *= invNumPoints;

    // Compute the covariance matrix of the points.
    Real sumXX = (Real)0, sumXY = (Real)0, sumXZ = (Real)0;
    Real sumYY = (Real)0, sumYZ = (Real)0, sumZZ = (Real)0;
    for (i = 0; i < numPoints; ++i)
    {
        Vector3<Real> diff = points[i] - box.Center;
        sumXX += diff[0]*diff[0];
        sumXY += diff[0]*diff[1];
        sumXZ += diff[0]*diff[2];
        sumYY += diff[1]*diff[1];
        sumYZ += diff[1]*diff[2];
        sumZZ += diff[2]*diff[2];
    }

    sumXX *= invNumPoints;
    sumXY *= invNumPoints;
    sumXZ *= invNumPoints;
    sumYY *= invNumPoints;
    sumYZ *= invNumPoints;
    sumZZ *= invNumPoints;

    // Setup the eigensolver.
    EigenDecomposition<Real> esystem(3);
    esystem(0,0) = sumXX;
    esystem(0,1) = sumXY;
    esystem(0,2) = sumXZ;
    esystem(1,0) = sumXY;
    esystem(1,1) = sumYY;
    esystem(1,2) = sumYZ;
    esystem(2,0) = sumXZ;
    esystem(2,1) = sumYZ;
    esystem(2,2) = sumZZ;
    esystem.Solve(true);

    for (i = 0; i < 3; ++i)
    {
        box.Extent[i] = esystem.GetEigenvalue(i);
        box.Axis[i] = esystem.GetEigenvector3(i);
    }

    return box;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
Box3<float> GaussPointsFit3<float> (int, const Vector3<float>*);

template WM5_MATHEMATICS_ITEM
Box3<double> GaussPointsFit3<double> (int, const Vector3<double>*);
//----------------------------------------------------------------------------
}
