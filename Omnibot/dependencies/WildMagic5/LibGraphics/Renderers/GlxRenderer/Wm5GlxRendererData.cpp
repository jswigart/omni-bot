// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5GlxRendererData.h"
#include "Wm5Renderer.h"
using namespace Wm5;

//----------------------------------------------------------------------------
bool GlxRendererData::FinishConstruction (Window window, Renderer* renderer)
{
    // Bind the rendering context to the window.
    mWindow = window;
    if (!glXMakeCurrent(mDisplay, mWindow, mContext))
    {
        return false;
    }

    // Load the function pointers for the OpenGL extensions.  This only
    // needs to occur once.  If you want a file to be created that stores
    // the OpenGL driver information, change the input filename to a
    // valid string; for example, "OpenGLDriverInfo.txt".
    const char* infoFile = 0;
    GTInitGLX(infoFile);

    // Select the shader profiles.
    VertexShader::SetProfile(VertexShader::VP_ARBVP1);
    PixelShader::SetProfile(PixelShader::PP_ARBFP1);

    // Query the device for its capabilities.  TODO:  The number of images
    // turns out to be larger than what Shader Model 3 supports.  Why is this?
    mMaxVShaderImages = 0;
    mMaxPShaderImages = 0;
    mMaxCombinedImages = 0;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &mMaxVShaderImages);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &mMaxPShaderImages);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &mMaxCombinedImages);

    // Set the default render states.
    mCurrentRS.Initialize(renderer->GetAlphaState(), renderer->GetCullState(),
        renderer->GetDepthState(), renderer->GetOffsetState(),
        renderer->GetStencilState(), renderer->GetWireState());

    return true;
}
//----------------------------------------------------------------------------
