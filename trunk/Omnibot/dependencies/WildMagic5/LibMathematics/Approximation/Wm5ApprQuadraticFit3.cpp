// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprQuadraticFit3.h"
#include "Wm5EigenDecomposition.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Real QuadraticFit3 (int numPoints, const Vector3<Real>* points,
    Real coeff[10])
{
    EigenDecomposition<Real> esystem(10);
    int row, col;
    for (row = 0; row < 10; ++row)
    {
        for (col = 0; col < 10; ++col)
        {
            esystem(row,col) = (Real)0;
        }
    }

    for (int i = 0; i < numPoints; ++i)
    {
        Real x = points[i].X();
        Real y = points[i].Y();
        Real z = points[i].Z();
        Real x2 = x*x;
        Real y2 = y*y;
        Real z2 = z*z;
        Real xy = x*y;
        Real xz = x*z;
        Real yz = y*z;
        Real x3 = x*x2;
        Real xy2 = x*y2;
        Real xz2 = x*z2;
        Real x2y = x*xy;
        Real x2z = x*xz;
        Real xyz = x*y*z;
        Real y3 = y*y2;
        Real yz2 = y*z2;
        Real y2z = y*yz;
        Real z3 = z*z2;
        Real fX4 = x*x3;
        Real x2y2 = x*xy2;
        Real x2z2 = x*xz2;
        Real x3y = x*x2y;
        Real x3z = x*x2z;
        Real x2yz = x*xyz;
        Real y4 = y*y3;
        Real y2z2 = y*yz2;
        Real xy3 = x*y3;
        Real xy2z = x*y2z;
        Real y3z = y*y2z;
        Real fZ4 = z*z3;
        Real xyz2 = x*yz2;
        Real xz3 = x*z3;
        Real yz3 = y*z3;

        esystem(0,1) += x;
        esystem(0,2) += y;
        esystem(0,3) += z;
        esystem(0,4) += x2;
        esystem(0,5) += y2;
        esystem(0,6) += z2;
        esystem(0,7) += xy;
        esystem(0,8) += xz;
        esystem(0,9) += yz;
        esystem(1,4) += x3;
        esystem(1,5) += xy2;
        esystem(1,6) += xz2;
        esystem(1,7) += x2y;
        esystem(1,8) += x2z;
        esystem(1,9) += xyz;
        esystem(2,5) += y3;
        esystem(2,6) += yz2;
        esystem(2,9) += y2z;
        esystem(3,6) += z3;
        esystem(4,4) += fX4;
        esystem(4,5) += x2y2;
        esystem(4,6) += x2z2;
        esystem(4,7) += x3y;
        esystem(4,8) += x3z;
        esystem(4,9) += x2yz;
        esystem(5,5) += y4;
        esystem(5,6) += y2z2;
        esystem(5,7) += xy3;
        esystem(5,8) += xy2z;
        esystem(5,9) += y3z;
        esystem(6,6) += fZ4;
        esystem(6,7) += xyz2;
        esystem(6,8) += xz3;
        esystem(6,9) += yz3;
        esystem(9,9) += y2z2;
    }

    esystem(0,0) = (Real)numPoints;
    esystem(1,1) = esystem(0,4);
    esystem(1,2) = esystem(0,7);
    esystem(1,3) = esystem(0,8);
    esystem(2,2) = esystem(0,5);
    esystem(2,3) = esystem(0,9);
    esystem(2,4) = esystem(1,7);
    esystem(2,7) = esystem(1,5);
    esystem(2,8) = esystem(1,9);
    esystem(3,3) = esystem(0,6);
    esystem(3,4) = esystem(1,8);
    esystem(3,5) = esystem(2,9);
    esystem(3,7) = esystem(1,9);
    esystem(3,8) = esystem(1,6);
    esystem(3,9) = esystem(2,6);
    esystem(7,7) = esystem(4,5);
    esystem(7,8) = esystem(4,9);
    esystem(7,9) = esystem(5,8);
    esystem(8,8) = esystem(4,6);
    esystem(8,9) = esystem(6,7);
    esystem(9,9) = esystem(5,6);

    for (row = 0; row < 10; ++row)
    {
        for (col = 0; col < row; ++col)
        {
            esystem(row,col) = esystem(col,row);
        }
    }

    Real invNumPoints = ((Real)1)/(Real)numPoints;
    for (row = 0; row < 10; ++row)
    {
        for (col = 0; col < 10; ++col)
        {
            esystem(row,col) *= invNumPoints;
        }
    }

    esystem.Solve(true);

    GVector<Real> evector = esystem.GetEigenvector(0);
    memcpy(coeff, (Real*)evector, 10*sizeof(Real));

    // For an exact fit, numeric round-off errors might make the minimum
    // eigenvalue just slightly negative.  Return the absolute value since
    // the application might rely on the return value being nonnegative.
    return Math<Real>::FAbs(esystem.GetEigenvalue(0));
}
//----------------------------------------------------------------------------
template <typename Real>
Real QuadraticSphereFit3 (int numPoints, const Vector3<Real>* points,
    Vector3<Real>& center, Real& radius)
{
    EigenDecomposition<Real> esystem(5);
    int row, col;
    for (row = 0; row < 5; ++row)
    {
        for (col = 0; col < 5; ++col)
        {
            esystem(row,col) = (Real)0;
        }
    }

    for (int i = 0; i < numPoints; ++i)
    {
        Real x = points[i].X();
        Real y = points[i].Y();
        Real z = points[i].Z();
        Real x2 = x*x;
        Real y2 = y*y;
        Real z2 = z*z;
        Real xy = x*y;
        Real xz = x*z;
        Real yz = y*z;
        Real r2 = x2+y2+z2;
        Real xr2 = x*r2;
        Real yr2 = y*r2;
        Real zr2 = z*r2;
        Real r4 = r2*r2;

        esystem(0,1) += x;
        esystem(0,2) += y;
        esystem(0,3) += z;
        esystem(0,4) += r2;
        esystem(1,1) += x2;
        esystem(1,2) += xy;
        esystem(1,3) += xz;
        esystem(1,4) += xr2;
        esystem(2,2) += y2;
        esystem(2,3) += yz;
        esystem(2,4) += yr2;
        esystem(3,3) += z2;
        esystem(3,4) += zr2;
        esystem(4,4) += r4;
    }

    esystem(0,0) = (Real)numPoints;

    for (row = 0; row < 5; ++row)
    {
        for (col = 0; col < row; ++col)
        {
            esystem(row,col) = esystem(col,row);
        }
    }

    Real invNumPoints = ((Real)1)/(Real)numPoints;
    for (row = 0; row < 5; ++row)
    {
        for (col = 0; col < 5; ++col)
        {
            esystem(row,col) *= invNumPoints;
        }
    }

    esystem.Solve(true);

    GVector<Real> evector = esystem.GetEigenvector(0);
    Real inv = ((Real)1)/evector[4];  // beware zero divide
    Real coeff[4];
    for (row = 0; row < 4; ++row)
    {
        coeff[row] = inv*evector[row];
    }

    center[0] = -((Real)0.5)*coeff[1];
    center[1] = -((Real)0.5)*coeff[2];
    center[2] = -((Real)0.5)*coeff[3];
    radius = Math<Real>::Sqrt(Math<Real>::FAbs(center[0]*center[0] +
        center[1]*center[1] + center[2]*center[2] - coeff[0]));

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
float QuadraticFit3<float> (int, const Vector3<float>*, float[10]);

template WM5_MATHEMATICS_ITEM
float QuadraticSphereFit3<float> (int, const Vector3<float>*,
    Vector3<float>&, float&);

template WM5_MATHEMATICS_ITEM
double QuadraticFit3<double> (int, const Vector3<double>*, double[10]);

template WM5_MATHEMATICS_ITEM
double QuadraticSphereFit3<double> (int, const Vector3<double>*,
    Vector3<double>&, double&);
//----------------------------------------------------------------------------
}
