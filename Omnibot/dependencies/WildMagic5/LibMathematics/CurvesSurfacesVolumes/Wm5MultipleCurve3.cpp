// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5MultipleCurve3.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
MultipleCurve3<Real>::MultipleCurve3 (int numSegments, Real* times)
    :
    Curve3<Real>(times[0], times[numSegments])
{
    mNumSegments = numSegments;
    mTimes = times;
    mLengths = 0;
    mAccumLengths = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
MultipleCurve3<Real>::~MultipleCurve3 ()
{
    delete1(mTimes);
    delete1(mLengths);
    delete1(mAccumLengths);
}
//----------------------------------------------------------------------------
template <typename Real>
int MultipleCurve3<Real>::GetSegments () const
{
    return mNumSegments;
}
//----------------------------------------------------------------------------
template <typename Real>
const Real* MultipleCurve3<Real>::GetTimes () const
{
    return mTimes;
}
//----------------------------------------------------------------------------
template <typename Real>
void MultipleCurve3<Real>::GetKeyInfo (Real t, int& key, Real& dt)
    const
{
    if (t <= mTimes[0])
    {
        key = 0;
        dt = (Real)0;
    }
    else if (t >= mTimes[mNumSegments])
    {
        key = mNumSegments - 1;
        dt = mTimes[mNumSegments] - mTimes[mNumSegments - 1];
    }
    else
    {
        for (int i = 0; i < mNumSegments; ++i)
        {
            if (t < mTimes[i + 1])
            {
                key = i;
                dt = t - mTimes[i];
                break;
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Real MultipleCurve3<Real>::GetSpeedWithData (Real t, void* data)
{
    MultipleCurve3* multi = *(MultipleCurve3**)data;
    int key = *(int*)((char*)data + sizeof(multi));
    return multi->GetSpeedKey(key, t);
}
//----------------------------------------------------------------------------
template <typename Real>
void MultipleCurve3<Real>::InitializeLength () const
{
    mLengths = new1<Real>(mNumSegments);
    mAccumLengths = new1<Real>(mNumSegments);

    // Arc lengths of the segments.
    int key;
    for (key = 0; key < mNumSegments; ++key)
    {
        mLengths[key] = GetLengthKey(key, (Real)0,
            mTimes[key + 1] - mTimes[key]);
    }

    // Accumulative arc length.
    mAccumLengths[0] = mLengths[0];
    for (key = 1; key < mNumSegments; ++key)
    {
        mAccumLengths[key] = mAccumLengths[key - 1] + mLengths[key];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Real MultipleCurve3<Real>::GetLength (Real t0, Real t1) const
{
    assertion(mTMin <= t0 && t0 <= mTMax, "Invalid input\n");
    assertion(mTMin <= t1 && t1 <= mTMax, "Invalid input\n");
    assertion(t0 <= t1, "Invalid input\n");

    if (!mLengths)
    {
        InitializeLength();
    }

    int key0, key1;
    Real dt0, dt1;
    GetKeyInfo(t0, key0, dt0);
    GetKeyInfo(t1, key1, dt1);

    Real length;
    if (key0 < key1)
    {
        // Accumulate full-segment lengths.
        length = (Real)0;
        for (int i = key0 + 1; i < key1; ++i)
        {
            length += mLengths[i];
        }
        
        // Add on partial first segment.
        length += GetLengthKey(key0, dt0, mTimes[key0 + 1] - mTimes[key0]);
        
        // Add on partial last segment.
        length += GetLengthKey(key1, (Real)0, dt1);
    }
    else
    {
        length = GetLengthKey(key0, dt0, dt1);
    }

    return length;
}
//----------------------------------------------------------------------------
template <typename Real>
Real MultipleCurve3<Real>::GetTime (Real length, int iterations,
    Real tolerance) const
{
    if (!mLengths)
    {
        InitializeLength();
    }

    if (length <= (Real)0)
    {
        return mTMin;
    }

    if (length >= mAccumLengths[mNumSegments - 1])
    {
        return mTMax;
    }

    int key;
    for (key = 0; key < mNumSegments; ++key)
    {
        if (length < mAccumLengths[key])
        {
            break;
        }
    }
    if (key >= mNumSegments)
    {
        return mTimes[mNumSegments];
    }

    Real len0, len1;
    if (key == 0)
    {
        len0 = length;
        len1 = mAccumLengths[0];
    }
    else
    {
        len0 = length - mAccumLengths[key - 1];
        len1 = mAccumLengths[key] - mAccumLengths[key - 1];
    }

    // If L(t) is the length function for t in [tmin,tmax], the derivative is
    // L'(t) = |x'(t)| >= 0 (the magnitude of speed).  Therefore, L(t) is a
    // nondecreasing function (and it is assumed that x'(t) is zero only at
    // isolated points; that is, no degenerate curves allowed).  The second
    // derivative is L"(t).  If L"(t) >= 0 for all t, L(t) is a convex
    // function and Newton's method for root finding is guaranteed to
    // converge.  However, L"(t) can be negative, which can lead to Newton
    // iterates outside the domain [tmin,tmax].  The algorithm here avoids
    // this problem by using a hybrid of Newton's method and bisection.

    // Initial guess for Newton's method is dt0.
    Real dt1 = mTimes[key + 1] - mTimes[key];
    Real dt0 = dt1*len0/len1;

    // Initial root-bounding interval for bisection.
    Real lower = (Real)0, upper = dt1;

    for (int i = 0; i < iterations; ++i)
    {
        Real difference = GetLengthKey(key, (Real)0, dt0) - len0;
        if (Math<Real>::FAbs(difference) <= tolerance)
        {
            // |L(mTimes[key]+dt0)-length| is close enough to zero, report
            // mTimes[key]+dt0 as the time at which 'length' is attained.
            return mTimes[key] + dt0;
        }

        // Generate a candidate for Newton's method.
        Real dt0Candidate = dt0 - difference/GetSpeedKey(key, dt0);

        // Update the root-bounding interval and test for containment of the
        // candidate.
        if (difference > (Real)0)
        {
            upper = dt0;
            if (dt0Candidate <= lower)
            {
                // Candidate is outside the root-bounding interval.  Use
                // bisection instead.
                dt0 = ((Real)0.5)*(upper + lower);
            }
            else
            {
                // There is no need to compare to 'upper' because the tangent
                // line has positive slope, guaranteeing that the t-axis
                // intercept is smaller than 'upper'.
                dt0 = dt0Candidate;
            }
        }
        else
        {
            lower = dt0;
            if (dt0Candidate >= upper)
            {
                // Candidate is outside the root-bounding interval.  Use
                // bisection instead.
                dt0 = ((Real)0.5)*(upper + lower);
            }
            else
            {
                // There is no need to compare to 'lower' because the tangent
                // line has positive slope, guaranteeing that the t-axis
                // intercept is larger than 'lower'.
                dt0 = dt0Candidate;
            }
        }
    }

    // A root was not found according to the specified number of iterations
    // and tolerance.  You might want to increase iterations or tolerance or
    // integration accuracy.  However, in this application it is likely that
    // the time values are oscillating, due to the limited numerical
    // precision of 32-bit floats.  It is safe to use the last computed time.
    return mTimes[key] + dt0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class MultipleCurve3<float>;

template WM5_MATHEMATICS_ITEM
class MultipleCurve3<double>;
//----------------------------------------------------------------------------
}
