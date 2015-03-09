// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/07/31)

#ifndef WM5DX9RENDERTARGET_H
#define WM5DX9RENDERTARGET_H

#include "Wm5Dx9RendererLIB.h"
#include "Wm5RenderTarget.h"

namespace Wm5
{

class Renderer;

class PdrRenderTarget
{
public:
    // Construction and destruction.
    PdrRenderTarget (Renderer* renderer, const RenderTarget* renderTarget);
    ~PdrRenderTarget ();

    // Render target operations.
    void Enable (Renderer* renderer);
    void Disable (Renderer* renderer);
    void ReadColor (int i, Renderer* renderer, Texture2D*& texture);

private:
    // Support for multiple render targets.
    int mNumTargets;
    unsigned int mWidth, mHeight;
    Texture::Format mFormat;
    bool mHasDepthStencil;

    // The color arrays have sizes equal to the number of targets.  The
    // depth-stencil buffer is shared (naturally, because multiple color
    // writes occur in the same pixel shader call, but only one depth is
    // computed).
    IDirect3DTexture9** mColorTextures;
    IDirect3DSurface9** mColorSurfaces;
    IDirect3DTexture9* mDepthStencilTexture;
    IDirect3DSurface9* mDepthStencilSurface;

    // Support for enable/disable.  We have to get/set the render target 0,
    // which is the backbuffer.
    IDirect3DSurface9* mSaveColorSurface;
    IDirect3DSurface9* mSaveDepthStencilSurface;
};

}

#endif
