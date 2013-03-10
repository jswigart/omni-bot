// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5OdeSolver.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
OdeSolver<Real>::OdeSolver (int dim, Real step, Function function,
    void* userData)
    :
    mDim(dim),
    mStep(step),
    mFunction(function),
    mUserData(userData)
{
    mFValue = new1<Real>(mDim);
}
//----------------------------------------------------------------------------
template <typename Real>
OdeSolver<Real>::~OdeSolver ()
{
    delete1(mFValue);
}
//----------------------------------------------------------------------------
template <typename Real>
Real OdeSolver<Real>::GetStepSize () const
{
    return mStep;
}
//----------------------------------------------------------------------------
template <typename Real>
void OdeSolver<Real>::SetUserData (void* userData)
{
    mUserData = userData;
}
//----------------------------------------------------------------------------
template <typename Real>
void* OdeSolver<Real>::GetUserData () const
{
    return mUserData;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class OdeSolver<float>;

template WM5_MATHEMATICS_ITEM
class OdeSolver<double>;
//----------------------------------------------------------------------------
}
