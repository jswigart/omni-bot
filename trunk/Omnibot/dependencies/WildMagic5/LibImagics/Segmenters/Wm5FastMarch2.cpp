// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5FastMarch2.h"
#include "Wm5Assert.h"
using namespace Wm5;

//----------------------------------------------------------------------------
FastMarch2::FastMarch2 (int xBound, int yBound, float xSpacing,
    float ySpacing, const float* speeds, const std::vector<int>& seeds)
    :
    FastMarch(xBound*yBound, speeds, seeds)
{
    Initialize(xBound, yBound, xSpacing, ySpacing);
}
//----------------------------------------------------------------------------
FastMarch2::FastMarch2 (int xBound, int yBound, float xSpacing,
    float ySpacing, const float speed, const std::vector<int>& seeds)
    :
    FastMarch(xBound*yBound, speed, seeds)
{
    Initialize(xBound, yBound, xSpacing, ySpacing);
}
//----------------------------------------------------------------------------
FastMarch2::~FastMarch2 ()
{
}
//----------------------------------------------------------------------------
void FastMarch2::Initialize (int xBound, int yBound, float xSpacing,
    float ySpacing)
{
    mXBound = xBound;
    mYBound = yBound;
    mXBm1 = mXBound - 1;
    mYBm1 = mYBound - 1;
    mXSpacing = xSpacing;
    mYSpacing = ySpacing;
    mInvXSpacing = 1.0f/xSpacing;
    mInvYSpacing = 1.0f/ySpacing;

    // Boundary pixels are marked as zero speed to allow us to avoid having
    // to process the boundary pixels separately during the iteration.
    int x, y, i;

    // vertex (0,0)
    i = Index(0,0);
    mInvSpeeds[i] = Mathf::MAX_REAL;
    mTimes[i] = -Mathf::MAX_REAL;

    // vertex (xmax,0)
    i = Index(mXBm1,0);
    mInvSpeeds[i] = Mathf::MAX_REAL;
    mTimes[i] = -Mathf::MAX_REAL;

    // vertex (0,ymax)
    i = Index(0,mYBm1);
    mInvSpeeds[i] = Mathf::MAX_REAL;
    mTimes[i] = -Mathf::MAX_REAL;

    // vertex (xmax,ymax)
    i = Index(mXBm1,mYBm1);
    mInvSpeeds[i] = Mathf::MAX_REAL;
    mTimes[i] = -Mathf::MAX_REAL;

    // edges (x,0) and (x,ymax)
    for (x = 0; x < mXBound; ++x)
    {
        i = Index(x,0);
        mInvSpeeds[i] = Mathf::MAX_REAL;
        mTimes[i] = -Mathf::MAX_REAL;
        i = Index(x,mYBm1);
        mInvSpeeds[i] = Mathf::MAX_REAL;
        mTimes[i] = -Mathf::MAX_REAL;
    }

    // edges (0,y) and (xmax,y)
    for (y = 0; y < mYBound; ++y)
    {
        i = Index(0,y);
        mInvSpeeds[i] = Mathf::MAX_REAL;
        mTimes[i] = -Mathf::MAX_REAL;
        i = Index(mXBm1,y);
        mInvSpeeds[i] = Mathf::MAX_REAL;
        mTimes[i] = -Mathf::MAX_REAL;
    }

    // Compute the first batch of trial pixels.  These are pixels a grid
    // distance of one away from the seed pixels.
    for (y = 1; y < mYBm1; ++y)
    {
        for (x = 1; x < mXBm1; ++x)
        {
            i = Index(x,y);
            if (IsFar(i))
            {
                if ((IsValid(i-1) && !IsTrial(i-1))
                ||  (IsValid(i+1) && !IsTrial(i+1))
                ||  (IsValid(i-mXBound) && !IsTrial(i-mXBound))
                ||  (IsValid(i+mXBound) && !IsTrial(i+mXBound)))
                {
                    ComputeTime(i);
                    mTrials[i] = mHeap.Insert(i, mTimes[i]);
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void FastMarch2::GetBoundary (std::vector<int>& boundary) const
{
    for (int i = 0; i < mQuantity; ++i)
    {
        if (IsValid(i) && !IsTrial(i))
        {
            if (IsTrial(i-1)
            ||  IsTrial(i+1)
            ||  IsTrial(i-mXBound)
            ||  IsTrial(i+mXBound))
            {
                boundary.push_back(i);
            }
        }
    }
}
//----------------------------------------------------------------------------
bool FastMarch2::IsBoundary (int i) const
{
    if (IsValid(i) && !IsTrial(i))
    {
        if (IsTrial(i-1)
        ||  IsTrial(i+1)
        ||  IsTrial(i-mXBound)
        ||  IsTrial(i+mXBound))
        {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
void FastMarch2::Iterate ()
{
    // Remove the minimum trial value from the heap.
    int i;
    float value;
    mHeap.Remove(i, value);

    // Promote the trial value to a known value.  The value was negative but
    // is now nonnegative (the heap stores only nonnegative numbers).
    assertion(IsTrial(i) && mTimes[i] == value, "Unexpected condition\n");
    mTrials[i] = 0;

    // All trial pixels must be updated.  All far neighbors must become trial
    // pixels.
    int iM1 = i - 1;
    if (IsTrial(iM1))
    {
        ComputeTime(iM1);
        mHeap.Update(mTrials[iM1], mTimes[iM1]);
    }
    else if (IsFar(iM1))
    {
        ComputeTime(iM1);
        mTrials[iM1] = mHeap.Insert(iM1, mTimes[iM1]);
    }

    int iP1 = i + 1;
    if (IsTrial(iP1))
    {
        ComputeTime(iP1);
        mHeap.Update(mTrials[iP1], mTimes[iP1]);
    }
    else if (IsFar(iP1))
    {
        ComputeTime(iP1);
        mTrials[iP1] = mHeap.Insert(iP1, mTimes[iP1]);
    }

    int iMXB = i - mXBound;
    if (IsTrial(iMXB))
    {
        ComputeTime(iMXB);
        mHeap.Update(mTrials[iMXB], mTimes[iMXB]);
    }
    else if (IsFar(iMXB))
    {
        ComputeTime(iMXB);
        mTrials[iMXB] = mHeap.Insert(iMXB, mTimes[iMXB]);
    }

    int iPXB = i + mXBound;
    if (IsTrial(iPXB))
    {
        ComputeTime(iPXB);
        mHeap.Update(mTrials[iPXB], mTimes[iPXB]);
    }
    else if (IsFar(iPXB))
    {
        ComputeTime(iPXB);
        mTrials[iPXB] = mHeap.Insert(iPXB, mTimes[iPXB]);
    }
}
//----------------------------------------------------------------------------
void FastMarch2::ComputeTime (int i)
{
    bool hasXTerm;
    float xConst;
    if (IsValid(i-1))
    {
        hasXTerm = true;
        xConst = mTimes[i-1];
        if (IsValid(i+1))
        {
            if (mTimes[i+1] < xConst)
            {
                xConst = mTimes[i+1];
            }
        }
    }
    else if (IsValid(i+1))
    {
        hasXTerm = true;
        xConst = mTimes[i+1];
    }
    else
    {
        hasXTerm = false;
        xConst = 0.0f;
    }

    bool hasYTerm;
    float yConst;
    if (IsValid(i-mXBound))
    {
        hasYTerm = true;
        yConst = mTimes[i-mXBound];
        if (IsValid(i+mXBound))
        {
            if (mTimes[i+mXBound] < yConst)
            {
                yConst = mTimes[i+mXBound];
            }
        }
    }
    else if (IsValid(i+mXBound))
    {
        hasYTerm = true;
        yConst = mTimes[i+mXBound];
    }
    else
    {
        hasYTerm = false;
        yConst = 0.0f;
    }

    if (hasXTerm)
    {
        if (hasYTerm)
        {
            float sum = xConst + yConst;
            float diff = xConst - yConst;
            float discr = 2.0f*mInvSpeeds[i]*mInvSpeeds[i] - diff*diff;
            if (discr >= 0.0f)
            {
                // The quadratic equation has a real-valued solution.  Choose
                // the largest positive root for the crossing time.
                mTimes[i] = 0.5f*(sum + Mathf::Sqrt(discr));
            }
            else
            {
                // The quadratic equation does not have a real-valued
                // solution.  This can happen when the speed is so large
                // that the time gradient has very small length, which means
                // that the time has not changed significantly from the
                // neighbors to the current pixel.  Just choose the maximum
                // time of the neighbors (Is there a better choice?).
                mTimes[i] = (diff >= 0.0f ? xConst : yConst);
            }
        }
        else
        {
            // The equation is linear.
            mTimes[i] = mInvSpeeds[i] + xConst;
        }
    }
    else if (hasYTerm)
    {
        // The equation is linear.
        mTimes[i] = mInvSpeeds[i] + yConst;
    }
    else
    {
        // The pixel must have at least one known neighbor.
        assertion(false, "Pixel must have a known neighbor\n");
    }
}
//----------------------------------------------------------------------------
