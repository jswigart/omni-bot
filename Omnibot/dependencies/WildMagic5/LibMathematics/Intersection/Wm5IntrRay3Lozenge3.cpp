// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrRay3Lozenge3.h"
#include "Wm5DistRay3Rectangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrRay3Lozenge3<Real>::IntrRay3Lozenge3 (const Ray3<Real>& ray,
    const Lozenge3<Real>& lozenge)
    :
    mRay(&ray),
    mLozenge(&lozenge)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray3<Real>& IntrRay3Lozenge3<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
const Lozenge3<Real>& IntrRay3Lozenge3<Real>::GetLozenge () const
{
    return *mLozenge;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay3Lozenge3<Real>::Test ()
{
    Real distance = DistRay3Rectangle3<Real>(*mRay,
        mLozenge->Rectangle).Get();

    return distance <= mLozenge->Radius;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrRay3Lozenge3<float>;

template WM5_MATHEMATICS_ITEM
class IntrRay3Lozenge3<double>;
//----------------------------------------------------------------------------
}
