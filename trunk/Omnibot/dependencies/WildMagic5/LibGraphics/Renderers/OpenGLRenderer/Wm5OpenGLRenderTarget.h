// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/07/31)

#ifndef WM5OPENGLRENDERTARGET_H
#define WM5OPENGLRENDERTARGET_H

#include "Wm5OpenGLRendererLIB.h"
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
    int mNumTargets, mWidth, mHeight;
    Texture::Format mFormat;
    bool mHasMipmaps, mHasDepthStencil;

    GLuint* mColorTextures;
    GLuint mDepthStencilTexture, mFrameBuffer;
    GLenum* mDrawBuffers;
    int mPrevViewport[4];
    double mPrevDepthRange[2];
};

}

#endif
