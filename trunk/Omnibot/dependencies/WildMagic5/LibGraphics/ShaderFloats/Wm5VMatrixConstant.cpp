// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5VMatrixConstant.h"
#include "Wm5Camera.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ShaderFloat, VMatrixConstant);
WM5_IMPLEMENT_STREAM(VMatrixConstant);
WM5_IMPLEMENT_FACTORY(VMatrixConstant);
WM5_IMPLEMENT_DEFAULT_NAMES(ShaderFloat, VMatrixConstant);
WM5_IMPLEMENT_DEFAULT_STREAM(ShaderFloat, VMatrixConstant);

//----------------------------------------------------------------------------
VMatrixConstant::VMatrixConstant ()
    :
    ShaderFloat(4)
{
    mAllowUpdater = true;
}
//----------------------------------------------------------------------------
VMatrixConstant::~VMatrixConstant ()
{
}
//----------------------------------------------------------------------------
void VMatrixConstant::Update (const Visual*, const Camera* camera)
{
    const HMatrix& viewMatrix = camera->GetViewMatrix();

    const float* source = (const float*)viewMatrix;
    float* target = mData;
    for (int i = 0; i < 16; ++i)
    {
        *target++ = *source++;
    }
}
//----------------------------------------------------------------------------
