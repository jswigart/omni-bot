// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrRay3Box3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrRay3Box3<Real>::IntrRay3Box3 (const Ray3<Real>& rkRay,
    const Box3<Real>& rkBox)
    :
    mRay(&rkRay),
    mBox(&rkBox)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray3<Real>& IntrRay3Box3<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
const Box3<Real>& IntrRay3Box3<Real>::GetBox () const
{
    return *mBox;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay3Box3<Real>::Test ()
{
    Real WdU[3], AWdU[3], DdU[3], ADdU[3], AWxDdU[3], RHS;

    Vector3<Real> diff = mRay->Origin - mBox->Center;

    WdU[0] = mRay->Direction.Dot(mBox->Axis[0]);
    AWdU[0] = Math<Real>::FAbs(WdU[0]);
    DdU[0] = diff.Dot(mBox->Axis[0]);
    ADdU[0] = Math<Real>::FAbs(DdU[0]);
    if (ADdU[0] > mBox->Extent[0] && DdU[0]*WdU[0] >= (Real)0)
    {
        return false;
    }

    WdU[1] = mRay->Direction.Dot(mBox->Axis[1]);
    AWdU[1] = Math<Real>::FAbs(WdU[1]);
    DdU[1] = diff.Dot(mBox->Axis[1]);
    ADdU[1] = Math<Real>::FAbs(DdU[1]);
    if (ADdU[1] > mBox->Extent[1] && DdU[1]*WdU[1] >= (Real)0)
    {
        return false;
    }

    WdU[2] = mRay->Direction.Dot(mBox->Axis[2]);
    AWdU[2] = Math<Real>::FAbs(WdU[2]);
    DdU[2] = diff.Dot(mBox->Axis[2]);
    ADdU[2] = Math<Real>::FAbs(DdU[2]);
    if (ADdU[2] > mBox->Extent[2] && DdU[2]*WdU[2] >= (Real)0)
    {
        return false;
    }

    Vector3<Real> WxD = mRay->Direction.Cross(diff);

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
bool IntrRay3Box3<Real>::Find ()
{
    Real t0 = (Real)0;
    Real t1 = Math<Real>::MAX_REAL;
    return IntrLine3Box3<Real>::DoClipping(t0, t1, mRay->Origin,
        mRay->Direction, *mBox, true, mQuantity, mPoint, mIntersectionType);
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrRay3Box3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrRay3Box3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrRay3Box3<float>;

template WM5_MATHEMATICS_ITEM
class IntrRay3Box3<double>;
//----------------------------------------------------------------------------
}
