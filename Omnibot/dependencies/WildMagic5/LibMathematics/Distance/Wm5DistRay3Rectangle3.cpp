// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistRay3Rectangle3.h"
#include "Wm5DistLine3Rectangle3.h"
#include "Wm5DistPoint3Rectangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistRay3Rectangle3<Real>::DistRay3Rectangle3 (const Ray3<Real>& ray,
    const Rectangle3<Real>& rectangle)
    :
    mRay(&ray),
    mRectangle(&rectangle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray3<Real>& DistRay3Rectangle3<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
const Rectangle3<Real>& DistRay3Rectangle3<Real>::GetRectangle () const
{
    return *mRectangle;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Rectangle3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Rectangle3<Real>::GetSquared ()
{
    DistLine3Rectangle3<Real> queryLR(Line3<Real>(mRay->Origin,
        mRay->Direction), *mRectangle);
    Real sqrDist = queryLR.GetSquared();
    mRayParameter = queryLR.GetLineParameter();

    if (mRayParameter >= (Real)0)
    {
        mClosestPoint0 = queryLR.GetClosestPoint0();
        mClosestPoint1 = queryLR.GetClosestPoint1();
        mRectCoord[0] = queryLR.GetRectangleCoordinate(0);
        mRectCoord[1] = queryLR.GetRectangleCoordinate(1);
    }
    else
    {
        mClosestPoint0 = mRay->Origin;
        DistPoint3Rectangle3<Real> queryPR(mClosestPoint0, *mRectangle);
        sqrDist = queryPR.GetSquared();
        mClosestPoint1 = queryPR.GetClosestPoint1();
        mRayParameter = (Real)0;
        mRectCoord[0] = queryPR.GetRectangleCoordinate(0);
        mRectCoord[1] = queryPR.GetRectangleCoordinate(1);
    }

    return sqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Rectangle3<Real>::Get (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin = mRay->Origin + t*velocity0;
    Vector3<Real> movedCenter = mRectangle->Center + t*velocity1;
    Ray3<Real> movedRay(movedOrigin, mRay->Direction);
    Rectangle3<Real> movedRectangle(movedCenter, mRectangle->Axis,
        mRectangle->Extent);
    return DistRay3Rectangle3<Real>(movedRay, movedRectangle).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Rectangle3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin = mRay->Origin + t*velocity0;
    Vector3<Real> movedCenter = mRectangle->Center + t*velocity1;
    Ray3<Real> movedRay(movedOrigin, mRay->Direction);
    Rectangle3<Real> movedRectangle(movedCenter, mRectangle->Axis,
        mRectangle->Extent);
    return DistRay3Rectangle3<Real>(movedRay, movedRectangle).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Rectangle3<Real>::GetRayParameter () const
{
    return mRayParameter;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Rectangle3<Real>::GetRectangleCoordinate (int i) const
{
    return mRectCoord[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistRay3Rectangle3<float>;

template WM5_MATHEMATICS_ITEM
class DistRay3Rectangle3<double>;
//----------------------------------------------------------------------------
}
