// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrTriangle3Box3.h"
#include "Wm5IntrSegment3Box3.h"
#include "Wm5IntrUtility3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrTriangle3Box3<Real>::IntrTriangle3Box3 (const Triangle3<Real>& triangle,
    const Box3<Real>& box)
    :
    mTriangle(&triangle),
    mBox(&box)
{
    mQuantity = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle3<Real>& IntrTriangle3Box3<Real>::GetTriangle () const
{
    return *mTriangle;
}
//----------------------------------------------------------------------------
template <typename Real>
const Box3<Real>& IntrTriangle3Box3<Real>::GetBox () const
{
    return *mBox;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Box3<Real>::Test ()
{
    Real min0, max0, min1, max1;
    Vector3<Real> D, edge[3];

    // Test direction of triangle normal.
    edge[0] = mTriangle->V[1] - mTriangle->V[0];
    edge[1] = mTriangle->V[2] - mTriangle->V[0];
    D = edge[0].Cross(edge[1]);
    min0 = D.Dot(mTriangle->V[0]);
    max0 = min0;
    IntrAxis<Real>::GetProjection(D, *mBox, min1, max1);
    if (max1 < min0 || max0 < min1)
    {
        return false;
    }

    // Test direction of box faces.
    for (int i = 0; i < 3; ++i)
    {
        D = mBox->Axis[i];
        IntrAxis<Real>::GetProjection(D, *mTriangle, min0, max0);
        Real DdC = D.Dot(mBox->Center);
        min1 = DdC - mBox->Extent[i];
        max1 = DdC + mBox->Extent[i];
        if (max1 < min0 || max0 < min1)
        {
            return false;
        }
    }

    // Test direction of triangle-box edge cross products.
    edge[2] = edge[1] - edge[0];
    for (int i0 = 0; i0 < 3; ++i0)
    {
        for (int i1 = 0; i1 < 3; ++i1)
        {
            D = edge[i0].Cross(mBox->Axis[i1]);
            IntrAxis<Real>::GetProjection(D, *mTriangle, min0, max0);
            IntrAxis<Real>::GetProjection(D, *mBox, min1, max1);
            if (max1 < min0 || max0 < min1)
            {
                return false;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Box3<Real>::Find ()
{
    // Start with the triangle and clip it against each face of the box.
    mQuantity = 3;
    for (int i = 0; i < 3; ++i)
    {
        mPoint[i] = mTriangle->V[i];
    }

    for (int dir = -1; dir <= 1; dir += 2)
    {
        for (int side = 0; side < 3; ++side)
        {
            Vector3<Real> innerNormal = ((Real)dir)*mBox->Axis[side];
            Real constant = innerNormal.Dot(mBox->Center) -
                mBox->Extent[side];
            ClipConvexPolygonAgainstPlane(innerNormal, constant,
                mQuantity, mPoint);
        }
    }

    return mQuantity > 0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Box3<Real>::Test (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    // Process as if triangle is stationary, box is moving.
    Vector3<Real> relVelocity = velocity1 - velocity0;
    mContactTime = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;

    // Test direction of triangle normal.
    Vector3<Real> edge[3] =
    {
        mTriangle->V[1] - mTriangle->V[0],
        mTriangle->V[2] - mTriangle->V[0],
        mTriangle->V[2] - mTriangle->V[1]
    };
    Vector3<Real> D = edge[0].Cross(edge[1]);
    if (!IntrAxis<Real>::Test(D, *mTriangle, *mBox, relVelocity, tmax,
        mContactTime, tlast))
    {
        return false;
    }

    // Test direction of box faces.
    for (int i = 0; i < 3; ++i)
    {
        D = mBox->Axis[i];
        if (!IntrAxis<Real>::Test(D, *mTriangle, *mBox, relVelocity, tmax,
            mContactTime, tlast))
        {
            return false;
        }
    }

    // Test direction of triangle-box edge cross products.
    for (int i0 = 0; i0 < 3; ++i0)
    {
        for (int i1 = 0; i1 < 3; ++i1)
        {
            D = edge[i0].Cross(mBox->Axis[i1]);
            if (!IntrAxis<Real>::Test(D, *mTriangle, *mBox, relVelocity,
                tmax, mContactTime, tlast))
            {
                return false;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrTriangle3Box3<Real>::Find (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    // Process as if triangle is stationary, box is moving.
    Vector3<Real> relVelocity = velocity1 - velocity0;

    mContactTime = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;

    int side = IntrConfiguration<Real>::NONE;
    IntrConfiguration<Real> triContact, boxContact;

    // Test tri-normal.
    Vector3<Real> edge[3] =
    {
        mTriangle->V[1] - mTriangle->V[0],
        mTriangle->V[2] - mTriangle->V[1],
        mTriangle->V[0] - mTriangle->V[2]
    };

    Vector3<Real> triNorm = edge[0].Cross(edge[1]);
    if (!IntrAxis<Real>::Find(triNorm, *mTriangle, *mBox, relVelocity, tmax,
        mContactTime, tlast, side, triContact, boxContact))
    {
        return false;
    }

    Vector3<Real> axis;
    int coplanar = -1; // triangle coplanar to none of the box normals
    int i0;
    for (i0 = 0; i0 < 3; ++i0)
    {
        axis = mBox->Axis[i0];
        if (!IntrAxis<Real>::Find(axis, *mTriangle, *mBox, relVelocity,
            tmax, mContactTime, tlast, side, triContact, boxContact))
        {
            return false;
        }

        // Test if axis is parallel to triangle normal.  The test is:
        // sin(Angle(normal,axis)) < epsilon
        Real NdA = triNorm.Dot(axis);
        Real NdN = triNorm.SquaredLength();
        Real AdA = axis.SquaredLength();
        Real sn = Math<Real>::Sqrt(Math<Real>::FAbs((Real)1 -
            NdA*NdA/(NdN*AdA)));
        if (sn < Math<Real>::ZERO_TOLERANCE)
        {
            coplanar = i0;
        }
    }

    if (coplanar == -1)
    {
        // Test triedges cross boxfaces.
        for (i0 = 0; i0 < 3; ++i0)
        {
            for (int i1 = 0; i1 < 3; ++i1)
            {
                axis = edge[i0].Cross(mBox->Axis[i1]);
                if (!IntrAxis<Real>::Find(axis, *mTriangle, *mBox,
                    relVelocity, tmax, mContactTime, tlast, side, triContact,
                    boxContact))
                {
                    return false;
                }
            }
        }
    }
    else
    {
        // Test triedges cross coplanar box axis.
        for (i0 = 0; i0 < 3; ++i0)
        {
            axis = edge[i0].Cross(triNorm);
            if (!IntrAxis<Real>::Find(axis, *mTriangle, *mBox, relVelocity,
                tmax, mContactTime, tlast, side, triContact, boxContact))
            {
                return false;
            }
        }
    }

    // Test velocity cross box faces.
    for (i0 = 0; i0 < 3; ++i0)
    {
        axis = relVelocity.Cross(mBox->Axis[i0]);
        if (!IntrAxis<Real>::Find(axis, *mTriangle, *mBox, relVelocity,
            tmax, mContactTime, tlast, side, triContact, boxContact))
        {
            return false;
        }
    }

    if (mContactTime < (Real)0 || side == IntrConfiguration<Real>::NONE)
    {
        return false;
    }

    FindContactSet<Real>(*mTriangle, *mBox, side, triContact, boxContact,
        velocity0, velocity1, mContactTime, mQuantity, mPoint);

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrTriangle3Box3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrTriangle3Box3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrTriangle3Box3<float>;

template WM5_MATHEMATICS_ITEM
class IntrTriangle3Box3<double>;
//----------------------------------------------------------------------------
}
