// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprLineFit2.h"
#include "Wm5EigenDecomposition.h"
#include "Wm5LinearSystem.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
bool HeightLineFit2 (int numPoints, const Vector2<Real>* points, Real& a,
    Real& b)
{
    // You need at least two points to determine the line.  Even so, if
    // the points are on a vertical line, there is no least-squares fit in
    // the 'height' sense.  This will be trapped by the determinant of the
    // coefficient matrix being (nearly) zero.

    // Compute sums for linear system.
    Real sumX = (Real)0, sumY = (Real)0;
    Real sumXX = (Real)0, sumXY = (Real)0;
    int i;
    for (i = 0; i < numPoints; ++i)
    {
        sumX += points[i].X();
        sumY += points[i].Y();
        sumXX += points[i].X()*points[i].X();
        sumXY += points[i].X()*points[i].Y();
    }

    Real A[2][2] =
    {
        {sumXX, sumX},
        {sumX, (Real)numPoints}
    };

    Real B[2] =
    {
        sumXY,
        sumY
    };

    Real X[2];

    bool nonsingular = LinearSystem<Real>().Solve2(A, B, X);
    if (nonsingular)
    {
        a = X[0];
        b = X[1];
    }
    else
    {
        a = Math<Real>::MAX_REAL;
        b = Math<Real>::MAX_REAL;
    }

    return nonsingular;
}
//----------------------------------------------------------------------------
template <typename Real>
Line2<Real> OrthogonalLineFit2 (int numPoints, const Vector2<Real>* points)
{
    Line2<Real> line(Vector2<Real>::ZERO, Vector2<Real>::ZERO);

    // Compute the mean of the points.
    line.Origin = points[0];
    int i;
    for (i = 1; i < numPoints; ++i)
    {
        line.Origin += points[i];
    }
    Real invNumPoints = ((Real)1)/numPoints;
    line.Origin *= invNumPoints;

    // Compute the covariance matrix of the points.
    Real sumXX = (Real)0, sumXY = (Real)0, sumYY = (Real)0;
    for (i = 0; i < numPoints; ++i) 
    {
        Vector2<Real> diff = points[i] - line.Origin;
        sumXX += diff[0]*diff[0];
        sumXY += diff[0]*diff[1];
        sumYY += diff[1]*diff[1];
    }

    sumXX *= invNumPoints;
    sumXY *= invNumPoints;
    sumYY *= invNumPoints;

    // Set up the eigensolver.
    EigenDecomposition<Real> esystem(2);
    esystem(0,0) = sumYY;
    esystem(0,1) = -sumXY;
    esystem(1,0) = -sumXY;
    esystem(1,1) = sumXX;

    // Compute eigenstuff, smallest eigenvalue is in last position.
    esystem.Solve(false);

    // Unit-length direction for best-fit line.
    line.Direction = esystem.GetEigenvector2(1);

    return line;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
bool HeightLineFit2<float> (int, const Vector2<float>*, float&, float&);

template WM5_MATHEMATICS_ITEM
Line2<float> OrthogonalLineFit2<float> (int, const Vector2<float>*);

template WM5_MATHEMATICS_ITEM
bool HeightLineFit2<double> (int, const Vector2<double>*, double&, double&);

template WM5_MATHEMATICS_ITEM
Line2<double> OrthogonalLineFit2<double> (int, const Vector2<double>*);
//----------------------------------------------------------------------------
}
