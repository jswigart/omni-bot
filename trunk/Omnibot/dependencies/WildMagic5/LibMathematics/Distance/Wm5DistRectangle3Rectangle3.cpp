// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistRectangle3Rectangle3.h"
#include "Wm5DistSegment3Rectangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistRectangle3Rectangle3<Real>::DistRectangle3Rectangle3 (
    const Rectangle3<Real>& rkRectangle0,
    const Rectangle3<Real>& rkRectangle1)
    :
    mRectangle0(&rkRectangle0),
    mRectangle1(&rkRectangle1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Rectangle3<Real>& DistRectangle3Rectangle3<Real>::GetRectangle0 () const
{
    return *mRectangle0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Rectangle3<Real>& DistRectangle3Rectangle3<Real>::GetRectangle1 () const
{
    return *mRectangle1;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRectangle3Rectangle3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRectangle3Rectangle3<Real>::GetSquared ()
{
    // Compare edges of rectangle0 to the interior of rectangle1.
    Real sqrDist = Math<Real>::MAX_REAL, sqrDistTmp;
    Segment3<Real> edge;
    int i0, i1;
    for (i1 = 0; i1 < 2; ++i1)
    {
        for (i0 = -1; i0 <= 1; i0 += 2)
        {
            edge.Center = mRectangle0->Center +
                (i0*mRectangle0->Extent[1-i1]) *
                mRectangle0->Axis[1-i1];
            edge.Direction = mRectangle0->Axis[i1];
            edge.Extent = mRectangle0->Extent[i1];
            edge.ComputeEndPoints();

            DistSegment3Rectangle3<Real> querySR(edge, *mRectangle1);
            sqrDistTmp = querySR.GetSquared();
            if (sqrDistTmp < sqrDist)
            {
                mClosestPoint0 = querySR.GetClosestPoint0();
                mClosestPoint1 = querySR.GetClosestPoint1();
                sqrDist = sqrDistTmp;
            }
        }
    }

    // Compare edges of rectangle1 to the interior of rectangle0.
    for (i1 = 0; i1 < 2; ++i1)
    {
        for (i0 = -1; i0 <= 1; i0 += 2)
        {
            edge.Center = mRectangle1->Center +
                (i0*mRectangle1->Extent[1-i1]) *
                mRectangle1->Axis[1-i1];
            edge.Direction = mRectangle1->Axis[i1];
            edge.Extent = mRectangle1->Extent[i1];
            edge.ComputeEndPoints();

            DistSegment3Rectangle3<Real> querySR(edge, *mRectangle0);
            sqrDistTmp = querySR.GetSquared();
            if (sqrDistTmp < sqrDist)
            {
                mClosestPoint0 = querySR.GetClosestPoint0();
                mClosestPoint1 = querySR.GetClosestPoint1();
                sqrDist = sqrDistTmp;
            }
        }
    }

    return sqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRectangle3Rectangle3<Real>::Get (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedCenter0 = mRectangle0->Center + t*velocity0;
    Vector3<Real> movedCenter1 = mRectangle1->Center + t*velocity1;
    Rectangle3<Real> movedRect0(movedCenter0, mRectangle0->Axis,
        mRectangle0->Extent);
    Rectangle3<Real> movedRect1(movedCenter1, mRectangle1->Axis,
        mRectangle1->Extent);
    return DistRectangle3Rectangle3<Real>(movedRect0,movedRect1).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRectangle3Rectangle3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedCenter0 = mRectangle0->Center + t*velocity0;
    Vector3<Real> movedCenter1 = mRectangle1->Center + t*velocity1;
    Rectangle3<Real> movedRect0(movedCenter0, mRectangle0->Axis,
        mRectangle0->Extent);
    Rectangle3<Real> movedRect1(movedCenter1, mRectangle1->Axis,
        mRectangle1->Extent);
    return DistRectangle3Rectangle3<Real>(movedRect0,movedRect1).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistRectangle3Rectangle3<float>;

template WM5_MATHEMATICS_ITEM
class DistRectangle3Rectangle3<double>;
//----------------------------------------------------------------------------
}
