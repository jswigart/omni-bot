// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrRay3Plane3.h"
#include "Wm5IntrLine3Plane3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrRay3Plane3<Real>::IntrRay3Plane3 (const Ray3<Real>& ray,
    const Plane3<Real>& plane)
    :
    mRay(&ray),
    mPlane(&plane)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray3<Real>& IntrRay3Plane3<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrRay3Plane3<Real>::GetPlane () const
{
    return *mPlane;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay3Plane3<Real>::Test ()
{
    return Find();
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay3Plane3<Real>::Find ()
{
    Line3<Real> line(mRay->Origin, mRay->Direction);
    IntrLine3Plane3<Real> intr(line, *mPlane);
    if (intr.Find())
    {
        // The line intersects the plane, but possibly at a point that is
        // not on the ray.
        mIntersectionType = intr.GetIntersectionType();
        mRayParameter = intr.GetLineParameter();
        return mRayParameter >= (Real)0;
    }

    mIntersectionType = IT_EMPTY;
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntrRay3Plane3<Real>::GetRayParameter () const
{
    return mRayParameter;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrRay3Plane3<float>;

template WM5_MATHEMATICS_ITEM
class IntrRay3Plane3<double>;
//----------------------------------------------------------------------------
}
