// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrPlane3Triangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrPlane3Triangle3<Real>::IntrPlane3Triangle3 (const Plane3<Real>& plane,
    const Triangle3<Real>& triangle, Real epsilon)
    :
    mPlane(&plane),
    mTriangle(&triangle)
{
    mEpsilon = (epsilon >= (Real)0 ? epsilon : (Real)0);
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrPlane3Triangle3<Real>::GetPlane () const
{
    return *mPlane;
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle3<Real>& IntrPlane3Triangle3<Real>::GetTriangle () const
{
    return *mTriangle;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Triangle3<Real>::Test ()
{
    // Compute the signed distances from the vertices to the plane.
    Real zero = (Real)0;
    Real SD[3];
    for (int i = 0; i < 3; ++i)
    {
        SD[i] = mPlane->DistanceTo(mTriangle->V[i]);
        if (Math<Real>::FAbs(SD[i]) <= mEpsilon)
        {
            SD[i] = zero;
        }
    };

    // The triangle intersects the plane if not all vertices are on the
    // positive side of the plane and not all vertices are on the negative
    // side of the plane.
    return !(SD[0] > zero && SD[1] > zero && SD[2] > zero)
        && !(SD[0] < zero && SD[1] < zero && SD[2] < zero);
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Triangle3<Real>::Find ()
{
    // Compute the signed distances from the vertices to the plane.
    Real zero = (Real)0;
    Real SD[3];
    for (int i = 0; i < 3; ++i)
    {
        SD[i] = mPlane->DistanceTo(mTriangle->V[i]);
        if (Math<Real>::FAbs(SD[i]) <= mEpsilon)
        {
            SD[i] = zero;
        }
    };

    const Vector3<Real>& V0 = mTriangle->V[0];
    const Vector3<Real>& V1 = mTriangle->V[1];
    const Vector3<Real>& V2 = mTriangle->V[2];

    if (SD[0] > zero)
    {
        if (SD[1] > zero)
        {
            if (SD[2] > zero)
            {
                // ppp
                mQuantity = 0;
                mIntersectionType = IT_EMPTY;
            }
            else if (SD[2] < zero)
            {
                // ppm
                mQuantity = 2;
                mPoint[0] = V0 + (SD[0]/(SD[0] - SD[2]))*(V2 - V0);
                mPoint[1] = V1 + (SD[1]/(SD[1] - SD[2]))*(V2 - V1);
                mIntersectionType = IT_SEGMENT;
            }
            else
            {
                // ppz
                mQuantity = 1;
                mPoint[0] = V2;
                mIntersectionType = IT_POINT;
            }
        }
        else if (SD[1] < zero)
        {
            if (SD[2] > zero)
            {
                // pmp
                mQuantity = 2;
                mPoint[0] = V0 + (SD[0]/(SD[0] - SD[1]))*(V1 - V0);
                mPoint[1] = V1 + (SD[1]/(SD[1] - SD[2]))*(V2 - V1);
                mIntersectionType = IT_SEGMENT;
            }
            else if (SD[2] < zero)
            {
                // pmm
                mQuantity = 2;
                mPoint[0] = V0 + (SD[0]/(SD[0] - SD[1]))*(V1 - V0);
                mPoint[1] = V0 + (SD[0]/(SD[0] - SD[2]))*(V2 - V0);
                mIntersectionType = IT_SEGMENT;
            }
            else
            {
                // pmz
                mQuantity = 2;
                mPoint[0] = V0 + (SD[0]/(SD[0] - SD[1]))*(V1 - V0);
                mPoint[1] = V2;
                mIntersectionType = IT_SEGMENT;
            }
        }
        else
        {
            if (SD[2] > zero)
            {
                // pzp
                mQuantity = 1;
                mPoint[0] = V1;
                mIntersectionType = IT_POINT;
            }
            else if (SD[2] < zero)
            {
                // pzm
                mQuantity = 2;
                mPoint[0] = V0 + (SD[0]/(SD[0] - SD[2]))*(V2 - V0);
                mPoint[1] = V1;
                mIntersectionType = IT_SEGMENT;
            }
            else
            {
                // pzz
                mQuantity = 2;
                mPoint[0] = V1;
                mPoint[1] = V2;
                mIntersectionType = IT_SEGMENT;
            }
        }
    }
    else if (SD[0] < zero)
    {
        if (SD[1] > zero)
        {
            if (SD[2] > zero)
            {
                // mpp
                mQuantity = 2;
                mPoint[0] = V0 + (SD[0]/(SD[0] - SD[1]))*(V1 - V0);
                mPoint[1] = V0 + (SD[0]/(SD[0] - SD[2]))*(V2 - V0);
                mIntersectionType = IT_SEGMENT;
            }
            else if (SD[2] < zero)
            {
                // mpm
                mQuantity = 2;
                mPoint[0] = V0 +(SD[0]/(SD[0] - SD[1]))*(V1 - V0);
                mPoint[1] = V1 +(SD[1]/(SD[1] - SD[2]))*(V2 - V1);
                mIntersectionType = IT_SEGMENT;
            }
            else
            {
                // mpz
                mQuantity = 2;
                mPoint[0] = V0 + (SD[0]/(SD[0] - SD[1]))*(V1 - V0);
                mPoint[1] = V2;
                mIntersectionType = IT_SEGMENT;
            }
        }
        else if (SD[1] < zero)
        {
            if (SD[2] > zero)
            {
                // mmp
                mQuantity = 2;
                mPoint[0] = V0 + (SD[0]/(SD[0] - SD[2]))*(V2 - V0);
                mPoint[1] = V1 + (SD[1]/(SD[1] - SD[2]))*(V2 - V1);
                mIntersectionType = IT_SEGMENT;
            }
            else if (SD[2] < zero)
            {
                // mmm
                mQuantity = 0;
                mIntersectionType = IT_EMPTY;
            }
            else
            {
                // mmz
                mQuantity = 1;
                mPoint[0] = mTriangle->V[2];
                mIntersectionType = IT_POINT;
            }
        }
        else
        {
            if (SD[2] > zero)
            {
                // mzp
                mQuantity = 2;
                mPoint[0] = V0 + (SD[0]/(SD[0] - SD[2]))*(V2 - V0);
                mPoint[1] = V1;
                mIntersectionType = IT_SEGMENT;
            }
            else if (SD[2] < zero)
            {
                // mzm
                mQuantity = 1;
                mPoint[0] = V1;
                mIntersectionType = IT_POINT;
            }
            else
            {
                // mzz
                mQuantity = 2;
                mPoint[0] = V1;
                mPoint[1] = V2;
                mIntersectionType = IT_SEGMENT;
            }
        }
    }
    else
    {
        if (SD[1] > zero)
        {
            if (SD[2] > zero)
            {
                // zpp
                mQuantity = 1;
                mPoint[0] = V0;
                mIntersectionType = IT_POINT;
            }
            else if (SD[2] < zero)
            {
                // zpm
                mQuantity = 2;
                mPoint[0] = V1 + (SD[1]/(SD[1] - SD[2]))*(V2 - V1);
                mPoint[1] = V0;
                mIntersectionType = IT_SEGMENT;
            }
            else
            {
                // zpz
                mQuantity = 2;
                mPoint[0] = V0;
                mPoint[1] = V2;
                mIntersectionType = IT_SEGMENT;
            }
        }
        else if (SD[1] < zero)
        {
            if (SD[2] > zero)
            {
                // zmp
                mQuantity = 2;
                mPoint[0] = V1 + (SD[1]/(SD[1] - SD[2]))*(V2 - V1);
                mPoint[1] = V0;
                mIntersectionType = IT_SEGMENT;
            }
            else if (SD[2] < zero)
            {
                // zmm
                mQuantity = 1;
                mPoint[0] = V0;
                mIntersectionType = IT_POINT;
            }
            else
            {
                // zmz
                mQuantity = 2;
                mPoint[0] = V0;
                mPoint[1] = V2;
                mIntersectionType = IT_SEGMENT;
            }
        }
        else
        {
            if (SD[2] > zero)
            {
                // zzp
                mQuantity = 2;
                mPoint[0] = V0;
                mPoint[1] = V1;
                mIntersectionType = IT_SEGMENT;
            }
            else if (SD[2] < zero)
            {
                // zzm
                mQuantity = 2;
                mPoint[0] = V0;
                mPoint[1] = V1;
                mIntersectionType = IT_SEGMENT;
            }
            else
            {
                // zzz
                mQuantity = 3;
                mPoint[0] = V0;
                mPoint[1] = V1;
                mPoint[2] = V2;
                mIntersectionType = IT_POLYGON;
            }
        }
    }

    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrPlane3Triangle3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrPlane3Triangle3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrPlane3Triangle3<float>;

template WM5_MATHEMATICS_ITEM
class IntrPlane3Triangle3<double>;
//----------------------------------------------------------------------------
}
