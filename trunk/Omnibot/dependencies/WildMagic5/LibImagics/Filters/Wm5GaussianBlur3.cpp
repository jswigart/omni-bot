// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5GaussianBlur3.h"
using namespace Wm5;

//----------------------------------------------------------------------------
GaussianBlur3::GaussianBlur3 (int xBound, int yBound, int zBound,
    float xSpacing, float ySpacing, float zSpacing, const float* data,
    const bool* mask, float borderValue, ScaleType scaleType)
    :
    PdeFilter3(xBound, yBound, zBound, xSpacing, ySpacing, zSpacing, data,
        mask, borderValue, scaleType)
{
    mMaximumTimeStep = 0.5f/(mInvDxDx + mInvDyDy + mInvDzDz);
}
//----------------------------------------------------------------------------
GaussianBlur3::~GaussianBlur3 ()
{
}
//----------------------------------------------------------------------------
float GaussianBlur3::GetMaximumTimeStep () const
{
    return mMaximumTimeStep;
}
//----------------------------------------------------------------------------
void GaussianBlur3::OnUpdate (int x, int y, int z)
{
    LookUp7(x, y, z);

    float uxx = mInvDxDx*(mUpzz - 2.0f*mUzzz + mUmzz);
    float uyy = mInvDyDy*(mUzpz - 2.0f*mUzzz + mUzmz);
    float uzz = mInvDzDz*(mUzzp - 2.0f*mUzzz + mUzzm);

    mDst[z][y][x] = mUzzz + mTimeStep*(uxx + uyy + uzz);
}
//----------------------------------------------------------------------------
