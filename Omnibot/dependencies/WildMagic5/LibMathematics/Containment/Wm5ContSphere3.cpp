// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContSphere3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Sphere3<Real> ContSphereOfAABB (int numPoints, const Vector3<Real>* points)
{
    Vector3<Real> minExtreme, maxExtreme;
    Vector3<Real>::ComputeExtremes(numPoints, points, minExtreme,
        maxExtreme);

    Sphere3<Real> sphere;
    sphere.Center = ((Real)0.5)*(maxExtreme + minExtreme);
    Vector3<Real> halfDiagonal = ((Real)0.5)*(maxExtreme - minExtreme);
    sphere.Radius = halfDiagonal.Length();
    return sphere;
}
//----------------------------------------------------------------------------
template <typename Real>
Sphere3<Real> ContSphereAverage (int numPoints, const Vector3<Real>* points)
{
    Sphere3<Real> sphere;

    sphere.Center = points[0];
    int i;
    for (i = 1; i < numPoints; ++i)
    {
        sphere.Center += points[i];
    }
    sphere.Center /= (Real)numPoints;

    for (i = 0; i < numPoints; ++i)
    {
        Vector3<Real> diff = points[i] - sphere.Center;
        Real radiusSqr = diff.SquaredLength();
        if (radiusSqr > sphere.Radius)
        {
            sphere.Radius = radiusSqr;
        }
    }

    sphere.Radius = Math<Real>::Sqrt(sphere.Radius);
    return sphere;
}
//----------------------------------------------------------------------------
template <typename Real>
bool InSphere (const Vector3<Real>& point, const Sphere3<Real>& sphere)
{
    Vector3<Real> diff = point - sphere.Center;
    return diff.Length() <= sphere.Radius;
}
//----------------------------------------------------------------------------
template <typename Real>
Sphere3<Real> MergeSpheres (const Sphere3<Real>& sphere0,
    const Sphere3<Real>& sphere1)
{
    Vector3<Real> cenDiff = sphere1.Center - sphere0.Center;
    Real lenSqr = cenDiff.SquaredLength();
    Real rDiff = sphere1.Radius - sphere0.Radius;
    Real rDiffSqr = rDiff*rDiff;

    if (rDiffSqr >= lenSqr)
    {
        return (rDiff >= (Real)0 ? sphere1 : sphere0);
    }

    Real length = Math<Real>::Sqrt(lenSqr);
    Sphere3<Real> sphere;

    if (length > Math<Real>::ZERO_TOLERANCE)
    {
        Real coeff = (length + rDiff)/(((Real)2)*length);
        sphere.Center = sphere0.Center + coeff*cenDiff;
    }
    else
    {
        sphere.Center = sphere0.Center;
    }

    sphere.Radius = ((Real)0.5)*(length + sphere0.Radius + sphere1.Radius);

    return sphere;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
Sphere3<float> ContSphereOfAABB<float> (int, const Vector3<float>*);

template WM5_MATHEMATICS_ITEM
Sphere3<float> ContSphereAverage<float> (int, const Vector3<float>*);

template WM5_MATHEMATICS_ITEM
bool InSphere<float> (const Vector3<float>&, const Sphere3<float>&);

template WM5_MATHEMATICS_ITEM
Sphere3<float> MergeSpheres<float> (const Sphere3<float>&,
    const Sphere3<float>&);


template WM5_MATHEMATICS_ITEM
Sphere3<double> ContSphereOfAABB<double> (int, const Vector3<double>*);

template WM5_MATHEMATICS_ITEM
Sphere3<double> ContSphereAverage<double> (int, const Vector3<double>*);

template WM5_MATHEMATICS_ITEM
bool InSphere<double> (const Vector3<double>&, const Sphere3<double>&);

template WM5_MATHEMATICS_ITEM
Sphere3<double> MergeSpheres<double> (const Sphere3<double>&,
    const Sphere3<double>&);
//----------------------------------------------------------------------------
}
