// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistTriangle3Triangle3.h"
#include "Wm5DistSegment3Triangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistTriangle3Triangle3<Real>::DistTriangle3Triangle3 (
    const Triangle3<Real>& triangle0, const Triangle3<Real>& triangle1)
    :
    mTriangle0(&triangle0),
    mTriangle1(&triangle1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle3<Real>& DistTriangle3Triangle3<Real>::GetTriangle0 () const
{
    return *mTriangle0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle3<Real>& DistTriangle3Triangle3<Real>::GetTriangle1 () const
{
    return *mTriangle1;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistTriangle3Triangle3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistTriangle3Triangle3<Real>::GetSquared ()
{
    // Compare edges of triangle0 to the interior of triangle1.
    Real sqrDist = Math<Real>::MAX_REAL, sqrDistTmp;
    Segment3<Real> edge;
    Real ratio;
    int i0, i1;
    for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        edge.Center = ((Real)0.5)*(mTriangle0->V[i0] +
            mTriangle0->V[i1]);
        edge.Direction = mTriangle0->V[i1] - mTriangle0->V[i0];
        edge.Extent = ((Real)0.5)*edge.Direction.Normalize();
        edge.ComputeEndPoints();

        DistSegment3Triangle3<Real> queryST(edge, *mTriangle1);
        sqrDistTmp = queryST.GetSquared();
        if (sqrDistTmp < sqrDist)
        {
            mClosestPoint0 = queryST.GetClosestPoint0();
            mClosestPoint1 = queryST.GetClosestPoint1();
            sqrDist = sqrDistTmp;

            ratio = queryST.GetSegmentParameter()/edge.Extent;
            mTriangleBary0[i0] = ((Real)0.5)*((Real)1 - ratio);
            mTriangleBary0[i1] = (Real)1 - mTriangleBary0[i0];
            mTriangleBary0[3-i0-i1] = (Real)0;
            mTriangleBary1[0] = queryST.GetTriangleBary(0);
            mTriangleBary1[1] = queryST.GetTriangleBary(1);
            mTriangleBary1[2] = queryST.GetTriangleBary(2);

            if (sqrDist <= Math<Real>::ZERO_TOLERANCE)
            {
                return (Real)0;
            }
        }
    }

    // Compare edges of triangle1 to the interior of triangle0.
    for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        edge.Center = ((Real)0.5)*(mTriangle1->V[i0] +
            mTriangle1->V[i1]);
        edge.Direction = mTriangle1->V[i1] - mTriangle1->V[i0];
        edge.Extent = ((Real)0.5)*edge.Direction.Normalize();
        edge.ComputeEndPoints();

        DistSegment3Triangle3<Real> queryST(edge, *mTriangle0);
        sqrDistTmp = queryST.GetSquared();
        if (sqrDistTmp < sqrDist)
        {
            mClosestPoint0 = queryST.GetClosestPoint0();
            mClosestPoint1 = queryST.GetClosestPoint1();
            sqrDist = sqrDistTmp;

            ratio = queryST.GetSegmentParameter()/edge.Extent;
            mTriangleBary1[i0] = ((Real)0.5)*((Real)1 - ratio);
            mTriangleBary1[i1] = (Real)1 - mTriangleBary1[i0];
            mTriangleBary1[3-i0-i1] = (Real)0;
            mTriangleBary0[0] = queryST.GetTriangleBary(0);
            mTriangleBary0[1] = queryST.GetTriangleBary(1);
            mTriangleBary0[2] = queryST.GetTriangleBary(2);

            if (sqrDist <= Math<Real>::ZERO_TOLERANCE)
            {
                return (Real)0;
            }
        }
    }

    return sqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistTriangle3Triangle3<Real>::Get (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedV00 = mTriangle0->V[0] + t*velocity0;
    Vector3<Real> movedV01 = mTriangle0->V[1] + t*velocity0;
    Vector3<Real> movedV02 = mTriangle0->V[2] + t*velocity0;
    Vector3<Real> movedV10 = mTriangle1->V[0] + t*velocity1;
    Vector3<Real> movedV11 = mTriangle1->V[1] + t*velocity1;
    Vector3<Real> movedV12 = mTriangle1->V[2] + t*velocity1;
    Triangle3<Real> movedTri0(movedV00, movedV01, movedV02);
    Triangle3<Real> movedTri1(movedV10, movedV11, movedV12);
    return DistTriangle3Triangle3<Real>(movedTri0, movedTri1).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistTriangle3Triangle3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedV00 = mTriangle0->V[0] + t*velocity0;
    Vector3<Real> movedV01 = mTriangle0->V[1] + t*velocity0;
    Vector3<Real> movedV02 = mTriangle0->V[2] + t*velocity0;
    Vector3<Real> movedV10 = mTriangle1->V[0] + t*velocity1;
    Vector3<Real> movedV11 = mTriangle1->V[1] + t*velocity1;
    Vector3<Real> movedV12 = mTriangle1->V[2] + t*velocity1;
    Triangle3<Real> movedTri0(movedV00, movedV01, movedV02);
    Triangle3<Real> movedTri1(movedV10, movedV11, movedV12);
    return DistTriangle3Triangle3<Real>(movedTri0, movedTri1).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistTriangle3Triangle3<Real>::GetTriangleBary0 (int i) const
{
    return mTriangleBary0[i];
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistTriangle3Triangle3<Real>::GetTriangleBary1 (int i) const
{
    return mTriangleBary1[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistTriangle3Triangle3<float>;

template WM5_MATHEMATICS_ITEM
class DistTriangle3Triangle3<double>;
//----------------------------------------------------------------------------
}
