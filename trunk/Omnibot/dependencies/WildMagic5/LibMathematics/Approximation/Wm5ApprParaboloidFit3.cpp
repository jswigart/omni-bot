// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprParaboloidFit3.h"
#include "Wm5LinearSystem.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
bool ParaboloidFit3 (int numPoints, const Vector3<Real>* points,
    Real coeff[6])
{
    // Allocate linear system (matrix is zeroed initially).
    GMatrix<Real> mat(6, 6);
    Real rhs[6];
    memset(rhs, 0, 6*sizeof(Real));

    for (int i = 0; i < numPoints; i++)
    {
        Real x2 = points[i][0]*points[i][0];
        Real xy = points[i][0]*points[i][1];
        Real y2 = points[i][1]*points[i][1];
        Real zx = points[i][2]*points[i][0];
        Real zy = points[i][2]*points[i][1];
        Real x3 = points[i][0]*x2;
        Real x2y = x2*points[i][1];
        Real xy2 = points[i][0]*y2;
        Real y3 = points[i][1]*y2;
        Real zx2 = points[i][2]*x2;
        Real zxy = points[i][2]*xy;
        Real zy2 = points[i][2]*y2;
        Real x4 = x2*x2;
        Real x3y = x3*points[i][1];
        Real x2y2 = x2*y2;
        Real xy3 = points[i][0]*y3;
        Real y4 = y2*y2;

        mat[0][0] += x4;
        mat[0][1] += x3y;
        mat[0][2] += x2y2;
        mat[0][3] += x3;
        mat[0][4] += x2y;
        mat[0][5] += x2;
        mat[1][2] += xy3;
        mat[1][4] += xy2;
        mat[1][5] += xy;
        mat[2][2] += y4;
        mat[2][4] += y3;
        mat[2][5] += y2;
        mat[3][3] += x2;
        mat[3][5] += points[i][0];
        mat[4][5] += points[i][1];

        rhs[0] += zx2;
        rhs[1] += zxy;
        rhs[2] += zy2;
        rhs[3] += zx;
        rhs[4] += zy;
        rhs[5] += points[i][2];
    }

    mat[1][0] = mat[0][1];
    mat[1][1] = mat[0][2];
    mat[1][3] = mat[0][4];
    mat[2][0] = mat[0][2];
    mat[2][1] = mat[1][2];
    mat[2][3] = mat[1][4];
    mat[3][0] = mat[0][3];
    mat[3][1] = mat[1][3];
    mat[3][2] = mat[2][3];
    mat[3][4] = mat[1][5];
    mat[4][0] = mat[0][4];
    mat[4][1] = mat[1][4];
    mat[4][2] = mat[2][4];
    mat[4][3] = mat[3][4];
    mat[4][4] = mat[2][5];
    mat[5][0] = mat[0][5];
    mat[5][1] = mat[1][5];
    mat[5][2] = mat[2][5];
    mat[5][3] = mat[3][5];
    mat[5][4] = mat[4][5];
    mat[5][5] = (Real)numPoints;

    return LinearSystem<Real>().Solve(mat, rhs, coeff);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
bool ParaboloidFit3<float> (int, const Vector3<float>*, float[6]);

template WM5_MATHEMATICS_ITEM
bool ParaboloidFit3<double> (int, const Vector3<double>*, double[6]);
//----------------------------------------------------------------------------
}
