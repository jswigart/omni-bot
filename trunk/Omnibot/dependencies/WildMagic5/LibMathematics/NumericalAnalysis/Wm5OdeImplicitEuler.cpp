// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/09)

#include "Wm5MathematicsPCH.h"
#include "Wm5OdeImplicitEuler.h"
#include "Wm5LinearSystem.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
OdeImplicitEuler<Real>::OdeImplicitEuler (int dim, Real step,
    typename OdeSolver<Real>::Function function,
    DerivativeFunction derFunction, void* userData)
    :
    OdeSolver<Real>(dim, step, function, userData),
    mDF(dim, dim),
    mF(dim),
    mIdentity(dim, dim)
{
    mDerFunction = derFunction;
    for (int i = 0; i < dim; ++i)
    {
        mIdentity[i][i] = (Real)1;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
OdeImplicitEuler<Real>::~OdeImplicitEuler ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
void OdeImplicitEuler<Real>::Update (Real tIn, Real* xIn, Real& tOut,
    Real* xOut)
{
    mFunction(tIn, xIn, mUserData, mF);
    mDerFunction(tIn, xIn, mUserData, mDF);
    GMatrix<Real> DG = mIdentity - mStep*mDF;
    GMatrix<Real> DGInverse(mDim, mDim);
    bool invertible = LinearSystem<Real>().Inverse(DG, DGInverse);

    if (invertible)
    {
        mF = DGInverse*mF;
        for (int i = 0; i < mDim; ++i)
        {
            xOut[i] = xIn[i] + mStep*mF[i];
        }
    }
    else
    {
        memcpy(xOut, xIn, mDim*sizeof(Real));
    }

    tOut = tIn + mStep;
}
//----------------------------------------------------------------------------
template <typename Real>
void OdeImplicitEuler<Real>::SetStepSize (Real step)
{
    mStep = step;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class OdeImplicitEuler<float>;

template WM5_MATHEMATICS_ITEM
class OdeImplicitEuler<double>;
//----------------------------------------------------------------------------
}
