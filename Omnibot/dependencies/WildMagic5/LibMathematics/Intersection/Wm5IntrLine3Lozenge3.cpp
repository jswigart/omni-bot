// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrLine3Lozenge3.h"
#include "Wm5DistLine3Rectangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrLine3Lozenge3<Real>::IntrLine3Lozenge3 (const Line3<Real>& line,
    const Lozenge3<Real>& lozenge)
    :
    mLine(&line),
    mLozenge(&lozenge)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& IntrLine3Lozenge3<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Lozenge3<Real>& IntrLine3Lozenge3<Real>::GetLozenge () const
{
    return *mLozenge;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine3Lozenge3<Real>::Test ()
{
    Real distance = DistLine3Rectangle3<Real>(*mLine,
        mLozenge->Rectangle).Get();

    return distance <= mLozenge->Radius;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrLine3Lozenge3<float>;

template WM5_MATHEMATICS_ITEM
class IntrLine3Lozenge3<double>;
//----------------------------------------------------------------------------
}
