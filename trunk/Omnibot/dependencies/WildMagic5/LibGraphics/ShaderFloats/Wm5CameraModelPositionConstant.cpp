// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5CameraModelPositionConstant.h"
#include "Wm5Camera.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ShaderFloat, CameraModelPositionConstant);
WM5_IMPLEMENT_STREAM(CameraModelPositionConstant);
WM5_IMPLEMENT_FACTORY(CameraModelPositionConstant);
WM5_IMPLEMENT_DEFAULT_NAMES(ShaderFloat, CameraModelPositionConstant);
WM5_IMPLEMENT_DEFAULT_STREAM(ShaderFloat, CameraModelPositionConstant);

//----------------------------------------------------------------------------
CameraModelPositionConstant::CameraModelPositionConstant ()
    :
    ShaderFloat(1)
{
    mAllowUpdater = true;
}
//----------------------------------------------------------------------------
CameraModelPositionConstant::~CameraModelPositionConstant ()
{
}
//----------------------------------------------------------------------------
void CameraModelPositionConstant::Update (const Visual* visual,
    const Camera* camera)
{
    const APoint& worldPosition = camera->GetPosition();
    const HMatrix& worldInvMatrix = visual->WorldTransform.Inverse();
    APoint modelPosition = worldInvMatrix*worldPosition;

    const float* source = (const float*)modelPosition;
    float* target = mData;
    for (int i = 0; i < 4; ++i)
    {
        *target++ = *source++;
    }
}
//----------------------------------------------------------------------------
