// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrPlane3Lozenge3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrPlane3Lozenge3<Real>::IntrPlane3Lozenge3 (const Plane3<Real>& plane,
    const Lozenge3<Real>& lozenge)
    :
    mPlane(&plane),
    mLozenge(&lozenge)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrPlane3Lozenge3<Real>::GetPlane () const
{
    return *mPlane;
}
//----------------------------------------------------------------------------
template <typename Real>
const Lozenge3<Real>& IntrPlane3Lozenge3<Real>::GetLozenge () const
{
    return *mLozenge;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Lozenge3<Real>::Test ()
{
    Real sDistMM = mPlane->DistanceTo(mLozenge->Rectangle.GetMMCorner());
    Real sDistPM = mPlane->DistanceTo(mLozenge->Rectangle.GetPMCorner());
    if (sDistMM*sDistPM <= (Real)0)
    {
        // Two lozenge ends on opposite sides of the plane.
        return true;
    }

    Real sDistMP = mPlane->DistanceTo(mLozenge->Rectangle.GetMPCorner());
    if (sDistMM*sDistMP <= (Real)0)
    {
        // Two lozenge ends on opposite sides of the plane.
        return true;
    }

    Real sDistPP = mPlane->DistanceTo(mLozenge->Rectangle.GetPPCorner());
    if (sDistPM*sDistPP <= (Real)0)
    {
        // Two lozenge ends on opposite sides of the plane.
        return true;
    }

    // The lozenge rectangle corners are all on one side of the plane.
    // The spheres centered at the corners, with radius that of the lozenge,
    // might intersect the plane.
    return Math<Real>::FAbs(sDistMM) <= mLozenge->Radius
        || Math<Real>::FAbs(sDistPM) <= mLozenge->Radius
        || Math<Real>::FAbs(sDistMP) <= mLozenge->Radius
        || Math<Real>::FAbs(sDistPP) <= mLozenge->Radius;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Lozenge3<Real>::LozengeIsCulled () const
{
    Real sDistMM = mPlane->DistanceTo(mLozenge->Rectangle.GetMMCorner());
    if (sDistMM < (Real)0)
    {
        Real sDistPM = mPlane->DistanceTo(
            mLozenge->Rectangle.GetPMCorner());
        if (sDistPM < (Real)0)
        {
            Real sDistMP = mPlane->DistanceTo(
                mLozenge->Rectangle.GetMPCorner());
            if (sDistMP < (Real)0)
            {
                Real sDistPP = mPlane->DistanceTo(
                    mLozenge->Rectangle.GetPPCorner());
                if (sDistPP < (Real)0)
                {
                    // All four lozenge corners on negative side of plane.
                    if (sDistMM <= sDistPM)
                    {
                        if (sDistMM <= sDistMP)
                        {
                            return sDistMM <= -mLozenge->Radius;
                        }
                        else
                        {
                            return sDistMP <= -mLozenge->Radius;
                        }
                    }
                    else
                    {
                        if (sDistPM <= sDistPP)
                        {
                            return sDistPM <= -mLozenge->Radius;
                        }
                        else
                        {
                            return sDistPP <= -mLozenge->Radius;
                        }
                    }
                }
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
class IntrPlane3Lozenge3<float>;

template WM5_MATHEMATICS_ITEM
class IntrPlane3Lozenge3<double>;
//----------------------------------------------------------------------------
}
