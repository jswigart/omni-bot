// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5CameraWorldDVectorConstant.h"
#include "Wm5Camera.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ShaderFloat, CameraWorldDVectorConstant);
WM5_IMPLEMENT_STREAM(CameraWorldDVectorConstant);
WM5_IMPLEMENT_FACTORY(CameraWorldDVectorConstant);
WM5_IMPLEMENT_DEFAULT_NAMES(ShaderFloat, CameraWorldDVectorConstant);
WM5_IMPLEMENT_DEFAULT_STREAM(ShaderFloat, CameraWorldDVectorConstant);

//----------------------------------------------------------------------------
CameraWorldDVectorConstant::CameraWorldDVectorConstant ()
    :
    ShaderFloat(1)
{
    mAllowUpdater = true;
}
//----------------------------------------------------------------------------
CameraWorldDVectorConstant::~CameraWorldDVectorConstant ()
{
}
//----------------------------------------------------------------------------
void CameraWorldDVectorConstant::Update (const Visual*, const Camera* camera)
{
    const AVector& worldDVector = camera->GetDVector();

    const float* source = (const float*)worldDVector;
    float* target = mData;
    for (int i = 0; i < 4; ++i)
    {
        *target++ = *source++;
    }
}
//----------------------------------------------------------------------------
