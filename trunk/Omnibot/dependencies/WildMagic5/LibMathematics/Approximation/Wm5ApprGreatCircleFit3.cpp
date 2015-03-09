// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ApprGreatCircleFit3.h"
#include "Wm5EigenDecomposition.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> GreatCircleFit3 (int numPoints, const Vector3<Real>* points)
{
    // Compute the covariance matrix of the vectors.
    Real sumXX = (Real)0, sumXY = (Real)0, sumXZ = (Real)0;
    Real sumYY = (Real)0, sumYZ = (Real)0, sumZZ = (Real)0;
    for (int i = 0; i < numPoints; i++) 
    {
        Vector3<Real> diff = points[i];
        sumXX += diff[0]*diff[0];
        sumXY += diff[0]*diff[1];
        sumXZ += diff[0]*diff[2];
        sumYY += diff[1]*diff[1];
        sumYZ += diff[1]*diff[2];
        sumZZ += diff[2]*diff[2];
    }

    Real invNumPoints = ((Real)1)/numPoints;
    sumXX *= invNumPoints;
    sumXY *= invNumPoints;
    sumXZ *= invNumPoints;
    sumYY *= invNumPoints;
    sumYZ *= invNumPoints;
    sumZZ *= invNumPoints;

    // Set up the eigensolver.
    EigenDecomposition<Real> esystem(3);
    esystem(0,0) = sumXX;
    esystem(0,1) = sumXY;
    esystem(0,2) = sumXZ;
    esystem(1,0) = esystem(0,1);
    esystem(1,1) = sumYY;
    esystem(1,2) = sumYZ;
    esystem(2,0) = esystem(0,2);
    esystem(2,1) = esystem(1,2);
    esystem(2,2) = sumZZ;

    // Compute eigenstuff; the smallest eigenvalue is in last position.
    esystem.Solve(false);

    // Unit-length direction for best-fit great circle.
    Vector3<Real> normal = esystem.GetEigenvector3(2);
    return normal;
}
//----------------------------------------------------------------------------
template <typename Real>
GreatArcFit3<Real>::GreatArcFit3 (int numPoints,
    const Vector3<Real>* points, Vector3<Real>& normal,
    Vector3<Real>& arcEnd0, Vector3<Real>& arcEnd1)
{
    // Get the least-squares great circle for the vectors.  The circle is on
    // the plane Dot(N,X) = 0.
    normal = GreatCircleFit3<Real>(numPoints, points);

    // Compute a coordinate system to allow projection of the vectors onto
    // the great circle.  The coordinates axes have directions U, V, and N.
    Vector3<Real> U, V;
    Vector3<Real>::GenerateComplementBasis(U, V, normal);

    // The vectors are X[i] = u[i]*U + v[i]*V + w[i]*N.  The projections
    // are P[i] = (u[i]*U + v[i]*V)/sqrt(u[i]*u[i] + v[i]*v[i]).  The great
    // circle is parameterized by C(t) = cos(t)*U + sin(t)*V.  Compute the
    // angles t in [-pi,pi] for the projections onto the great circle.  It
    // is not necesarily to normalize (u[i],v[i]), instead computing
    // t = atan2(v[i],u[i]).
    std::vector<Item> items(numPoints);
    int i;
    for (i = 0; i < numPoints; ++i)
    {
        Item& item = items[i];
        item.U = U.Dot(points[i]);
        item.V = V.Dot(points[i]);
        item.Angle = Math<Real>::ATan2(item.V, item.U);
    }
    std::sort(items.begin(), items.end());

    // Locate the pair of consecutive angles whose difference is a maximum.
    // Effectively, we are constructing a cone of minimum angle that contains
    // the unit-length vectors.
    int numPointsM1 = numPoints - 1;
    Real maxDiff = Math<Real>::TWO_PI + items[0].Angle -
        items[numPointsM1].Angle;
    int end0 = 0, end1 = numPointsM1;
    for (int i0 = 0, i1 = 1; i0 < numPointsM1; i0 = i1++)
    {
        Real diff = items[i1].Angle - items[i0].Angle;
        if (diff > maxDiff)
        {
            maxDiff = diff;
            end0 = i1;
            end1 = i0;
        }
    }

    arcEnd0 = items[end0].U*U + items[end0].V*V;
    arcEnd1 = items[end1].U*U + items[end1].V*V;
    arcEnd0.Normalize();
    arcEnd1.Normalize();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// GreatArcFit3::Item
//----------------------------------------------------------------------------
template <typename Real>
bool GreatArcFit3<Real>::Item::operator< (const Item& item) const
{
    return Angle < item.Angle;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
Vector3<float> GreatCircleFit3<float> (int, const Vector3<float>*);

template WM5_MATHEMATICS_ITEM
class GreatArcFit3<float>;

template WM5_MATHEMATICS_ITEM
Vector3<double> GreatCircleFit3<double> (int, const Vector3<double>*);

template WM5_MATHEMATICS_ITEM
class GreatArcFit3<double>;
//----------------------------------------------------------------------------
}
