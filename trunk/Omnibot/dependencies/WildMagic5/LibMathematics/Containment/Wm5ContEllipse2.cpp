// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContEllipse2.h"
#include "Wm5ApprGaussPointsFit2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Ellipse2<Real> ContEllipse (int numPoints, const Vector2<Real>* points)
{
    // Fit the points with a Gaussian distribution.  The covariance matrix
    // is M = D[0]*U[0]*U[0]^T+D[1]*U[1]*U[1]^T, where D[0] and D[1] are the
    // eigenvalues and U[0] and U[1] are corresponding unit-length
    // eigenvectors.
    Box2<Real> box = GaussPointsFit2<Real>(numPoints, points);
    Real* diag = box.Extent;
    Vector2<Real>* axis = box.Axis;

    // If either eigenvalue is nonpositive, adjust the D[] values so that
    // we actually build an ellipse.
    int i;
    for (i = 0; i < 2; ++i)
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

    // Grow the ellipse, while retaining its shape determined by the
    // covariance matrix, to enclose all the input points.  The quadratic form
    // that is used for the ellipse construction is
    //
    //   Q(X) = (X-C)^T*M*(X-C)
    //        = (X-C)^T*(U[0]*U[0]^T/D[0]+U[1]*U[1]^T/D[1])*(X-C)
    //        = Dot(U[0],X-C)^2/D[0] + Dot(U[1],X-C)^2/D[1]
    //
    // If the maximum value of Q(X[i]) for all input points is V^2, then a
    // bounding ellipse is Q(X) = V^2 since Q(X[i]) <= V^2 for all i.

    Real maxValue = (Real)0;
    for (i = 0; i < numPoints; ++i)
    {
        Vector2<Real> diff = points[i] - box.Center;
        Real dot[2] = { axis[0].Dot(diff), axis[1].Dot(diff) };
        Real value = dot[0]*dot[0]/diag[0] + dot[1]*dot[1]/diag[1];
        if (value > maxValue)
        {
            maxValue = value;
        }
    }

    // Arrange for quadratic to satisfy Q(X) <= 1.
    for (i = 0; i < 2; ++i)
    {
        diag[i] *= maxValue;
    }

    Ellipse2<Real> ellipse;
    ellipse.Center = box.Center;
    for (i = 0; i < 2; ++i)
    {
        ellipse.Axis[i] = axis[i];
        ellipse.Extent[i] = Math<Real>::Sqrt(diag[i]);
    }
    return ellipse;
}
//----------------------------------------------------------------------------
template <typename Real>
void ProjectEllipse (const Ellipse2<Real>& ellipse, const Line2<Real>& line,
    Real& smin, Real& smax)
{
    // Center of projection interval.
    Real center = line.Direction.Dot(ellipse.Center - line.Origin);

    // Radius of projection interval.
    Real tmp[2] =
    {
        ellipse.Extent[0]*(line.Direction.Dot(ellipse.Axis[0])),
        ellipse.Extent[1]*(line.Direction.Dot(ellipse.Axis[1]))
    };
    Real rSqr = tmp[0]*tmp[0] + tmp[1]*tmp[1];
    Real radius = Math<Real>::Sqrt(rSqr);

    smin = center - radius;
    smax = center + radius;
}
//----------------------------------------------------------------------------
template <typename Real>
const Ellipse2<Real> MergeEllipses (const Ellipse2<Real>& ellipse0,
    const Ellipse2<Real>& ellipse1)
{
    Ellipse2<Real> merge;

    // Compute the average of the input centers.
    merge.Center = ((Real)0.5)*(ellipse0.Center + ellipse1.Center);

    // Bounding ellipse orientation is average of input orientations.
    if (ellipse0.Axis[0].Dot(ellipse1.Axis[0]) >= (Real)0)
    {
        merge.Axis[0] = ((Real)0.5)*(ellipse0.Axis[0] + ellipse1.Axis[0]);
        merge.Axis[0].Normalize();
    }
    else
    {
        merge.Axis[0] = ((Real)0.5)*(ellipse0.Axis[0] - ellipse1.Axis[0]);
        merge.Axis[0].Normalize();
    }
    merge.Axis[1] = -merge.Axis[0].Perp();

    // Project the input ellipses onto the axes obtained by the average
    // of the orientations and that go through the center obtained by the
    // average of the centers.
    for (int i = 0; i < 2; ++i)
    {
        // Projection axis.
        Line2<Real> line(merge.Center, merge.Axis[i]);

        // Project ellipsoids onto the axis.
        Real min0, max0, min1, max1;
        ProjectEllipse(ellipse0, line, min0, max0);
        ProjectEllipse(ellipse1, line, min1, max1);

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
Ellipse2<float> ContEllipse<float> (int, const Vector2<float>*);

template WM5_MATHEMATICS_ITEM
void ProjectEllipse<float> (const Ellipse2<float>&, const Line2<float>&,
    float&, float&);

template WM5_MATHEMATICS_ITEM
const Ellipse2<float> MergeEllipses<float> (const Ellipse2<float>&,
    const Ellipse2<float>&);


template WM5_MATHEMATICS_ITEM
Ellipse2<double> ContEllipse<double> (int, const Vector2<double>*);

template WM5_MATHEMATICS_ITEM
void ProjectEllipse<double> (const Ellipse2<double>&, const Line2<double>&,
    double&, double&);

template WM5_MATHEMATICS_ITEM
const Ellipse2<double> MergeEllipses<double> (const Ellipse2<double>&,
    const Ellipse2<double>&);
//----------------------------------------------------------------------------
}
