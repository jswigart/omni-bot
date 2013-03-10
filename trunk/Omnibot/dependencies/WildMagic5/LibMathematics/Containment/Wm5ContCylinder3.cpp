// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContCylinder3.h"
#include "Wm5ApprLineFit3.h"
#include "Wm5DistPoint3Line3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Cylinder3<Real> ContCylinder (int numPoints, const Vector3<Real>* points)
{
    Cylinder3<Real> cylinder;

    Line3<Real> line = OrthogonalLineFit3(numPoints, points);

    Real maxRadiusSqr = (Real)0;
    int i;
    for (i = 0; i < numPoints; ++i)
    {
        Real radiusSqr = DistPoint3Line3<Real>(points[i], line).GetSquared();
        if (radiusSqr > maxRadiusSqr)
        {
            maxRadiusSqr = radiusSqr;
        }
    }

    Vector3<Real> diff = points[0] - line.Origin;
    Real wMin = line.Direction.Dot(diff);
    Real wMax = wMin;
    for (i = 1; i < numPoints; ++i)
    {
        diff = points[i] - line.Origin;
        Real w = line.Direction.Dot(diff);
        if (w < wMin)
        {
            wMin = w;
        }
        else if (w > wMax)
        {
            wMax = w;
        }
    }

    cylinder.Axis.Origin = line.Origin +
        (((Real)0.5)*(wMax + wMin))*line.Direction;
    cylinder.Axis.Direction = line.Direction;
    cylinder.Radius = Math<Real>::Sqrt(maxRadiusSqr);
    cylinder.Height = wMax - wMin;

    return cylinder;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
Cylinder3<float> ContCylinder<float> (int, const Vector3<float>*);

template WM5_MATHEMATICS_ITEM
Cylinder3<double> ContCylinder<double> (int, const Vector3<double>*);
//----------------------------------------------------------------------------
}
