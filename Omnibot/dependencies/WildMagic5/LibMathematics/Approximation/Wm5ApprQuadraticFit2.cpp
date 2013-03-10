// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprQuadraticFit2.h"
#include "Wm5EigenDecomposition.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Real QuadraticFit2 (int numPoints, const Vector2<Real>* points,
    Real coeff[6])
{
    EigenDecomposition<Real> esystem(6);
    int row, col;
    for (row = 0; row < 6; ++row)
    {
        for (col = 0; col < 6; ++col)
        {
            esystem(row,col) = (Real)0;
        }
    }

    for (int i = 0; i < numPoints; ++i)
    {
        Real x = points[i].X();
        Real y = points[i].Y();
        Real x2 = x*x;
        Real y2 = y*y;
        Real xy = x*y;
        Real x3 = x*x2;
        Real xy2 = x*y2;
        Real x2y = x*xy;
        Real y3 = y*y2;
        Real x4 = x*x3;
        Real x2y2 = x*xy2;
        Real x3y = x*x2y;
        Real y4 = y*y3;
        Real xy3 = x*y3;

        esystem(0,1) += x;
        esystem(0,2) += y;
        esystem(0,3) += x2;
        esystem(0,4) += y2;
        esystem(0,5) += xy;
        esystem(1,3) += x3;
        esystem(1,4) += xy2;
        esystem(1,5) += x2y;
        esystem(2,4) += y3;
        esystem(3,3) += x4;
        esystem(3,4) += x2y2;
        esystem(3,5) += x3y;
        esystem(4,4) += y4;
        esystem(4,5) += xy3;
    }

    esystem(0,0) = (Real)numPoints;
    esystem(1,1) = esystem(0,3);
    esystem(1,2) = esystem(0,5);
    esystem(2,2) = esystem(0,4);
    esystem(2,3) = esystem(1,5);
    esystem(2,5) = esystem(1,4);
    esystem(5,5) = esystem(3,4);

    for (row = 0; row < 6; ++row)
    {
        for (col = 0; col < row; ++col)
        {
            esystem(row,col) = esystem(col,row);
        }
    }

    Real invNumPoints = ((Real)1)/(Real)numPoints;
    for (row = 0; row < 6; ++row)
    {
        for (col = 0; col < 6; ++col)
        {
            esystem(row,col) *= invNumPoints;
        }
    }

    esystem.Solve(true);

    GVector<Real> evector = esystem.GetEigenvector(0);
    memcpy(coeff, (Real*)evector, 6*sizeof(Real));

    // For an exact fit, numeric round-off errors might make the minimum
    // eigenvalue just slightly negative.  Return the absolute value since
    // the application might rely on the return value being nonnegative.
    return Math<Real>::FAbs(esystem.GetEigenvalue(0));
}
//----------------------------------------------------------------------------
template <typename Real>
Real QuadraticCircleFit2 (int numPoints, const Vector2<Real>* points,
    Vector2<Real>& center, Real& radius)
{
    EigenDecomposition<Real> esystem(4);
    int row, col;
    for (row = 0; row < 4; ++row)
    {
        for (col = 0; col < 4; ++col)
        {
            esystem(row,col) = (Real)0;
        }
    }

    for (int i = 0; i < numPoints; ++i)
    {
        Real x = points[i].X();
        Real y = points[i].Y();
        Real x2 = x*x;
        Real y2 = y*y;
        Real xy = x*y;
        Real r2 = x2+y2;
        Real xr2 = x*r2;
        Real yr2 = y*r2;
        Real r4 = r2*r2;

        esystem(0,1) += x;
        esystem(0,2) += y;
        esystem(0,3) += r2;
        esystem(1,1) += x2;
        esystem(1,2) += xy;
        esystem(1,3) += xr2;
        esystem(2,2) += y2;
        esystem(2,3) += yr2;
        esystem(3,3) += r4;
    }

    esystem(0,0) = (Real)numPoints;

    for (row = 0; row < 4; ++row)
    {
        for (col = 0; col < row; ++col)
        {
            esystem(row,col) = esystem(col,row);
        }
    }

    Real invNumPoints = ((Real)1)/(Real)numPoints;
    for (row = 0; row < 4; ++row)
    {
        for (col = 0; col < 4; ++col)
        {
            esystem(row,col) *= invNumPoints;
        }
    }

    esystem.Solve(true);

    GVector<Real> evector = esystem.GetEigenvector(0);
    Real inv = ((Real)1)/evector[3];  // beware zero divide
    Real coeff[3];
    for (row = 0; row < 3; ++row)
    {
        coeff[row] = inv*evector[row];
    }

    center[0] = -((Real)0.5)*coeff[1];
    center[1] = -((Real)0.5)*coeff[2];
    radius = Math<Real>::Sqrt(Math<Real>::FAbs(center[0]*center[0] +
        center[1]*center[1] - coeff[0]));

    // For an exact fit, numeric round-off errors might make the minimum
    // eigenvalue just slightly negative.  Return the absolute value since
    // the application might rely on the return value being nonnegative.
    return Math<Real>::FAbs(esystem.GetEigenvalue(0));
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
float QuadraticFit2<float> (int, const Vector2<float>*, float[6]);

template WM5_MATHEMATICS_ITEM
float QuadraticCircleFit2<float> (int, const Vector2<float>*,
    Vector2<float>&, float&);

template WM5_MATHEMATICS_ITEM
double QuadraticFit2<double> (int, const Vector2<double>*, double[6]);

template WM5_MATHEMATICS_ITEM
double QuadraticCircleFit2<double> (int, const Vector2<double>*,
    Vector2<double>&, double&);
//----------------------------------------------------------------------------
}
