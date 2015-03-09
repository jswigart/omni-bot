// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5PVWMatrixConstant.h"
#include "Wm5Camera.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ShaderFloat, PVWMatrixConstant);
WM5_IMPLEMENT_STREAM(PVWMatrixConstant);
WM5_IMPLEMENT_FACTORY(PVWMatrixConstant);
WM5_IMPLEMENT_DEFAULT_NAMES(ShaderFloat, PVWMatrixConstant);
WM5_IMPLEMENT_DEFAULT_STREAM(ShaderFloat, PVWMatrixConstant);

//----------------------------------------------------------------------------
PVWMatrixConstant::PVWMatrixConstant ()
    :
    ShaderFloat(4)
{
    mAllowUpdater = true;
}
//----------------------------------------------------------------------------
PVWMatrixConstant::~PVWMatrixConstant ()
{
}
//----------------------------------------------------------------------------
void PVWMatrixConstant::Update (const Visual* visual, const Camera* camera)
{
    const HMatrix& projViewMatrix = camera->GetProjectionViewMatrix();
    const HMatrix& worldMatrix = visual->WorldTransform.Matrix();
    HMatrix projViewWorldMatrix = projViewMatrix*worldMatrix;

    const float* source = (const float*)projViewWorldMatrix;
    float* target = mData;
    for (int i = 0; i < 16; ++i)
    {
        *target++ = *source++;
    }
}
//----------------------------------------------------------------------------
