// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrBox2Circle2.h"
#include "Wm5DistPoint2Box2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrBox2Circle2<Real>::IntrBox2Circle2 (const Box2<Real>& box,
    const Circle2<Real>& circle)
    :
    mBox(&box),
    mCircle(&circle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Box2<Real>& IntrBox2Circle2<Real>::GetBox () const
{
    return *mBox;
}
//----------------------------------------------------------------------------
template <typename Real>
const Circle2<Real>& IntrBox2Circle2<Real>::GetCircle () const
{
    return *mCircle;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrBox2Circle2<Real>::Test ()
{
    Real distance = DistPoint2Box2<Real>(mCircle->Center, *mBox).Get();
    return distance <= mCircle->Radius;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrBox2Circle2<Real>::Find (Real tmax,
    const Vector2<Real>& velocity0, const Vector2<Real>& velocity1)
{
    // Convert circle center to box coordinates.
    Vector2<Real> diff = mCircle->Center - mBox->Center;
    Vector2<Real> vel = velocity1 - velocity0;
    Real cx = diff.Dot(mBox->Axis[0]);
    Real cy = diff.Dot(mBox->Axis[1]);
    Real vx = vel.Dot(mBox->Axis[0]);
    Real vy = vel.Dot(mBox->Axis[1]);
    Real ex = mBox->Extent[0];
    Real ey = mBox->Extent[1];
    Real ix, iy;

    int type = 0;

    if (cx < -ex)
    {
        if (cy < -ey)
        {
            // region Rmm
            type = TestVertexRegion(cx, cy, vx, vy, ex, ey, ix, iy);
        }
        else if (cy <= ey)
        {
            // region Rmz
            type = TestEdgeRegion(cx, cy, vx, vy, ex, ey, ix, iy);
        }
        else
        {
            // region Rmp
            type = TestVertexRegion(cx, -cy, vx, -vy, ex, ey, ix, iy);
            iy = -iy;
        }
    }
    else if (cx <= ex)
    {
        if (cy < -ey)
        {
            // region Rzm
            type = TestEdgeRegion(cy, cx, vy, vx, ey, ex, iy, ix);
        }
        else if (cy <= ey)
        {
            // region Rzz: The circle is already intersecting the box.  Use
            // the circle center as the intersection point, but let the
            // caller know that the objects overlap by returning an "other"
            // intersection.
            mContactTime = (Real)0;
            mContactPoint = mCircle->Center;
            mIntersectionType = IT_OTHER;
            return true;
        }
        else
        {
            // region Rzp
            type = TestEdgeRegion(-cy, cx, -vy, vx, ey, ex, iy, ix);
            iy = -iy;
        }
    }
    else
    {
        if (cy < -ey)
        {
            // region Rpm
            type = TestVertexRegion(-cx, cy, -vx, vy, ex, ey, ix, iy);
            ix = -ix;
        }
        else if (cy <= ey)
        {
            // region Rpz
            type = TestEdgeRegion(-cx, cy, -vx, vy, ex, ey, ix, iy);
            ix = -ix;
        }
        else
        {
            // region Rpp
            type = TestVertexRegion(-cx, -cy, -vx, -vy, ex, ey, ix, iy);
            ix = -ix;
            iy = -iy;
        }
    }

    if (type != 1 || mContactTime > tmax)
    {
        mIntersectionType = IT_EMPTY;
        return false;
    }

    mContactPoint = mBox->Center + ix*mBox->Axis[0] + iy*mBox->Axis[1];
    mIntersectionType = IT_POINT;
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrBox2Circle2<Real>::GetContactPoint () const
{
    return mContactPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrBox2Circle2<Real>::TestVertexRegion (Real cx, Real cy, Real vx,
    Real vy, Real ex, Real ey, Real& ix, Real& iy)
{
    Real dx = cx + ex;
    Real dy = cy + ey;
    Real rsqr = mCircle->Radius*mCircle->Radius;
    Real diff = dx*dx + dy*dy - rsqr;
    if (diff <= (Real)0)
    {
        // Circle is already intersecting the box.
        mContactTime = (Real)0;
        return -1;
    }

    Real dot = vx*dx + vy*dy;
    if (dot >= (Real)0)
    {
        // Circle not moving towards box.
        return 0;
    }

    Real dotPerp = vx*dy - vy*dx;
    Real vsqr, inv;

    if (dotPerp >= (Real)0)
    {
        // Potential contact on left edge.
        if (dotPerp <= mCircle->Radius*vy)
        {
            // Lower left corner is first point of contact.
            ix = -ex;
            iy = -ey;
            vsqr = vx*vx + vy*vy;
            inv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                dot*dot - vsqr*diff));
            mContactTime = diff*inv/((Real)1 - dot*inv);
            return 1;
        }

        if (vx <= (Real)0)
        {
            // Passed corner, moving away from box.
            return 0;
        }

        vsqr = vx*vx + vy*vy;
        dy = cy - ey;
        dotPerp = vx*dy - vy*dx;
        if (dotPerp >= (Real)0 && dotPerp*dotPerp > rsqr*vsqr)
        {
            // Circle misses box.
            return 0;
        }

        // Circle will intersect box.  Determine first time and place of
        // contact with x = xmin.
        ix = -ex;

        if (dotPerp <= mCircle->Radius*vy)
        {
            // First contact on left edge of box.
            mContactTime = -(dx + mCircle->Radius)/vx;
            iy = cy + mContactTime*vy;
        }
        else
        {
            // First contact at upper left corner of box.
            dot = vx*dx + vy*dy;
            diff = dx*dx + dy*dy - rsqr;
            inv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                dot*dot - vsqr*diff));
            mContactTime = diff*inv/((Real)1 - dot*inv);
            iy = ey;
        }
    }
    else
    {
        // Potential contact on bottom edge.
        if (-dotPerp <= mCircle->Radius*vx)
        {
            // Lower left corner is first point of contact.
            ix = -ex;
            iy = -ey;
            vsqr = vx*vx + vy*vy;
            inv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                dot*dot - vsqr*diff));
            mContactTime = diff*inv/((Real)1 - dot*inv);
            return 1;
        }

        if (vy <= (Real)0)
        {
            // Passed corner, moving away from box.
            return 0;
        }

        vsqr = vx*vx + vy*vy;
        dx = cx - ex;
        dotPerp = vx*dy - vy*dx;
        if (-dotPerp >= (Real)0 && dotPerp*dotPerp > rsqr*vsqr)
        {
            // Circle misses box.
            return 0;
        }

        // Circle will intersect box.  Determine first time and place of
        // contact with y = ymin.
        iy = -ey;

        if (-dotPerp <= mCircle->Radius*vx)
        {
            // First contact on bottom edge of box.
            mContactTime = -(dy + mCircle->Radius)/vy;
            ix = cx + mContactTime*vx;
        }
        else
        {
            // First contact at lower right corner of box.
            dot = vx*dx + vy*dy;
            diff = dx*dx + dy*dy - rsqr;
            inv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                dot*dot - vsqr*diff));
            mContactTime = diff*inv/((Real)1 - dot*inv);
            ix = ex;
        }
    }

    return 1;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrBox2Circle2<Real>::TestEdgeRegion (Real cx, Real cy, Real vx,
    Real vy, Real ex, Real ey, Real& ix, Real& iy)
{
    Real dx = cx + ex;
    Real xSignedDist = dx + mCircle->Radius;
    if (xSignedDist >= (Real)0)
    {
        // Circle is already intersecting the box.
        mContactTime = (Real)0;
        return -1;
    }

    if (vx <= (Real)0)
    {
        // Circle not moving towards box.
        return 0;
    }

    Real rsqr = mCircle->Radius*mCircle->Radius;
    Real vsqr = vx*vx + vy*vy;
    Real dy, dot, dotPerp, diff, inv;

    if (vy >= (Real)0)
    {
        dy = cy - ey;
        dotPerp = vx*dy - vy*dx;
        if (dotPerp >= (Real)0 && dotPerp*dotPerp > rsqr*vsqr)
        {
            // Circle misses box.
            return 0;
        }

        // Circle will intersect box.  Determine first time and place of
        // contact with x = xmin.
        ix = -ex;

        if (dotPerp <= mCircle->Radius*vy)
        {
            // First contact on left edge of box.
            mContactTime = -xSignedDist/vx;
            iy = cy + mContactTime*vy;
        }
        else
        {
            // First contact at corner of box.
            dot = vx*dx + vy*dy;
            diff = dx*dx + dy*dy - rsqr;
            inv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                dot*dot - vsqr*diff));
            mContactTime = diff*inv/((Real)1 - dot*inv);
            iy = ey;
        }
    }
    else
    {
        dy = cy + ey;
        dotPerp = vx*dy - vy*dx;
        if (dotPerp <= (Real)0 && dotPerp*dotPerp > rsqr*vsqr)
        {
            // Circle misses box.
            return 0;
        }

        // Circle will intersect box.  Determine first time and place of
        // contact with x = xmin.
        ix = -ex;

        if (dotPerp >= mCircle->Radius*vy)
        {
            // First contact on left edge of box.
            mContactTime = -xSignedDist/vx;
            iy = cy + mContactTime*vy;
        }
        else
        {
            // First contact at corner of box.
            dot = vx*dx + vy*dy;
            diff = dx*dx + dy*dy - rsqr;
            inv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                dot*dot - vsqr*diff));
            mContactTime = diff*inv/((Real)1 - dot*inv);
            iy = -ey;
        }
    }

    return 1;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrBox2Circle2<float>;

template WM5_MATHEMATICS_ITEM
class IntrBox2Circle2<double>;
//----------------------------------------------------------------------------
}
