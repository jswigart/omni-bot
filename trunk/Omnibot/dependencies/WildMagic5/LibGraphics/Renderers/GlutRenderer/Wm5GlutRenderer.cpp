// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.6.0 (2011/07/23)

#include "Wm5GraphicsPCH.h"
#include "Wm5Renderer.h"
#include "Wm5GlutRendererData.h"
#include "Wm5GlutRendererInput.h"
using namespace Wm5;

//----------------------------------------------------------------------------
Renderer::Renderer (RendererInput& input, int width, int height,
    Texture::Format colorFormat, Texture::Format depthStencilFormat,
    int numMultisamples)
{
    Initialize(width, height, colorFormat, depthStencilFormat,
        numMultisamples);

    GlutRendererData* data = new0 GlutRendererData();
    mData = data;
    data->mWindowID = input.mWindowID;

    // Load the function pointers for the OpenGL extensions.  This only
    // needs to occur once.  If you want a file to be created that stores
    // the OpenGL driver information, change the input filename to a
    // valid string; for example, "OpenGLDriverInfo.txt".
    //const char* infoFile = "OpenGLDriverInfo.txt";
    const char* infoFile = 0;
#ifdef WIN32
    GTInitWGL(infoFile);
    if (input.mDisableVerticalSync)
    {
        // Disable the wait for vertical sync.
        wglSwapIntervalEXT(0);
    }
#endif
#ifdef __APPLE__
    GTInitGL(infoFile);
#endif

    // Select the shader profiles.
    VertexShader::SetProfile(VertexShader::VP_ARBVP1);
    PixelShader::SetProfile(PixelShader::PP_ARBFP1);

    // Query the device for its capabilities.  TODO:  The number of images
    // turns out to be larger than what Shader Model 3 supports.  Why is this?
    data->mMaxVShaderImages = 0;
    data->mMaxPShaderImages = 0;
    data->mMaxCombinedImages = 0;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
        &data->mMaxVShaderImages);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,
        &data->mMaxPShaderImages);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
        &data->mMaxCombinedImages);

    // Set the default render states.
    data->mCurrentRS.Initialize(mDefaultAlphaState, mDefaultCullState,
        mDefaultDepthState, mDefaultOffsetState, mDefaultStencilState,
        mDefaultWireState);
}
//----------------------------------------------------------------------------
Renderer::~Renderer ()
{
    delete0(mData);
    Terminate();
}
//----------------------------------------------------------------------------
void Renderer::DisplayColorBuffer ()
{
    GlutRendererData* data = (GlutRendererData*)mData;
    if (glutGetWindow() == data->mWindowID)
    {
        glutSwapBuffers();
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// For function extension support on Mac OS X.
//----------------------------------------------------------------------------
#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <cstdlib>
#include <cstring>

extern "C"
{
//----------------------------------------------------------------------------
void* GTGetFunctionPointer (const char* function)
{
    NSSymbol symbol;
    char* symbolName;

    // Prepend a '_' for the Unix C symbol mangling convention.
    symbolName = (char*)malloc(strlen((const char*)function) + 2);
    strcpy(symbolName + 1,(const char*)function);
    symbolName[0] = '_';

    symbol = 0;
    if (NSIsSymbolNameDefined(symbolName))
	{
        symbol = NSLookupAndBindSymbol(symbolName);
	}

    free(symbolName);
    return symbol ? NSAddressOfSymbol(symbol) : 0;
}
//----------------------------------------------------------------------------
}
#endif
//----------------------------------------------------------------------------
