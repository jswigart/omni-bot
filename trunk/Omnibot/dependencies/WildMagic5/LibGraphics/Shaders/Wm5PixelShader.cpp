// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5PixelShader.h"
#include "Wm5Renderer.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Shader, PixelShader);
WM5_IMPLEMENT_STREAM(PixelShader);
WM5_IMPLEMENT_FACTORY(PixelShader);
WM5_IMPLEMENT_DEFAULT_NAMES(Shader, PixelShader);
WM5_IMPLEMENT_DEFAULT_STREAM(Shader, PixelShader);

PixelShader::Profile PixelShader::msProfile = PixelShader::PP_NONE;

//----------------------------------------------------------------------------
PixelShader::PixelShader (const std::string& programName, int numInputs,
    int numOutputs, int numConstants, int numSamplers, bool profileOwner)
    :
    Shader(programName, numInputs, numOutputs, numConstants, numSamplers,
        profileOwner)
{
}
//----------------------------------------------------------------------------
PixelShader::~PixelShader ()
{
    Renderer::UnbindAll(this);
}
//----------------------------------------------------------------------------
void PixelShader::SetProfile (Profile profile)
{
    msProfile = profile;
}
//----------------------------------------------------------------------------
PixelShader::Profile PixelShader::GetProfile ()
{
    return msProfile;
}
//----------------------------------------------------------------------------
