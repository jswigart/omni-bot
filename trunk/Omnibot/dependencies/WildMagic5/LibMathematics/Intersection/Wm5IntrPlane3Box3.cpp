// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrPlane3Box3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrPlane3Box3<Real>::IntrPlane3Box3 (const Plane3<Real>& plane,
    const Box3<Real>& box)
    :
    mPlane(&plane),
    mBox(&box)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrPlane3Box3<Real>::GetPlane () const
{
    return *mPlane;
}
//----------------------------------------------------------------------------
template <typename Real>
const Box3<Real>& IntrPlane3Box3<Real>::GetBox () const
{
    return *mBox;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Box3<Real>::Test ()
{
    Real tmp[3] =
    {
        mBox->Extent[0]*(mPlane->Normal.Dot(mBox->Axis[0])),
        mBox->Extent[1]*(mPlane->Normal.Dot(mBox->Axis[1])),
        mBox->Extent[2]*(mPlane->Normal.Dot(mBox->Axis[2]))
    };

    Real radius = Math<Real>::FAbs(tmp[0]) + Math<Real>::FAbs(tmp[1]) +
        Math<Real>::FAbs(tmp[2]);

    Real signedDistance = mPlane->DistanceTo(mBox->Center);
    return Math<Real>::FAbs(signedDistance) <= radius;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Box3<Real>::BoxIsCulled () const
{
    Real tmp[3] =
    {
        mBox->Extent[0]*(mPlane->Normal.Dot(mBox->Axis[0])),
        mBox->Extent[1]*(mPlane->Normal.Dot(mBox->Axis[1])),
        mBox->Extent[2]*(mPlane->Normal.Dot(mBox->Axis[2]))
    };

    Real radius = Math<Real>::FAbs(tmp[0]) + Math<Real>::FAbs(tmp[1]) +
        Math<Real>::FAbs(tmp[2]);

    Real signedDistance = mPlane->DistanceTo(mBox->Center);
    return signedDistance <= -radius;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrPlane3Box3<float>;

template WM5_MATHEMATICS_ITEM
class IntrPlane3Box3<double>;
//----------------------------------------------------------------------------
}
