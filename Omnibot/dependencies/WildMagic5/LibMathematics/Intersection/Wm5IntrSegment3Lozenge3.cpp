// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrSegment3Lozenge3.h"
#include "Wm5DistSegment3Rectangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrSegment3Lozenge3<Real>::IntrSegment3Lozenge3 (
    const Segment3<Real>& rkSegment, const Lozenge3<Real>& rkLozenge)
    :
    mSegment(&rkSegment),
    mLozenge(&rkLozenge)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment3<Real>& IntrSegment3Lozenge3<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
const Lozenge3<Real>& IntrSegment3Lozenge3<Real>::GetLozenge () const
{
    return *mLozenge;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Lozenge3<Real>::Test ()
{
    Real distance = DistSegment3Rectangle3<Real>(*mSegment,
        mLozenge->Rectangle).Get();

    return distance <= mLozenge->Radius;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrSegment3Lozenge3<float>;

template WM5_MATHEMATICS_ITEM
class IntrSegment3Lozenge3<double>;
//----------------------------------------------------------------------------
}
