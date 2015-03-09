// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONTELLIPSE2MINCR_H
#define WM5CONTELLIPSE2MINCR_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Matrix2.h"

// Compute the minimum-area ellipse, (X-C)^T R D R^T (X-C) = 1, given the
// center C and the orientation matrix R.  The columns of R are the axes of
// the ellipse.  The algorithm computes the diagonal matrix D.  The minimum
// area is pi/sqrt(D[0]*D[1]), where D = diag(D[0],D[1]).  The problem is
// equivalent to maximizing the product D[0]*D[1] for a given C and R, and
// subject to the constraints
//   (P[i]-C)^T R D R^T (P[i]-C) <= 1
// for all input points P[i] with 0 <= i < N.  Each constraint has the form
//   A[0]*D[0] + A[1]*D[1] <= 1
// where A[0] >= 0 and A[1] >= 0.

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM ContEllipse2MinCR
{
public:
    ContEllipse2MinCR (int numPoints, const Vector2<Real>* points,
        const Vector2<Real>& C, const Matrix2<Real>& R, Real D[2]);

private:
    static bool XGreater (const Vector2<Real>& P0, const Vector2<Real>& P1);
    static bool XEqual (const Vector2<Real>& P0, const Vector2<Real>& P1);
    static bool YGreater (const Vector2<Real>& P0, const Vector2<Real>& P1);
    static bool YEqual (const Vector2<Real>& P0, const Vector2<Real>& P1);
    static void MaxProduct (std::vector<Vector2<Real> >& A, Real D[2]);
};

typedef ContEllipse2MinCR<float> ContEllipse2MinCRf;
typedef ContEllipse2MinCR<double> ContEllipse2MinCRd;

}

#endif
