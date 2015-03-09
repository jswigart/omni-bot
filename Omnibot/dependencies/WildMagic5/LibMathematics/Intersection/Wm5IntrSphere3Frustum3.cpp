// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrSphere3Frustum3.h"
#include "Wm5DistPoint3Frustum3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrSphere3Frustum3<Real>::IntrSphere3Frustum3 (const Sphere3<Real>& sphere,
    const Frustum3<Real>& frustum)
    :
    mSphere(&sphere),
    mFrustum(&frustum)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Sphere3<Real>& IntrSphere3Frustum3<Real>::GetSphere () const
{
    return *mSphere;
}
//----------------------------------------------------------------------------
template <typename Real>
const Frustum3<Real>& IntrSphere3Frustum3<Real>::GetFrustum () const
{
    return *mFrustum;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSphere3Frustum3<Real>::Test ()
{
    Real distance = DistPoint3Frustum3<Real>(mSphere->Center,
        *mFrustum).Get();

    return distance <= mSphere->Radius;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrSphere3Frustum3<float>;

template WM5_MATHEMATICS_ITEM
class IntrSphere3Frustum3<double>;
//----------------------------------------------------------------------------
}
