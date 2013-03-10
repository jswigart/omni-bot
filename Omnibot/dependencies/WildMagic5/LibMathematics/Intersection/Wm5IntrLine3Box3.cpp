// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrLine3Box3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrLine3Box3<Real>::IntrLine3Box3 (const Line3<Real>& line,
    const Box3<Real>& box)
    :
    mLine(&line),
    mBox(&box)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& IntrLine3Box3<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Box3<Real>& IntrLine3Box3<Real>::GetBox () const
{
    return *mBox;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine3Box3<Real>::Test ()
{
    Real AWdU[3], AWxDdU[3], RHS;

    Vector3<Real> diff = mLine->Origin - mBox->Center;
    Vector3<Real> WxD = mLine->Direction.Cross(diff);

    AWdU[1] = Math<Real>::FAbs(mLine->Direction.Dot(mBox->Axis[1]));
    AWdU[2] = Math<Real>::FAbs(mLine->Direction.Dot(mBox->Axis[2]));
    AWxDdU[0] = Math<Real>::FAbs(WxD.Dot(mBox->Axis[0]));
    RHS = mBox->Extent[1]*AWdU[2] + mBox->Extent[2]*AWdU[1];
    if (AWxDdU[0] > RHS)
    {
        return false;
    }

    AWdU[0] = Math<Real>::FAbs(mLine->Direction.Dot(mBox->Axis[0]));
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
bool IntrLine3Box3<Real>::Find ()
{
    Real t0 = -Math<Real>::MAX_REAL;
    Real t1 = Math<Real>::MAX_REAL;
    return DoClipping(t0, t1, mLine->Origin, mLine->Direction, *mBox,
        true, mQuantity, mPoint, mIntersectionType);
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrLine3Box3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrLine3Box3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine3Box3<Real>::DoClipping (Real t0, Real t1,
    const Vector3<Real>& origin, const Vector3<Real>& direction,
    const Box3<Real>& box, bool solid, int& quantity, Vector3<Real> point[2],
    int& intrType)
{
    // Convert linear component to box coordinates.
    Vector3<Real> diff = origin - box.Center;
    Vector3<Real> BOrigin(
        diff.Dot(box.Axis[0]),
        diff.Dot(box.Axis[1]),
        diff.Dot(box.Axis[2])
    );
    Vector3<Real> BDirection(
        direction.Dot(box.Axis[0]),
        direction.Dot(box.Axis[1]),
        direction.Dot(box.Axis[2])
    );

    Real saveT0 = t0, saveT1 = t1;
    bool notAllClipped =
        Clip(+BDirection.X(), -BOrigin.X()-box.Extent[0], t0, t1) &&
        Clip(-BDirection.X(), +BOrigin.X()-box.Extent[0], t0, t1) &&
        Clip(+BDirection.Y(), -BOrigin.Y()-box.Extent[1], t0, t1) &&
        Clip(-BDirection.Y(), +BOrigin.Y()-box.Extent[1], t0, t1) &&
        Clip(+BDirection.Z(), -BOrigin.Z()-box.Extent[2], t0, t1) &&
        Clip(-BDirection.Z(), +BOrigin.Z()-box.Extent[2], t0, t1);

    if (notAllClipped && (solid || t0 != saveT0 || t1 != saveT1))
    {
        if (t1 > t0)
        {
            intrType = IT_SEGMENT;
            quantity = 2;
            point[0] = origin + t0*direction;
            point[1] = origin + t1*direction;
        }
        else
        {
            intrType = IT_POINT;
            quantity = 1;
            point[0] = origin + t0*direction;
        }
    }
    else
    {
        quantity = 0;
        intrType = IT_EMPTY;
    }

    return intrType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine3Box3<Real>::Clip (Real denom, Real numer, Real& t0, Real& t1)
{
    // Return value is 'true' if line segment intersects the current test
    // plane.  Otherwise 'false' is returned in which case the line segment
    // is entirely clipped.

    if (denom > (Real)0)
    {
        if (numer > denom*t1)
        {
            return false;
        }
        if (numer > denom*t0)
        {
            t0 = numer/denom;
        }
        return true;
    }
    else if (denom < (Real)0)
    {
        if (numer > denom*t0)
        {
            return false;
        }
        if (numer > denom*t1)
        {
            t1 = numer/denom;
        }
        return true;
    }
    else
    {
        return numer <= (Real)0;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrLine3Box3<float>;

template WM5_MATHEMATICS_ITEM
class IntrLine3Box3<double>;
//----------------------------------------------------------------------------
}
