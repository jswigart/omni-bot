// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Renderer.h"
#include "Wm5AglRendererData.h"
#include "Wm5AglRendererInput.h"
using namespace Wm5;

//----------------------------------------------------------------------------
Renderer::Renderer (RendererInput& input, int width, int height,
    Texture::Format colorFormat, Texture::Format depthStencilFormat,
    int numMultisamples)
{
    Initialize(width, height, colorFormat, depthStencilFormat,
        numMultisamples);

    AglRendererData* data = new0 AglRendererData();
    mData = data;
    data->mDevice = input.mDevice;
    data->mWindow = input.mWindow;
    data->mDrawable = (AGLDrawable)GetWindowPort(input.mWindow);
    data->mDSpContext = 0;
    
    // See whether we have a renderer available.
    AGLRendererInfo info = aglQueryRendererInfo(&input.mDevice, 1);
    if (!info)
    {
        info = aglQueryRendererInfo(0, 0);
    }
    if (!info)
    {
        data->FatalErrorMessage(
            "Cannot obtain information about the renderer.");
    }

    // Select the attributes for the frame buffer.
    int attributes[256];

    int i = 0;
    attributes[i++] = AGL_ACCELERATED;
    attributes[i++] = AGL_NO_RECOVERY;

    // colorFormat is ignored, create 32-bit RGBA buffer.
    attributes[i++] = AGL_RGBA;
    attributes[i++] = AGL_RED_SIZE;
    attributes[i++] = 8;
    attributes[i++] = AGL_GREEN_SIZE;
    attributes[i++] = 8;
    attributes[i++] = AGL_BLUE_SIZE;
    attributes[i++] = 8;
    attributes[i++] = AGL_ALPHA_SIZE;
    attributes[i++] = 8;

    // depthStencilFormat is ignored, create 24-8 depthstencil buffer.
    attributes[i++] = AGL_DEPTH_SIZE;
    attributes[i++] = 24;
    attributes[i++] = AGL_STENCIL_SIZE;
    attributes[i++] = 8;

    // Use double buffering.  TODO: GLX has 'attributes[i++] = 1' after this.
    attributes[i++] = AGL_DOUBLEBUFFER;

    // Request multisampling.
    if (numMultisamples == 2 || numMultisamples == 4)
    {
        attributes[i++] = AGL_SAMPLE_BUFFERS_ARB;
        attributes[i++] = 1;
        attributes[i++] = AGL_SAMPLES_ARB;
        attributes[i++] = numMultisamples;
    }

    // The list is zero terminated.
    attributes[i] = AGL_NONE;

    AGLPixelFormat pixelFormat = aglChoosePixelFormat(&data->mDevice, 1,
        attributes);
    if (!pixelFormat)
    {
        // Retry without specifying a device.
        pixelFormat = aglChoosePixelFormat(0, 0, attributes);
        if (!pixelFormat)
        {
            data->FatalErrorMessage("Cannot choose pixel format.");
        }
    }
    
    // Create an OpenGL context.
    data->mContext = aglCreateContext(pixelFormat, 0);
    if (!data->mContext || aglGetError() != noErr)
    {
        data->FatalErrorMessage("Cannot create rendering context.");
    }
    if (!aglSetDrawable(data->mContext, data->mDrawable)
    ||   aglGetError() != noErr)
    {
        data->FatalErrorMessage("Cannot set the current drawable.");
    }
    if (!aglSetCurrentContext(data->mContext) || aglGetError() != noErr)
    {
        data->FatalErrorMessage("Cannot set the current rendering context.");
    }
    aglDestroyPixelFormat(pixelFormat);
    data->SetUpBufferRect(0, 0, width, height);

    // Create default screen text font.
    const char* defaultFontName = "Monaco";
    const int defaultFontSize = 9;
    data->LoadFont(defaultFontName, defaultFontSize, false, false);
    
    // Load the function pointers for the OpenGL extensions.  This only
    // needs to occur once.  If you want a file to be created that stores
    // the OpenGL driver information, change the input filename to a
    // valid string; for example, "OpenGLDriverInfo.txt".
    const char* infoFile = 0;
    GTInitGL(infoFile);

    data->Finalize(this);

    // Disable vertical sync.  If you want to enable this, change the value
    // of vsValue to 1.
    int vsValue = 0;
    aglSetInteger(data->mContext, AGL_SWAP_INTERVAL, &vsValue);

    // Allocate temporary back buffer to be used for font management.
    PixMapHandle pixmap = (**(GDHandle)input.mDevice).gdPMap;
    Rect area = { 0, 0, 32, 32 };
    int depth = GetPixDepth(pixmap);
    
    OSErr error = NewGWorld(&data->mBack, depth, &area, 0, 0,
        useTempMem | pixPurge);
    if (error != noErr || !data->mBack)
    {
        data->FatalErrorMessage("Cannot initialize font management.");
    }

    // Assign desired font settings to back buffer.
    GWorldPtr currentWorld;
    GDHandle currentDevice;
    GetGWorld(&currentWorld, &currentDevice);
    SetGWorld(data->mBack, 0);

    Str255 fontName;
    fontName[0] = (int)strlen(defaultFontName);
    strcpy((char*)fontName + 1, defaultFontName);
    short fontNum;
    GetFNum(fontName, &fontNum);
    TextFont(fontNum);
    TextSize(defaultFontSize);

    SetGWorld(currentWorld, currentDevice);
}
//----------------------------------------------------------------------------
Renderer::~Renderer ()
{
    AglRendererData* data = (AglRendererData*)mData;

    glDeleteLists(data->mFont.mStart, data->mFont.mQuantity);

    DisposeGWorld(data->mBack);
    aglSetDrawable(data->mContext, 0);
    aglDestroyContext(data->mContext);

    delete0(data);
    Terminate();
}
//----------------------------------------------------------------------------
void Renderer::DisplayColorBuffer ()
{
    AglRendererData* data = (AglRendererData*)mData;
    if (aglGetDrawable(data->mContext))
    {
        aglSwapBuffers(data->mContext);
    }
}
//----------------------------------------------------------------------------
