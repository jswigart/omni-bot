// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContEllipsoid3.h"
#include "Wm5ApprGaussPointsFit3.h"
#include "Wm5Quaternion.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Ellipsoid3<Real> ContEllipsoid (int numPoints, const Vector3<Real>* points)
{
    // Fit the points with a Gaussian distribution.  The covariance matrix
    // is M = D[0]*U[0]*U[0]^T+D[1]*U[1]*U[1]^T+D[2]*U[2]*U[2]^T where D[0],
    // D[1], and D[2] are the eigenvalues and U[0], U[1], and U[2] are
    // corresponding unit-length eigenvectors.
    Box3<Real> box = GaussPointsFit3(numPoints, points);
    Real* diag = box.Extent;
    Vector3<Real>* axis = box.Axis;

    // If either eigenvalue is nonpositive, adjust the D[] values so that
    // we actually build an ellipse.
    int i;
    for (i = 0; i < 3; ++i)
    {
        if (diag[i] < (Real)0)
        {
            diag[i] = -diag[i];
        }
        if (diag[i] < Math<Real>::ZERO_TOLERANCE)
        {
            diag[i] = Math<Real>::ZERO_TOLERANCE;
        }
    }

    // Grow the ellipsoid, while retaining its shape determined by the
    // covariance matrix, to enclose all the input points.  The quadratic form
    // that is used for the ellipsoid construction is
    //
    //   Q(X) = (X-C)^T*M*(X-C)
    //        = (X-C)^T*(sum_{j=0}^2 U[j]*U[j]^T/D[j])*(X-C)
    //        = sum_{j=0}^2 Dot(U[i],X-C)^2/D[j]
    //
    // If the maximum value of Q(X[i]) for all input points is V^2, then a
    // bounding ellipsoid is Q(X) = V^2 since Q(X[i]) <= V^2 for all i.

    Real maxValue = (Real)0;
    for (i = 0; i < numPoints; ++i)
    {
        Vector3<Real> diff = points[i] - box.Center;
        Real dot[3] =
        {
            axis[0].Dot(diff),
            axis[1].Dot(diff),
            axis[2].Dot(diff)
        };

        Real value = dot[0]*dot[0]/diag[0] + dot[1]*dot[1]/diag[1] +
            dot[2]*dot[2]/diag[2];

        if (value > maxValue)
        {
            maxValue = value;
        }
    }

    // Arrange for quadratic to satisfy Q(X) <= 1.
    for (i = 0; i < 3; ++i)
    {
        diag[i] *= maxValue;
    }

    Ellipsoid3<Real> ellipsoid;
    ellipsoid.Center = box.Center;
    for (i = 0; i < 3; ++i)
    {
        ellipsoid.Axis[i] = axis[i];
        ellipsoid.Extent[i] = Math<Real>::Sqrt(diag[i]);
    }
    return ellipsoid;
}
//----------------------------------------------------------------------------
template <typename Real>
void ProjectEllipsoid (const Ellipsoid3<Real>& ellipsoid,
   const Line3<Real>& line, Real& smin, Real& smax)
{
    // Center of projection interval.
    Real center = line.Direction.Dot(ellipsoid.Center - line.Origin);

    // Radius of projection interval.
    Real tmp[3] =
    {
        ellipsoid.Extent[0]*(line.Direction.Dot(ellipsoid.Axis[0])),
        ellipsoid.Extent[1]*(line.Direction.Dot(ellipsoid.Axis[1])),
        ellipsoid.Extent[2]*(line.Direction.Dot(ellipsoid.Axis[2]))
    };
    Real rSqr = tmp[0]*tmp[0] + tmp[1]*tmp[1] + tmp[2]*tmp[2];
    Real radius = Math<Real>::Sqrt(rSqr);

    smin = center - radius;
    smax = center + radius;
}
//----------------------------------------------------------------------------
template <typename Real>
const Ellipsoid3<Real> MergeEllipsoids (const Ellipsoid3<Real>& ellipsoid0,
    const Ellipsoid3<Real>& ellipsoid1)
{
    Ellipsoid3<Real> merge;

    // compute the average of the input centers
    merge.Center = ((Real)0.5)*(ellipsoid0.Center + ellipsoid1.Center);

    // bounding ellipsoid orientation is average of input orientations
    Quaternion<Real> q0(ellipsoid0.Axis), q1(ellipsoid1.Axis);
    if (q0.Dot(q1) < (Real)0)
    {
        q1 = -q1;
    }
    Quaternion<Real> q = q0 + q1;
    q = Math<Real>::InvSqrt(q.Dot(q))*q;
    q.ToRotationMatrix(merge.Axis);

    // Project the input ellipsoids onto the axes obtained by the average
    // of the orientations and that go through the center obtained by the
    // average of the centers.
    for (int i = 0; i < 3; ++i)
    {
        // Projection axis.
        Line3<Real> line(merge.Center, merge.Axis[i]);

        // Project ellipsoids onto the axis.
        Real min0, max0, min1, max1;
        ProjectEllipsoid(ellipsoid0, line, min0, max0);
        ProjectEllipsoid(ellipsoid1, line, min1, max1);

        // Determine the smallest interval containing the projected
        // intervals.
        Real maxIntr = (max0 >= max1 ? max0 : max1);
        Real minIntr = (min0 <= min1 ? min0 : min1);

        // Update the average center to be the center of the bounding box
        // defined by the projected intervals.
        merge.Center += line.Direction*(((Real)0.5)*(minIntr + maxIntr));

        // Compute the extents of the box based on the new center.
        merge.Extent[i] = ((Real)0.5)*(maxIntr - minIntr);
    }

    return merge;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
Ellipsoid3<float> ContEllipsoid<float> (int, const Vector3<float>*);

template WM5_MATHEMATICS_ITEM
void ProjectEllipsoid<float> (const Ellipsoid3<float>&,
    const Line3<float>&, float&, float&);

template WM5_MATHEMATICS_ITEM
const Ellipsoid3<float> MergeEllipsoids<float> (const Ellipsoid3<float>&,
    const Ellipsoid3<float>&);


template WM5_MATHEMATICS_ITEM
Ellipsoid3<double> ContEllipsoid<double> (int, const Vector3<double>*);

template WM5_MATHEMATICS_ITEM
void ProjectEllipsoid<double> (const Ellipsoid3<double>&,
    const Line3<double>&, double&, double&);

template WM5_MATHEMATICS_ITEM
const Ellipsoid3<double> MergeEllipsoids<double> (const Ellipsoid3<double>&,
    const Ellipsoid3<double>&);
//----------------------------------------------------------------------------
}
