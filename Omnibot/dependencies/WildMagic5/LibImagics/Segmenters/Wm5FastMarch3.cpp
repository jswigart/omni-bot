// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5FastMarch3.h"
#include "Wm5Assert.h"
using namespace Wm5;

const float FastMarch3::msOneThird = 1.0f/3.0f;

//----------------------------------------------------------------------------
FastMarch3::FastMarch3 (int xBound, int yBound, int zBound,
    float xSpacing, float ySpacing, float zSpacing, const float* speeds,
    const std::vector<int>& seeds)
    :
    FastMarch(xBound*yBound*zBound, speeds, seeds)
{
    Initialize(xBound, yBound, zBound, xSpacing, ySpacing, zSpacing);
}
//----------------------------------------------------------------------------
FastMarch3::FastMarch3 (int xBound, int yBound, int zBound,
    float xSpacing, float ySpacing, float zSpacing, const float speed,
    const std::vector<int>& seeds)
    :
    FastMarch(xBound*yBound*zBound, speed, seeds)
{
    Initialize(xBound, yBound, zBound, xSpacing, ySpacing, zSpacing);
}
//----------------------------------------------------------------------------
FastMarch3::~FastMarch3 ()
{
}
//----------------------------------------------------------------------------
void FastMarch3::Initialize (int xBound, int yBound, int zBound,
    float xSpacing, float ySpacing, float zSpacing)
{
    mXBound = xBound;
    mYBound = yBound;
    mZBound = zBound;
    mXYBound = xBound*yBound;
    mXBm1 = mXBound - 1;
    mYBm1 = mYBound - 1;
    mZBm1 = mZBound - 1;
    mXSpacing = xSpacing;
    mYSpacing = ySpacing;
    mZSpacing = zSpacing;
    mInvXSpacing = 1.0f/xSpacing;
    mInvYSpacing = 1.0f/ySpacing;
    mInvZSpacing = 1.0f/zSpacing;

    // Boundary pixels are marked as zero speed to allow us to avoid having
    // to process the boundary pixels separately during the iteration.
    int x, y, z, i;

    // vertex (0,0,0)
    i = Index(0,0,0);
    mInvSpeeds[i] = Mathf::MAX_REAL;
    mTimes[i] = -Mathf::MAX_REAL;

    // vertex (xmax,0,0)
    i = Index(mXBm1,0,0);
    mInvSpeeds[i] = Mathf::MAX_REAL;
    mTimes[i] = -Mathf::MAX_REAL;

    // vertex (0,ymax,0)
    i = Index(0,mYBm1,0);
    mInvSpeeds[i] = Mathf::MAX_REAL;
    mTimes[i] = -Mathf::MAX_REAL;

    // vertex (xmax,ymax,0)
    i = Index(mXBm1,mYBm1,0);
    mInvSpeeds[i] = Mathf::MAX_REAL;
    mTimes[i] = -Mathf::MAX_REAL;

    // vertex (0,0,zmax)
    i = Index(0,0,mZBm1);
    mInvSpeeds[i] = Mathf::MAX_REAL;
    mTimes[i] = -Mathf::MAX_REAL;

    // vertex (xmax,0,zmax)
    i = Index(mXBm1,0,mZBm1);
    mInvSpeeds[i] = Mathf::MAX_REAL;
    mTimes[i] = -Mathf::MAX_REAL;

    // vertex (0,ymax,zmax)
    i = Index(0,mYBm1,mZBm1);
    mInvSpeeds[i] = Mathf::MAX_REAL;
    mTimes[i] = -Mathf::MAX_REAL;

    // vertex (xmax,ymax,zmax)
    i = Index(mXBm1,mYBm1,mZBm1);
    mInvSpeeds[i] = Mathf::MAX_REAL;
    mTimes[i] = -Mathf::MAX_REAL;

    // edges (x,0,0) and (x,ymax,0)
    for (x = 0; x < mXBound; ++x)
    {
        i = Index(x,0,0);
        mInvSpeeds[i] = Mathf::MAX_REAL;
        mTimes[i] = -Mathf::MAX_REAL;
        i = Index(x,mYBm1,0);
        mInvSpeeds[i] = Mathf::MAX_REAL;
        mTimes[i] = -Mathf::MAX_REAL;
    }

    // edges (0,y,0) and (xmax,y,0)
    for (y = 0; y < mYBound; ++y)
    {
        i = Index(0,y,0);
        mInvSpeeds[i] = Mathf::MAX_REAL;
        mTimes[i] = -Mathf::MAX_REAL;
        i = Index(mXBm1,y,0);
        mInvSpeeds[i] = Mathf::MAX_REAL;
        mTimes[i] = -Mathf::MAX_REAL;
    }

    // edges (x,0,zmax) and (x,ymax,zmax)
    for (x = 0; x < mXBound; ++x)
    {
        i = Index(x,0,mZBm1);
        mInvSpeeds[i] = Mathf::MAX_REAL;
        mTimes[i] = -Mathf::MAX_REAL;
        i = Index(x,mYBm1,mZBm1);
        mInvSpeeds[i] = Mathf::MAX_REAL;
        mTimes[i] = -Mathf::MAX_REAL;
    }

    // edges (0,y,zmax) and (xmax,y,zmax)
    for (y = 0; y < mYBound; ++y)
    {
        i = Index(0,y,mZBm1);
        mInvSpeeds[i] = Mathf::MAX_REAL;
        mTimes[i] = -Mathf::MAX_REAL;
        i = Index(mXBm1,y,mZBm1);
        mInvSpeeds[i] = Mathf::MAX_REAL;
        mTimes[i] = -Mathf::MAX_REAL;
    }

    // edges (0,0,z) and (xmax,0,z)
    for (z = 0; z < mZBound; ++z)
    {
        i = Index(0,0,z);
        mInvSpeeds[i] = Mathf::MAX_REAL;
        mTimes[i] = -Mathf::MAX_REAL;
        i = Index(mXBm1,0,z);
        mInvSpeeds[i] = Mathf::MAX_REAL;
        mTimes[i] = -Mathf::MAX_REAL;
    }

    // edges (0,ymax,z) and (xmax,ymax,z)
    for (z = 0; z < mZBound; ++z)
    {
        i = Index(0,mYBm1,z);
        mInvSpeeds[i] = Mathf::MAX_REAL;
        mTimes[i] = -Mathf::MAX_REAL;
        i = Index(mXBm1,mYBm1,z);
        mInvSpeeds[i] = Mathf::MAX_REAL;
        mTimes[i] = -Mathf::MAX_REAL;
    }

    // Compute the first batch of trial pixels.  These are pixels a grid
    // distance of one away from the seed pixels.
    for (z = 1; z < mZBm1; ++z)
    {
        for (y = 1; y < mYBm1; ++y)
        {
            for (x = 1; x < mXBm1; ++x)
            {
                i = Index(x,y,z);
                if (IsFar(i))
                {
                    if ((IsValid(i-1) && !IsTrial(i-1))
                    ||  (IsValid(i+1) && !IsTrial(i+1))
                    ||  (IsValid(i-mXBound) && !IsTrial(i-mXBound))
                    ||  (IsValid(i+mXBound) && !IsTrial(i+mXBound))
                    ||  (IsValid(i-mXYBound) && !IsTrial(i-mXYBound))
                    ||  (IsValid(i+mXYBound) && !IsTrial(i+mXYBound)))
                    {
                        ComputeTime(i);
                        mTrials[i] = mHeap.Insert(i, mTimes[i]);
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void FastMarch3::GetBoundary (std::vector<int>& rkBoundary) const
{
    for (int i = 0; i < mQuantity; ++i)
    {
        if (IsValid(i) && !IsTrial(i))
        {
            if (IsTrial(i-1)
            ||  IsTrial(i+1)
            ||  IsTrial(i-mXBound)
            ||  IsTrial(i+mXBound)
            ||  IsTrial(i-mXYBound)
            ||  IsTrial(i+mXYBound))
            {
                rkBoundary.push_back(i);
            }
        }
    }
}
//----------------------------------------------------------------------------
bool FastMarch3::IsBoundary (int i) const
{
    if (IsValid(i) && !IsTrial(i))
    {
        if (IsTrial(i-1)
        ||  IsTrial(i+1)
        ||  IsTrial(i-mXBound)
        ||  IsTrial(i+mXBound)
        ||  IsTrial(i-mXYBound)
        ||  IsTrial(i+mXYBound))
        {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
void FastMarch3::Iterate ()
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

    int iMXYB = i - mXYBound;
    if (IsTrial(iMXYB))
    {
        ComputeTime(iMXYB);
        mHeap.Update(mTrials[iMXYB], mTimes[iMXYB]);
    }
    else if (IsFar(iMXYB))
    {
        ComputeTime(iMXYB);
        mTrials[iMXYB] = mHeap.Insert(iMXYB, mTimes[iMXYB]);
    }

    int iPXYB = i + mXYBound;
    if (IsTrial(iPXYB))
    {
        ComputeTime(iPXYB);
        mHeap.Update(mTrials[iPXYB], mTimes[iPXYB]);
    }
    else if (IsFar(iPXYB))
    {
        ComputeTime(iPXYB);
        mTrials[iPXYB] = mHeap.Insert(iPXYB, mTimes[iPXYB]);
    }
}
//----------------------------------------------------------------------------
void FastMarch3::ComputeTime (int i)
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

    bool hasZTerm;
    float zConst;
    if (IsValid(i-mXYBound))
    {
        hasZTerm = true;
        zConst = mTimes[i-mXYBound];
        if (IsValid(i+mXYBound))
        {
            if (mTimes[i+mXYBound] < zConst)
            {
                zConst = mTimes[i+mXYBound];
            }
        }
    }
    else if (IsValid(i+mXYBound))
    {
        hasZTerm = true;
        zConst = mTimes[i+mXYBound];
    }
    else
    {
        hasZTerm = false;
        zConst = 0.0f;
    }

    float sum, diff, discr;

    if (hasXTerm)
    {
        if (hasYTerm)
        {
            if (hasZTerm)
            {
                // xyz
                sum = xConst + yConst + zConst;
                discr = 3.0f*mInvSpeeds[i]*mInvSpeeds[i];
                diff = xConst - yConst;
                discr -= diff*diff;
                diff = xConst - zConst;
                discr -= diff*diff;
                diff = yConst - zConst;
                discr -= diff*diff;
                if (discr >= 0.0f)
                {
                    // The quadratic equation has a real-valued solution.
                    // Choose the largest positive root for the crossing time.
                    mTimes[i] = msOneThird*(sum + Mathf::Sqrt(discr));
                }
                else
                {
                    // The quadratic equation does not have a real-valued
                    // solution.  This can happen when the speed is so large
                    // that the time gradient has very small length, which
                    // means that the time has not changed significantly from
                    // the neighbors to the current pixel.  Just choose the
                    // maximum time of the neighbors.  Is there a better
                    // choice?.
                    mTimes[i] = xConst;
                    if (yConst > mTimes[i])
                    {
                        mTimes[i] = yConst;
                    }
                    if (zConst > mTimes[i])
                    {
                        mTimes[i] = zConst;
                    }
                }
            }
            else
            {
                // xy
                sum = xConst + yConst;
                diff = xConst - yConst;
                discr = 2.0f*mInvSpeeds[i]*mInvSpeeds[i] - diff*diff;
                if (discr >= 0.0f)
                {
                    // The quadratic equation has a real-valued solution.
                    // Choose the largest positive root for the crossing time.
                    mTimes[i] = 0.5f*(sum + Mathf::Sqrt(discr));
                }
                else
                {
                    // The quadratic equation does not have a real-valued
                    // solution.  This can happen when the speed is so large
                    // that the time gradient has very small length, which
                    // means that the time has not changed significantly from
                    // the neighbors to the current pixel.  Just choose the
                    // maximum time of the neighbors.  Is there a better
                    // choice?.
                    mTimes[i] = (diff >= 0.0f ? xConst : yConst);
                }
            }
        }
        else
        {
            if (hasZTerm)
            {
                // xz
                sum = xConst + zConst;
                diff = xConst - zConst;
                discr = 2.0f*mInvSpeeds[i]*mInvSpeeds[i] - diff*diff;
                if (discr >= 0.0f)
                {
                    // The quadratic equation has a real-valued solution.
                    // Choose the largest positive root for the crossing time.
                    mTimes[i] = 0.5f*(sum + Mathf::Sqrt(discr));
                }
                else
                {
                    // The quadratic equation does not have a real-valued
                    // solution.  This can happen when the speed is so large
                    // that the time gradient has very small length, which
                    // means that the time has not changed significantly from
                    // the neighbors to the current pixel.  Just choose the
                    // maximum time of the neighbors.  Is there a better
                    // choice?.
                    mTimes[i] = (diff >= 0.0f ? xConst : zConst);
                }
            }
            else
            {
                // x
                mTimes[i] = mInvSpeeds[i] + xConst;
            }
        }
    }
    else
    {
        if (hasYTerm)
        {
            if (hasZTerm)
            {
                // yz
                sum = yConst + zConst;
                diff = yConst - zConst;
                discr = 2.0f*mInvSpeeds[i]*mInvSpeeds[i] - diff*diff;
                if (discr >= 0.0f)
                {
                    // The quadratic equation has a real-valued solution.
                    // Choose the largest positive root for the crossing time.
                    mTimes[i] = 0.5f*(sum + Mathf::Sqrt(discr));
                }
                else
                {
                    // The quadratic equation does not have a real-valued
                    // solution.  This can happen when the speed is so large
                    // that the time gradient has very small length, which
                    // means that the time has not changed significantly from
                    // the neighbors to the current pixel.  Just choose the
                    // maximum time of the neighbors.  Is there a better
                    // choice?.
                    mTimes[i] = (diff >= 0.0f ? yConst : zConst);
                }
            }
            else
            {
                // y
                mTimes[i] = mInvSpeeds[i] + yConst;
            }
        }
        else
        {
            if (hasZTerm)
            {
                // z
                mTimes[i] = mInvSpeeds[i] + zConst;
            }
            else
            {
                // No terms, which cannot happen.  The voxel must have at
                // least one valid neighbor.
                assertion(false, "Voxel must have a known neighbor\n");
            }
        }
    }
}
//----------------------------------------------------------------------------
