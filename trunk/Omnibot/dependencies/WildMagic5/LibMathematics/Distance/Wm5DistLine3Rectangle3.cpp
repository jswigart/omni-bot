// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistLine3Rectangle3.h"
#include "Wm5DistLine3Segment3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistLine3Rectangle3<Real>::DistLine3Rectangle3 (const Line3<Real>& line,
    const Rectangle3<Real>& rectangle)
    :
    mLine(&line),
    mRectangle(&rectangle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& DistLine3Rectangle3<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Rectangle3<Real>& DistLine3Rectangle3<Real>::GetRectangle () const
{
    return *mRectangle;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Rectangle3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Rectangle3<Real>::GetSquared ()
{
    // Test if line intersects rectangle.  If so, the squared distance is
    // zero.
    Vector3<Real> N = mRectangle->Axis[0].Cross(mRectangle->Axis[1]);
    Real NdD = N.Dot(mLine->Direction);
    if (Math<Real>::FAbs(NdD) > Math<Real>::ZERO_TOLERANCE)
    {
        // The line and rectangle are not parallel, so the line intersects
        // the plane of the rectangle.
        Vector3<Real> diff = mLine->Origin - mRectangle->Center;
        Vector3<Real> U, V;
        Vector3<Real>::GenerateComplementBasis(U, V, mLine->Direction);
        Real UdD0 = U.Dot(mRectangle->Axis[0]);
        Real UdD1 = U.Dot(mRectangle->Axis[1]);
        Real UdPmC = U.Dot(diff);
        Real VdD0 = V.Dot(mRectangle->Axis[0]);
        Real VdD1 = V.Dot(mRectangle->Axis[1]);
        Real VdPmC = V.Dot(diff);
        Real invDet = ((Real)1)/(UdD0*VdD1 - UdD1*VdD0);

        // Rectangle coordinates for the point of intersection.
        Real s0 = (VdD1*UdPmC - UdD1*VdPmC)*invDet;
        Real s1 = (UdD0*VdPmC - VdD0*UdPmC)*invDet;

        if (Math<Real>::FAbs(s0) <= mRectangle->Extent[0]
        &&  Math<Real>::FAbs(s1) <= mRectangle->Extent[1])
        {
            // Line parameter for the point of intersection.
            Real DdD0 = mLine->Direction.Dot(mRectangle->Axis[0]);
            Real DdD1 = mLine->Direction.Dot(mRectangle->Axis[1]);
            Real DdDiff = mLine->Direction.Dot(diff);
            mLineParameter = s0*DdD0 + s1*DdD1 - DdDiff;

            // Rectangle coordinates for the point of intersection.
            mRectCoord[0] = s0;
            mRectCoord[1] = s1;

            // The intersection point is inside or on the rectangle.
            mClosestPoint0 = mLine->Origin +
                mLineParameter*mLine->Direction;

            mClosestPoint1 = mRectangle->Center +
                s0*mRectangle->Axis[0] + s1*mRectangle->Axis[1];

            return (Real)0;
        }
    }

    // Either (1) the line is not parallel to the rectangle and the point of
    // intersection of the line and the plane of the rectangle is outside the
    // rectangle or (2) the line and rectangle are parallel.  Regardless, the
    // closest point on the rectangle is on an edge of the rectangle.  Compare
    // the line to all four edges of the rectangle.
    Real sqrDist = Math<Real>::MAX_REAL;
    Vector3<Real> scaledDir[2] =
    {
        mRectangle->Extent[0]*mRectangle->Axis[0],
        mRectangle->Extent[1]*mRectangle->Axis[1]
    };
    for (int i1 = 0; i1 < 2; ++i1)
    {
        for (int i0 = 0; i0 < 2; ++i0)
        {
            Segment3<Real> segment;
            segment.Center = mRectangle->Center +
                ((Real)(2*i0-1))*scaledDir[i1];
            segment.Direction = mRectangle->Axis[1-i1];
            segment.Extent = mRectangle->Extent[1-i1];
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
                mRectCoord[0] = mRectangle->Extent[0]*((1-i1)*(2*i0-1) +
                    i1*ratio);
                mRectCoord[1] = mRectangle->Extent[1]*((1-i0)*(2*i1-1) +
                    i0*ratio);
            }
        }
    }

    return sqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Rectangle3<Real>::Get (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin = mLine->Origin + t*velocity0;
    Vector3<Real> movedCenter = mRectangle->Center + t*velocity1;
    Line3<Real> movedLine(movedOrigin, mLine->Direction);
    Rectangle3<Real> movedRectangle(movedCenter, mRectangle->Axis,
        mRectangle->Extent);
    return DistLine3Rectangle3<Real>(movedLine, movedRectangle).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Rectangle3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin = mLine->Origin + t*velocity0;
    Vector3<Real> movedCenter = mRectangle->Center + t*velocity1;
    Line3<Real> movedLine(movedOrigin, mLine->Direction);
    Rectangle3<Real> movedRectangle(movedCenter, mRectangle->Axis,
        mRectangle->Extent);
    return DistLine3Rectangle3<Real>(movedLine, movedRectangle).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Rectangle3<Real>::GetLineParameter () const
{
    return mLineParameter;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Rectangle3<Real>::GetRectangleCoordinate (int i) const
{
    return mRectCoord[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistLine3Rectangle3<float>;

template WM5_MATHEMATICS_ITEM
class DistLine3Rectangle3<double>;
//----------------------------------------------------------------------------
}
