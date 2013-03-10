// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5PdeFilter.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PdeFilter::PdeFilter (int quantity, const float* data, float borderValue,
    ScaleType scaleType)
{
    mQuantity = quantity;
    mBorderValue = borderValue;
    mScaleType = scaleType;
    mTimeStep = 0.0f;

    float maxValue = data[0];
    mMin = maxValue;
    for (int i = 1; i < mQuantity; i++)
    {
        float value = data[i];
        if (value < mMin)
        {
            mMin = value;
        }
        else if (value > maxValue)
        {
            maxValue = value;
        }
    }

    if (mMin != maxValue)
    {
        switch (mScaleType)
        {
        case ST_NONE:
            mOffset = 0.0f;
            mScale = 1.0f;
            break;
        case ST_UNIT:
            mOffset = 0.0f;
            mScale = 1.0f/(maxValue - mMin);
            break;
        case ST_SYMMETRIC:
            mOffset = -1.0f;
            mScale = 2.0f/(maxValue - mMin);
            break;
        case ST_PRESERVE_ZERO:
            mOffset = 0.0f;
            mScale = (maxValue >= -mMin ? 1.0f/maxValue : -1.0f/mMin);
            mMin = 0.0f;
            break;
        }
    }
    else
    {
        mOffset = 0.0f;
        mScale = 1.0f;
    }
}
//----------------------------------------------------------------------------
PdeFilter::~PdeFilter ()
{
}
//----------------------------------------------------------------------------
void PdeFilter::Update ()
{
    OnPreUpdate();
    OnUpdate();
    OnPostUpdate();
}
//----------------------------------------------------------------------------
