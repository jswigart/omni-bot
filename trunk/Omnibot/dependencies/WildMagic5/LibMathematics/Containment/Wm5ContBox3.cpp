// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContBox3.h"
#include "Wm5ApprGaussPointsFit3.h"
#include "Wm5Quaternion.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Box3<Real> ContAlignedBox (int numPoints, const Vector3<Real>* points)
{
    Vector3<Real> pmin, pmax;
    Vector3<Real>::ComputeExtremes(numPoints, points, pmin, pmax);

    Box3<Real> box;
    box.Center = ((Real)0.5)*(pmin + pmax);
    box.Axis[0] = Vector3<Real>::UNIT_X;
    box.Axis[1] = Vector3<Real>::UNIT_Y;
    box.Axis[2] = Vector3<Real>::UNIT_Z;
    Vector3<Real> halfDiagonal = ((Real)0.5)*(pmax - pmin);
    for (int i = 0; i < 3; ++i)
    {
        box.Extent[i] = halfDiagonal[i];
    }

    return box;
}
//----------------------------------------------------------------------------
template <typename Real>
Box3<Real> ContOrientedBox (int numPoints, const Vector3<Real>* points)
{
    Box3<Real> box = GaussPointsFit3<Real>(numPoints, points);

    // Let C be the box center and let U0, U1, and U2 be the box axes.  Each
    // input point is of the form X = C + y0*U0 + y1*U1 + y2*U2.  The
    // following code computes min(y0), max(y0), min(y1), max(y1), min(y2),
    // and max(y2).  The box center is then adjusted to be
    //   C' = C + 0.5*(min(y0)+max(y0))*U0 + 0.5*(min(y1)+max(y1))*U1 +
    //        0.5*(min(y2)+max(y2))*U2

    Vector3<Real> diff = points[0] - box.Center;
    Vector3<Real> pmin(diff.Dot(box.Axis[0]), diff.Dot(box.Axis[1]),
        diff.Dot(box.Axis[2]));
    Vector3<Real> pmax = pmin;
    for (int i = 1; i < numPoints; ++i)
    {
        diff = points[i] - box.Center;
        for (int j = 0; j < 3; ++j)
        {
            Real dot = diff.Dot(box.Axis[j]);
            if (dot < pmin[j])
            {
                pmin[j] = dot;
            }
            else if (dot > pmax[j])
            {
                pmax[j] = dot;
            }
        }
    }

    box.Center +=
        (((Real)0.5)*(pmin[0] + pmax[0]))*box.Axis[0] +
        (((Real)0.5)*(pmin[1] + pmax[1]))*box.Axis[1] +
        (((Real)0.5)*(pmin[2] + pmax[2]))*box.Axis[2];

    box.Extent[0] = ((Real)0.5)*(pmax[0] - pmin[0]);
    box.Extent[1] = ((Real)0.5)*(pmax[1] - pmin[1]);
    box.Extent[2] = ((Real)0.5)*(pmax[2] - pmin[2]);

    return box;
}
//----------------------------------------------------------------------------
template <typename Real>
bool InBox (const Vector3<Real>& point, const Box3<Real>& box)
{
    Vector3<Real> diff = point - box.Center;
    for (int i = 0; i < 3; ++i)
    {
        Real coeff = diff.Dot(box.Axis[i]);
        if (Math<Real>::FAbs(coeff) > box.Extent[i])
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
Box3<Real> MergeBoxes (const Box3<Real>& box0, const Box3<Real>& box1)
{
    // Construct a box that contains the input boxes.
    Box3<Real> box;

    // The first guess at the box center.  This value will be updated later
    // after the input box vertices are projected onto axes determined by an
    // average of box axes.
    box.Center = ((Real)0.5)*(box0.Center + box1.Center);

    // A box's axes, when viewed as the columns of a matrix, form a rotation
    // matrix.  The input box axes are converted to quaternions.  The average
    // quaternion is computed, then normalized to unit length.  The result is
    // the slerp of the two input quaternions with t-value of 1/2.  The result
    // is converted back to a rotation matrix and its columns are selected as
    // the merged box axes.
    Quaternion<Real> q0, q1;
    q0.FromRotationMatrix(box0.Axis);
    q1.FromRotationMatrix(box1.Axis);
    if (q0.Dot(q1) < (Real)0)
    {
        q1 = -q1;
    }

    Quaternion<Real> q = q0 + q1;
    Real invLength = Math<Real>::InvSqrt(q.Dot(q));
    q = invLength*q;
    q.ToRotationMatrix(box.Axis);

    // Project the input box vertices onto the merged-box axes.  Each axis
    // D[i] containing the current center C has a minimum projected value
    // min[i] and a maximum projected value max[i].  The corresponding end
    // points on the axes are C+min[i]*D[i] and C+max[i]*D[i].  The point C
    // is not necessarily the midpoint for any of the intervals.  The actual
    // box center will be adjusted from C to a point C' that is the midpoint
    // of each interval,
    //   C' = C + sum_{i=0}^2 0.5*(min[i]+max[i])*D[i]
    // The box extents are
    //   e[i] = 0.5*(max[i]-min[i])

    int i, j;
    Real dot;
    Vector3<Real> vertex[8], diff;
    Vector3<Real> pmin = Vector3<Real>::ZERO;
    Vector3<Real> pmax = Vector3<Real>::ZERO;

    box0.ComputeVertices(vertex);
    for (i = 0; i < 8; ++i)
    {
        diff = vertex[i] - box.Center;
        for (j = 0; j < 3; ++j)
        {
            dot = diff.Dot(box.Axis[j]);
            if (dot > pmax[j])
            {
                pmax[j] = dot;
            }
            else if (dot < pmin[j])
            {
                pmin[j] = dot;
            }
        }
    }

    box1.ComputeVertices(vertex);
    for (i = 0; i < 8; ++i)
    {
        diff = vertex[i] - box.Center;
        for (j = 0; j < 3; ++j)
        {
            dot = diff.Dot(box.Axis[j]);
            if (dot > pmax[j])
            {
                pmax[j] = dot;
            }
            else if (dot < pmin[j])
            {
                pmin[j] = dot;
            }
        }
    }

    // [min,max] is the axis-aligned box in the coordinate system of the
    // merged box axes.  Update the current box center to be the center of
    // the new box.  Compute the extents based on the new center.
    for (j = 0; j < 3; ++j)
    {
        box.Center += (((Real)0.5)*(pmax[j] + pmin[j]))*box.Axis[j];
        box.Extent[j] = ((Real)0.5)*(pmax[j] - pmin[j]);
    }

    return box;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
Box3<float> ContAlignedBox<float> (int, const Vector3<float>*);

template WM5_MATHEMATICS_ITEM
Box3<float> ContOrientedBox<float> (int, const Vector3<float>*);

template WM5_MATHEMATICS_ITEM
bool InBox<float> (const Vector3<float>&, const Box3<float>&);

template WM5_MATHEMATICS_ITEM
Box3<float> MergeBoxes<float> (const Box3<float>&, const Box3<float>&);


template WM5_MATHEMATICS_ITEM
Box3<double> ContAlignedBox<double> (int, const Vector3<double>*);

template WM5_MATHEMATICS_ITEM
Box3<double> ContOrientedBox<double> (int, const Vector3<double>*);

template WM5_MATHEMATICS_ITEM
bool InBox<double> (const Vector3<double>&, const Box3<double>&);

template WM5_MATHEMATICS_ITEM
Box3<double> MergeBoxes<double> (const Box3<double>&, const Box3<double>&);
//----------------------------------------------------------------------------
}
