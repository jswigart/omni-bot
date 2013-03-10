// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5OdeRungeKutta4.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
OdeRungeKutta4<Real>::OdeRungeKutta4 (int dim, Real step,
    typename OdeSolver<Real>::Function function, void* userData)
    :
    OdeSolver<Real>(dim, step, function, userData)
{
    mHalfStep = ((Real)0.5)*step;
    mSixthStep = step/((Real)6);
    mTemp1 = new1<Real>(mDim);
    mTemp2 = new1<Real>(mDim);
    mTemp3 = new1<Real>(mDim);
    mTemp4 = new1<Real>(mDim);
    mXTemp = new1<Real>(mDim);
}
//----------------------------------------------------------------------------
template <typename Real>
OdeRungeKutta4<Real>::~OdeRungeKutta4 ()
{
    delete1(mTemp1);
    delete1(mTemp2);
    delete1(mTemp3);
    delete1(mTemp4);
    delete1(mXTemp);
}
//----------------------------------------------------------------------------
template <typename Real>
void OdeRungeKutta4<Real>::Update (Real tIn, Real* xIn, Real& tOut,
    Real* xOut)
{
    // first step
    mFunction(tIn, xIn, mUserData, mTemp1);
    int i;
    for (i = 0; i < mDim; ++i)
    {
        mXTemp[i] = xIn[i] + mHalfStep*mTemp1[i];
    }

    // second step
    Real halfT = tIn + mHalfStep;
    mFunction(halfT, mXTemp, mUserData, mTemp2);
    for (i = 0; i < mDim; ++i)
    {
        mXTemp[i] = xIn[i] + mHalfStep*mTemp2[i];
    }

    // third step
    mFunction(halfT, mXTemp, mUserData, mTemp3);
    for (i = 0; i < mDim; ++i)
    {
        mXTemp[i] = xIn[i] + mStep*mTemp3[i];
    }

    // fourth step
    tOut = tIn + mStep;
    mFunction(tOut, mXTemp, mUserData, mTemp4);
    for (i = 0; i < mDim; ++i)
    {
        xOut[i] = xIn[i] + mSixthStep*(mTemp1[i] +
            ((Real)2)*(mTemp2[i] + mTemp3[i]) + mTemp4[i]);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void OdeRungeKutta4<Real>::SetStepSize (Real step)
{
    mStep = step;
    mHalfStep = ((Real)0.5)*step;
    mSixthStep = step/((Real)6);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class OdeRungeKutta4<float>;

template WM5_MATHEMATICS_ITEM
class OdeRungeKutta4<double>;
//----------------------------------------------------------------------------
}
