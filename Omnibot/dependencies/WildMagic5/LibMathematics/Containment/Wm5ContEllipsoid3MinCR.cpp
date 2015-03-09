// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContEllipsoid3MinCR.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
ContEllipsoid3MinCR<Real>::ContEllipsoid3MinCR (int numPoints,
    const Vector3<Real>* points, const Vector3<Real>& C,
    const Matrix3<Real>& R, Real D[3])
{
    // Compute the constraint coefficients, of the form (A[0],A[1]) for
    // each i.
    std::vector<Vector3<Real> > A(numPoints);
    for (int i = 0; i < numPoints; ++i)
    {
        Vector3<Real> diff = points[i] - C;  // P[i] - C
        Vector3<Real> prod = diff*R;  // R^T*(P[i] - C) = (u,v,w)

        A[i].X() = prod.X()*prod.X();  // u^2
        A[i].Y() = prod.Y()*prod.Y();  // v^2
        A[i].Z() = prod.Z()*prod.Z();  // w^2
    }

    // TODO:  Sort the constraints to eliminate redundant ones.  It is clear
    // how to do this in ContEllipse2MinCR.  How to do this in 3D?

    MaxProduct(A, D);
}
//----------------------------------------------------------------------------
template <typename Real>
void ContEllipsoid3MinCR<Real>::FindEdgeMax (std::vector<Vector3<Real> >& A,
    int& plane0, int& plane1, Real D[3])
{
    // Compute direction to local maximum point on line of intersection.
    Real xDir = A[plane0][1]*A[plane1][2] - A[plane1][1]*A[plane0][2];
    Real yDir = A[plane0][2]*A[plane1][0] - A[plane1][2]*A[plane0][0];
    Real zDir = A[plane0][0]*A[plane1][1] - A[plane1][0]*A[plane0][1];

    // Build quadratic Q'(t) = (d/dt)(x(t)y(t)z(t)) = a0+a1*t+a2*t^2.
    Real a0 = D[0]*D[1]*zDir + D[0]*D[2]*yDir + D[1]*D[2]*xDir;
    Real a1 = ((Real)2)*(D[2]*xDir*yDir + D[1]*xDir*zDir + D[0]*yDir*zDir);
    Real a2 = ((Real)3)*(xDir*yDir*zDir);

    // Find root to Q'(t) = 0 corresponding to maximum.
    Real tFinal;
    if (a2 != (Real)0)
    {
        Real invA2 = ((Real)1)/a2;
        Real discr = a1*a1 - ((Real)4)*a0*a2;
        discr = Math<Real>::Sqrt(Math<Real>::FAbs(discr));
        tFinal = -((Real)0.5)*(a1 + discr)*invA2;
        if (a1 + ((Real)2)*a2*tFinal > (Real)0)
        {
            tFinal = ((Real)0.5)*(-a1 + discr)*invA2;
        }
    }
    else if (a1 != (Real)0)
    {
        tFinal = -a0/a1;
    }
    else if (a0 != (Real)0)
    {
        tFinal =
            (a0 >= (Real)0 ? Math<Real>::MAX_REAL : -Math<Real>::MAX_REAL);
    }
    else
    {
        return;
    }

    if (tFinal < (Real)0)
    {
        // Make (xDir,yDir,zDir) point in direction of increase of Q.
        tFinal = -tFinal;
        xDir = -xDir;
        yDir = -yDir;
        zDir = -zDir;
    }

    // Sort remaining planes along line from current point to local maximum.
    Real tMax = tFinal;
    int plane2 = -1;
    const int numPoints = (int)A.size();
    for (int i = 0; i < numPoints; ++i)
    {
        if (i == plane0 || i == plane1)
        {
            continue;
        }

        Real norDotDir = A[i][0]*xDir + A[i][1]*yDir + A[i][2]*zDir;
        if (norDotDir <= (Real)0)
        {
            continue;
        }

        // Theoretically the numerator must be nonnegative since an invariant
        // in the algorithm is that (x0,y0,z0) is on the convex hull of the
        // constraints.  However, some numerical error may make this a small
        // negative number.  In that case set tmax = 0 (no change in
        // position).
        Real numer = (Real)1 - A[i][0]*D[0] - A[i][1]*D[1] - A[i][2]*D[2];
        if (numer < (Real)0)
        {
            assertion(numer >= -Math<Real>::ZERO_TOLERANCE,
                "Unexpected condition\n");

            plane2 = i;
            tMax = (Real)0;
            break;
        }

        Real t = numer/norDotDir;
        if (0 <= t && t < tMax)
        {
            plane2 = i;
            tMax = t;
        }
    }

    D[0] += tMax*xDir;
    D[1] += tMax*yDir;
    D[2] += tMax*zDir;

    if (tMax == tFinal)
    {
        return;
    }

    if (tMax > Math<Real>::ZERO_TOLERANCE)
    {
        plane0 = plane2;
        FindFacetMax(A, plane0, D);
        return;
    }

    // tmax == 0, so return with D[0], D[1], and D[2] unchanged.
}
//----------------------------------------------------------------------------
template <typename Real>
void ContEllipsoid3MinCR<Real>::FindFacetMax (std::vector<Vector3<Real> >& A,
    int& plane0, Real D[3])
{
    Real tFinal, xDir, yDir, zDir;

    if (A[plane0][0] > Math<Real>::ZERO_TOLERANCE
    &&  A[plane0][1] > Math<Real>::ZERO_TOLERANCE
    &&  A[plane0][2] > Math<Real>::ZERO_TOLERANCE)
    {
        // Compute local maximum point on plane.
        const Real oneThird = (Real)(1.0/3.0);
        Real xMax = oneThird/A[plane0][0];
        Real yMax = oneThird/A[plane0][1];
        Real zMax = oneThird/A[plane0][2];

        // Compute direction to local maximum point on plane.
        tFinal = (Real)1;
        xDir = xMax - D[0];
        yDir = yMax - D[1];
        zDir = zMax - D[2];
    }
    else
    {
        tFinal = Math<Real>::MAX_REAL;

        if (A[plane0][0] > Math<Real>::ZERO_TOLERANCE)
        {
            xDir = (Real)0;
        }
        else
        {
            xDir = (Real)1;
        }

        if (A[plane0][1] > Math<Real>::ZERO_TOLERANCE)
        {
            yDir = (Real)0;
        }
        else
        {
            yDir = (Real)1;
        }

        if (A[plane0][2] > Math<Real>::ZERO_TOLERANCE)
        {
            zDir = (Real)0;
        }
        else
        {
            zDir = (Real)1;
        }
    }
    
    // Sort remaining planes along line from current point.
    Real tMax = tFinal;
    int plane1 = -1;
    const int numPoints = (int)A.size();
    for (int i = 0; i < numPoints; ++i)
    {
        if (i == plane0)
        {
            continue;
        }

        Real norDotDir = A[i][0]*xDir + A[i][1]*yDir + A[i][2]*zDir;
        if (norDotDir <= (Real)0)
        {
            continue;
        }

        // Theoretically the numerator must be nonnegative since an invariant
        // in the algorithm is that (x0,y0,z0) is on the convex hull of the
        // constraints.  However, some numerical error may make this a small
        // negative number.  In that case, set tmax = 0 (no change in
        // position).
        Real numer = (Real)1 - A[i][0]*D[0] - A[i][1]*D[1] - A[i][2]*D[2];
        if (numer < (Real)0)
        {
            assertion(numer >= -Math<Real>::ZERO_TOLERANCE,
                "Unexpected condition\n");

            plane1 = i;
            tMax = (Real)0;
            break;
        }

        Real t = numer/norDotDir;
        if (0 <= t && t < tMax)
        {
            plane1 = i;
            tMax = t;
        }
    }

    D[0] += tMax*xDir;
    D[1] += tMax*yDir;
    D[2] += tMax*zDir;

    if (tMax == (Real)1)
    {
        return;
    }

    if (tMax > Math<Real>::ZERO_TOLERANCE)
    {
        plane0 = plane1;
        FindFacetMax(A, plane0, D);
        return;
    }

    FindEdgeMax(A, plane0, plane1, D);
}
//----------------------------------------------------------------------------
template <typename Real>
void ContEllipsoid3MinCR<Real>::MaxProduct (std::vector<Vector3<Real> >& A,
    Real D[3])
{
    // Maximize x*y*z subject to x >= 0, y >= 0, z >= 0, and
    // A[i]*x+B[i]*y+C[i]*z <= 1 for 0 <= i < N where A[i] >= 0,
    // B[i] >= 0, and C[i] >= 0.

    // Jitter the lines to avoid cases where more than three planes
    // intersect at the same point.  Should also break parallelism
    // and planes parallel to the coordinate planes.
    const Real maxJitter = (Real)1e-12;
    const int numPoints = (int)A.size();
    int i;
    for (i = 0; i < numPoints; ++i)
    {
        A[i][0] += maxJitter*Math<Real>::UnitRandom();
        A[i][1] += maxJitter*Math<Real>::UnitRandom();
        A[i][2] += maxJitter*Math<Real>::UnitRandom();
    }

    // Sort lines along the z-axis (x = 0 and y = 0).
    int plane = -1;
    Real zmax = (Real)0;
    for (i = 0; i < numPoints; ++i)
    {
        if (A[i][2] > zmax)
        {
            zmax = A[i][2];
            plane = i;
        }
    }
    assertion(plane != -1, "Unexpected condition\n");

    // Walk along convex hull searching for maximum.
    D[0] = (Real)0;
    D[1] = (Real)0;
    D[2] = ((Real)1)/zmax;
    FindFacetMax(A, plane, D);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class ContEllipsoid3MinCR<float>;

template WM5_MATHEMATICS_ITEM
class ContEllipsoid3MinCR<double>;
//----------------------------------------------------------------------------
}
