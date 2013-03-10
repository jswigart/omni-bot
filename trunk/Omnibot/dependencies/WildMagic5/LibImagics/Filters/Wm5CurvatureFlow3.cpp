// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5CurvatureFlow3.h"
using namespace Wm5;

//----------------------------------------------------------------------------
CurvatureFlow3::CurvatureFlow3 (int xBound, int yBound, int zBound,
    float xSpacing, float ySpacing, float zSpacing, const float* data,
    const bool* mask, float borderValue, ScaleType scaleType)
    :
    PdeFilter3(xBound,yBound,zBound,xSpacing,ySpacing,zSpacing,data,
        mask,borderValue,scaleType)
{
}
//----------------------------------------------------------------------------
CurvatureFlow3::~CurvatureFlow3 ()
{
}
//----------------------------------------------------------------------------
void CurvatureFlow3::OnUpdate (int x, int y, int z)
{
    LookUp27(x, y, z);

    float ux = mHalfInvDx*(mUpzz - mUmzz);
    float uy = mHalfInvDy*(mUzpz - mUzmz);
    float uz = mHalfInvDz*(mUzzp - mUzzm);
    float uxx = mInvDxDx*(mUpzz - 2.0f*mUzzz + mUmzz);
    float uxy = mFourthInvDxDy*(mUmmz + mUppz - mUpmz - mUmpz);
    float uxz = mFourthInvDxDz*(mUmzm + mUpzp - mUpzm - mUmzp);
    float uyy = mInvDyDy*(mUzpz - 2.0f*mUzzz + mUzmz);
    float uyz = mFourthInvDyDz*(mUzmm+ mUzpp - mUzpm  - mUzmp);
    float uzz = mInvDzDz*(mUzzp - 2.0f*mUzzz + mUzzm);

    float gradMagSqr = ux*ux + uy*uy + uz*uz;
    float numer0 = uy*(uxx*uy - uxy*ux) + ux*(uyy*ux - uxy*uy);
    float numer1 = uz*(uxx*uz - uxz*ux) + ux*(uzz*ux - uxz*uz);
    float numer2 = uz*(uyy*uz - uyz*uy) + uy*(uzz*uy - uyz*uz);
    float denom = gradMagSqr + 1e-08f;  // prevent zero division
    float curvatureLength = (numer0 + numer1 + numer2)/denom;

    mDst[z][y][x] = mUzzz + mTimeStep*curvatureLength;
}
//----------------------------------------------------------------------------
