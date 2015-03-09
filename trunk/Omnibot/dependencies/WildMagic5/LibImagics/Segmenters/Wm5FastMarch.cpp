// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5FastMarch.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
FastMarch::FastMarch (int quantity, const float* speeds,
    const std::vector<int>& seeds)
    :
    mHeap(quantity, 1, Mathf::MAX_REAL)
{
    Initialize(quantity,seeds);
    InitializeSpeed(speeds);
}
//----------------------------------------------------------------------------
FastMarch::FastMarch (int quantity, const float speed,
    const std::vector<int>& seeds)
    :
    mHeap(quantity, 1, Mathf::MAX_REAL)
{
    Initialize(quantity, seeds);
    InitializeSpeed(speed);
}
//----------------------------------------------------------------------------
FastMarch::~FastMarch ()
{
    delete1(mTrials);
    delete1(mInvSpeeds);
    delete1(mTimes);
}
//----------------------------------------------------------------------------
void FastMarch::Initialize (int quantity, const std::vector<int>& seeds)
{
    mQuantity = quantity;

    // The seed points have a crossing time of 0.  As the iterations occur,
    // some of the non-seed points are visited by the moving front.  The
    // valid crossing times are 0 <= t < MAX_REAL.  A value of MAX_REAL
    // indicates the pixel has not yet been reached by the moving front.  If
    // the speed value at a pixel is 0, the pixel is marked with a time of
    // -MAX_REAL.  Such pixels can never be visited; the minus sign
    // distinguishes these from pixels not yet reached during iteration.
    mTimes = new1<float>(mQuantity);
    int i;
    for (i = 0; i < mQuantity; ++i)
    {
        mTimes[i] = Mathf::MAX_REAL;
    }

    for (i = 0; i < (int)seeds.size(); ++i)
    {
        mTimes[seeds[i]] = 0.0f;
    }

    // Trial pixels are identified by having min-heap record associated with
    // them.  Known or Far pixels have no associated record.
    mTrials = new1<const MinHeapRecord<int,float>*>(mQuantity);
    size_t numBytes = mQuantity*sizeof(const MinHeapRecord<int,float>*);
    memset(mTrials, 0, numBytes);
}
//----------------------------------------------------------------------------
void FastMarch::InitializeSpeed (const float* speeds)
{
    // Invert the speeds since the reciprocals are all that are needed in the
    // numerical method.  Mark all zero-speed interior pixels.
    mInvSpeeds = new1<float>(mQuantity);
    for (int i = 0; i < mQuantity; ++i)
    {
        assertion(speeds[i] >= 0.0f, "Invalid speed\n");
        if (speeds[i] > 0.0f)
        {
            mInvSpeeds[i] = 1.0f/speeds[i];
        }
        else
        {
            mInvSpeeds[i] = Mathf::MAX_REAL;
            mTimes[i] = -Mathf::MAX_REAL;
        }
    }
}
//----------------------------------------------------------------------------
void FastMarch::InitializeSpeed (const float speed)
{
    float invSpeed = 1.0f/speed;
    mInvSpeeds = new1<float>(mQuantity);
    for (int i = 0; i < mQuantity; ++i)
    {
        mInvSpeeds[i] = invSpeed;
    }
}
//----------------------------------------------------------------------------
void FastMarch::GetTimeExtremes (float& minValue, float& maxValue) const
{
    minValue = Mathf::MAX_REAL;
    maxValue = -Mathf::MAX_REAL;
    int i;
    for (i = 0; i < mQuantity; ++i)
    {
        if (IsValid(i))
        {
            minValue = mTimes[i];
            maxValue = minValue;
            break;
        }
    }
    assertion(i < mQuantity, "At least one time must be valid\n");

    for (/**/; i < mQuantity; ++i)
    {
        if (IsValid(i))
        {
            if (mTimes[i] < minValue)
            {
                minValue = mTimes[i];
            }
            else if (mTimes[i] > maxValue)
            {
                maxValue = mTimes[i];
            }
        }
    }
}
//----------------------------------------------------------------------------
void FastMarch::GetInterior (std::vector<int>& interior) const
{
    for (int i = 0; i < mQuantity; ++i)
    {
        if (IsValid(i) && !IsTrial(i))
        {
            interior.push_back(i);
        }
    }
}
//----------------------------------------------------------------------------
