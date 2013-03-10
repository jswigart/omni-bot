// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5GaussianBlur2.h"
using namespace Wm5;

//----------------------------------------------------------------------------
GaussianBlur2::GaussianBlur2 (int xBound, int yBound, float xSpacing,
    float ySpacing, const float* data, const bool* mask, float borderValue,
    ScaleType scaleType)
    :
    PdeFilter2(xBound, yBound, xSpacing, ySpacing, data, mask, borderValue,
        scaleType)
{
    mMaximumTimeStep = 0.5f/(mInvDxDx + mInvDyDy);
}
//----------------------------------------------------------------------------
GaussianBlur2::~GaussianBlur2 ()
{
}
//----------------------------------------------------------------------------
float GaussianBlur2::GetMaximumTimeStep () const
{
    return mMaximumTimeStep;
}
//----------------------------------------------------------------------------
void GaussianBlur2::OnUpdate (int x, int y)
{
    LookUp5(x, y);

    float uxx = mInvDxDx*(mUpz - 2.0f*mUzz + mUmz);
    float uyy = mInvDyDy*(mUzp - 2.0f*mUzz + mUzm);

    mDst[y][x] = mUzz + mTimeStep*(uxx + uyy);
}
//----------------------------------------------------------------------------
