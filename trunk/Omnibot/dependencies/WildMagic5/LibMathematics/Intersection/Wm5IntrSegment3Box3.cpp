// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrSegment3Box3.h"
#include "Wm5IntrLine3Box3.h"
#include "Wm5IntrUtility3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrSegment3Box3<Real>::IntrSegment3Box3 (const Segment3<Real>& segment,
    const Box3<Real>& box, bool solid)
    :
    mSegment(&segment),
    mBox(&box)
{
    mQuantity = 0;
    mSolid = solid;
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment3<Real>& IntrSegment3Box3<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
const Box3<Real>& IntrSegment3Box3<Real>::GetBox () const
{
    return *mBox;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Box3<Real>::Test ()
{
    Real AWdU[3], ADdU[3], AWxDdU[3], RHS;

    Vector3<Real> diff = mSegment->Center - mBox->Center;

    AWdU[0] = Math<Real>::FAbs(mSegment->Direction.Dot(mBox->Axis[0]));
    ADdU[0] = Math<Real>::FAbs(diff.Dot(mBox->Axis[0]));
    RHS = mBox->Extent[0] + mSegment->Extent*AWdU[0];
    if (ADdU[0] > RHS)
    {
        return false;
    }

    AWdU[1] = Math<Real>::FAbs(mSegment->Direction.Dot(mBox->Axis[1]));
    ADdU[1] = Math<Real>::FAbs(diff.Dot(mBox->Axis[1]));
    RHS = mBox->Extent[1] + mSegment->Extent*AWdU[1];
    if (ADdU[1] > RHS)
    {
        return false;
    }

    AWdU[2] = Math<Real>::FAbs(mSegment->Direction.Dot(mBox->Axis[2]));
    ADdU[2] = Math<Real>::FAbs(diff.Dot(mBox->Axis[2]));
    RHS = mBox->Extent[2] + mSegment->Extent*AWdU[2];
    if (ADdU[2] > RHS)
    {
        return false;
    }

    Vector3<Real> WxD = mSegment->Direction.Cross(diff);

    AWxDdU[0] = Math<Real>::FAbs(WxD.Dot(mBox->Axis[0]));
    RHS = mBox->Extent[1]*AWdU[2] + mBox->Extent[2]*AWdU[1];
    if (AWxDdU[0] > RHS)
    {
        return false;
    }

    AWxDdU[1] = Math<Real>::FAbs(WxD.Dot(mBox->Axis[1]));
    RHS = mBox->Extent[0]*AWdU[2] + mBox->Extent[2]*AWdU[0];
    if (AWxDdU[1] > RHS)
    {
        return false;
    }

    AWxDdU[2] = Math<Real>::FAbs(WxD.Dot(mBox->Axis[2]));
    RHS = mBox->Extent[0]*AWdU[1] + mBox->Extent[1]*AWdU[0];
    if (AWxDdU[2] > RHS)
    {
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Box3<Real>::Find ()
{
    Real t0 = -mSegment->Extent;
    Real t1 = mSegment->Extent;
    return IntrLine3Box3<Real>::DoClipping(t0, t1, mSegment->Center,
        mSegment->Direction, *mBox, mSolid, mQuantity, mPoint,
        mIntersectionType);
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Box3<Real>::Test (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    mQuantity = 0;

    // Get the endpoints of the segment.
    Vector3<Real> segment[2] =
    {
        mSegment->P0,
        mSegment->P1
    };

    // Get the box velocity relative to the segment.
    Vector3<Real> relVelocity = velocity1 - velocity0;

    mContactTime = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;

    int i;
    Vector3<Real> axis;

    // Test box normals.
    for (i = 0; i < 3; ++i)
    {
        axis = mBox->Axis[i];
        if (!IntrAxis<Real>::Test(axis, segment, *mBox, relVelocity, tmax,
            mContactTime, tlast))
        {
            return false;
        }
    }

    // Test seg-direction cross box-edges.
    for (i = 0; i < 3; ++i)
    {
        axis = mBox->Axis[i].Cross(mSegment->Direction);
        if (!IntrAxis<Real>::Test(axis, segment, *mBox, relVelocity, tmax,
            mContactTime, tlast))
        {
            return false;
        }
    }

    // Test velocity cross box-faces.
    for (i = 0; i < 3; i++)
    {
        axis = relVelocity.Cross(mBox->Axis[i]);
        if (!IntrAxis<Real>::Test(axis, segment, *mBox, relVelocity, tmax,
            mContactTime, tlast))
        {
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Box3<Real>::Find (Real tmax,
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

    // Get the box velocity relative to the segment.
    Vector3<Real> relVelocity = velocity1 - velocity0;

    mContactTime = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;

    int i;
    Vector3<Real> axis;
    int side = IntrConfiguration<Real>::NONE;
    IntrConfiguration<Real> segContact, boxContact;

    // Test box normals.
    for (i = 0; i < 3; ++i)
    {
        axis = mBox->Axis[i];
        if (!IntrAxis<Real>::Find(axis, segment, *mBox, relVelocity, tmax,
            mContactTime, tlast, side, segContact, boxContact))
        {
            return false;
        }
    }

    // Test seg-direction cross box-edges.
    for (i = 0; i < 3; i++)
    {
        axis = mBox->Axis[i].Cross(mSegment->Direction);
        if (!IntrAxis<Real>::Find(axis, segment, *mBox, relVelocity, tmax,
            mContactTime, tlast, side, segContact, boxContact))
        {
            return false;
        }
    }

    // Test velocity cross box-faces.
    for (i = 0; i < 3; i++)
    {
        axis = relVelocity.Cross(mBox->Axis[i]);
        if (!IntrAxis<Real>::Find(axis, segment, *mBox, relVelocity, tmax,
            mContactTime, tlast, side, segContact, boxContact))
        {
            return false;
        }
    }

    if (mContactTime < (Real)0 || side == IntrConfiguration<Real>::NONE)
    {
        // intersecting now
        return false;
    }

    FindContactSet<Real>(segment, *mBox, side, segContact, boxContact,
        velocity0, velocity1, mContactTime, mQuantity, mPoint);

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
int IntrSegment3Box3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrSegment3Box3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrSegment3Box3<float>;

template WM5_MATHEMATICS_ITEM
class IntrSegment3Box3<double>;
//----------------------------------------------------------------------------
}
