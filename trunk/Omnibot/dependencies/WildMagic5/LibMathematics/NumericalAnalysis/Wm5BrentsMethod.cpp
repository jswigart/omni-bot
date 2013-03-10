// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.6.0 (2011/07/09)

#include "Wm5MathematicsPCH.h"
#include "Wm5BrentsMethod.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
BrentsMethod<Real>::BrentsMethod (Function function, int maxIterations,
    Real negFTolerance, Real posFTolerance, Real stepXTolerance,
    Real convXTolerance, void* userData)
{
    SetFunction(function);
    SetMaxIterations(maxIterations);
    SetNegFTolerance(negFTolerance);
    SetPosFTolerance(posFTolerance);
    SetStepXTolerance(stepXTolerance);
    SetConvXTolerance(convXTolerance);
    SetUserData(userData);
}
//----------------------------------------------------------------------------
template <typename Real>
BrentsMethod<Real>::~BrentsMethod ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
void BrentsMethod<Real>::SetFunction (Function function)
{
    assertion(function != 0, "Invalid function.\n");
    mFunction = function;
}
//----------------------------------------------------------------------------
template <typename Real>
typename BrentsMethod<Real>::Function BrentsMethod<Real>::GetFunction () const
{
    return mFunction;
}
//----------------------------------------------------------------------------
template <typename Real>
void BrentsMethod<Real>::SetMaxIterations (int maxIterations)
{
    assertion(maxIterations >= 1, "Invalid maxIterations.\n");
    mMaxIterations = maxIterations;
}
//----------------------------------------------------------------------------
template <typename Real>
int BrentsMethod<Real>::GetMaxIterations () const
{
    return mMaxIterations;
}
//----------------------------------------------------------------------------
template <typename Real>
void BrentsMethod<Real>::SetNegFTolerance (Real negFTolerance)
{
    assertion(negFTolerance <= (Real)0, "Invalid negFTolerance.\n");
    mNegFTolerance = negFTolerance;
}
//----------------------------------------------------------------------------
template <typename Real>
Real BrentsMethod<Real>::GetNegFTolerance () const
{
    return mNegFTolerance;
}
//----------------------------------------------------------------------------
template <typename Real>
void BrentsMethod<Real>::SetPosFTolerance (Real posFTolerance)
{
    assertion(posFTolerance >= (Real)0, "Invalid posFTolerance.\n");
    mPosFTolerance = posFTolerance;
}
//----------------------------------------------------------------------------
template <typename Real>
Real BrentsMethod<Real>::GetPosFTolerance () const
{
    return mPosFTolerance;
}
//----------------------------------------------------------------------------
template <typename Real>
void BrentsMethod<Real>::SetStepXTolerance (Real stepXTolerance)
{
    assertion(stepXTolerance >= (Real)0, "Invalid stepXTolerance.\n");
    mStepXTolerance = stepXTolerance;
}
//----------------------------------------------------------------------------
template <typename Real>
Real BrentsMethod<Real>::GetStepXTolerance () const
{
    return mStepXTolerance;
}
//----------------------------------------------------------------------------
template <typename Real>
void BrentsMethod<Real>::SetConvXTolerance (Real convXTolerance)
{
    assertion(convXTolerance >= (Real)0, "Invalid convXTolerance.\n");
    mConvXTolerance = convXTolerance;
}
//----------------------------------------------------------------------------
template <typename Real>
Real BrentsMethod<Real>::GetConvXTolerance () const
{
    return mConvXTolerance;
}
//----------------------------------------------------------------------------
template <typename Real>
void BrentsMethod<Real>::SetUserData (void* userData)
{
    mUserData = userData;
}
//----------------------------------------------------------------------------
template <typename Real>
void* BrentsMethod<Real>::GetUserData () const
{
    return mUserData;
}
//----------------------------------------------------------------------------
template <typename Real>
bool BrentsMethod<Real>::GetRoot (Real x0, Real x1, Real& xRoot, Real& fRoot)
{
    if (x1 <= x0)
    {
        // The interval is invalid.
        return false;
    }

    Real f0 = mFunction(x0, mUserData);
    if (mNegFTolerance <= f0 && f0 <= mPosFTolerance)
    {
        // This endpoint is an approximate root that satisfies the function
        // tolerance.
        xRoot = x0;
        fRoot = f0;
        return true;
    }

    Real f1 = mFunction(x1, mUserData);
    if (mNegFTolerance <= f1 && f1 <= mPosFTolerance)
    {
        // This endpoint is an approximate root that satisfies the function
        // tolerance.
        xRoot = x1;
        fRoot = f1;
        return true;
    }

    if (f0*f1 >= (Real)0)
    {
        // The input interval must bound a root.
        return false;
    }

    if (fabs(f0) < fabs(f1))
    {
        // Swap x0 and x1 so that |f(x1)| <= |f(x0)|.  The number x1 is
        // considered to be the best estimate of the root.
        Real save = x0;
        x0 = x1;
        x1 = save;
        save = f0;
        f0 = f1;
        f1 = save;
    }

    // Initialize values for the root search.
    Real x2 = x0, x3 = x0, f2 = f0;
    bool prevBisected = true;

    // The root search.
    for (int i = 0; i < mMaxIterations; ++i)
    {
        Real fDiff01 = f0 - f1, fDiff02 = f0 - f2, fDiff12 = f1 - f2;
        Real invFDiff01 = ((Real)1)/fDiff01;
        Real s;
        if (fDiff02 != (Real)0 && fDiff12 != (Real)0)
        {
            // Use inverse quadratic interpolation.
            Real infFDiff02 = ((Real)1)/fDiff02;
            Real invFDiff12 = ((Real)1)/fDiff12;
            s =
                x0*f1*f2*invFDiff01*infFDiff02 -
                x1*f0*f2*invFDiff01*invFDiff12 +
                x2*f0*f1*infFDiff02*invFDiff12;
        }
        else
        {
            // Use inverse linear interpolation (secant method).
            s = (x1*f0 - x0*f1)*invFDiff01;
        }

        // Compute values need in the accept-or-reject tests.
        Real xDiffSAvr = s - ((Real)0.75)*x0 - ((Real)0.25)*x1;
        Real xDiffS1 = s - x1;
        Real absXDiffS1 = fabs(xDiffS1);
        Real absXDiff12 = fabs(x1 - x2);
        Real absXDiff23 = fabs(x2 - x3);

        bool currBisected = false;
        if (xDiffSAvr*xDiffS1 > (Real)0)
        {
            // The value s is not between 0.75*x0+0.25*x1 and x1.  NOTE:  The
            // algorithm sometimes has x0 < x1 but sometimes x1 < x0, so the
            // betweenness test does not use simple comparisons.
            currBisected = true;
        }
        else if (prevBisected)
        {
            // The first of Brent's tests to determine whether to accept the
            // interpolated s-value.
            currBisected =
                (absXDiffS1 >= ((Real)0.5)*absXDiff12) ||
                (absXDiff12 <= mStepXTolerance);
        }
        else
        {
            // The second of Brent's tests to determine whether to accept the
            // interpolated s-value.
            currBisected =
                (absXDiffS1 >= ((Real)0.5)*absXDiff23) ||
                (absXDiff23 <= mStepXTolerance);
        }

        if (currBisected)
        {
            // One of the additional tests failed, so reject the interpolated
            // s-value and use bisection instead.
            s = ((Real)0.5)*(x0 + x1);
            prevBisected = true;
        }
        else
        {
            prevBisected = false;
        }

        // Evaluate the function at the new estimate and test for convergence.
        Real fs = mFunction(s, mUserData);
        if (mNegFTolerance <= fs && fs <= mPosFTolerance)
        {
            xRoot = s;
            fRoot = fs;
            return true;
        }

        // Update the subinterval to include the new estimate as an endpoint.
        x3 = x2;
        x2 = x1;
        f2 = f1;
        if (f0*fs < (Real)0)
        {
            x1 = s;
            f1 = fs;
        }
        else
        {
            x0 = s;
            f0 = fs;
        }

        // Allow the algorithm to terminate when the subinterval is
        // sufficiently small.
        if (fabs(x1 - x0) <= mConvXTolerance)
        {
            xRoot = x1;
            fRoot = f1;
            return true;
        }

        // A loop invariant is that x1 is the root estimate, f(x0)*f(x1) < 0,
        // and |f(x1)| <= |f(x0)|.
        if (fabs(f0) < fabs(f1))
        {
            Real save = x0;
            x0 = x1;
            x1 = save;
            save = f0;
            f0 = f1;
            f1 = save;
        }
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class BrentsMethod<float>;

template WM5_MATHEMATICS_ITEM
class BrentsMethod<double>;
}
//----------------------------------------------------------------------------
