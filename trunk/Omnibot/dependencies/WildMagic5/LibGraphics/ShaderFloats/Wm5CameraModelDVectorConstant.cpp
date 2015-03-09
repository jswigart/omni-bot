// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5CameraModelDVectorConstant.h"
#include "Wm5Camera.h"
#include "Wm5Visual.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ShaderFloat, CameraModelDVectorConstant);
WM5_IMPLEMENT_STREAM(CameraModelDVectorConstant);
WM5_IMPLEMENT_FACTORY(CameraModelDVectorConstant);
WM5_IMPLEMENT_DEFAULT_NAMES(ShaderFloat, CameraModelDVectorConstant);
WM5_IMPLEMENT_DEFAULT_STREAM(ShaderFloat, CameraModelDVectorConstant);

//----------------------------------------------------------------------------
CameraModelDVectorConstant::CameraModelDVectorConstant ()
    :
    ShaderFloat(1)
{
    mAllowUpdater = true;
}
//----------------------------------------------------------------------------
CameraModelDVectorConstant::~CameraModelDVectorConstant ()
{
}
//----------------------------------------------------------------------------
void CameraModelDVectorConstant::Update (const Visual* visual,
    const Camera* camera)
{
    const AVector& worldDVector = camera->GetDVector();
    const HMatrix& worldInvMatrix = visual->WorldTransform.Inverse();
    AVector modelDVector = worldInvMatrix*worldDVector;

    const float* source = (const float*)modelDVector;
    float* target = mData;
    for (int i = 0; i < 4; ++i)
    {
        *target++ = *source++;
    }
}
//----------------------------------------------------------------------------
