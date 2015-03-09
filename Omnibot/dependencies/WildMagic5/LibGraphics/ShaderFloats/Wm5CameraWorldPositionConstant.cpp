// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5CameraWorldPositionConstant.h"
#include "Wm5Camera.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ShaderFloat, CameraWorldPositionConstant);
WM5_IMPLEMENT_STREAM(CameraWorldPositionConstant);
WM5_IMPLEMENT_FACTORY(CameraWorldPositionConstant);
WM5_IMPLEMENT_DEFAULT_NAMES(ShaderFloat, CameraWorldPositionConstant);
WM5_IMPLEMENT_DEFAULT_STREAM(ShaderFloat, CameraWorldPositionConstant);

//----------------------------------------------------------------------------
CameraWorldPositionConstant::CameraWorldPositionConstant ()
    :
    ShaderFloat(1)
{
    mAllowUpdater = true;
}
//----------------------------------------------------------------------------
CameraWorldPositionConstant::~CameraWorldPositionConstant ()
{
}
//----------------------------------------------------------------------------
void CameraWorldPositionConstant::Update (const Visual*, const Camera* camera)
{
    const APoint& worldPosition = camera->GetPosition();

    const float* source = (const float*)worldPosition;
    float* target = mData;
    for (int i = 0; i < 4; ++i)
    {
        *target++ = *source++;
    }
}
//----------------------------------------------------------------------------
