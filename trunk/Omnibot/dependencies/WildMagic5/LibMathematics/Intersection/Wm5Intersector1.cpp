// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Intersector1.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Intersector1<Real>::Intersector1 (Real u0, Real u1, Real v0, Real v1)
{
    assertion(u0 <= u1 && v0 <= v1, "Malformed interval\n");
    mU[0] = u0;
    mU[1] = u1;
    mV[0] = v0;
    mV[1] = v1;
    mFirstTime = (Real)0;
    mLastTime = (Real)0;
    mNumIntersections = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
Intersector1<Real>::Intersector1 (Real u[2], Real v[2])
{
    assertion(u[0] <= u[1] && v[0] <= v[1], "Malformed interval\n");
    for (int i = 0; i < 2; ++i)
    {
        mU[i] = u[i];
        mV[i] = v[i];
    }
    mFirstTime = (Real)0;
    mLastTime = (Real)0;
    mNumIntersections = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
Intersector1<Real>::~Intersector1 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
Real Intersector1<Real>::GetU (int i) const
{
    return mU[i];
}
//----------------------------------------------------------------------------
template <typename Real>
Real Intersector1<Real>::GetV (int i) const
{
    return mV[i];
}
//----------------------------------------------------------------------------
template <typename Real>
bool Intersector1<Real>::Test ()
{
    return mU[0] <= mV[1] && mU[1] >= mV[0];
}
//----------------------------------------------------------------------------
template <typename Real>
bool Intersector1<Real>::Find ()
{
    if (mU[1] < mV[0] || mU[0] > mV[1])
    {
        mNumIntersections = 0;
    }
    else if (mU[1] > mV[0])
    {
        if (mU[0] < mV[1])
        {
            mNumIntersections = 2;
            mIntersections[0] = (mU[0] < mV[0] ? mV[0] : mU[0]);
            mIntersections[1] = (mU[1] > mV[1] ? mV[1] : mU[1]);
            if (mIntersections[0] == mIntersections[1])
            {
                mNumIntersections = 1;
            }
        }
        else  // mU[0] == mV[1]
        {
            mNumIntersections = 1;
            mIntersections[0] = mU[0];
        }
    }
    else  // mU[1] == mV[0]
    {
        mNumIntersections = 1;
        mIntersections[0] = mU[1];
    }

    return mNumIntersections > 0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Intersector1<Real>::Test (Real tmax, Real speedU, Real speedV)
{
    Real diffSpeed, invDiffSpeed, diffPos;

    if (mU[1] < mV[0])
    {
        // [u0,u1] initially to the left of [v0,v1].
        diffSpeed = speedU - speedV;
        if (diffSpeed > (Real)0)
        {
            // The intervals must move towards each other.
            diffPos = mV[0] - mU[1];
            if (diffPos <= tmax*diffSpeed)
            {
                // The intervals intersect within the specified time.
                invDiffSpeed = ((Real)1)/diffSpeed;
                mFirstTime = diffPos*invDiffSpeed;
                mLastTime = (mV[1] - mU[0])*invDiffSpeed;
                return true;
            }
        }
    }
    else if (mU[0] > mV[1])
    {
        // [u0,u1] initially to the right of [v0,v1].
        diffSpeed = speedV - speedU;
        if (diffSpeed > (Real)0)
        {
            // The intervals must move towards each other.
            diffPos = mU[0] - mV[1];
            if (diffPos <= tmax*diffSpeed)
            {
                // The intervals intersect within the specified time.
                invDiffSpeed = ((Real)1)/diffSpeed;
                mFirstTime = diffPos*invDiffSpeed;
                mLastTime = (mU[1] - mV[0])*invDiffSpeed;
                return true;
            }
        }
    }
    else
    {
        // The intervals are initially intersecting.
        mFirstTime = (Real)0;
        if (speedV > speedU)
        {
            mLastTime = (mU[1] - mV[0])/(speedV - speedU);
        }
        else if (speedV < speedU)
        {
            mLastTime = (mV[1] - mU[0])/(speedU - speedV);
        }
        else
        {
            mLastTime = Math<Real>::MAX_REAL;
        }

        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Intersector1<Real>::Find (Real tmax, Real speedU, Real speedV)
{
    Real diffSpeed, invDiffSpeed, diffPos;

    if (mU[1] < mV[0])
    {
        // [u0,u1] initially to the left of [v0,v1].
        diffSpeed = speedU - speedV;
        if (diffSpeed > (Real)0)
        {
            // The intervals must move towards each other.
            diffPos = mV[0] - mU[1];
            if (diffPos <= tmax*diffSpeed)
            {
                // The intervals intersect within the specified time.
                invDiffSpeed = ((Real)1)/diffSpeed;
                mFirstTime = diffPos*invDiffSpeed;
                mLastTime = (mV[1] - mU[0])*invDiffSpeed;
                mNumIntersections = 1;
                mIntersections[0] = mU[0] + mFirstTime*speedU;
                return true;
            }
        }
    }
    else if (mU[0] > mV[1])
    {
        // [u0,u1] initially to the right of [v0,v1].
        diffSpeed = speedV - speedU;
        if (diffSpeed > (Real)0)
        {
            // The intervals must move towards each other.
            diffPos = mU[0] - mV[1];
            if (diffPos <= tmax*diffSpeed)
            {
                // The intervals intersect within the specified time.
                invDiffSpeed = ((Real)1)/diffSpeed;
                mFirstTime = diffPos*invDiffSpeed;
                mLastTime = (mU[1] - mV[0])*invDiffSpeed;
                mNumIntersections = 1;
                mIntersections[0] = mV[1] + mFirstTime*speedV;
                return true;
            }
        }
    }
    else
    {
        // The intervals are initially intersecting.
        mFirstTime = (Real)0;
        if (speedV > speedU)
        {
            mLastTime = (mU[1] - mV[0])/(speedV - speedU);
        }
        else if (speedV < speedU)
        {
            mLastTime = (mV[1] - mU[0])/(speedU - speedV);
        }
        else
        {
            mLastTime = Math<Real>::MAX_REAL;
        }

        if (mU[1] > mV[0])
        {
            if (mU[0] < mV[1])
            {
                mNumIntersections = 2;
                mIntersections[0] = (mU[0] < mV[0] ? mV[0] : mU[0]);
                mIntersections[1] = (mU[1] > mV[1] ? mV[1] : mU[1]);
            }
            else  // mU[0] == mV[1]
            {
                mNumIntersections = 1;
                mIntersections[0] = mU[0];
            }
        }
        else  // mU[1] == mV[0]
        {
            mNumIntersections = 1;
            mIntersections[0] = mU[1];
        }
        return true;
    }

    mNumIntersections = 0;
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Intersector1<Real>::GetFirstTime () const
{
    return mFirstTime;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Intersector1<Real>::GetLastTime () const
{
    return mLastTime;
}
//----------------------------------------------------------------------------
template <typename Real>
int Intersector1<Real>::GetNumIntersections () const
{
    return mNumIntersections;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Intersector1<Real>::GetIntersection (int i) const
{
    return mIntersections[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class Intersector1<float>;

template WM5_MATHEMATICS_ITEM
class Intersector1<double>;
//----------------------------------------------------------------------------
}
