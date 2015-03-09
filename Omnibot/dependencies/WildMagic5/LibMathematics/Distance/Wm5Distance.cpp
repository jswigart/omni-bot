// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Distance.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Distance<Real,TVector>::Distance ()
    :
    MaximumIterations(8),
    ZeroThreshold(Math<Real>::ZERO_TOLERANCE),
    mContactTime(Math<Real>::MAX_REAL),
    mHasMultipleClosestPoints0(false),
    mHasMultipleClosestPoints1(false)
{
    SetDifferenceStep((Real)1e-03);
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Distance<Real,TVector>::~Distance ()
{
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real Distance<Real,TVector>::GetDifferenceStep () const
{
    return mDifferenceStep;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real Distance<Real,TVector>::GetContactTime () const
{
    return mContactTime;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
const TVector& Distance<Real,TVector>::GetClosestPoint0 () const
{
    return mClosestPoint0;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
const TVector& Distance<Real,TVector>::GetClosestPoint1 () const
{
    return mClosestPoint1;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
bool Distance<Real,TVector>::HasMultipleClosestPoints0 () const
{
    return mHasMultipleClosestPoints0;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
bool Distance<Real,TVector>::HasMultipleClosestPoints1 () const
{
    return mHasMultipleClosestPoints1;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
void Distance<Real,TVector>::SetDifferenceStep (Real differenceStep)
{
    if (differenceStep > (Real)0)
    {
        mDifferenceStep = differenceStep;
    }
    else
    {
        assertion(differenceStep > (Real)0, "Invalid difference step\n");
        mDifferenceStep = (Real)1e-03;
    }

    mInvTwoDifferenceStep = ((Real)0.5)/mDifferenceStep;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real Distance<Real,TVector>::GetDerivative (Real t,
    const TVector& velocity0, const TVector& velocity1)
{
    // Use a finite difference approximation:  f'(t) = (f(t+h)-f(t-h))/(2*h)
    Real funcp = Get(t + mDifferenceStep, velocity0, velocity1);
    Real funcm = Get(t - mDifferenceStep, velocity0, velocity1);
    Real derApprox = mInvTwoDifferenceStep*(funcp - funcm);
    return derApprox;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real Distance<Real,TVector>::GetDerivativeSquared (Real t,
    const TVector& velocity0, const TVector& velocity1)
{
    // A derived class should override this only if there is a faster method
    // to compute the derivative of the squared distance for the specific
    // class.
    Real distance = Get(t, velocity0, velocity1);
    Real derivative = GetDerivative(t, velocity0, velocity1);
    return ((Real)2)*distance*derivative;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real Distance<Real,TVector>::Get (Real tmin, Real tmax,
    const TVector& velocity0, const TVector& velocity1)
{
    // The assumption is that distance f(t) is a convex function.  If
    // f'(tmin) >= 0, then the minimum occurs at tmin.  If f'(tmax) <= 0,
    // then the minimum occurs at tmax.  Otherwise, f'(0) < 0 and
    // f'(tmax) > 0 and the minimum occurs at some t in (tmin,tmax).

    Real t0 = tmin;
    Real f0 = Get(t0, velocity0, velocity1);
    if (f0 <= ZeroThreshold)
    {
        // The distance is effectively zero.  The objects are initially in
        // contact.
        mContactTime = t0;
        return (Real)0;
    }
    Real df0 = GetDerivative(t0, velocity0, velocity1);
    if (df0 >= (Real)0)
    {
        // The distance is increasing on [0,tmax].
        mContactTime = t0;
        return f0;
    }

    Real t1 = tmax;
    Real f1 = Get(t1, velocity0, velocity1);
    if (f1 <= ZeroThreshold)
    {
        // The distance is effectively zero.
        mContactTime = t1;
        return (Real)0;
    }
    Real df1 = GetDerivative(t1, velocity0, velocity1);
    if (df1 <= (Real)0)
    {
        // The distance is decreasing on [0,tmax].
        mContactTime = t1;
        return f1;
    }

    // Start the process with Newton's method for computing a time when the
    // distance is zero.  During this process we will switch to a numerical
    // minimizer if we decide that the distance cannot be zero.
    int i;
    for (i = 0; i < MaximumIterations; ++i)
    {
        // Compute the next Newton's iterate.
        Real t = t0 - f0/df0;
        if (t >= tmax)
        {
            // The convexity of the graph guarantees that when this condition
            // happens, the distance is always positive.  Switch to a
            // numerical minimizer.
            break;
        }

        Real f = Get(t, velocity0, velocity1);
        if (f <= ZeroThreshold)
        {
            // The distance is effectively zero.
            mContactTime = t;
            return (Real)0;
        }

        Real df = GetDerivative(t, velocity0, velocity1);
        if (df >= (Real)0)
        {
            // The convexity of the graph guarantees that when this condition
            // happens, the distance is always positive.  Switch to a
            // numerical minimizer.
            break;
        }

        t0 = t;
        f0 = f;
        df0 = df;
    }

    if (i == MaximumIterations)
    {
        // Failed to converge within desired number of iterations.  To
        // reach here, the derivative values were always negative, so report
        // the distance at the last time.
        mContactTime = t0;
        return f0;
    }

    // The distance is always positive.  Use bisection to find the root of
    // the derivative function.
    Real tm = t0;
    for (i = 0; i < MaximumIterations; ++i)
    {
        tm = ((Real)0.5)*(t0 + t1);
        Real dfm = GetDerivative(tm, velocity0, velocity1);
        Real product = dfm*df0;
        if (product < -ZeroThreshold)
        {
            t1 = tm;
            df1 = dfm;
        }
        else if (product > ZeroThreshold)
        {
            t0 = tm;
            df0 = dfm;
        }
        else
        {
            break;
        }
    }

    // This is the time at which the minimum occurs and is not the contact
    // time.  Store it anyway for debugging purposes.
    mContactTime = tm;
    Real fm = Get(tm, velocity0, velocity1);
    return fm;
}
//----------------------------------------------------------------------------
template <typename Real, typename TVector>
Real Distance<Real,TVector>::GetSquared (Real tmin, Real tmax,
    const TVector& velocity0, const TVector& velocity1)
{
    // The assumption is that distance f(t) is a convex function.  If
    // f'(tmin) >= 0, then the minimum occurs at tmin.  If f'(tmax) <= 0,
    // then the minimum occurs at tmax.  Otherwise, f'(0) < 0 and
    // f'(tmax) > 0 and the minimum occurs at some t in (tmin,tmax).

    Real t0 = tmin;
    Real f0 = GetSquared(t0, velocity0, velocity1);
    if (f0 <= ZeroThreshold)
    {
        // The distance is effectively zero.  The objects are initially in
        // contact.
        mContactTime = t0;
        return (Real)0;
    }
    Real df0 = GetDerivativeSquared(t0, velocity0, velocity1);
    if (df0 >= (Real)0)
    {
        // The distance is increasing on [0,tmax].
        mContactTime = t0;
        return f0;
    }

    Real t1 = tmax;
    Real f1 = GetSquared(t1, velocity0, velocity1);
    if (f1 <= ZeroThreshold)
    {
        // The distance is effectively zero.
        mContactTime = t1;
        return (Real)0;
    }
    Real df1 = GetDerivativeSquared(t1, velocity0, velocity1);
    if (df1 <= (Real)0)
    {
        // The distance is decreasing on [0,tmax].
        mContactTime = t1;
        return f1;
    }

    // Start the process with Newton's method for computing a time when the
    // distance is zero.  During this process we will switch to a numerical
    // minimizer if we decide that the distance cannot be zero.
    int i;
    for (i = 0; i < MaximumIterations; ++i)
    {
        // Compute the next Newton's iterate.
        Real t = t0 - f0/df0;
        if (t >= tmax)
        {
            // The convexity of the graph guarantees that when this condition
            // happens, the distance is always positive.  Switch to a
            // numerical minimizer.
            break;
        }

        Real f = GetSquared(t, velocity0, velocity1);
        if (f <= ZeroThreshold)
        {
            // The distance is effectively zero.
            mContactTime = t;
            return (Real)0;
        }

        Real df = GetDerivativeSquared(t, velocity0, velocity1);
        if (df >= (Real)0)
        {
            // The convexity of the graph guarantees that when this condition
            // happens, the distance is always positive.  Switch to a
            // numerical minimizer.
            break;
        }

        t0 = t;
        f0 = f;
        df0 = df;
    }

    if (i == MaximumIterations)
    {
        // Failed to converge within desired number of iterations.  To
        // reach here, the derivative values were always negative, so report
        // the distance at the last time.
        mContactTime = t0;
        return f0;
    }

    // The distance is always positive.  Use bisection to find the root of
    // the derivative function.
    Real tm = t0;
    for (i = 0; i < MaximumIterations; ++i)
    {
        tm = ((Real)0.5)*(t0 + t1);
        Real dfm = GetDerivativeSquared(tm, velocity0, velocity1);
        Real product = dfm*df0;
        if (product < -ZeroThreshold)
        {
            t1 = tm;
            df1 = dfm;
        }
        else if (product > ZeroThreshold)
        {
            t0 = tm;
            df0 = dfm;
        }
        else
        {
            break;
        }
    }

    // This is the time at which the minimum occurs and is not the contact
    // time.  Store it anyway for debugging purposes.
    mContactTime = tm;
    Real fm = GetSquared(tm, velocity0, velocity1);
    return fm;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class Distance<float,Vector2f>;

template WM5_MATHEMATICS_ITEM
class Distance<float,Vector3f>;

template WM5_MATHEMATICS_ITEM
class Distance<double,Vector2d>;

template WM5_MATHEMATICS_ITEM
class Distance<double,Vector3d>;
//----------------------------------------------------------------------------
}
