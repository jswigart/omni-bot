// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/07/09)

#include "Wm5GraphicsPCH.h"
#include "Wm5OpenGLRenderTarget.h"
#include "Wm5OpenGLTexture2D.h"
#include "Wm5Renderer.h"
#include "Wm5OpenGLMapping.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PdrRenderTarget::PdrRenderTarget (Renderer* renderer,
    const RenderTarget* renderTarget)
{
    mNumTargets = renderTarget->GetNumTargets();
    assertion(mNumTargets >= 1,
        "Number of render targets must be at least one.\n");

    mWidth = renderTarget->GetWidth();
    mHeight = renderTarget->GetHeight();
    mFormat = renderTarget->GetFormat();
    mHasMipmaps = renderTarget->HasMipmaps();
    mHasDepthStencil = renderTarget->HasDepthStencil();

    mPrevViewport[0] = 0;
    mPrevViewport[1] = 0;
    mPrevViewport[2] = 0;
    mPrevViewport[3] = 0;
    mPrevDepthRange[0] = 0.0;
    mPrevDepthRange[1] = 0.0;

    // Create a framebuffer object.
    glGenFramebuffersEXT(1, &mFrameBuffer);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer);

    GLuint previousBind = GetBoundTexture(Shader::ST_2D);

    mColorTextures = new1<GLuint>(mNumTargets);
    mDrawBuffers = new1<GLenum>(mNumTargets);
    for (int i = 0; i < mNumTargets; ++i)
    {
        Texture2D* colorTexture = renderTarget->GetColorTexture(i);
        assertion(!renderer->InTexture2DMap(colorTexture),
            "Texture should not yet exist.\n");

        PdrTexture2D* ogColorTexture = new0 PdrTexture2D(renderer,
            colorTexture);
        renderer->InsertInTexture2DMap(colorTexture, ogColorTexture);
        mColorTextures[i] = ogColorTexture->GetTexture();
        mDrawBuffers[i] = GL_COLOR_ATTACHMENT0_EXT + i;

        // Bind the color texture.
        glBindTexture(GL_TEXTURE_2D, mColorTextures[i]);
        if (mHasMipmaps)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                GL_LINEAR_MIPMAP_LINEAR);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }

        // Attach the texture to the framebuffer.
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, mDrawBuffers[i],
            GL_TEXTURE_2D, mColorTextures[i], 0);
    }

    Texture2D* depthStencilTexture = renderTarget->GetDepthStencilTexture();
    if (depthStencilTexture)
    {
        assertion(!renderer->InTexture2DMap(depthStencilTexture),
            "Texture should not yet exist.\n");

        PdrTexture2D* ogDepthStencilTexture = new0 PdrTexture2D(renderer,
            depthStencilTexture);
        renderer->InsertInTexture2DMap(depthStencilTexture,
            ogDepthStencilTexture);
        mDepthStencilTexture = ogDepthStencilTexture->GetTexture();

        // Bind the depthstencil texture.
        glBindTexture(GL_TEXTURE_2D, mDepthStencilTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // Attach the depth to the framebuffer.
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
            GL_TEXTURE_2D, mDepthStencilTexture, 0);

        // Attach the stencil to the framebuffer.
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
            GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, mDepthStencilTexture,
            0);
    }

    glBindTexture(GL_TEXTURE_2D, previousBind);

    switch (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT))
    {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENTS_EXT:
        assertion(false, "Framebuffer incomplete attachments\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        assertion(false, "Framebuffer incomplete missing attachment\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
        assertion(false, "Framebuffer incomplete duplicate attachment\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        assertion(false, "Framebuffer incomplete dimensions\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        assertion(false, "Framebuffer incomplete formats\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        assertion(false, "Framebuffer incomplete draw buffer\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        assertion(false, "Framebuffer incomplete read buffer\n");
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        assertion(false, "Framebuffer unsupported\n");
        break;
    default:
        assertion(false, "Framebuffer unknown error\n");
        break;
    }
}
//----------------------------------------------------------------------------
PdrRenderTarget::~PdrRenderTarget ()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDeleteFramebuffersEXT(1, &mFrameBuffer);
    delete1(mColorTextures);
    delete1(mDrawBuffers);
}
//----------------------------------------------------------------------------
void PdrRenderTarget::Enable (Renderer*)
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer);
    glDrawBuffers(mNumTargets, mDrawBuffers);

    glGetIntegerv(GL_VIEWPORT, mPrevViewport);
    glGetDoublev(GL_DEPTH_RANGE, mPrevDepthRange);
    glViewport(0, 0, mWidth, mHeight);
    glDepthRange(0.0, 1.0);
}
//----------------------------------------------------------------------------
void PdrRenderTarget::Disable (Renderer*)
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    if (mHasMipmaps)
    {
        GLuint previousBind = GetBoundTexture(Shader::ST_2D);
        for (int i = 0; i < mNumTargets; ++i)
        {
            glBindTexture(GL_TEXTURE_2D, mColorTextures[i]);
            glGenerateMipmapEXT(GL_TEXTURE_2D);
        }
        glBindTexture(GL_TEXTURE_2D, previousBind);
    }

    glViewport(mPrevViewport[0], mPrevViewport[1], mPrevViewport[2],
        mPrevViewport[3]);
    glDepthRange(mPrevDepthRange[0], mPrevDepthRange[1]);
}
//----------------------------------------------------------------------------
void PdrRenderTarget::ReadColor (int i, Renderer* renderer,
    Texture2D*& texture)
{
    if (i < 0 || i >= mNumTargets)
    {
        assertion(false, "Invalid target index.\n");
    }

    Enable(renderer);

    if (texture)
    {
        if (texture->GetFormat() != mFormat ||
            texture->GetWidth() != mWidth ||
            texture->GetHeight() != mHeight)
        {
            assertion(false, "Incompatible texture.\n");
            delete0(texture);
            texture = new0 Texture2D(mFormat, mWidth, mHeight, 1);
        }
    }
    else
    {
        texture = new0 Texture2D(mFormat, mWidth, mHeight, 1);
    }

    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT + i);

    glReadPixels(
        0,
        0,
        mWidth,
        mHeight,
        gOGLTextureFormat[mFormat],
        gOGLTextureType[mFormat],
        texture->GetData(0)
    );

    Disable(renderer);
}
//----------------------------------------------------------------------------
