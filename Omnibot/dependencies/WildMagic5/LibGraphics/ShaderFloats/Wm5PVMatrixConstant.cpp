// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5PVMatrixConstant.h"
#include "Wm5Camera.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ShaderFloat, PVMatrixConstant);
WM5_IMPLEMENT_STREAM(PVMatrixConstant);
WM5_IMPLEMENT_FACTORY(PVMatrixConstant);
WM5_IMPLEMENT_DEFAULT_NAMES(ShaderFloat, PVMatrixConstant);
WM5_IMPLEMENT_DEFAULT_STREAM(ShaderFloat, PVMatrixConstant);

//----------------------------------------------------------------------------
PVMatrixConstant::PVMatrixConstant ()
    :
    ShaderFloat(4)
{
    mAllowUpdater = true;
}
//----------------------------------------------------------------------------
PVMatrixConstant::~PVMatrixConstant ()
{
}
//----------------------------------------------------------------------------
void PVMatrixConstant::Update (const Visual*, const Camera* camera)
{
    const HMatrix& projViewMatrix = camera->GetProjectionViewMatrix();

    const float* source = (const float*)projViewMatrix;
    float* target = mData;
    for (int i = 0; i < 16; ++i)
    {
        *target++ = *source++;
    }
}
//----------------------------------------------------------------------------
