// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/04/14)

#include "Wm5GraphicsPCH.h"
#include "Wm5Renderer.h"
#include "Wm5WglRendererData.h"
#include "Wm5WglRendererInput.h"
using namespace Wm5;

//----------------------------------------------------------------------------
Renderer::Renderer (RendererInput& input, int width, int height,
    Texture::Format colorFormat, Texture::Format depthStencilFormat,
    int numMultisamples)
{
    Initialize(width, height, colorFormat, depthStencilFormat,
        numMultisamples);

    WglRendererData* data = new0 WglRendererData();
    mData = data;
    data->mWindowHandle = input.mWindowHandle;
    data->mWindowDC = ::GetDC(input.mWindowHandle);

    // The application layer needs to know the device context in case it has
    // to recreate a window to support multisampling.
    input.mRendererDC = data->mWindowDC;

    // Select the format for the drawing surface.
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags =
        PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_OPENGL |
        PFD_GENERIC_ACCELERATED;
    pfd.dwFlags |= PFD_DOUBLEBUFFER;

    // colorFormat is ignored, create 32-bit RGBA buffer.
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    // depthStencilFormat is ignored, create 24-8 depthstencil buffer.
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;

    // Set the pixel format for the rendering context.
    bool firstTime = (input.mPixelFormat == 0 ? true : false);
    if (firstTime)
    {
        // First time creation.  If multisampling is requested, a second
        // creation will occur and the input pixel format will be supplied
        // directly to SetPixelFormat.
        input.mPixelFormat = ChoosePixelFormat(data->mWindowDC, &pfd);
        if (input.mPixelFormat == 0)
        {
            assertion(false, "ChoosePixelFormat failed\n");
            return;
        }
    }

    BOOL success = SetPixelFormat(data->mWindowDC, input.mPixelFormat, &pfd);
    if (!success)
    {
        assertion(false, "SetPixelFormat failed\n");
        return;
    }

    // Create an OpenGL context.
    data->mWindowRC = wglCreateContext(data->mWindowDC);
    if (!data->mWindowRC)
    {
        assertion(false, "wglCreateContext failed\n");
        return;
    }

    // Activate the context.
    success = wglMakeCurrent(data->mWindowDC, data->mWindowRC);
    if (!success)
    {
        assertion(false, "wglMakeCurrent failed\n");
        return;
    }

    if (firstTime)
    {
        // Load the function pointers for the OpenGL extensions.  This only
        // needs to occur once.  If you want a file to be created that stores
        // the OpenGL driver information, change the input filename to a
        // valid string; for example, "OpenGLDriverInfo.txt".
        //const char* infoFile = "OpenGLDriverInfo.txt";
        const char* infoFile = 0;
        GTInitWGL(infoFile);
    }

    if (input.mDisableVerticalSync)
    {
        // Disable the wait for vertical sync.
        wglSwapIntervalEXT(0);
    }

#ifdef WM5_USE_TEXT_DISPLAY_LIST
    // Use the system font as default.
    int firstChar = int(' ');
    data->mFont.mQuantity = 127 - firstChar;
    data->mFont.mStart = glGenLists(data->mFont.mQuantity);
    data->mFont.mBase = data->mFont.mStart - firstChar;
    SelectObject(data->mWindowDC, GetStockObject(SYSTEM_FONT)); 
    wglUseFontBitmaps(data->mWindowDC, firstChar, data->mFont.mQuantity,
        data->mFont.mStart);
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
    WglRendererData* data = (WglRendererData*)mData;

#ifdef WM5_USE_TEXT_DISPLAY_LIST
    glDeleteLists(data->mFont.mStart, data->mFont.mQuantity);
#endif

    if (data->mWindowRC)
    {
        wglDeleteContext(data->mWindowRC);
    }

    if (data->mWindowDC)
    {
        ::ReleaseDC(data->mWindowHandle, data->mWindowDC);
    }

    delete0(data);
    Terminate();
}
//----------------------------------------------------------------------------
void Renderer::DisplayColorBuffer ()
{
    WglRendererData* data = (WglRendererData*)mData;
    SwapBuffers(data->mWindowDC);
}
//----------------------------------------------------------------------------
