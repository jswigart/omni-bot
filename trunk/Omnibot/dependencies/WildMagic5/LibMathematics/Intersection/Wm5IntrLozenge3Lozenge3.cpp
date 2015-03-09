// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrLozenge3Lozenge3.h"
#include "Wm5DistRectangle3Rectangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrLozenge3Lozenge3<Real>::IntrLozenge3Lozenge3 (
    const Lozenge3<Real>& lozenge0, const Lozenge3<Real>& lozenge1)
    :
    mLozenge0(&lozenge0),
    mLozenge1(&lozenge1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Lozenge3<Real>& IntrLozenge3Lozenge3<Real>::GetLozenge0 () const
{
    return *mLozenge0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Lozenge3<Real>& IntrLozenge3Lozenge3<Real>::GetLozenge1 () const
{
    return *mLozenge1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLozenge3Lozenge3<Real>::Test ()
{
    Real distance = DistRectangle3Rectangle3<Real>(mLozenge0->Rectangle,
        mLozenge1->Rectangle).Get();
    Real rSum = mLozenge0->Radius + mLozenge1->Radius;
    return distance <= rSum;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrLozenge3Lozenge3<float>;

template WM5_MATHEMATICS_ITEM
class IntrLozenge3Lozenge3<double>;
//----------------------------------------------------------------------------
}
