// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistSegment3Rectangle3.h"
#include "Wm5DistLine3Rectangle3.h"
#include "Wm5DistPoint3Rectangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistSegment3Rectangle3<Real>::DistSegment3Rectangle3 (
    const Segment3<Real>& segment, const Rectangle3<Real>& rectangle)
    :
    mSegment(&segment),
    mRectangle(&rectangle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment3<Real>& DistSegment3Rectangle3<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
const Rectangle3<Real>& DistSegment3Rectangle3<Real>::GetRectangle () const
{
    return *mRectangle;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistSegment3Rectangle3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistSegment3Rectangle3<Real>::GetSquared ()
{
    DistLine3Rectangle3<Real> queryLR(Line3<Real>(mSegment->Center,
        mSegment->Direction), *mRectangle);
    Real sqrDist = queryLR.GetSquared();
    mSegmentParameter = queryLR.GetLineParameter();

    if (mSegmentParameter >= -mSegment->Extent)
    {
        if (mSegmentParameter <= mSegment->Extent)
        {
            mClosestPoint0 = queryLR.GetClosestPoint0();
            mClosestPoint1 = queryLR.GetClosestPoint1();
            mRectCoord[0] = queryLR.GetRectangleCoordinate(0);
            mRectCoord[1] = queryLR.GetRectangleCoordinate(1);
        }
        else
        {
            mClosestPoint0 = mSegment->P1;
            DistPoint3Rectangle3<Real> queryPR(mClosestPoint0,
                *mRectangle);
            sqrDist = queryPR.GetSquared();
            mClosestPoint1 = queryPR.GetClosestPoint1();
            mSegmentParameter = mSegment->Extent;
            mRectCoord[0] = queryPR.GetRectangleCoordinate(0);
            mRectCoord[1] = queryPR.GetRectangleCoordinate(1);
        }
    }
    else
    {
        mClosestPoint0 = mSegment->P0;
        DistPoint3Rectangle3<Real> queryPR(mClosestPoint0, *mRectangle);
        sqrDist = queryPR.GetSquared();
        mClosestPoint1 = queryPR.GetClosestPoint1();
        mSegmentParameter = -mSegment->Extent;
        mRectCoord[0] = queryPR.GetRectangleCoordinate(0);
        mRectCoord[1] = queryPR.GetRectangleCoordinate(1);
    }

    return sqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistSegment3Rectangle3<Real>::Get (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedCenter0 = mSegment->Center + t*velocity0;
    Vector3<Real> movedCenter1 = mRectangle->Center + t*velocity1;
    Segment3<Real> movedSeg(movedCenter0, mSegment->Direction,
        mSegment->Extent);
    Rectangle3<Real> movedRect(movedCenter1, mRectangle->Axis,
        mRectangle->Extent);
    return DistSegment3Rectangle3<Real>(movedSeg, movedRect).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistSegment3Rectangle3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedCenter0 = mSegment->Center + t*velocity0;
    Vector3<Real> movedCenter1 = mRectangle->Center + t*velocity1;
    Segment3<Real> movedSeg(movedCenter0, mSegment->Direction,
        mSegment->Extent);
    Rectangle3<Real> movedRect(movedCenter1, mRectangle->Axis,
        mRectangle->Extent);
    return DistSegment3Rectangle3<Real>(movedSeg, movedRect).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistSegment3Rectangle3<Real>::GetSegmentParameter () const
{
    return mSegmentParameter;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistSegment3Rectangle3<Real>::GetRectangleCoordinate (int i) const
{
    return mRectCoord[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistSegment3Rectangle3<float>;

template WM5_MATHEMATICS_ITEM
class DistSegment3Rectangle3<double>;
//----------------------------------------------------------------------------
}
