// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5OdeEuler.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
OdeEuler<Real>::OdeEuler (int dim, Real step,
    typename OdeSolver<Real>::Function function, void* userData)
    :
    OdeSolver<Real>(dim, step, function, userData)
{
}
//----------------------------------------------------------------------------
template <typename Real>
OdeEuler<Real>::~OdeEuler ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
void OdeEuler<Real>::Update (Real tIn, Real* xIn, Real& tOut, Real* xOut)
{
    mFunction(tIn, xIn, mUserData, mFValue);
    for (int i = 0; i < mDim; ++i)
    {
        xOut[i] = xIn[i] + mStep*mFValue[i];
    }

    tOut = tIn + mStep;
}
//----------------------------------------------------------------------------
template <typename Real>
void OdeEuler<Real>::SetStepSize (Real step)
{
    mStep = step;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class OdeEuler<float>;

template WM5_MATHEMATICS_ITEM
class OdeEuler<double>;
//----------------------------------------------------------------------------
}
