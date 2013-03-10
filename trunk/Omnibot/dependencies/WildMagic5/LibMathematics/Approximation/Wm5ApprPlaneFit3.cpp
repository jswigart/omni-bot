// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprPlaneFit3.h"
#include "Wm5EigenDecomposition.h"
#include "Wm5LinearSystem.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
bool HeightPlaneFit3 (int numPoints, const Vector3<Real>* points, Real& a,
    Real& b, Real& c)
{
    // You need at least three points to determine the plane.  Even so, if
    // the points are on a vertical plane, there is no least-squares fit in
    // the 'height' sense.  This will be trapped by the determinant of the
    // coefficient matrix.

    // Compute sums for linear system.
    Real sumX = (Real)0, sumY = (Real)0, sumZ = (Real)0.0;
    Real sumXX = (Real)0, sumXY = (Real)0, sumXZ = (Real)0;
    Real sumYY = (Real)0, sumYZ = (Real)0;
    int i;
    for (i = 0; i < numPoints; ++i)
    {
        sumX += points[i][0];
        sumY += points[i][1];
        sumZ += points[i][2];
        sumXX += points[i][0]*points[i][0];
        sumXY += points[i][0]*points[i][1];
        sumXZ += points[i][0]*points[i][2];
        sumYY += points[i][1]*points[i][1];
        sumYZ += points[i][1]*points[i][2];
    }

    Real A[3][3] =
    {
        {sumXX, sumXY, sumX},
        {sumXY, sumYY, sumY},
        {sumX,  sumY,  (Real)numPoints}
    };

    Real B[3] =
    {
        sumXZ,
        sumYZ,
        sumZ
    };

    Real X[3];

    bool nonsingular = LinearSystem<Real>().Solve3(A, B, X);
    if (nonsingular)
    {
        a = X[0];
        b = X[1];
        c = X[2];
    }
    else
    {
        a = Math<Real>::MAX_REAL;
        b = Math<Real>::MAX_REAL;
        c = Math<Real>::MAX_REAL;
    }

    return nonsingular;
}
//----------------------------------------------------------------------------
template <typename Real>
Plane3<Real> OrthogonalPlaneFit3 (int numPoints, const Vector3<Real>* points)
{
    // Compute the mean of the points.
    Vector3<Real> origin = Vector3<Real>::ZERO;
    int i;
    for (i = 0; i < numPoints; i++)
    {
        origin += points[i];
    }
    Real invNumPoints = ((Real)1)/numPoints;
    origin *= invNumPoints;

    // compute sums of products
    Real sumXX = (Real)0, sumXY = (Real)0, sumXZ = (Real)0;
    Real sumYY = (Real)0, sumYZ = (Real)0, sumZZ = (Real)0;
    for (i = 0; i < numPoints; ++i) 
    {
        Vector3<Real> diff = points[i] - origin;
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

    // Compute eigenstuff, smallest eigenvalue is in last position.
    esystem.Solve(false);

    // Get plane normal.
    Vector3<Real> normal = esystem.GetEigenvector3(2);

    // The minimum energy.
    return Plane3<Real>(normal, origin);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
bool HeightPlaneFit3<float> (int, const Vector3<float>*, float&, float&,
    float&);

template WM5_MATHEMATICS_ITEM
Plane3<float> OrthogonalPlaneFit3<float> (int, const Vector3<float>*);

template WM5_MATHEMATICS_ITEM
bool HeightPlaneFit3<double> (int, const Vector3<double>*, double&, double&,
    double&);

template WM5_MATHEMATICS_ITEM
Plane3<double> OrthogonalPlaneFit3<double> (int, const Vector3<double>*);
//----------------------------------------------------------------------------
}
