// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrPlane3Cylinder3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrPlane3Cylinder3<Real>::IntrPlane3Cylinder3 (const Plane3<Real>& rkPlane,
    const Cylinder3<Real>& rkCylinder)
    :
    mPlane(&rkPlane),
    mCylinder(&rkCylinder)
{
    mType = PC_EMPTY_SET;
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrPlane3Cylinder3<Real>::GetPlane () const
{
    return *mPlane;
}
//----------------------------------------------------------------------------
template <typename Real>
const Cylinder3<Real>& IntrPlane3Cylinder3<Real>::GetCylinder () const
{
    return *mCylinder;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Cylinder3<Real>::Test ()
{
    // Compute extremes of signed distance Dot(N,X)-d for points on the
    // cylinder.  These are
    //   min = (Dot(N,C)-d) - r*sqrt(1-Dot(N,W)^2) - (h/2)*|Dot(N,W)|
    //   max = (Dot(N,C)-d) + r*sqrt(1-Dot(N,W)^2) + (h/2)*|Dot(N,W)|
    Real sDist = mPlane->DistanceTo(mCylinder->Axis.Origin);
    Real absNdW = Math<Real>::FAbs(mPlane->Normal.Dot(
        mCylinder->Axis.Direction));
    Real root = Math<Real>::Sqrt(Math<Real>::FAbs((Real)1 - absNdW*absNdW));
    Real term = mCylinder->Radius*root +
        ((Real)0.5)*mCylinder->Height*absNdW;

    // Intersection occurs if and only if 0 is in the interval [min,max].
    return Math<Real>::FAbs(sDist) <= term;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Cylinder3<Real>::Find ()
{
    Real sDist = mPlane->DistanceTo(mCylinder->Axis.Origin);
    Vector3<Real> center =  mCylinder->Axis.Origin -
        sDist*mPlane->Normal;
    Real cosTheta = mCylinder->Axis.Direction.Dot(mPlane->Normal);
    Real absCosTheta = Math<Real>::FAbs(cosTheta);

    if (absCosTheta > (Real)0)
    {
        // The cylinder axis intersects the plane in a unique point.
        if (absCosTheta < (Real)1)
        {
            mType = PC_ELLIPSE;
            mEllipse.Normal = mPlane->Normal;
            mEllipse.Center = center -
                (sDist/cosTheta)*mCylinder->Axis.Direction;
            mEllipse.Major = mCylinder->Axis.Direction -
                cosTheta*mPlane->Normal;
            mEllipse.Minor = mPlane->Normal.Cross(mEllipse.Major);
            mEllipse.MajorLength = mCylinder->Radius/absCosTheta;
            mEllipse.MinorLength = mCylinder->Radius;
            mEllipse.Major.Normalize();
            mEllipse.Minor.Normalize();
            return true;
        }
        else
        {
            mType = PC_CIRCLE;
            mCircle.Normal = mPlane->Normal;
            mCircle.Center = center;
            mCircle.Radius = mCylinder->Radius;
            return true;
        }
    }
    else
    {
        // The cylinder is parallel to the plane.
        Real absSDist = Math<Real>::FAbs(sDist);
        if (absSDist < mCylinder->Radius)
        {
            mType = PC_TWO_LINES;

            Vector3<Real> offset = mCylinder->Axis.Direction.Cross(
                mPlane->Normal);
            Real extent = Math<Real>::Sqrt(
                mCylinder->Radius*mCylinder->Radius - sDist*sDist);

            mLine0.Origin = center - extent*offset;
            mLine0.Direction = mCylinder->Axis.Direction;
            mLine1.Origin = center + extent*offset;
            mLine1.Direction = mCylinder->Axis.Direction;
            return true;
        }
        else if (absSDist == mCylinder->Radius)
        {
            mType = PC_ONE_LINE;
            mLine0.Origin = center;
            mLine0.Direction = mCylinder->Axis.Direction;
            return true;
        }
        else
        {
            mType = PC_EMPTY_SET;
            return false;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Cylinder3<Real>::CylinderIsCulled () const
{
    // Compute extremes of signed distance Dot(N,X)-d for points on the
    // cylinder.  These are
    //   min = (Dot(N,C)-d) - r*sqrt(1-Dot(N,W)^2) - (h/2)*|Dot(N,W)|
    //   max = (Dot(N,C)-d) + r*sqrt(1-Dot(N,W)^2) + (h/2)*|Dot(N,W)|
    Real sDist = mPlane->DistanceTo(mCylinder->Axis.Origin);
    Real absNdW = Math<Real>::FAbs(mPlane->Normal.Dot(
        mCylinder->Axis.Direction));
    Real root = Math<Real>::Sqrt(Math<Real>::FAbs((Real)1 - absNdW*absNdW));
    Real term = mCylinder->Radius*root +
        ((Real)0.5)*mCylinder->Height*absNdW;

    // Culling occurs if and only if max <= 0.
    return sDist + term <= (Real)0;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrPlane3Cylinder3<Real>::GetType () const
{
    return mType;
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrPlane3Cylinder3<Real>::GetOneLine (Line3<Real>& line) const
{
    line = mLine0;
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrPlane3Cylinder3<Real>::GetTwoLines (Line3<Real>& line0,
    Line3<Real>& line1) const
{
    line0 = mLine0;
    line1 = mLine1;
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrPlane3Cylinder3<Real>::GetCircle (Circle3<Real>& circle) const
{
    circle = mCircle;
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrPlane3Cylinder3<Real>::GetEllipse (Ellipse3<Real>& ellipse) const
{
    ellipse = mEllipse;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrPlane3Cylinder3<float>;

template WM5_MATHEMATICS_ITEM
class IntrPlane3Cylinder3<double>;
//----------------------------------------------------------------------------
}
