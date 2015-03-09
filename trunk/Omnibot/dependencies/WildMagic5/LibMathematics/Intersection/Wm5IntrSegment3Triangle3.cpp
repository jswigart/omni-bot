// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2012/05/09)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrSegment3Triangle3.h"
#include "Wm5IntrUtility3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrSegment3Triangle3<Real>::IntrSegment3Triangle3 (
    const Segment3<Real>& segment, const Triangle3<Real>& triangle)
    :
    mSegment(&segment),
    mTriangle(&triangle)
{
    mQuantity = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment3<Real>& IntrSegment3Triangle3<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle3<Real>& IntrSegment3Triangle3<Real>::GetTriangle () const
{
    return *mTriangle;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Triangle3<Real>::Test ()
{
    // Compute the offset origin, edges, and normal.
    Vector3<Real> diff = mSegment->Center - mTriangle->V[0];
    Vector3<Real> edge1 = mTriangle->V[1] - mTriangle->V[0];
    Vector3<Real> edge2 = mTriangle->V[2] - mTriangle->V[0];
    Vector3<Real> normal = edge1.Cross(edge2);

    // Solve Q + t*D = b1*E1 + b2*E2 (Q = diff, D = segment direction,
    // E1 = edge1, E2 = edge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
    Real DdN = mSegment->Direction.Dot(normal);
    Real sign;
    if (DdN > Math<Real>::ZERO_TOLERANCE)
    {
        sign = (Real)1;
    }
    else if (DdN < -Math<Real>::ZERO_TOLERANCE)
    {
        sign = (Real)-1;
        DdN = -DdN;
    }
    else
    {
        // Segment and triangle are parallel, call it a "no intersection"
        // even if the segment does intersect.
        mIntersectionType = IT_EMPTY;
        return false;
    }

    Real DdQxE2 = sign*mSegment->Direction.Dot(diff.Cross(edge2));
    if (DdQxE2 >= (Real)0)
    {
        Real DdE1xQ = sign*mSegment->Direction.Dot(edge1.Cross(diff));
        if (DdE1xQ >= (Real)0)
        {
            if (DdQxE2 + DdE1xQ <= DdN)
            {
                // Line intersects triangle, check if segment does.
                Real QdN = -sign*diff.Dot(normal);
                Real extDdN = mSegment->Extent*DdN;
                if (-extDdN <= QdN && QdN <= extDdN)
                {
                    // Segment intersects triangle.
                    mIntersectionType = IT_POINT;
                    return true;
                }
                // else: |t| > extent, no intersection
            }
            // else: b1+b2 > 1, no intersection
        }
        // else: b2 < 0, no intersection
    }
    // else: b1 < 0, no intersection

    mIntersectionType = IT_EMPTY;
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Triangle3<Real>::Find ()
{
    // Compute the offset origin, edges, and normal.
    Vector3<Real> diff = mSegment->Center - mTriangle->V[0];
    Vector3<Real> edge1 = mTriangle->V[1] - mTriangle->V[0];
    Vector3<Real> edge2 = mTriangle->V[2] - mTriangle->V[0];
    Vector3<Real> normal = edge1.Cross(edge2);

    // Solve Q + t*D = b1*E1 + b2*E2 (Q = diff, D = segment direction,
    // E1 = edge1, E2 = edge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
    Real DdN = mSegment->Direction.Dot(normal);
    Real sign;
    if (DdN > Math<Real>::ZERO_TOLERANCE)
    {
        sign = (Real)1;
    }
    else if (DdN < -Math<Real>::ZERO_TOLERANCE)
    {
        sign = (Real)-1;
        DdN = -DdN;
    }
    else
    {
        // Segment and triangle are parallel, call it a "no intersection"
        // even if the segment does intersect.
        mIntersectionType = IT_EMPTY;
        return false;
    }

    Real DdQxE2 = sign*mSegment->Direction.Dot(diff.Cross(edge2));
    if (DdQxE2 >= (Real)0)
    {
        Real DdE1xQ = sign*mSegment->Direction.Dot(edge1.Cross(diff));
        if (DdE1xQ >= (Real)0)
        {
            if (DdQxE2 + DdE1xQ <= DdN)
            {
                // Line intersects triangle, check if segment does.
                Real QdN = -sign*diff.Dot(normal);
                Real extDdN = mSegment->Extent*DdN;
                if (-extDdN <= QdN && QdN <= extDdN)
                {
                    // Segment intersects triangle.
                    Real inv = ((Real)1)/DdN;
                    mSegmentParameter = QdN*inv;
                    mTriBary1 = DdQxE2*inv;
                    mTriBary2 = DdE1xQ*inv;
                    mTriBary0 = (Real)1 - mTriBary1 - mTriBary2;
                    return true;
                }
                // else: |t| > extent, no intersection
            }
            // else: b1+b2 > 1, no intersection
        }
        // else: b2 < 0, no intersection
    }
    // else: b1 < 0, no intersection

    mIntersectionType = IT_EMPTY;
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntrSegment3Triangle3<Real>::GetSegmentParameter () const
{
    return mSegmentParameter;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntrSegment3Triangle3<Real>::GetTriBary0 () const
{
    return mTriBary0;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntrSegment3Triangle3<Real>::GetTriBary1 () const
{
    return mTriBary1;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntrSegment3Triangle3<Real>::GetTriBary2 () const
{
    return mTriBary2;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Triangle3<Real>::Test (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    mQuantity = 0;

    // Get the endpoints of the segment.
    Vector3<Real> segment[2] =
    {
        mSegment->P0,
        mSegment->P1
    };

    // Get the triangle edges.
    Vector3<Real> edge0 = mTriangle->V[1] - mTriangle->V[0];
    Vector3<Real> edge1 = mTriangle->V[2] - mTriangle->V[0];

    // Get the triangle velocity relative to the segment.
    Vector3<Real> relVelocity = velocity1 - velocity0;

    mContactTime = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;

    // Test tri-normal.
    Vector3<Real> normV = edge0.Cross(edge1);
    if (!IntrAxis<Real>::Test(normV, segment, *mTriangle, relVelocity, tmax,
        mContactTime, tlast))
    {
        return false;
    }

    // Test whether the segment is parallel to the triangle, effectively the
    // test:  sin(Angle(NormV,DirU)) > 1-epsilon
    Vector3<Real> dirU = segment[1] - segment[0];
    Vector3<Real> normU = normV.Cross(dirU);
    Real dirSqrLen = dirU.SquaredLength();
    Real norUSqrLen = normU.SquaredLength();
    Real norVSqrLen = normV.SquaredLength();
    Real oneMinusEpsilon = (Real)1 - Math<Real>::ZERO_TOLERANCE;

    int i0, i1;
    Vector3<Real> axis;

    if (norUSqrLen > oneMinusEpsilon*norVSqrLen*dirSqrLen)  // parallel
    {
        // Test tri-normal cross seg-direction.
        if (!IntrAxis<Real>::Test(normU, segment, *mTriangle, relVelocity,
            tmax, mContactTime, tlast))
        {
            return false;
        }

        // Test tri-normal cross tri-edges.
        for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
        {
            axis = normV.Cross(mTriangle->V[i1] -
                mTriangle->V[i0]);

            if (!IntrAxis<Real>::Test(axis, segment, *mTriangle,
                relVelocity, tmax, mContactTime, tlast))
            {
                return false;
            }
        }
    }
    else  // not parallel
    {
        // Test seg-direction cross tri-edges.
        for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
        {
            axis = dirU.Cross(mTriangle->V[i1] -
                mTriangle->V[i0]);

            if (!IntrAxis<Real>::Test(axis, segment, *mTriangle,
                relVelocity, tmax, mContactTime, tlast))
            {
                return false;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Triangle3<Real>::Find (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    mQuantity = 0;
    mIntersectionType = IT_EMPTY;

    // Get the endpoints of the segment.
    Vector3<Real> segment[2] =
    {
        mSegment->P0,
        mSegment->P1
    };

    // Get the triangle edges.
    Vector3<Real> edge0 = mTriangle->V[1] - mTriangle->V[0];
    Vector3<Real> edge1 = mTriangle->V[2] - mTriangle->V[0];

    // Get the triangle velocity relative to the segment.
    Vector3<Real> relVelocity = velocity1 - velocity0;

    mContactTime = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;

    // Test tri-normal.
    Vector3<Real> normV = edge0.Cross(edge1);
    IntrConfiguration<Real> segContact, triContact;
    int side;
    if (!IntrAxis<Real>::Find(normV, segment, *mTriangle, relVelocity,
        tmax, mContactTime, tlast, side, segContact, triContact))
    {
        return false;
    }

    // Test whether the segment is parallel to the triangle, effectively the
    // test:  sin(Angle(NormV,DirU)) > 1-epsilon
    Vector3<Real> dirU = segment[1] - segment[0];
    Vector3<Real> normU = normV.Cross(dirU);
    Real dirSqrLen = dirU.SquaredLength();
    Real norUSqrLen = normU.SquaredLength();
    Real norVSqrLen = normV.SquaredLength();
    Real oneMinusEpsilon = (Real)1 - Math<Real>::ZERO_TOLERANCE;

    int i0, i1;
    Vector3<Real> axis;

    if (norUSqrLen > oneMinusEpsilon*norVSqrLen*dirSqrLen)  // parallel
    {
        // Find tri-normal cross seg-direction.
        if (!IntrAxis<Real>::Find(normU, segment, *mTriangle, relVelocity,
            tmax, mContactTime, tlast, side, segContact, triContact))
        {
            return false;
        }

        // Find tri-normal cross tri-edges.
        for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
        {
            axis = normV.Cross(mTriangle->V[i1] - mTriangle->V[i0]);

            if (!IntrAxis<Real>::Find(axis, segment, *mTriangle,
                relVelocity, tmax, mContactTime, tlast, side, segContact,
                triContact))
            {
                return false;
            }
        }
    } 
    else 
    {
        // Test seg-direction cross tri-edges.
        for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
        {
            axis = dirU.Cross(mTriangle->V[i1] - mTriangle->V[i0]);

            if (!IntrAxis<Real>::Find(axis, segment, *mTriangle,
                relVelocity, tmax, mContactTime, tlast, side, segContact,
                triContact))
            {
                return false;
            }
        }
    }

    if (mContactTime < (Real)0)
    {
        return false;
    }

    FindContactSet<Real>(segment, *mTriangle, side, segContact,
        triContact, velocity0, velocity1, mContactTime, mQuantity,
        mPoint);

    if (mQuantity == 1)
    {
        mIntersectionType = IT_POINT;
    }
    else
    {
        mIntersectionType = IT_SEGMENT;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrSegment3Triangle3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrSegment3Triangle3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrSegment3Triangle3<float>;

template WM5_MATHEMATICS_ITEM
class IntrSegment3Triangle3<double>;
//----------------------------------------------------------------------------
}
