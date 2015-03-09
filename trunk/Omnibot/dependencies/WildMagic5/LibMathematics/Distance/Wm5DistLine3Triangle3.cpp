// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistLine3Triangle3.h"
#include "Wm5DistLine3Segment3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistLine3Triangle3<Real>::DistLine3Triangle3 (const Line3<Real>& rkLine,
    const Triangle3<Real>& rkTriangle)
    :
    mLine(&rkLine),
    mTriangle(&rkTriangle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& DistLine3Triangle3<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle3<Real>& DistLine3Triangle3<Real>::GetTriangle () const
{
    return *mTriangle;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Triangle3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Triangle3<Real>::GetSquared ()
{
    // Test if line intersects triangle.  If so, the squared distance is zero.
    Vector3<Real> edge0 = mTriangle->V[1] - mTriangle->V[0];
    Vector3<Real> edge1 = mTriangle->V[2] - mTriangle->V[0];
    Vector3<Real> normal = edge0.UnitCross(edge1);
    Real NdD = normal.Dot(mLine->Direction);
    if (Math<Real>::FAbs(NdD) > Math<Real>::ZERO_TOLERANCE)
    {
        // The line and triangle are not parallel, so the line intersects
        // the plane of the triangle.
        Vector3<Real> diff = mLine->Origin - mTriangle->V[0];
        Vector3<Real> U, V;
        Vector3<Real>::GenerateComplementBasis(U, V, mLine->Direction);
        Real UdE0 = U.Dot(edge0);
        Real UdE1 = U.Dot(edge1);
        Real UdDiff = U.Dot(diff);
        Real VdE0 = V.Dot(edge0);
        Real VdE1 = V.Dot(edge1);
        Real VdDiff = V.Dot(diff);
        Real invDet = ((Real)1)/(UdE0*VdE1 - UdE1*VdE0);

        // Barycentric coordinates for the point of intersection.
        Real b1 = (VdE1*UdDiff - UdE1*VdDiff)*invDet;
        Real b2 = (UdE0*VdDiff - VdE0*UdDiff)*invDet;
        Real b0 = (Real)1 - b1 - b2;

        if (b0 >= (Real)0 && b1 >= (Real)0 && b2 >= (Real)0)
        {
            // Line parameter for the point of intersection.
            Real DdE0 = mLine->Direction.Dot(edge0);
            Real DdE1 = mLine->Direction.Dot(edge1);
            Real DdDiff = mLine->Direction.Dot(diff);
            mLineParameter = b1*DdE0 + b2*DdE1 - DdDiff;

            // Barycentric coordinates for the point of intersection.
            mTriangleBary[0] = b0;
            mTriangleBary[1] = b1;
            mTriangleBary[2] = b2;

            // The intersection point is inside or on the triangle.
            mClosestPoint0 = mLine->Origin +
                mLineParameter*mLine->Direction;

            mClosestPoint1 = mTriangle->V[0] + b1*edge0 + b2*edge1;

            return (Real)0;
        }
    }

    // Either (1) the line is not parallel to the triangle and the point of
    // intersection of the line and the plane of the triangle is outside the
    // triangle or (2) the line and triangle are parallel.  Regardless, the
    // closest point on the triangle is on an edge of the triangle.  Compare
    // the line to all three edges of the triangle.
    Real sqrDist = Math<Real>::MAX_REAL;
    for (int i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        Segment3<Real> segment;
        segment.Center = ((Real)0.5)*(mTriangle->V[i0] +
            mTriangle->V[i1]);
        segment.Direction = mTriangle->V[i1] - mTriangle->V[i0];
        segment.Extent = ((Real)0.5)*segment.Direction.Normalize();
        segment.ComputeEndPoints();

        DistLine3Segment3<Real> queryLS(*mLine, segment);
        Real sqrDistTmp = queryLS.GetSquared();
        if (sqrDistTmp < sqrDist)
        {
            mClosestPoint0 = queryLS.GetClosestPoint0();
            mClosestPoint1 = queryLS.GetClosestPoint1();
            sqrDist = sqrDistTmp;

            mLineParameter = queryLS.GetLineParameter();
            Real ratio = queryLS.GetSegmentParameter()/segment.Extent;
            mTriangleBary[i0] = ((Real)0.5)*((Real)1 - ratio);
            mTriangleBary[i1] = (Real)1 - mTriangleBary[i0];
            mTriangleBary[3-i0-i1] = (Real)0;
        }
    }

    return sqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Triangle3<Real>::Get (Real t, const Vector3<Real>& velocity0,
    const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin = mLine->Origin + t*velocity0;
    Vector3<Real> movedV0 = mTriangle->V[0] + t*velocity1;
    Vector3<Real> movedV1 = mTriangle->V[1] + t*velocity1;
    Vector3<Real> movedV2 = mTriangle->V[2] + t*velocity1;
    Line3<Real> movedLine(movedOrigin, mLine->Direction);
    Triangle3<Real> movedTriangle(movedV0, movedV1, movedV2);
    return DistLine3Triangle3<Real>(movedLine, movedTriangle).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Triangle3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin = mLine->Origin + t*velocity0;
    Vector3<Real> movedV0 = mTriangle->V[0] + t*velocity1;
    Vector3<Real> movedV1 = mTriangle->V[1] + t*velocity1;
    Vector3<Real> movedV2 = mTriangle->V[2] + t*velocity1;
    Line3<Real> movedLine(movedOrigin, mLine->Direction);
    Triangle3<Real> movedTriangle(movedV0, movedV1, movedV2);
    return DistLine3Triangle3<Real>(movedLine, movedTriangle).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Triangle3<Real>::GetLineParameter () const
{
    return mLineParameter;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Triangle3<Real>::GetTriangleBary (int i) const
{
    return mTriangleBary[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistLine3Triangle3<float>;

template WM5_MATHEMATICS_ITEM
class DistLine3Triangle3<double>;
//----------------------------------------------------------------------------
}
