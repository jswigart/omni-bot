// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrPlane3Ellipsoid3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrPlane3Ellipsoid3<Real>::IntrPlane3Ellipsoid3 (const Plane3<Real>& plane,
    const Ellipsoid3<Real>& ellipsoid)
    :
    mPlane(&plane),
    mEllipsoid(&ellipsoid)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrPlane3Ellipsoid3<Real>::GetPlane () const
{
    return *mPlane;
}
//----------------------------------------------------------------------------
template <typename Real>
const Ellipsoid3<Real>& IntrPlane3Ellipsoid3<Real>::GetEllipsoid () const
{
    return *mEllipsoid;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Ellipsoid3<Real>::Test ()
{
    Matrix3<Real> MInverse;
    mEllipsoid->GetMInverse(MInverse);
    Real discr = MInverse.QForm(mPlane->Normal,mPlane->Normal);
    Real root = Math<Real>::Sqrt(Math<Real>::FAbs(discr));
    Real sDist = mPlane->DistanceTo(mEllipsoid->Center);
    return Math<Real>::FAbs(sDist) <= root;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Ellipsoid3<Real>::EllipsoidIsCulled () const
{
    Matrix3<Real> MInverse;
    mEllipsoid->GetMInverse(MInverse);
    Real discr = MInverse.QForm(mPlane->Normal,mPlane->Normal);
    Real root = Math<Real>::Sqrt(Math<Real>::FAbs(discr));
    Real sDist = mPlane->DistanceTo(mEllipsoid->Center);
    return sDist <= -root;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrPlane3Ellipsoid3<float>;

template WM5_MATHEMATICS_ITEM
class IntrPlane3Ellipsoid3<double>;
//----------------------------------------------------------------------------
}
