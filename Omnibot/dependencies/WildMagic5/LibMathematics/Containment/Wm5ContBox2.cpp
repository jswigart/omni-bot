// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContBox2.h"
#include "Wm5ApprGaussPointsFit2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Box2<Real> ContAlignedBox (int numPoints, const Vector2<Real>* points)
{
    Vector2<Real> pmin, pmax;
    Vector2<Real>::ComputeExtremes(numPoints, points, pmin, pmax);

    Box2<Real> box;
    box.Center = ((Real)0.5)*(pmin + pmax);
    box.Axis[0] = Vector2<Real>::UNIT_X;
    box.Axis[1] = Vector2<Real>::UNIT_Y;
    Vector2<Real> halfDiagonal = ((Real)0.5)*(pmax - pmin);
    for (int i = 0; i < 2; i++)
    {
        box.Extent[i] = halfDiagonal[i];
    }

    return box;
}
//----------------------------------------------------------------------------
template <typename Real>
Box2<Real> ContOrientedBox (int numPoints, const Vector2<Real>* points)
{
    Box2<Real> box = GaussPointsFit2<Real>(numPoints,points);

    // Let C be the box center and let U0 and U1 be the box axes.  Each
    // input point is of the form X = C + y0*U0 + y1*U1.  The following code
    // computes min(y0), max(y0), min(y1), max(y1), min(y2), and max(y2).
    // The box center is then adjusted to be
    //   C' = C + 0.5*(min(y0)+max(y0))*U0 + 0.5*(min(y1)+max(y1))*U1

    Vector2<Real> diff = points[0] - box.Center;
    Vector2<Real> pmin(diff.Dot(box.Axis[0]), diff.Dot(box.Axis[1]));
    Vector2<Real> pmax = pmin;
    for (int i = 1; i < numPoints; ++i)
    {
        diff = points[i] - box.Center;
        for (int j = 0; j < 2; ++j)
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
        (((Real)0.5)*(pmin[1] + pmax[1]))*box.Axis[1];

    box.Extent[0] = ((Real)0.5)*(pmax[0] - pmin[0]);
    box.Extent[1] = ((Real)0.5)*(pmax[1] - pmin[1]);

    return box;
}
//----------------------------------------------------------------------------
template <typename Real>
bool InBox (const Vector2<Real>& point, const Box2<Real>& box)
{
    Vector2<Real> diff = point - box.Center;
    for (int i = 0; i < 2; ++i)
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
Box2<Real> MergeBoxes (const Box2<Real>& box0, const Box2<Real>& box1)
{
    // Construct a box that contains the input boxes.
    Box2<Real> box;

    // The first guess at the box center.  This value will be updated later
    // after the input box vertices are projected onto axes determined by an
    // average of box axes.
    box.Center = ((Real)0.5)*(box0.Center + box1.Center);

    // The merged box axes are the averages of the input box axes.  The
    // axes of the second box are negated, if necessary, so they form acute
    // angles with the axes of the first box.
    if (box0.Axis[0].Dot(box1.Axis[0]) >= (Real)0)
    {
        box.Axis[0] = ((Real)0.5)*(box0.Axis[0] + box1.Axis[0]);
        box.Axis[0].Normalize();
    }
    else
    {
        box.Axis[0] = ((Real)0.5)*(box0.Axis[0] - box1.Axis[0]);
        box.Axis[0].Normalize();
    }
    box.Axis[1] = -box.Axis[0].Perp();

    // Project the input box vertices onto the merged-box axes.  Each axis
    // D[i] containing the current center C has a minimum projected value
    // min[i] and a maximum projected value max[i].  The corresponding end
    // points on the axes are C+min[i]*D[i] and C+max[i]*D[i].  The point C
    // is not necessarily the midpoint for any of the intervals.  The actual
    // box center will be adjusted from C to a point C' that is the midpoint
    // of each interval,
    //   C' = C + sum_{i=0}^1 0.5*(min[i]+max[i])*D[i]
    // The box extents are
    //   e[i] = 0.5*(max[i]-min[i])

    int i, j;
    Real dot;
    Vector2<Real> vertex[4], diff;
    Vector2<Real> pmin = Vector2<Real>::ZERO;
    Vector2<Real> pmax = Vector2<Real>::ZERO;

    box0.ComputeVertices(vertex);
    for (i = 0; i < 4; ++i)
    {
        diff = vertex[i] - box.Center;
        for (j = 0; j < 2; ++j)
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
    for (i = 0; i < 4; ++i)
    {
        diff = vertex[i] - box.Center;
        for (j = 0; j < 2; ++j)
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
    for (j = 0; j < 2; ++j)
    {
        box.Center += box.Axis[j]*(((Real)0.5)*(pmax[j] + pmin[j]));
        box.Extent[j] = ((Real)0.5)*(pmax[j] - pmin[j]);
    }

    return box;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
Box2<float> ContAlignedBox<float> (int, const Vector2<float>*);

template WM5_MATHEMATICS_ITEM
Box2<float> ContOrientedBox<float> (int, const Vector2<float>*);

template WM5_MATHEMATICS_ITEM
bool InBox<float> (const Vector2<float>&, const Box2<float>&);

template WM5_MATHEMATICS_ITEM
Box2<float> MergeBoxes<float> (const Box2<float>&, const Box2<float>&);


template WM5_MATHEMATICS_ITEM
Box2<double> ContAlignedBox<double> (int, const Vector2<double>*);

template WM5_MATHEMATICS_ITEM
Box2<double> ContOrientedBox<double> (int, const Vector2<double>*);

template WM5_MATHEMATICS_ITEM
bool InBox<double> (const Vector2<double>&, const Box2<double>&);

template WM5_MATHEMATICS_ITEM
Box2<double> MergeBoxes<double> (const Box2<double>&, const Box2<double>&);
//----------------------------------------------------------------------------
}
