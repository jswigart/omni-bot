// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5OdeMidpoint.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
OdeMidpoint<Real>::OdeMidpoint (int dim, Real step,
    typename OdeSolver<Real>::Function function, void* userData)
    :
    OdeSolver<Real>(dim, step, function, userData)
{
    mHalfStep = ((Real)0.5)*step;
    mXTemp = new1<Real>(mDim);
}
//----------------------------------------------------------------------------
template <typename Real>
OdeMidpoint<Real>::~OdeMidpoint ()
{
    delete1(mXTemp);
}
//----------------------------------------------------------------------------
template <typename Real>
void OdeMidpoint<Real>::Update (Real tIn, Real* xIn, Real& tOut, Real* xOut)
{
    // first step
    mFunction(tIn, xIn, mUserData, mFValue);
    int i;
    for (i = 0; i < mDim; ++i)
    {
        mXTemp[i] = xIn[i] + mHalfStep*mFValue[i];
    }

    // second step
    Real halfT = tIn + mHalfStep;
    mFunction(halfT, mXTemp, mUserData, mFValue);
    for (i = 0; i < mDim; ++i)
    {
        xOut[i] = xIn[i] + mStep*mFValue[i];
    }

    tOut = tIn + mStep;
}
//----------------------------------------------------------------------------
template <typename Real>
void OdeMidpoint<Real>::SetStepSize (Real step)
{
    mStep = step;
    mHalfStep = ((Real)0.5)*step;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class OdeMidpoint<float>;

template WM5_MATHEMATICS_ITEM
class OdeMidpoint<double>;
//----------------------------------------------------------------------------
}
