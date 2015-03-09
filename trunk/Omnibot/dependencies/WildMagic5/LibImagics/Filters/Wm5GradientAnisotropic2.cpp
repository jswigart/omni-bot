// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5GradientAnisotropic2.h"
#include "Wm5Math.h"
using namespace Wm5;

//----------------------------------------------------------------------------
GradientAnisotropic2::GradientAnisotropic2 (int xBound, int yBound,
    float xSpacing, float ySpacing, const float* data, const bool* mask,
    float borderValue, ScaleType scaleType, float K)
    :
    PdeFilter2(xBound, yBound, xSpacing, ySpacing, data, mask, borderValue,
        scaleType)
{
    mK = K;
    ComputeParam();
}
//----------------------------------------------------------------------------
GradientAnisotropic2::~GradientAnisotropic2 ()
{
}
//----------------------------------------------------------------------------
void GradientAnisotropic2::ComputeParam ()
{
    float gradMagSqr = 0.0f;
    for (int y = 1; y <= mYBound; ++y)
    {
        for (int x = 1; x <= mXBound; ++x)
        {
            float ux = GetUx(x, y);
            float uy = GetUy(x, y);
            gradMagSqr += ux*ux + uy*uy;
        }
    }
    gradMagSqr /= (float)mQuantity;

    mParam = 1.0f/(mK*mK*gradMagSqr);
    mMHalfParam = -0.5f*mParam;
}
//----------------------------------------------------------------------------
void GradientAnisotropic2::OnPreUpdate ()
{
    ComputeParam();
}
//----------------------------------------------------------------------------
void GradientAnisotropic2::OnUpdate (int x, int y)
{
    LookUp9(x, y);

    // one-sided U-derivative estimates
    float uxFwd = mInvDx*(mUpz - mUzz);
    float uxBwd = mInvDx*(mUzz - mUmz);
    float uyFwd = mInvDy*(mUzp - mUzz);
    float uyBwd = mInvDy*(mUzz - mUzm);

    // centered U-derivative estimates
    float uxCenM = mHalfInvDx*(mUpm - mUmm);
    float uxCenZ = mHalfInvDx*(mUpz - mUmz);
    float uxCenP = mHalfInvDx*(mUpp - mUmp);
    float uyCenM = mHalfInvDy*(mUmp - mUmm);
    float uyCenZ = mHalfInvDy*(mUzp - mUzm);
    float uyCenP = mHalfInvDy*(mUpp - mUpm);

    float uxCenZSqr = uxCenZ*uxCenZ;
    float uyCenZSqr = uyCenZ*uyCenZ;
    float gradMagSqr;

    // estimate for C(x+1,y)
    float uyEstP = 0.5f*(uyCenZ + uyCenP);
    gradMagSqr = uxCenZSqr + uyEstP*uyEstP;
    float cxp = Mathf::Exp(mMHalfParam*gradMagSqr);

    // estimate for C(x-1,y)
    float uyEstM = 0.5f*(uyCenZ + uyCenM);
    gradMagSqr = uxCenZSqr + uyEstM*uyEstM;
    float cxm = Mathf::Exp(mMHalfParam*gradMagSqr);

    // estimate for C(x,y+1)
    float uxEstP = 0.5f*(uxCenZ + uxCenP);
    gradMagSqr = uyCenZSqr + uxEstP*uxEstP;
    float cyp = Mathf::Exp(mMHalfParam*gradMagSqr);

    // estimate for C(x,y-1)
    float uxEstM = 0.5f*(uxCenZ + uxCenM);
    gradMagSqr = uyCenZSqr + uxEstM*uxEstM;
    float cym = Mathf::Exp(mMHalfParam*gradMagSqr);

    mDst[y][x] = mUzz + mTimeStep*(
        cxp*uxFwd - cxm*uxBwd + cyp*uyFwd - cym*uyBwd);
}
//----------------------------------------------------------------------------
