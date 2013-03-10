// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrCircle3Plane3.h"
#include "Wm5IntrPlane3Plane3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrCircle3Plane3<Real>::IntrCircle3Plane3 (const Circle3<Real>& circle,
    const Plane3<Real>& plane)
    :
    mCircle(&circle),
    mPlane(&plane)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Circle3<Real>& IntrCircle3Plane3<Real>::GetCircle () const
{
    return *mCircle;
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrCircle3Plane3<Real>::GetPlane () const
{
    return *mPlane;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrCircle3Plane3<Real>::Test ()
{
    mQuantity = 0;

    // Construct the plane of the circle.
    Plane3<Real> CPlane(mCircle->Normal,mCircle->Center);

    // Compute the intersection of this plane with the input plane.
    IntrPlane3Plane3<Real> intr(*mPlane,CPlane);
    if (!intr.Find())
    {
        // Planes are parallel and nonintersecting.
        mIntersectionType = IT_EMPTY;
        return false;
    }

    if (intr.GetIntersectionType() == IT_PLANE)
    {
        // Planes are the same, the circle is the common intersection set.
        mIntersectionType = IT_OTHER;
        return true;
    }

    // The planes intersect in a line.
    const Line3<Real>& line = intr.GetIntersectionLine();

    // Locate one or two points that are on the circle and line.  If the
    // line is t*D+P, the circle center is C, and the circle radius is r,
    // then r^2 = |t*D+P-C|^2 = |D|^2*t^2 + 2*Dot(D,P-C)*t + |P-C|^2.  This
    // is a quadratic equation of the form:  a2*t^2 + 2*a1*t + a0 = 0.
    Vector3<Real> diff = line.Origin - mCircle->Center;
    Real a2 = line.Direction.SquaredLength();
    Real a1 = diff.Dot(line.Direction);
    Real a0 = diff.SquaredLength() - mCircle->Radius*mCircle->Radius;

    // Real-valued roots imply an intersection.
    Real discr = a1*a1 - a0*a2;
    mIntersectionType = (discr >= (Real)0 ? IT_POINT : IT_EMPTY);
    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrCircle3Plane3<Real>::Find ()
{
    mQuantity = 0;

    // Construct the plane of the circle.
    Plane3<Real> CPlane(mCircle->Normal,mCircle->Center);

    // Compute the intersection of this plane with the input plane.
    IntrPlane3Plane3<Real> intr(*mPlane,CPlane);
    if (!intr.Find())
    {
        // Planes are parallel and nonintersecting.
        mIntersectionType = IT_EMPTY;
        return false;
    }

    if (intr.GetIntersectionType() == IT_PLANE)
    {
        // Planes are the same, the circle is the common intersection set.
        mIntersectionType = IT_OTHER;
        return true;
    }

    // The planes intersect in a line.
    const Line3<Real>& line = intr.GetIntersectionLine();

    // Locate one or two points that are on the circle and line.  If the
    // line is t*D+P, the circle center is C, and the circle radius is r,
    // then r^2 = |t*D+P-C|^2 = |D|^2*t^2 + 2*Dot(D,P-C)*t + |P-C|^2.  This
    // is a quadratic equation of the form:  a2*t^2 + 2*a1*t + a0 = 0.
    Vector3<Real> diff = line.Origin - mCircle->Center;
    Real a2 = line.Direction.SquaredLength();
    Real a1 = diff.Dot(line.Direction);
    Real a0 = diff.SquaredLength() - mCircle->Radius*mCircle->Radius;

    Real discr = a1*a1 - a0*a2;
    if (discr < (Real)0)
    {
        // No real roots, the circle does not intersect the plane.
        mIntersectionType = IT_EMPTY;
        return false;
    }

    mIntersectionType = IT_POINT;

    Real inv = ((Real)1)/a2;
    if (discr < Math<Real>::ZERO_TOLERANCE)
    {
        // One repeated root, the circle just touches the plane.
        mQuantity = 1;
        mPoint[0] = line.Origin - (a1*inv)*line.Direction;
        return true;
    }

    // Two distinct, real-valued roots, the circle intersects the plane in
    // two points.
    Real root = Math<Real>::Sqrt(discr);
    mQuantity = 2;
    mPoint[0] = line.Origin - ((a1 + root)*inv)*line.Direction;
    mPoint[1] = line.Origin - ((a1 - root)*inv)*line.Direction;
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrCircle3Plane3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrCircle3Plane3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
const Circle3<Real>& IntrCircle3Plane3<Real>::GetIntersectionCircle () const
{
    return *mCircle;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrCircle3Plane3<float>;

template WM5_MATHEMATICS_ITEM
class IntrCircle3Plane3<double>;
//----------------------------------------------------------------------------
}
