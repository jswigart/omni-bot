// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprLineFit3.h"
#include "Wm5EigenDecomposition.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Line3<Real> OrthogonalLineFit3 (int numPoints, const Vector3<Real>* points)
{
    Line3<Real> line(Vector3<Real>::ZERO, Vector3<Real>::ZERO);

    // Compute the mean of the points.
    line.Origin = points[0];
    int i;
    for (i = 1; i < numPoints; i++)
    {
        line.Origin += points[i];
    }
    Real invNumPoints = ((Real)1)/numPoints;
    line.Origin *= invNumPoints;

    // Compute the covariance matrix of the points.
    Real sumXX = (Real)0, sumXY = (Real)0, sumXZ = (Real)0;
    Real sumYY = (Real)0, sumYZ = (Real)0, sumZZ = (Real)0;
    for (i = 0; i < numPoints; i++) 
    {
        Vector3<Real> diff = points[i] - line.Origin;
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

    // Set up the eigensolver.
    EigenDecomposition<Real> esystem(3);
    esystem(0,0) = sumYY+sumZZ;
    esystem(0,1) = -sumXY;
    esystem(0,2) = -sumXZ;
    esystem(1,0) = esystem(0,1);
    esystem(1,1) = sumXX+sumZZ;
    esystem(1,2) = -sumYZ;
    esystem(2,0) = esystem(0,2);
    esystem(2,1) = esystem(1,2);
    esystem(2,2) = sumXX+sumYY;

    // Compute eigenstuff, smallest eigenvalue is in last position.
    esystem.Solve(false);

    // Unit-length direction for best-fit line.
    line.Direction = esystem.GetEigenvector3(2);

    return line;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
Line3<float> OrthogonalLineFit3<float> (int, const Vector3<float>*);

template WM5_MATHEMATICS_ITEM
Line3<double> OrthogonalLineFit3<double> (int, const Vector3<double>*);
//----------------------------------------------------------------------------
}
