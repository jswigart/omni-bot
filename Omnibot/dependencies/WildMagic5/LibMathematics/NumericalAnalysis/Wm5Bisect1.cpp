// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Bisect1.h"
#include "Wm5Math.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Bisect1<Real>::Bisect1 (Function function, int maxLevel, Real tolerance)
    :
    mFunction(function),
    mMaxLevel(maxLevel),
    mTolerance(tolerance)
{
}
//----------------------------------------------------------------------------
template <typename Real>
bool Bisect1<Real>::Bisect (Real x0, Real x1, Real& root)
{
    // test two endpoints
    Real f0 = mFunction(x0);
    if (Math<Real>::FAbs(f0) <= mTolerance)
    {
        root = x0;
        return true;
    }

    Real f1 = mFunction(x1);
    if (Math<Real>::FAbs(f1) <= mTolerance)
    {
        root = x1;
        return true;
    }

    if (f0*f1 > (Real)0)
    {
        return false;
    }

    for (int level = 0; level < mMaxLevel; ++level)
    {
        Real fXm = ((Real)0.5)*(x0 + x1);
        Real fFm = mFunction(fXm);
        if (Math<Real>::FAbs(fFm) <= mTolerance)
        {
            root = fXm;
            return true;
        }

        if (f0*fFm < (Real)0)
        {
            x1 = fXm;
            f1 = fFm;
        }
        else if (f1*fFm < (Real)0)
        {
            x0 = fXm;
            f0 = fFm;
        }
    }

    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class Bisect1<float>;

template WM5_MATHEMATICS_ITEM
class Bisect1<double>;
//----------------------------------------------------------------------------
}
