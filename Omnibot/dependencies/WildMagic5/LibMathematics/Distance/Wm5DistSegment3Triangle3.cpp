// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistSegment3Triangle3.h"
#include "Wm5DistLine3Triangle3.h"
#include "Wm5DistPoint3Triangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistSegment3Triangle3<Real>::DistSegment3Triangle3 (
    const Segment3<Real>& rkSegment, const Triangle3<Real>& rkTriangle)
    :
    mSegment(&rkSegment),
    mTriangle(&rkTriangle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment3<Real>& DistSegment3Triangle3<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle3<Real>& DistSegment3Triangle3<Real>::GetTriangle () const
{
    return *mTriangle;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistSegment3Triangle3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistSegment3Triangle3<Real>::GetSquared ()
{
    DistLine3Triangle3<Real> queryLT(Line3<Real>(mSegment->Center,
        mSegment->Direction), *mTriangle);
    Real sqrDist = queryLT.GetSquared();
    mSegmentParameter = queryLT.GetLineParameter();

    if (mSegmentParameter >= -mSegment->Extent)
    {
        if (mSegmentParameter <= mSegment->Extent)
        {
            mClosestPoint0 = queryLT.GetClosestPoint0();
            mClosestPoint1 = queryLT.GetClosestPoint1();
            mTriangleBary[0] = queryLT.GetTriangleBary(0);
            mTriangleBary[1] = queryLT.GetTriangleBary(1);
            mTriangleBary[2] = queryLT.GetTriangleBary(2);
        }
        else
        {
            mClosestPoint0 = mSegment->P1;
            DistPoint3Triangle3<Real> queryPT(mClosestPoint0, *mTriangle);
            sqrDist = queryPT.GetSquared();
            mClosestPoint1 = queryPT.GetClosestPoint1();
            mSegmentParameter = mSegment->Extent;
            mTriangleBary[0] = queryPT.GetTriangleBary(0);
            mTriangleBary[1] = queryPT.GetTriangleBary(1);
            mTriangleBary[2] = queryPT.GetTriangleBary(2);
        }
    }
    else
    {
        mClosestPoint0 = mSegment->P0;
        DistPoint3Triangle3<Real> queryPT(mClosestPoint0, *mTriangle);
        sqrDist = queryPT.GetSquared();
        mClosestPoint1 = queryPT.GetClosestPoint1();
        mSegmentParameter = -mSegment->Extent;
        mTriangleBary[0] = queryPT.GetTriangleBary(0);
        mTriangleBary[1] = queryPT.GetTriangleBary(1);
        mTriangleBary[2] = queryPT.GetTriangleBary(2);
    }

    return sqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistSegment3Triangle3<Real>::Get (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedCenter = mSegment->Center + t*velocity0;
    Vector3<Real> movedV0 = mTriangle->V[0] + t*velocity1;
    Vector3<Real> movedV1 = mTriangle->V[1] + t*velocity1;
    Vector3<Real> movedV2 = mTriangle->V[2] + t*velocity1;
    Segment3<Real> movedSeg(movedCenter, mSegment->Direction,
        mSegment->Extent);
    Triangle3<Real> movedTriangle(movedV0, movedV1, movedV2);
    return DistSegment3Triangle3<Real>(movedSeg, movedTriangle).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistSegment3Triangle3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedCenter = mSegment->Center + t*velocity0;
    Vector3<Real> movedV0 = mTriangle->V[0] + t*velocity1;
    Vector3<Real> movedV1 = mTriangle->V[1] + t*velocity1;
    Vector3<Real> movedV2 = mTriangle->V[2] + t*velocity1;
    Segment3<Real> movedSeg(movedCenter, mSegment->Direction,
        mSegment->Extent);
    Triangle3<Real> movedTriangle(movedV0, movedV1, movedV2);
    return DistSegment3Triangle3<Real>(movedSeg, movedTriangle).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistSegment3Triangle3<Real>::GetSegmentParameter () const
{
    return mSegmentParameter;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistSegment3Triangle3<Real>::GetTriangleBary (int i) const
{
    return mTriangleBary[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistSegment3Triangle3<float>;

template WM5_MATHEMATICS_ITEM
class DistSegment3Triangle3<double>;
//----------------------------------------------------------------------------
}
