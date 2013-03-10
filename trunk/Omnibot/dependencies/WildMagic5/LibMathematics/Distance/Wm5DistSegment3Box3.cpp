// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistSegment3Box3.h"
#include "Wm5DistLine3Box3.h"
#include "Wm5DistPoint3Box3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistSegment3Box3<Real>::DistSegment3Box3 (const Segment3<Real>& segment,
    const Box3<Real>& box)
    :
    mSegment(&segment),
    mBox(&box)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment3<Real>& DistSegment3Box3<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
const Box3<Real>& DistSegment3Box3<Real>::GetBox () const
{
    return *mBox;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistSegment3Box3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistSegment3Box3<Real>::GetSquared ()
{
    Line3<Real> line(mSegment->Center, mSegment->Direction);
    DistLine3Box3<Real> queryLB(line, *mBox);
    Real sqrDistance = queryLB.GetSquared();
    Real lineParameter = queryLB.GetLineParameter();

    if (lineParameter >= -mSegment->Extent)
    {
        if (lineParameter <= mSegment->Extent)
        {
            mClosestPoint0 = queryLB.GetClosestPoint0();
            mClosestPoint1 = queryLB.GetClosestPoint1();
        }
        else
        {
            DistPoint3Box3<Real> queryPB(mSegment->P1, *mBox);
            sqrDistance = queryPB.GetSquared();
            mClosestPoint0 = queryPB.GetClosestPoint0();
            mClosestPoint1 = queryPB.GetClosestPoint1();
        }
    }
    else
    {
        DistPoint3Box3<Real> queryPB(mSegment->P0, *mBox);
        sqrDistance = queryPB.GetSquared();
        mClosestPoint0 = queryPB.GetClosestPoint0();
        mClosestPoint1 = queryPB.GetClosestPoint1();
    }

    return sqrDistance;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistSegment3Box3<Real>::Get (Real t, const Vector3<Real>& velocity0,
    const Vector3<Real>& velocity1)
{
    Vector3<Real> movedCenter0 = mSegment->Center + t*velocity0;
    Vector3<Real> movedCenter1 = mBox->Center + t*velocity1;
    Segment3<Real> movedSegment(movedCenter0, mSegment->Direction,
        mSegment->Extent);
    Box3<Real> movedBox(movedCenter1,mBox->Axis, mBox->Extent);
    return DistSegment3Box3<Real>(movedSegment, movedBox).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistSegment3Box3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedCenter0 = mSegment->Center + t*velocity0;
    Vector3<Real> movedCenter1 = mBox->Center + t*velocity1;
    Segment3<Real> movedSegment(movedCenter0, mSegment->Direction,
        mSegment->Extent);
    Box3<Real> movedBox(movedCenter1,mBox->Axis, mBox->Extent);
    return DistSegment3Box3<Real>(movedSegment, movedBox).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistSegment3Box3<float>;

template WM5_MATHEMATICS_ITEM
class DistSegment3Box3<double>;
//----------------------------------------------------------------------------
}
