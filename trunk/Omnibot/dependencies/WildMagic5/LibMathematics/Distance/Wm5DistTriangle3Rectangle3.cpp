// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/04/16)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistTriangle3Rectangle3.h"
#include "Wm5DistSegment3Triangle3.h"
#include "Wm5DistSegment3Rectangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistTriangle3Rectangle3<Real>::DistTriangle3Rectangle3 (
    const Triangle3<Real>& rkTriangle, const Rectangle3<Real>& rkRectangle)
    :
    mTriangle(&rkTriangle),
    mRectangle(&rkRectangle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle3<Real>& DistTriangle3Rectangle3<Real>::GetTriangle () const
{
    return *mTriangle;
}
//----------------------------------------------------------------------------
template <typename Real>
const Rectangle3<Real>& DistTriangle3Rectangle3<Real>::GetRectangle () const
{
    return *mRectangle;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistTriangle3Rectangle3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistTriangle3Rectangle3<Real>::GetSquared ()
{
    // Compare edges of triangle to the interior of rectangle.
    Real sqrDist = Math<Real>::MAX_REAL, sqrDistTmp;
    Segment3<Real> edge;
    int i0, i1;
    for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        edge.Center = ((Real)0.5)*(mTriangle->V[i0] +
            mTriangle->V[i1]);
        edge.Direction = mTriangle->V[i1] - mTriangle->V[i0];
        edge.Extent = ((Real)0.5)*edge.Direction.Normalize();
        edge.ComputeEndPoints();

        DistSegment3Rectangle3<Real> querySR(edge, *mRectangle);
        sqrDistTmp = querySR.GetSquared();
        if (sqrDistTmp < sqrDist)
        {
            // The triangle point is reported in mClosestPoint0 and the
            // rectangle point is reported in mClosestPoint1.  The querySR
            // calculator is for triangleEdge-rectangle, so GetClosestPoint0()
            // and GetClosestPoint1() must be called as listed next.
            mClosestPoint0 = querySR.GetClosestPoint0();
            mClosestPoint1 = querySR.GetClosestPoint1();
            sqrDist = sqrDistTmp;
        }
    }

    // Compare edges of rectangle to the interior of triangle.
    for (i1 = 0; i1 < 2; ++i1)
    {
        for (i0 = -1; i0 <= 1; i0 += 2)
        {
            edge.Center = mRectangle->Center +
                (i0*mRectangle->Extent[1-i1]) *
                mRectangle->Axis[1-i1];
            edge.Direction = mRectangle->Axis[i1];
            edge.Extent = mRectangle->Extent[i1];
            edge.ComputeEndPoints();

            DistSegment3Triangle3<Real> queryST(edge, *mTriangle);
            sqrDistTmp = queryST.GetSquared();
            if (sqrDistTmp < sqrDist)
            {
                // The triangle point is reported in mClosestPoint0 and the
                // rectangle point is reported in mClosestPoint1.  The queryST
                // calculator is for rectangleEdge-triangle, so
                // GetClosestPoint1() and GetClosestPoint0() must be called as
                // listed next.
                mClosestPoint0 = queryST.GetClosestPoint1();
                mClosestPoint1 = queryST.GetClosestPoint0();
                sqrDist = sqrDistTmp;
            }
        }
    }

    return sqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistTriangle3Rectangle3<Real>::Get (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedV0 = mTriangle->V[0] + t*velocity0;
    Vector3<Real> movedV1 = mTriangle->V[1] + t*velocity0;
    Vector3<Real> movedV2 = mTriangle->V[2] + t*velocity0;
    Vector3<Real> movedCenter = mRectangle->Center + t*velocity1;
    Triangle3<Real> movedTri(movedV0, movedV1, movedV2);
    Rectangle3<Real> movedRect(movedCenter, mRectangle->Axis,
        mRectangle->Extent);
    return DistTriangle3Rectangle3<Real>(movedTri, movedRect).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistTriangle3Rectangle3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedV0 = mTriangle->V[0] + t*velocity0;
    Vector3<Real> movedV1 = mTriangle->V[1] + t*velocity0;
    Vector3<Real> movedV2 = mTriangle->V[2] + t*velocity0;
    Vector3<Real> movedCenter = mRectangle->Center + t*velocity1;
    Triangle3<Real> movedTri(movedV0, movedV1, movedV2);
    Rectangle3<Real> movedRect(movedCenter, mRectangle->Axis,
        mRectangle->Extent);
    return DistTriangle3Rectangle3<Real>(movedTri, movedRect).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistTriangle3Rectangle3<float>;

template WM5_MATHEMATICS_ITEM
class DistTriangle3Rectangle3<double>;
//----------------------------------------------------------------------------
}
