// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistRay3Box3.h"
#include "Wm5DistLine3Box3.h"
#include "Wm5DistPoint3Box3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistRay3Box3<Real>::DistRay3Box3 (const Ray3<Real>& ray,
    const Box3<Real>& box)
    :
    mRay(&ray),
    mBox(&box)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray3<Real>& DistRay3Box3<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
const Box3<Real>& DistRay3Box3<Real>::GetBox () const
{
    return *mBox;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Box3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Box3<Real>::GetSquared ()
{
    Line3<Real> line(mRay->Origin, mRay->Direction);
    DistLine3Box3<Real> queryLB(line, *mBox);
    Real sqrDistance = queryLB.GetSquared();
    Real lineParameter = queryLB.GetLineParameter();

    if (lineParameter >= (Real)0)
    {
        mClosestPoint0 = queryLB.GetClosestPoint0();
        mClosestPoint1 = queryLB.GetClosestPoint1();
    }
    else
    {
        DistPoint3Box3<Real> queryPB(mRay->Origin, *mBox);
        sqrDistance = queryPB.GetSquared();
        mClosestPoint0 = queryPB.GetClosestPoint0();
        mClosestPoint1 = queryPB.GetClosestPoint1();
    }
    return sqrDistance;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Box3<Real>::Get (Real t, const Vector3<Real>& velocity0,
    const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin = mRay->Origin + t*velocity0;
    Vector3<Real> movedCenter = mBox->Center + t*velocity1;
    Ray3<Real> movedRay(movedOrigin, mRay->Direction);
    Box3<Real> movedBox(movedCenter, mBox->Axis, mBox->Extent);
    return DistRay3Box3<Real>(movedRay, movedBox).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Box3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin = mRay->Origin + t*velocity0;
    Vector3<Real> movedCenter = mBox->Center + t*velocity1;
    Ray3<Real> movedRay(movedOrigin, mRay->Direction);
    Box3<Real> movedBox(movedCenter, mBox->Axis, mBox->Extent);
    return DistRay3Box3<Real>(movedRay, movedBox).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistRay3Box3<float>;

template WM5_MATHEMATICS_ITEM
class DistRay3Box3<double>;
//----------------------------------------------------------------------------
}
