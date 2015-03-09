// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrPlane3Capsule3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrPlane3Capsule3<Real>::IntrPlane3Capsule3 (const Plane3<Real>& plane,
    const Capsule3<Real>& capsule)
    :
    mPlane(&plane),
    mCapsule(&capsule)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrPlane3Capsule3<Real>::GetPlane () const
{
    return *mPlane;
}
//----------------------------------------------------------------------------
template <typename Real>
const Capsule3<Real>& IntrPlane3Capsule3<Real>::GetCapsule () const
{
    return *mCapsule;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Capsule3<Real>::Test ()
{
    Real pDist = mPlane->DistanceTo(mCapsule->Segment.P1);
    Real nDist = mPlane->DistanceTo(mCapsule->Segment.P0);
    if (pDist*nDist <= (Real)0)
    {
        // Capsule segment endpoints on opposite sides of the plane.
        return true;
    }

    // Endpoints on same side of plane, but the endpoint spheres (with
    // radius of the capsule) might intersect the plane.
    return Math<Real>::FAbs(pDist) <= mCapsule->Radius
        || Math<Real>::FAbs(nDist) <= mCapsule->Radius;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Capsule3<Real>::CapsuleIsCulled () const
{
    Real pDist = mPlane->DistanceTo(mCapsule->Segment.P1);
    if (pDist < (Real)0)
    {
        Real nDist = mPlane->DistanceTo(mCapsule->Segment.P0);
        if (nDist < (Real)0)
        {
            if (pDist <= nDist)
            {
                return pDist <= -mCapsule->Radius;
            }
            else
            {
                return nDist <= -mCapsule->Radius;
            }
        }
    }

    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrPlane3Capsule3<float>;

template WM5_MATHEMATICS_ITEM
class IntrPlane3Capsule3<double>;
//----------------------------------------------------------------------------
}
