// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONTELLIPSOID3MINCR_H
#define WM5CONTELLIPSOID3MINCR_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Matrix3.h"

namespace Wm5
{

// Compute the minimum-volume ellipsoid, (X-C)^T R D R^T (X-C) = 1, given the
// center C and orientation matrix R.  The columns of R are the axes of the
// ellipsoid.  The algorithm computes the diagonal matrix D.  The minimum
// volume is (4*pi/3)/sqrt(D[0]*D[1]*D[2]), where D = diag(D[0],D[1],D[2]).
// The problem is equivalent to maximizing the product D[0]*D[1]*D[2] for a
// given C and R, and subject to the constraints
//   (P[i]-C)^T R D R^T (P[i]-C) <= 1
// for all input points P[i] with 0 <= i < N.  Each constraint has the form
//   A[0]*D[0] + A[1]*D[1] + A[2]*D[2] <= 1
// where A[0] >= 0, A[1] >= 0, and A[2] >= 0.

template <typename Real>
class WM5_MATHEMATICS_ITEM ContEllipsoid3MinCR
{
public:
    ContEllipsoid3MinCR (int numPoints, const Vector3<Real>* points,
        const Vector3<Real>& C, const Matrix3<Real>& R, Real D[3]);

private:
    void FindEdgeMax (std::vector<Vector3<Real> >& A, int& plane0,
        int& plane1, Real D[3]);

    void FindFacetMax (std::vector<Vector3<Real> >& A, int& plane0,
        Real D[3]);

    void MaxProduct (std::vector<Vector3<Real> >& A, Real D[3]);
};

typedef ContEllipsoid3MinCR<float> ContEllipsoid3MinCRf;
typedef ContEllipsoid3MinCR<double> ContEllipsoid3MinCRd;

}

#endif
