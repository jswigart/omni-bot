// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Renderer.h"
#include "Wm5GlxRendererData.h"
#include "Wm5GlxRendererInput.h"
using namespace Wm5;

//----------------------------------------------------------------------------
Renderer::Renderer (RendererInput& input, int width, int height,
    Texture::Format colorFormat, Texture::Format depthStencilFormat,
    int numMultisamples)
{
    Initialize(width, height, colorFormat, depthStencilFormat,
        numMultisamples);

    GlxRendererData* data = new0 GlxRendererData();
    mData = data;
    data->mDisplay = input.mDisplay;
    data->mWindow = 0;  // Set by the application after window creation.

    // Select the attributes for the frame buffer.
    int attributes[256];

    // colorFormat is ignored, create 32-bit RGBA buffer.
    int i = 0;
    attributes[i++] = GLX_RGBA;
    attributes[i++] = GLX_RED_SIZE;
    attributes[i++] = 8;
    attributes[i++] = GLX_GREEN_SIZE;
    attributes[i++] = 8;
    attributes[i++] = GLX_BLUE_SIZE;
    attributes[i++] = 8;
    attributes[i++] = GLX_ALPHA_SIZE;
    attributes[i++] = 8;

    // depthStencilFormat is ignored, create 24-8 depthstencil buffer.
    attributes[i++] = GLX_DEPTH_SIZE;
    attributes[i++] = 24;
    attributes[i++] = GLX_STENCIL_SIZE;
    attributes[i++] = 8;

    // Use double buffering.
    attributes[i++] = GLX_DOUBLEBUFFER;
    attributes[i++] = 1;

    // Request multisampling.
    if (numMultisamples == 2 || numMultisamples == 4)
    {
        attributes[i++] = GLX_SAMPLE_BUFFERS_ARB;
        attributes[i++] = 1;
        attributes[i++] = GLX_SAMPLES_ARB;
        attributes[i++] = numMultisamples;
    }

    // The list is zero terminated.
    attributes[i] = 0;

    // Get an OpenGL-capable visual.
    int screen = DefaultScreen(data->mDisplay);
    data->mVisual = glXChooseVisual(data->mDisplay, screen, attributes);
    input.mVisual = data->mVisual;
    assertion(data->mVisual, "Cannot choose visual.\n");
    if (!data->mVisual)
    {
        data->mDisplay = 0;
        data->mVisual = 0;
        data->mContext = 0;
        return;
    }

    // Create an OpenGL rendering context.
    GLXContext sharedList = 0;
    Bool directRender = True;
    data->mContext = glXCreateContext(data->mDisplay, data->mVisual,
        sharedList, directRender);
    input.mContext = data->mContext;
    if (!data->mContext)
    {
        data->mDisplay = 0;
        data->mVisual = 0;
        data->mContext = 0;
        return;
    }
}
//----------------------------------------------------------------------------
Renderer::~Renderer ()
{
    GlxRendererData* data = (GlxRendererData*)mData;

    if (data->mDisplay && data->mContext)
    {
        glXDestroyContext(data->mDisplay, data->mContext);
    }

    delete0(data);
    Terminate();
}
//----------------------------------------------------------------------------
void Renderer::DisplayColorBuffer ()
{
    GlxRendererData* data = (GlxRendererData*)mData;
    glXSwapBuffers(data->mDisplay, data->mWindow);
}
//----------------------------------------------------------------------------
