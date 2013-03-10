// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5GradientAnisotropic3.h"
using namespace Wm5;

//----------------------------------------------------------------------------
GradientAnisotropic3::GradientAnisotropic3 (int xBound, int yBound,
    int zBound, float xSpacing, float ySpacing, float zSpacing,
    const float* data, const bool* mask, float borderValue,
    ScaleType scaleType, float K)
    :
    PdeFilter3(xBound, yBound, zBound, xSpacing, ySpacing, zSpacing, data,
        mask, borderValue, scaleType)
{
    mK = K;
    ComputeParam();
}
//----------------------------------------------------------------------------
GradientAnisotropic3::~GradientAnisotropic3 ()
{
}
//----------------------------------------------------------------------------
void GradientAnisotropic3::ComputeParam ()
{
    float gradMagSqr = 0.0f;
    for (int z = 1; z <= mZBound; ++z)
    {
        for (int y = 1; y <= mYBound; ++y)
        {
            for (int x = 1; x <= mXBound; ++x)
            {
                float ux = GetUx(x, y, z);
                float uy = GetUy(x, y, z);
                float uz = GetUz(x, y, z);
                gradMagSqr += ux*ux + uy*uy + uz*uz;
            }
        }
    }
    gradMagSqr /= (float)mQuantity;

    mParam = 1.0f/(mK*mK*gradMagSqr);
    mMHalfParam = -0.5f*mParam;
}
//----------------------------------------------------------------------------
void GradientAnisotropic3::OnPreUpdate ()
{
    ComputeParam();
}
//----------------------------------------------------------------------------
void GradientAnisotropic3::OnUpdate (int x, int y, int z)
{
    LookUp27(x, y, z);

    // one-sided U-derivative estimates
    float uxFwd = mInvDx*(mUpzz - mUzzz);
    float uxBwd = mInvDx*(mUzzz - mUmzz);
    float uyFwd = mInvDy*(mUzpz - mUzzz);
    float uyBwd = mInvDy*(mUzzz - mUzmz);
    float uzFwd = mInvDz*(mUzzp - mUzzz);
    float uzBwd = mInvDz*(mUzzz - mUzzm);

    // centered U-derivative estimates
    float duvzz = mHalfInvDx*(mUpzz - mUmzz);
    float duvpz = mHalfInvDx*(mUppz - mUmpz);
    float duvmz = mHalfInvDx*(mUpmz - mUmmz);
    float duvzp = mHalfInvDx*(mUpzp - mUmzp);
    float duvzm = mHalfInvDx*(mUpzm - mUmzm);

    float duzvz = mHalfInvDy*(mUzpz - mUzmz);
    float dupvz = mHalfInvDy*(mUppz - mUpmz);
    float dumvz = mHalfInvDy*(mUmpz - mUmmz);
    float duzvp = mHalfInvDy*(mUzpp - mUzmp);
    float duzvm = mHalfInvDy*(mUzpm - mUzmm);

    float duzzv = mHalfInvDz*(mUzzp - mUzzm);
    float dupzv = mHalfInvDz*(mUpzp - mUpzm);
    float dumzv = mHalfInvDz*(mUmzp - mUmzm);
    float duzpv = mHalfInvDz*(mUzpp - mUzpm);
    float duzmv = mHalfInvDz*(mUzmp - mUzmm);

    float uxCenSqr = duvzz*duvzz;
    float uyCenSqr = duzvz*duzvz;
    float uzCenSqr = duzzv*duzzv;

    float uxEst, uyEst, uzEst, gradMagSqr;

    // estimate for C(x+1,y,z)
    uyEst = 0.5f*(duzvz + dupvz);
    uzEst = 0.5f*(duzzv + dupzv);
    gradMagSqr = uxCenSqr + uyEst*uyEst + uzEst*uzEst;
    float cxp = expf(mMHalfParam*gradMagSqr);

    // estimate for C(x-1,y,z)
    uyEst = 0.5f*(duzvz + dumvz);
    uzEst = 0.5f*(duzzv + dumzv);
    gradMagSqr = uxCenSqr + uyEst*uyEst + uzEst*uzEst;
    float cxm = expf(mMHalfParam*gradMagSqr);

    // estimate for C(x,y+1,z)
    uxEst = 0.5f*(duvzz + duvpz);
    uzEst = 0.5f*(duzzv + duzpv);
    gradMagSqr = uxEst*uxEst + uyCenSqr + uzEst*uzEst;
    float cyp = expf(mMHalfParam*gradMagSqr);

    // estimate for C(x,y-1,z)
    uxEst = 0.5f*(duvzz + duvmz);
    uzEst = 0.5f*(duzzv + duzmv);
    gradMagSqr = uxEst*uxEst + uyCenSqr + uzEst*uzEst;
    float cym = expf(mMHalfParam*gradMagSqr);

    // estimate for C(x,y,z+1)
    uxEst = 0.5f*(duvzz + duvzp);
    uyEst = 0.5f*(duzvz + duzvp);
    gradMagSqr = uxEst*uxEst + uyEst*uyEst + uzCenSqr;
    float czp = expf(mMHalfParam*gradMagSqr);

    // estimate for C(x,y,z-1)
    uxEst = 0.5f*(duvzz + duvzm);
    uyEst = 0.5f*(duzvz + duzvm);
    gradMagSqr = uxEst*uxEst + uyEst*uyEst + uzCenSqr;
    float czm = expf(mMHalfParam*gradMagSqr);

    mDst[z][y][x] = mUzzz + mTimeStep*(
        cxp*uxFwd - cxm*uxBwd +
        cyp*uyFwd - cym*uyBwd +
        czp*uzFwd - czm*uzBwd);
}
//----------------------------------------------------------------------------
