// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5CurvatureFlow2.h"
using namespace Wm5;

//----------------------------------------------------------------------------
CurvatureFlow2::CurvatureFlow2 (int xBound, int yBound, float xSpacing,
    float ySpacing, const float* data, const bool* mask, float borderValue,
    ScaleType scaleType)
    :
    PdeFilter2(xBound, yBound, xSpacing, ySpacing, data, mask, borderValue,
        scaleType)
{
}
//----------------------------------------------------------------------------
CurvatureFlow2::~CurvatureFlow2 ()
{
}
//----------------------------------------------------------------------------
void CurvatureFlow2::OnUpdate (int x, int y)
{
    LookUp9(x, y);

    float ux = mHalfInvDx*(mUpz - mUmz);
    float uy = mHalfInvDy*(mUzp - mUzm);
    float uxx = mInvDxDx*(mUpz - 2.0f*mUzz + mUmz);
    float uxy = mFourthInvDxDy*(mUmm + mUpp - mUmp - mUpm);
    float uyy = mInvDyDy*(mUzp - 2.0f*mUzz + mUzm);

    float sqrUx = ux*ux;
    float sqrUy = uy*uy;
    float sqrLength = sqrUx + sqrUy + 1e-08f;  // prevent zero division
    mDst[y][x] = mUzz + mTimeStep*(uxx*sqrUy + uyy*sqrUx -
        0.5f*uxy*ux*uy)/sqrLength;
}
//----------------------------------------------------------------------------
