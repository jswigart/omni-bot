// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContCapsule3.h"
#include "Wm5ApprLineFit3.h"
#include "Wm5DistPoint3Line3.h"
#include "Wm5DistPoint3Segment3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Capsule3<Real> ContCapsule (int numPoints, const Vector3<Real>* points)
{
    Line3<Real> line = OrthogonalLineFit3<Real>(numPoints, points);

    Real maxRadiusSqr = (Real)0;
    int i;
    for (i = 0; i < numPoints; ++i)
    {
        Real rSqr = DistPoint3Line3<Real>(points[i], line).GetSquared();
        if (rSqr > maxRadiusSqr)
        {
            maxRadiusSqr = rSqr;
        }
    }

    Vector3<Real> U, V, W = line.Direction;
    Vector3<Real>::GenerateComplementBasis(U, V, W);

    Real minValue = Math<Real>::MAX_REAL;
    Real maxValue = -Math<Real>::MAX_REAL;
    for (i = 0; i < numPoints; ++i)
    {
        Vector3<Real> diff = points[i] - line.Origin;
        Real uDotDiff = U.Dot(diff);
        Real vDotDiff = V.Dot(diff);
        Real wDotDiff = W.Dot(diff);
        Real discr = maxRadiusSqr - (uDotDiff*uDotDiff + vDotDiff*vDotDiff);
        Real radical = Math<Real>::Sqrt(Math<Real>::FAbs(discr));

        Real test = wDotDiff + radical;
        if (test < minValue)
        {
            minValue = test;
        }

        test = wDotDiff - radical;
        if (test > maxValue)
        {
            maxValue = test;
        }
    }

    Capsule3<Real> capsule;
    capsule.Radius = Math<Real>::Sqrt(maxRadiusSqr);
    capsule.Segment.Center = line.Origin +
        (((Real)0.5)*(minValue + maxValue))*line.Direction;
    capsule.Segment.Direction = line.Direction;

    if (maxValue > minValue)
    {
        // Container is a capsule.
        capsule.Segment.Extent = ((Real)0.5)*(maxValue - minValue);
    }
    else
    {
        // Container is a sphere.
        capsule.Segment.Extent = (Real)0;
    }

    capsule.Segment.ComputeEndPoints();

    return capsule;
}
//----------------------------------------------------------------------------
template <typename Real>
bool InCapsule (const Vector3<Real>& point, const Capsule3<Real>& capsule)
{
    Real distance = DistPoint3Segment3<Real>(point, capsule.Segment).Get();
    return distance <= capsule.Radius;
}
//----------------------------------------------------------------------------
template <typename Real>
bool InCapsule (const Sphere3<Real>& sphere, const Capsule3<Real>& capsule)
{
    Real rDiff = capsule.Radius - sphere.Radius;
    if (rDiff >= (Real)0)
    {
        Real distance = DistPoint3Segment3<Real>(sphere.Center,
            capsule.Segment).Get();
        return distance <= rDiff;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool InCapsule (const Capsule3<Real>& testCapsule,
    const Capsule3<Real>& capsule)
{
    Sphere3<Real> spherePosEnd(testCapsule.Segment.P1, testCapsule.Radius);
    Sphere3<Real> sphereNegEnd(testCapsule.Segment.P0, testCapsule.Radius);

    return InCapsule<Real>(spherePosEnd, capsule)
        && InCapsule<Real>(sphereNegEnd, capsule);
}
//----------------------------------------------------------------------------
template <typename Real>
Capsule3<Real> MergeCapsules (const Capsule3<Real>& capsule0,
    const Capsule3<Real>& capsule1)
{
    if (InCapsule<Real>(capsule0, capsule1))
    {
        return capsule1;
    }

    if (InCapsule<Real>(capsule1, capsule0))
    {
        return capsule0;
    }

    const Vector3<Real>& P0 = capsule0.Segment.Center;
    const Vector3<Real>& P1 = capsule1.Segment.Center;
    const Vector3<Real>& D0 = capsule0.Segment.Direction;
    const Vector3<Real>& D1 = capsule1.Segment.Direction;

    // Axis of final capsule.
    Line3<Real> line;

    // Axis center is average of input axis centers.
    line.Origin = ((Real)0.5)*(P0 + P1);

    // Axis unit direction is average of input axis unit directions.
    if (D0.Dot(D1) >= (Real)0)
    {
        line.Direction = D0 + D1;
    }
    else
    {
        line.Direction = D0 - D1;
    }
    line.Direction.Normalize();

    // Cylinder with axis 'line' must contain the spheres centered at the
    // endpoints of the input capsules.
    Vector3<Real> posEnd0 = capsule0.Segment.P1;
    Real radius = DistPoint3Line3<Real>(posEnd0, line).Get() +
        capsule0.Radius;

    Vector3<Real> negEnd0 = capsule0.Segment.P0;
    Real tmp = DistPoint3Line3<Real>(negEnd0, line).Get() + capsule0.Radius;
    if (tmp > radius)
    {
        radius = tmp;
    }

    Vector3<Real> posEnd1 = capsule1.Segment.P1;
    tmp = DistPoint3Line3<Real>(posEnd1, line).Get() + capsule1.Radius;
    if (tmp > radius)
    {
        radius = tmp;
    }

    Vector3<Real> negEnd1 = capsule1.Segment.P0;
    tmp = DistPoint3Line3<Real>(negEnd1, line).Get() + capsule1.Radius;
    if (tmp > radius)
    {
        radius = tmp;
    }

    // Process sphere <posEnd0,r0>.
    Real rDiff = radius - capsule0.Radius;
    Real rDiffSqr = rDiff*rDiff;
    Vector3<Real> diff = line.Origin - posEnd0;
    Real k0 = diff.SquaredLength() - rDiffSqr;
    Real k1 = diff.Dot(line.Direction);
    Real discr = k1*k1 - k0;  // assert:  k1*k1-k0 >= 0
    Real root = Math<Real>::Sqrt(Math<Real>::FAbs(discr));
    Real tPos = -k1 - root;
    Real tNeg = -k1 + root;

    // Process sphere <negEnd0,r0>.
    diff = line.Origin - negEnd0;
    k0 = diff.SquaredLength() - rDiffSqr;
    k1 = diff.Dot(line.Direction);
    discr = k1*k1 - k0;  // assert:  k1*k1-k0 >= 0
    root = Math<Real>::Sqrt(Math<Real>::FAbs(discr));
    tmp = -k1 - root;
    if (tmp > tPos)
    {
        tPos = tmp;
    }
    tmp = -k1 + root;
    if (tmp < tNeg)
    {
        tNeg = tmp;
    }

    // Process sphere <posEnd1,r1>.
    rDiff = radius - capsule1.Radius;
    rDiffSqr = rDiff*rDiff;
    diff = line.Origin - posEnd1;
    k0 = diff.SquaredLength() - rDiffSqr;
    k1 = diff.Dot(line.Direction);
    discr = k1*k1 - k0;  // assert:  k1*k1-k0 >= 0
    root = Math<Real>::Sqrt(Math<Real>::FAbs(discr));
    tmp = -k1 - root;
    if (tmp > tPos)
    {
        tPos = tmp;
    }
    tmp = -k1 + root;
    if (tmp < tNeg)
    {
        tNeg = tmp;
    }

    // Process sphere <negEnd1,r1>.
    diff = line.Origin - negEnd1;
    k0 = diff.SquaredLength() - rDiffSqr;
    k1 = diff.Dot(line.Direction);
    discr = k1*k1 - k0;  // assert:  K1*K1-K0 >= 0
    root = Math<Real>::Sqrt(Math<Real>::FAbs(discr));
    tmp = -k1 - root;
    if (tmp > tPos)
    {
        tPos = tmp;
    }
    tmp = -k1 + root;
    if (tmp < tNeg)
    {
        tNeg = tmp;
    }

    Capsule3<Real> capsule;
    capsule.Radius = radius;
    capsule.Segment.Center = line.Origin +
        ((Real)0.5)*(tPos + tNeg)*line.Direction;
    capsule.Segment.Direction = line.Direction;
    capsule.Segment.ComputeEndPoints();

    if (tPos > tNeg)
    {
        // Container is a capsule.
        capsule.Segment.Extent = ((Real)0.5)*(tPos - tNeg);
    }
    else
    {
        // Container is a sphere.
        capsule.Segment.Extent = (Real)0;
    }

    return capsule;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
Capsule3<float> ContCapsule<float> (int, const Vector3<float>*);

template WM5_MATHEMATICS_ITEM
bool InCapsule<float> (const Vector3<float>&, const Capsule3<float>&);

template WM5_MATHEMATICS_ITEM
bool InCapsule<float> (const Sphere3<float>&, const Capsule3<float>&);

template WM5_MATHEMATICS_ITEM
bool InCapsule<float> (const Capsule3<float>&, const Capsule3<float>&);

template WM5_MATHEMATICS_ITEM
Capsule3<float> MergeCapsules<float> (const Capsule3<float>&,
    const Capsule3<float>&);


template WM5_MATHEMATICS_ITEM
Capsule3<double> ContCapsule<double> (int, const Vector3<double>*);

template WM5_MATHEMATICS_ITEM
bool InCapsule<double> (const Vector3<double>&, const Capsule3<double>&);

template WM5_MATHEMATICS_ITEM
bool InCapsule<double> (const Sphere3<double>&, const Capsule3<double>&);

template WM5_MATHEMATICS_ITEM
bool InCapsule<double> (const Capsule3<double>&, const Capsule3<double>&);

template WM5_MATHEMATICS_ITEM
Capsule3<double> MergeCapsules<double> (const Capsule3<double>&,
    const Capsule3<double>&);
//----------------------------------------------------------------------------
}
