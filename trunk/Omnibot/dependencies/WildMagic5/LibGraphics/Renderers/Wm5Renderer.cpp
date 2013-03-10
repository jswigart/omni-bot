// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2011/06/19)

#include "Wm5GraphicsPCH.h"
#include "Wm5Renderer.h"
#include "Wm5GlobalEffect.h"

#ifdef WM5_USE_DX9
#include "Wm5Dx9IndexBuffer.h"
#include "Wm5Dx9PixelShader.h"
#include "Wm5Dx9RenderTarget.h"
#include "Wm5Dx9Shader.h"
#include "Wm5Dx9Texture1D.h"
#include "Wm5Dx9Texture2D.h"
#include "Wm5Dx9Texture3D.h"
#include "Wm5Dx9TextureCube.h"
#include "Wm5Dx9VertexBuffer.h"
#include "Wm5Dx9VertexFormat.h"
#include "Wm5Dx9VertexShader.h"
#endif

#ifdef WM5_USE_OPENGL
#include "Wm5OpenGLIndexBuffer.h"
#include "Wm5OpenGLPixelShader.h"
#include "Wm5OpenGLRendererData.h"
#include "Wm5OpenGLRenderTarget.h"
#include "Wm5OpenGLShader.h"
#include "Wm5OpenGLTexture1D.h"
#include "Wm5OpenGLTexture2D.h"
#include "Wm5OpenGLTexture3D.h"
#include "Wm5OpenGLTextureCube.h"
#include "Wm5OpenGLVertexBuffer.h"
#include "Wm5OpenGLVertexFormat.h"
#include "Wm5OpenGLVertexShader.h"
#endif

using namespace Wm5;

Mutex Renderer::msMutex;
std::set<Renderer*> Renderer::msRenderers;

//----------------------------------------------------------------------------
void Renderer::Initialize (int width, int height, Texture::Format colorFormat,
    Texture::Format depthStencilFormat, int numMultisamples)
{
    assertion(width > 0, "Width must be positive\n");

    assertion(height > 0, "Height must be positive\n");

    assertion(depthStencilFormat == Texture::TF_D24S8,
        "Only 24-bit depth and 8-bit stencil are currently supported\n");

    assertion(
        numMultisamples == 0 || numMultisamples == 2 || numMultisamples == 4,
        "The number of multisamples can be only 0, 2, or 4\n");

    mWidth = width;
    mHeight = height;
    mColorFormat = colorFormat;
    mDepthStencilFormat = depthStencilFormat;
    mNumMultisamples = numMultisamples;
    mDefaultAlphaState = new0 AlphaState();
    mDefaultCullState = new0 CullState();
    mDefaultDepthState = new0 DepthState();
    mDefaultOffsetState = new0 OffsetState();
    mDefaultStencilState = new0 StencilState();
    mDefaultWireState = new0 WireState();
    mAlphaState = mDefaultAlphaState;
    mCullState = mDefaultCullState;
    mDepthState = mDefaultDepthState;
    mOffsetState = mDefaultOffsetState;
    mStencilState = mDefaultStencilState;
    mWireState = mDefaultWireState;
    mReverseCullOrder = false;
    mOverrideAlphaState = 0;
    mOverrideCullState = 0;
    mOverrideDepthState = 0;
    mOverrideOffsetState = 0;
    mOverrideStencilState = 0;
    mOverrideWireState = 0;
    mCamera = 0;
    mClearColor = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    mClearDepth = 1.0f;
    mClearStencil = 0;
    mAllowRed = true;
    mAllowGreen = true;
    mAllowBlue = true;
    mAllowAlpha = true;

    msMutex.Enter();
    msRenderers.insert(this);
    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Terminate ()
{
    delete0(mDefaultAlphaState);
    delete0(mDefaultCullState);
    delete0(mDefaultDepthState);
    delete0(mDefaultOffsetState);
    delete0(mDefaultStencilState);
    delete0(mDefaultWireState);

    // Clean up any resources not released by the application.
    DestroyAllVertexFormats();
    DestroyAllVertexBuffers();
    DestroyAllIndexBuffers();
    DestroyAllTexture1Ds();
    DestroyAllTexture2Ds();
    DestroyAllTexture3Ds();
    DestroyAllTextureCubes();
    DestroyAllRenderTargets();
    DestroyAllVertexShaders();
    DestroyAllPixelShaders();

    msMutex.Enter();
    msRenderers.erase(this);
    msMutex.Leave();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Vertex format management.
//----------------------------------------------------------------------------
void Renderer::Bind (const VertexFormat* vformat)
{
    if (mVertexFormats.find(vformat) == mVertexFormats.end())
    {
        mVertexFormats[vformat] = new0 PdrVertexFormat(this, vformat);
    }
}
//----------------------------------------------------------------------------
void Renderer::BindAll (const VertexFormat* vformat)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Bind(vformat);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Unbind (const VertexFormat* vformat)
{
    VertexFormatMap::iterator iter = mVertexFormats.find(vformat);
    if (iter != mVertexFormats.end())
    {
        PdrVertexFormat* dxVFormat = iter->second;
        delete0(dxVFormat);
        mVertexFormats.erase(iter);
    }
}
//----------------------------------------------------------------------------
void Renderer::UnbindAll (const VertexFormat* vformat)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Unbind(vformat);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Enable (const VertexFormat* vformat)
{
    VertexFormatMap::iterator iter = mVertexFormats.find(vformat);
    PdrVertexFormat* pdrVFormat;
    if (iter != mVertexFormats.end())
    {
        pdrVFormat = iter->second;
    }
    else
    {
        // Lazy creation.
        pdrVFormat = new0 PdrVertexFormat(this, vformat);
        mVertexFormats[vformat] = pdrVFormat;
    }

    pdrVFormat->Enable(this);
}
//----------------------------------------------------------------------------
void Renderer::Disable (const VertexFormat* vformat)
{
    VertexFormatMap::iterator iter = mVertexFormats.find(vformat);
    if (iter != mVertexFormats.end())
    {
        PdrVertexFormat* pdrVFormat = iter->second;
        pdrVFormat->Disable(this);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Vertex buffer management.
//----------------------------------------------------------------------------
void Renderer::Bind (const VertexBuffer* vbuffer)
{
    if (mVertexBuffers.find(vbuffer) == mVertexBuffers.end())
    {
        mVertexBuffers[vbuffer] = new0 PdrVertexBuffer(this, vbuffer);
    }
}
//----------------------------------------------------------------------------
void Renderer::BindAll (const VertexBuffer* vbuffer)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Bind(vbuffer);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Unbind (const VertexBuffer* vbuffer)
{
    VertexBufferMap::iterator iter = mVertexBuffers.find(vbuffer);
    if (iter != mVertexBuffers.end())
    {
        PdrVertexBuffer* pdrVBuffer = iter->second;
        delete0(pdrVBuffer);
        mVertexBuffers.erase(iter);
    }
}
//----------------------------------------------------------------------------
void Renderer::UnbindAll (const VertexBuffer* vbuffer)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Unbind(vbuffer);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Enable (const VertexBuffer* vbuffer,
    unsigned int streamIndex, unsigned int offset)
{
    // TODO:  Renderer::Draw calls Enable, but using the default values
    // of 0 for streamIndex and offset.  This means that the DX9 renderer can
    // never set a streamIndex different from 1.  The DX9 and OpenGL renderers
    // always enabled the buffer starting at offset 0.  Minimally, the
    // streamIndex handling needs to be different.

    VertexBufferMap::iterator iter = mVertexBuffers.find(vbuffer);
    PdrVertexBuffer* pdrVBuffer;
    if (iter != mVertexBuffers.end())
    {
        pdrVBuffer = iter->second;
    }
    else
    {
        // Lazy creation.
        pdrVBuffer = new0 PdrVertexBuffer(this, vbuffer);
        mVertexBuffers[vbuffer] = pdrVBuffer;
    }

    pdrVBuffer->Enable(this, vbuffer->GetElementSize(), streamIndex, offset);
}
//----------------------------------------------------------------------------
void Renderer::Disable (const VertexBuffer* vbuffer, unsigned int streamIndex)
{
    VertexBufferMap::iterator iter = mVertexBuffers.find(vbuffer);
    if (iter != mVertexBuffers.end())
    {
        PdrVertexBuffer* pdrVBuffer = iter->second;
        pdrVBuffer->Disable(this, streamIndex);
    }
}
//----------------------------------------------------------------------------
void* Renderer::Lock (const VertexBuffer* vbuffer, Buffer::Locking mode)
{
    VertexBufferMap::iterator iter = mVertexBuffers.find(vbuffer);
    PdrVertexBuffer* pdrVBuffer;
    if (iter != mVertexBuffers.end())
    {
        pdrVBuffer = iter->second;
    }
    else
    {
        // Lazy creation.
        pdrVBuffer = new0 PdrVertexBuffer(this, vbuffer);
        mVertexBuffers[vbuffer] = pdrVBuffer;
    }

    return pdrVBuffer->Lock(mode);
}
//----------------------------------------------------------------------------
void Renderer::Unlock (const VertexBuffer* vbuffer)
{
    VertexBufferMap::iterator iter = mVertexBuffers.find(vbuffer);
    if (iter != mVertexBuffers.end())
    {
        PdrVertexBuffer* pdrVBuffer = iter->second;
        pdrVBuffer->Unlock();
    }
}
//----------------------------------------------------------------------------
void Renderer::Update (const VertexBuffer* vbuffer)
{
    int numBytes = vbuffer->GetNumBytes();
    char* srcData = vbuffer->GetData();
    void* trgData = Lock(vbuffer, Buffer::BL_WRITE_ONLY);
    memcpy(trgData, srcData, numBytes);
    Unlock(vbuffer);
}
//----------------------------------------------------------------------------
void Renderer::UpdateAll (const VertexBuffer* vbuffer)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Update(vbuffer);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Index buffer management.
//----------------------------------------------------------------------------
void Renderer::Bind (const IndexBuffer* ibuffer)
{
    if (mIndexBuffers.find(ibuffer) == mIndexBuffers.end())
    {
        mIndexBuffers[ibuffer] = new0 PdrIndexBuffer(this, ibuffer);
    }
}
//----------------------------------------------------------------------------
void Renderer::BindAll (const IndexBuffer* ibuffer)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Bind(ibuffer);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Unbind (const IndexBuffer* ibuffer)
{
    IndexBufferMap::iterator iter = mIndexBuffers.find(ibuffer);
    if (iter != mIndexBuffers.end())
    {
        PdrIndexBuffer* pdrIBuffer = iter->second;
        delete0(pdrIBuffer);
        mIndexBuffers.erase(iter);
    }
}
//----------------------------------------------------------------------------
void Renderer::UnbindAll (const IndexBuffer* ibuffer)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Unbind(ibuffer);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Enable (const IndexBuffer* ibuffer)
{
    IndexBufferMap::iterator iter = mIndexBuffers.find(ibuffer);
    PdrIndexBuffer* pdrIBuffer;
    if (iter != mIndexBuffers.end())
    {
        pdrIBuffer = iter->second;
    }
    else
    {
        // Lazy creation.
        pdrIBuffer = new0 PdrIndexBuffer(this, ibuffer);
        mIndexBuffers[ibuffer] = pdrIBuffer;
    }

    pdrIBuffer->Enable(this);
}
//----------------------------------------------------------------------------
void Renderer::Disable (const IndexBuffer* ibuffer)
{
    IndexBufferMap::iterator iter = mIndexBuffers.find(ibuffer);
    if (iter != mIndexBuffers.end())
    {
        PdrIndexBuffer* pdrIBuffer = iter->second;
        pdrIBuffer->Disable(this);
    }
}
//----------------------------------------------------------------------------
void* Renderer::Lock (const IndexBuffer* ibuffer, Buffer::Locking mode)
{
    IndexBufferMap::iterator iter = mIndexBuffers.find(ibuffer);
    PdrIndexBuffer* pdrIBuffer;
    if (iter != mIndexBuffers.end())
    {
        pdrIBuffer = iter->second;
    }
    else
    {
        // Lazy creation.
        pdrIBuffer = new0 PdrIndexBuffer(this, ibuffer);
        mIndexBuffers[ibuffer] = pdrIBuffer;
    }

    return pdrIBuffer->Lock(mode);
}
//----------------------------------------------------------------------------
void Renderer::Unlock (const IndexBuffer* ibuffer)
{
    IndexBufferMap::iterator iter = mIndexBuffers.find(ibuffer);
    if (iter != mIndexBuffers.end())
    {
        PdrIndexBuffer* pdrIBuffer = iter->second;
        pdrIBuffer->Unlock();
    }
}
//----------------------------------------------------------------------------
void Renderer::Update (const IndexBuffer* ibuffer)
{
    int numBytes = ibuffer->GetNumBytes();
    char* srcData = ibuffer->GetData();
    void* trgData = Lock(ibuffer, Buffer::BL_WRITE_ONLY);
    memcpy(trgData, srcData, numBytes);
    Unlock(ibuffer);
}
//----------------------------------------------------------------------------
void Renderer::UpdateAll (const IndexBuffer* ibuffer)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Update(ibuffer);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Texture1D management.
//----------------------------------------------------------------------------
void Renderer::Bind (const Texture1D* texture)
{
    if (mTexture1Ds.find(texture) == mTexture1Ds.end())
    {
        mTexture1Ds[texture] = new0 PdrTexture1D(this, texture);
    }
}
//----------------------------------------------------------------------------
void Renderer::BindAll (const Texture1D* texture)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Bind(texture);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Unbind (const Texture1D* texture)
{
    Texture1DMap::iterator iter = mTexture1Ds.find(texture);
    if (iter != mTexture1Ds.end())
    {
        PdrTexture1D* pdrTexture = iter->second;
        delete0(pdrTexture);
        mTexture1Ds.erase(iter);
    }
}
//----------------------------------------------------------------------------
void Renderer::UnbindAll (const Texture1D* texture)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Unbind(texture);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Enable (const Texture1D* texture, int textureUnit)
{
    Texture1DMap::iterator iter = mTexture1Ds.find(texture);
    PdrTexture1D* pdrTexture;
    if (iter != mTexture1Ds.end())
    {
        pdrTexture = iter->second;
    }
    else
    {
        // Lazy creation.
        pdrTexture = new0 PdrTexture1D(this, texture);
        mTexture1Ds[texture] = pdrTexture;
    }

    pdrTexture->Enable(this, textureUnit);
}
//----------------------------------------------------------------------------
void Renderer::Disable (const Texture1D* texture, int textureUnit)
{
    Texture1DMap::iterator iter = mTexture1Ds.find(texture);
    if (iter != mTexture1Ds.end())
    {
        PdrTexture1D* pdrTexture = iter->second;
        pdrTexture->Disable(this, textureUnit);
    }
}
//----------------------------------------------------------------------------
void* Renderer::Lock (const Texture1D* texture, int level,
    Buffer::Locking mode)
{
    Texture1DMap::iterator iter = mTexture1Ds.find(texture);
    PdrTexture1D* pdrTexture;
    if (iter != mTexture1Ds.end())
    {
        pdrTexture = iter->second;
    }
    else
    {
        // Lazy creation.
        pdrTexture = new0 PdrTexture1D(this, texture);
        mTexture1Ds[texture] = pdrTexture;
    }

    return pdrTexture->Lock(level, mode);
}
//----------------------------------------------------------------------------
void Renderer::Unlock (const Texture1D* texture, int level)
{
    Texture1DMap::iterator iter = mTexture1Ds.find(texture);
    if (iter != mTexture1Ds.end())
    {
        PdrTexture1D* pdrTexture = iter->second;
        pdrTexture->Unlock(level);
    }
}
//----------------------------------------------------------------------------
void Renderer::Update (const Texture1D* texture, int level)
{
    int numBytes = texture->GetNumLevelBytes(level);
    char* srcData = texture->GetData(level);
    void* trgData = Lock(texture, level, Buffer::BL_WRITE_ONLY);
    memcpy(trgData, srcData, numBytes);
    Unlock(texture, level);
}
//----------------------------------------------------------------------------
void Renderer::UpdateAll (const Texture1D* texture, int level)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Update(texture, level);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Texture2D management.
//----------------------------------------------------------------------------
void Renderer::Bind (const Texture2D* texture)
{
    if (mTexture2Ds.find(texture) == mTexture2Ds.end())
    {
        mTexture2Ds[texture] = new0 PdrTexture2D(this, texture);
    }
}
//----------------------------------------------------------------------------
void Renderer::BindAll (const Texture2D* texture)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Bind(texture);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Unbind (const Texture2D* texture)
{
    Texture2DMap::iterator iter = mTexture2Ds.find(texture);
    if (iter != mTexture2Ds.end())
    {
        PdrTexture2D* pdrTexture = iter->second;
        delete0(pdrTexture);
        mTexture2Ds.erase(iter);
    }
}
//----------------------------------------------------------------------------
void Renderer::UnbindAll (const Texture2D* texture)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Unbind(texture);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Enable (const Texture2D* texture, int textureUnit)
{
    Texture2DMap::iterator iter = mTexture2Ds.find(texture);
    PdrTexture2D* pdrTexture;
    if (iter != mTexture2Ds.end())
    {
        pdrTexture = iter->second;
    }
    else
    {
        // Lazy creation.
        pdrTexture = new0 PdrTexture2D(this, texture);
        mTexture2Ds[texture] = pdrTexture;
    }

    pdrTexture->Enable(this, textureUnit);
}
//----------------------------------------------------------------------------
void Renderer::Disable (const Texture2D* texture, int textureUnit)
{
    Texture2DMap::iterator iter = mTexture2Ds.find(texture);
    if (iter != mTexture2Ds.end())
    {
        PdrTexture2D* pdrTexture = iter->second;
        pdrTexture->Disable(this, textureUnit);
    }
}
//----------------------------------------------------------------------------
void* Renderer::Lock (const Texture2D* texture, int level,
    Buffer::Locking mode)
{
    Texture2DMap::iterator iter = mTexture2Ds.find(texture);
    PdrTexture2D* pdrTexture;
    if (iter != mTexture2Ds.end())
    {
        pdrTexture = iter->second;
    }
    else
    {
        // Lazy creation.
        pdrTexture = new0 PdrTexture2D(this, texture);
        mTexture2Ds[texture] = pdrTexture;
    }

    return pdrTexture->Lock(level, mode);
}
//----------------------------------------------------------------------------
void Renderer::Unlock (const Texture2D* texture, int level)
{
    Texture2DMap::iterator iter = mTexture2Ds.find(texture);
    if (iter != mTexture2Ds.end())
    {
        PdrTexture2D* pdrTexture = iter->second;
        pdrTexture->Unlock(level);
    }
}
//----------------------------------------------------------------------------
void Renderer::Update (const Texture2D* texture, int level)
{
    int numBytes = texture->GetNumLevelBytes(level);
    char* srcData = texture->GetData(level);
    void* trgData = Lock(texture, level, Buffer::BL_WRITE_ONLY);
    memcpy(trgData, srcData, numBytes);
    Unlock(texture, level);
}
//----------------------------------------------------------------------------
void Renderer::UpdateAll (const Texture2D* texture, int level)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Update(texture, level);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Texture3D management.
//----------------------------------------------------------------------------
void Renderer::Bind (const Texture3D* texture)
{
    if (mTexture3Ds.find(texture) == mTexture3Ds.end())
    {
        mTexture3Ds[texture] = new0 PdrTexture3D(this, texture);
    }
}
//----------------------------------------------------------------------------
void Renderer::BindAll (const Texture3D* texture)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Bind(texture);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Unbind (const Texture3D* texture)
{
    Texture3DMap::iterator iter = mTexture3Ds.find(texture);
    if (iter != mTexture3Ds.end())
    {
        PdrTexture3D* pdrTexture = iter->second;
        delete0(pdrTexture);
        mTexture3Ds.erase(iter);
    }
}
//----------------------------------------------------------------------------
void Renderer::UnbindAll (const Texture3D* texture)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Unbind(texture);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Enable (const Texture3D* texture, int textureUnit)
{
    Texture3DMap::iterator iter = mTexture3Ds.find(texture);
    PdrTexture3D* pdrTexture;
    if (iter != mTexture3Ds.end())
    {
        pdrTexture = iter->second;
    }
    else
    {
        // Lazy creation.
        pdrTexture = new0 PdrTexture3D(this, texture);
        mTexture3Ds[texture] = pdrTexture;
    }

    pdrTexture->Enable(this, textureUnit);
}
//----------------------------------------------------------------------------
void Renderer::Disable (const Texture3D* texture, int textureUnit)
{
    Texture3DMap::iterator iter = mTexture3Ds.find(texture);
    if (iter != mTexture3Ds.end())
    {
        PdrTexture3D* pdrTexture = iter->second;
        pdrTexture->Disable(this, textureUnit);
    }
}
//----------------------------------------------------------------------------
void* Renderer::Lock (const Texture3D* texture, int level,
    Buffer::Locking mode)
{
    Texture3DMap::iterator iter = mTexture3Ds.find(texture);
    PdrTexture3D* pdrTexture;
    if (iter != mTexture3Ds.end())
    {
        pdrTexture = iter->second;
    }
    else
    {
        // Lazy creation.
        pdrTexture = new0 PdrTexture3D(this, texture);
        mTexture3Ds[texture] = pdrTexture;
    }

    return pdrTexture->Lock(level, mode);
}
//----------------------------------------------------------------------------
void Renderer::Unlock (const Texture3D* texture, int level)
{
    Texture3DMap::iterator iter = mTexture3Ds.find(texture);
    if (iter != mTexture3Ds.end())
    {
        PdrTexture3D* pdrTexture = iter->second;
        pdrTexture->Unlock(level);
    }
}
//----------------------------------------------------------------------------
void Renderer::Update (const Texture3D* texture, int level)
{
    int numBytes = texture->GetNumLevelBytes(level);
    char* srcData = texture->GetData(level);
    void* trgData = Lock(texture, level, Buffer::BL_WRITE_ONLY);
    memcpy(trgData, srcData, numBytes);
    Unlock(texture, level);
}
//----------------------------------------------------------------------------
void Renderer::UpdateAll (const Texture3D* texture, int level)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Update(texture, level);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// TextureCube management.
//----------------------------------------------------------------------------
void Renderer::Bind (const TextureCube* texture)
{
    if (mTextureCubes.find(texture) == mTextureCubes.end())
    {
        mTextureCubes[texture] = new0 PdrTextureCube(this, texture);
    }
}
//----------------------------------------------------------------------------
void Renderer::BindAll (const TextureCube* texture)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Bind(texture);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Unbind (const TextureCube* texture)
{
    TextureCubeMap::iterator iter = mTextureCubes.find(texture);
    if (iter != mTextureCubes.end())
    {
        PdrTextureCube* pdrTexture = iter->second;
        delete0(pdrTexture);
        mTextureCubes.erase(iter);
    }
}
//----------------------------------------------------------------------------
void Renderer::UnbindAll (const TextureCube* texture)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Unbind(texture);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Enable (const TextureCube* texture, int textureUnit)
{
    TextureCubeMap::iterator iter = mTextureCubes.find(texture);
    PdrTextureCube* pdrTexture;
    if (iter != mTextureCubes.end())
    {
        pdrTexture = iter->second;
    }
    else
    {
        // Lazy creation.
        pdrTexture = new0 PdrTextureCube(this, texture);
        mTextureCubes[texture] = pdrTexture;
    }

    pdrTexture->Enable(this, textureUnit);
}
//----------------------------------------------------------------------------
void Renderer::Disable (const TextureCube* texture, int textureUnit)
{
    TextureCubeMap::iterator iter = mTextureCubes.find(texture);
    if (iter != mTextureCubes.end())
    {
        PdrTextureCube* pdrTexture = iter->second;
        pdrTexture->Disable(this, textureUnit);
    }
}
//----------------------------------------------------------------------------
void* Renderer::Lock (const TextureCube* texture, int face, int level,
    Buffer::Locking mode)
{
    TextureCubeMap::iterator iter = mTextureCubes.find(texture);
    PdrTextureCube* pdrTexture;
    if (iter != mTextureCubes.end())
    {
        pdrTexture = iter->second;
    }
    else
    {
        // Lazy creation.
        pdrTexture = new0 PdrTextureCube(this, texture);
        mTextureCubes[texture] = pdrTexture;
    }

    return pdrTexture->Lock(face, level, mode);
}
//----------------------------------------------------------------------------
void Renderer::Unlock (const TextureCube* texture, int face, int level)
{
    TextureCubeMap::iterator iter = mTextureCubes.find(texture);
    if (iter != mTextureCubes.end())
    {
        PdrTextureCube* pdrTexture = iter->second;
        pdrTexture->Unlock(face, level);
    }
}
//----------------------------------------------------------------------------
void Renderer::Update (const TextureCube* texture, int face, int level)
{
    int numBytes = texture->GetNumLevelBytes(level);
    char* srcData = texture->GetData(face, level);
    void* trgData = Lock(texture, face, level, Buffer::BL_WRITE_ONLY);
    memcpy(trgData, srcData, numBytes);
    Unlock(texture, face, level);
}
//----------------------------------------------------------------------------
void Renderer::UpdateAll (const TextureCube* texture, int face, int level)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Update(texture, face, level);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Render target management.
//----------------------------------------------------------------------------
void Renderer::Bind (const RenderTarget* renderTarget)
{
    if (mRenderTargets.find(renderTarget) == mRenderTargets.end())
    {
        mRenderTargets[renderTarget] =
            new0 PdrRenderTarget(this, renderTarget);
    }
}
//----------------------------------------------------------------------------
void Renderer::BindAll (const RenderTarget* renderTarget)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Bind(renderTarget);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Unbind (const RenderTarget* renderTarget)
{
    RenderTargetMap::iterator iter = mRenderTargets.find(renderTarget);
    if (iter != mRenderTargets.end())
    {
        PdrRenderTarget* pdrRenderTarget = iter->second;
        delete0(pdrRenderTarget);
        mRenderTargets.erase(iter);
    }
}
//----------------------------------------------------------------------------
void Renderer::UnbindAll (const RenderTarget* renderTarget)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Unbind(renderTarget);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Enable (const RenderTarget* renderTarget)
{
    RenderTargetMap::iterator iter = mRenderTargets.find(renderTarget);
    PdrRenderTarget* pdrRenderTarget;
    if (iter != mRenderTargets.end())
    {
        pdrRenderTarget = iter->second;
    }
    else
    {
        // Lazy creation.
        pdrRenderTarget = new0 PdrRenderTarget(this, renderTarget);
        mRenderTargets[renderTarget] = pdrRenderTarget;
    }

    pdrRenderTarget->Enable(this);
}
//----------------------------------------------------------------------------
void Renderer::Disable (const RenderTarget* renderTarget)
{
    RenderTargetMap::iterator iter = mRenderTargets.find(renderTarget);
    if (iter != mRenderTargets.end())
    {
        PdrRenderTarget* pdrRenderTarget = iter->second;
        pdrRenderTarget->Disable(this);
    }
}
//----------------------------------------------------------------------------
void Renderer::ReadColor (int i, const RenderTarget* renderTarget,
    Texture2D*& texture)
{
    RenderTargetMap::iterator iter = mRenderTargets.find(renderTarget);
    if (iter != mRenderTargets.end())
    {
        PdrRenderTarget* pdrRenderTarget = iter->second;
        pdrRenderTarget->ReadColor(i, this, texture);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Vertex shader management.
//----------------------------------------------------------------------------
void Renderer::Bind (const VertexShader* vshader)
{
    if (mVertexShaders.find(vshader) == mVertexShaders.end())
    {
        mVertexShaders[vshader] = new0 PdrVertexShader(this, vshader);
    }
}
//----------------------------------------------------------------------------
void Renderer::BindAll (const VertexShader* vshader)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Bind(vshader);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Unbind (const VertexShader* vshader)
{
    VertexShaderMap::iterator iter = mVertexShaders.find(vshader);
    if (iter != mVertexShaders.end())
    {
        PdrVertexShader* pdrVShader = iter->second;
        delete0(pdrVShader);
        mVertexShaders.erase(iter);
    }
}
//----------------------------------------------------------------------------
void Renderer::UnbindAll (const VertexShader* vshader)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Unbind(vshader);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Enable (const VertexShader* vshader,
    const ShaderParameters* parameters)
{
    VertexShaderMap::iterator iter = mVertexShaders.find(vshader);
    PdrVertexShader* pdrVShader;
    if (iter != mVertexShaders.end())
    {
        pdrVShader = iter->second;
    }
    else
    {
        // Lazy creation.
        pdrVShader = new0 PdrVertexShader(this, vshader);
        mVertexShaders[vshader] = pdrVShader;
    }

    pdrVShader->Enable(this, vshader, parameters);
}
//----------------------------------------------------------------------------
void Renderer::Disable (const VertexShader* vshader,
    const ShaderParameters* parameters)
{
    VertexShaderMap::iterator iter = mVertexShaders.find(vshader);
    if (iter != mVertexShaders.end())
    {
        PdrVertexShader* pdrVShader = iter->second;
        pdrVShader->Disable(this, vshader, parameters);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Pixel shader management.
//----------------------------------------------------------------------------
void Renderer::Bind (const PixelShader* pshader)
{
    if (mPixelShaders.find(pshader) == mPixelShaders.end())
    {
        mPixelShaders[pshader] = new0 PdrPixelShader(this, pshader);
    }
}
//----------------------------------------------------------------------------
void Renderer::BindAll (const PixelShader* pshader)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Bind(pshader);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Unbind (const PixelShader* pshader)
{
    PixelShaderMap::iterator iter = mPixelShaders.find(pshader);
    if (iter != mPixelShaders.end())
    {
        PdrPixelShader* pdrPShader = iter->second;
        delete0(pdrPShader);
        mPixelShaders.erase(iter);
    }
}
//----------------------------------------------------------------------------
void Renderer::UnbindAll (const PixelShader* pshader)
{
    msMutex.Enter();

    RendererSet::iterator iter = msRenderers.begin();
    RendererSet::iterator end = msRenderers.end();
    for (/**/; iter != end; ++iter)
    {
        Renderer* renderer = *iter;
        renderer->Unbind(pshader);
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Renderer::Enable (const PixelShader* pshader,
    const ShaderParameters* parameters)
{
    PixelShaderMap::iterator iter = mPixelShaders.find(pshader);
    PdrPixelShader* pdrPShader;
    if (iter != mPixelShaders.end())
    {
        pdrPShader = iter->second;
    }
    else
    {
        // Lazy creation.
        pdrPShader = new0 PdrPixelShader(this, pshader);
        mPixelShaders[pshader] = pdrPShader;
    }

    pdrPShader->Enable(this, pshader, parameters);
}
//----------------------------------------------------------------------------
void Renderer::Disable (const PixelShader* pshader,
    const ShaderParameters* parameters)
{
    PixelShaderMap::iterator iter = mPixelShaders.find(pshader);
    if (iter != mPixelShaders.end())
    {
        PdrPixelShader* pdrPShader = iter->second;
        pdrPShader->Disable(this, pshader, parameters);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Compute a picking ray.
//----------------------------------------------------------------------------
bool Renderer::GetPickRay (int x, int y, APoint& origin, AVector& direction)
    const
{
    if (!mCamera)
    {
        return false;
    }

    // Get the current viewport and test whether (x,y) is in it.
    int viewX, viewY, viewW, viewH;
    GetViewport(viewX, viewY, viewW, viewH);
    if (x < viewX || x > viewX + viewW || y < viewY || y > viewY + viewH)
    {
        return false;
    }

    // Get the [0,1]^2-normalized coordinates of (x,y).
    float r = ((float)(x - viewX))/(float)viewW;
    float u = ((float)(y - viewY))/(float)viewH;

    // Get the relative coordinates in [rmin,rmax]x[umin,umax].
    float rBlend = (1.0f - r)*mCamera->GetRMin() + r*mCamera->GetRMax();
    float uBlend = (1.0f - u)*mCamera->GetUMin() + u*mCamera->GetUMax();

    if (mCamera->IsPerspective())
    {
        origin = mCamera->GetPosition();
        direction = mCamera->GetDMin()*mCamera->GetDVector() +
            rBlend*mCamera->GetRVector() + uBlend*mCamera->GetUVector();
        direction.Normalize();
    }
    else
    {
        origin = mCamera->GetPosition() + rBlend*mCamera->GetRVector() +
            uBlend*mCamera->GetUVector();
        direction = mCamera->GetDVector();
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Entry points for drawing.
//----------------------------------------------------------------------------
void Renderer::Draw (const VisibleSet& visibleSet, GlobalEffect* globalEffect)
{
    if (!globalEffect)
    {
        const int numVisible = visibleSet.GetNumVisible();
        for (int i = 0; i < numVisible; ++i)
        {
            const Visual* visual = (const Visual*)visibleSet.GetVisible(i);
            const VisualEffectInstance* instance =
                visual->GetEffectInstance();
            Draw(visual, instance);
        }
    }
    else
    {
        globalEffect->Draw(this, visibleSet);
    }
}
//----------------------------------------------------------------------------
void Renderer::Draw (const Visual* visual)
{
    const VisualEffectInstance* instance = visual->GetEffectInstance();
    Draw(visual, instance);
}
//----------------------------------------------------------------------------
void Renderer::Draw (const Visual* visual,
    const VisualEffectInstance* instance)
{
    if (!visual)
    {
        assertion(false, "The visual object must exist.\n");
        return;
    }

    if (!instance)
    {
        assertion(false, "The visual object must have an effect instance.\n");
        return;
    }

    const VertexFormat* vformat = visual->GetVertexFormat();
    const VertexBuffer* vbuffer = visual->GetVertexBuffer();
    const IndexBuffer* ibuffer = visual->GetIndexBuffer();

    // The OpenGL renderer requires the vertex buffer to be enabled before
    // the vertex format is enabled.  The order is irrelevant for the
    // DirectX9 renderer.
    Enable(vbuffer);
    Enable(vformat);
    if (ibuffer)
    {
        Enable(ibuffer);
    }

    const int numPasses = instance->GetNumPasses();
    for (int i = 0; i < numPasses; ++i)
    {
        const VisualPass* pass = instance->GetPass(i);
        ShaderParameters* vparams = instance->GetVertexParameters(i);
        ShaderParameters* pparams = instance->GetPixelParameters(i);
        VertexShader* vshader = pass->GetVertexShader();
        PixelShader* pshader = pass->GetPixelShader();

        // Update any shader constants that vary during runtime.
        vparams->UpdateConstants(visual, mCamera);
        pparams->UpdateConstants(visual, mCamera);

        // Set visual state.
        SetAlphaState(pass->GetAlphaState());
        SetCullState(pass->GetCullState());
        SetDepthState(pass->GetDepthState());
        SetOffsetState(pass->GetOffsetState());
        SetStencilState(pass->GetStencilState());
        SetWireState(pass->GetWireState());

        // Enable the shaders.
        Enable(vshader, vparams);
        Enable(pshader, pparams);

        // Draw the primitive.
        DrawPrimitive(visual);

        // Disable the shaders.
        Disable(vshader, vparams);
        Disable(pshader, pparams);

#ifdef WM5_RESET_STATE_AFTER_DRAW
        // Restore visual state.
        SetAlphaState(mDefaultAlphaState);
        SetCullState(mDefaultCullState);
        SetDepthState(mDefaultDepthState);
        SetOffsetState(mDefaultOffsetState);
        SetStencilState(mDefaultStencilState);
        SetWireState(mDefaultWireState);
#endif
    }

    if (ibuffer)
    {
        Disable(ibuffer);
    }

    Disable(vformat);
    Disable(vbuffer);
}
//----------------------------------------------------------------------------
void Renderer::DestroyAllVertexFormats ()
{
    VertexFormatMap::iterator iter = mVertexFormats.begin();
    VertexFormatMap::iterator end = mVertexFormats.end();
    for (/**/; iter != end; ++iter)
    {
        PdrVertexFormat* pdrVFormat = iter->second;
        delete0(pdrVFormat);
    }
}
//----------------------------------------------------------------------------
void Renderer::DestroyAllVertexBuffers ()
{
    VertexBufferMap::iterator iter = mVertexBuffers.begin();
    VertexBufferMap::iterator end = mVertexBuffers.end();
    for (/**/; iter != end; ++iter)
    {
        PdrVertexBuffer* pdrVBuffer = iter->second;
        delete0(pdrVBuffer);
    }
}
//----------------------------------------------------------------------------
void Renderer::DestroyAllIndexBuffers ()
{
    IndexBufferMap::iterator iter = mIndexBuffers.begin();
    IndexBufferMap::iterator end = mIndexBuffers.end();
    for (/**/; iter != end; ++iter)
    {
        PdrIndexBuffer* pdrIBuffer = iter->second;
        delete0(pdrIBuffer);
    }
}
//----------------------------------------------------------------------------
void Renderer::DestroyAllTexture1Ds ()
{
    Texture1DMap::iterator iter = mTexture1Ds.begin();
    Texture1DMap::iterator end = mTexture1Ds.end();
    for (/**/; iter != end; ++iter)
    {
        PdrTexture1D* pdrTexture = iter->second;
        delete0(pdrTexture);
    }
}
//----------------------------------------------------------------------------
void Renderer::DestroyAllTexture2Ds ()
{
    Texture2DMap::iterator iter = mTexture2Ds.begin();
    Texture2DMap::iterator end = mTexture2Ds.end();
    for (/**/; iter != end; ++iter)
    {
        PdrTexture2D* pdrTexture = iter->second;
        delete0(pdrTexture);
    }
}
//----------------------------------------------------------------------------
void Renderer::DestroyAllTexture3Ds ()
{
    Texture3DMap::iterator iter = mTexture3Ds.begin();
    Texture3DMap::iterator end = mTexture3Ds.end();
    for (/**/; iter != end; ++iter)
    {
        PdrTexture3D* pdrTexture = iter->second;
        delete0(pdrTexture);
    }
}
//----------------------------------------------------------------------------
void Renderer::DestroyAllTextureCubes ()
{
    TextureCubeMap::iterator iter = mTextureCubes.begin();
    TextureCubeMap::iterator end = mTextureCubes.end();
    for (/**/; iter != end; ++iter)
    {
        PdrTextureCube* pdrTexture = iter->second;
        delete0(pdrTexture);
    }
}
//----------------------------------------------------------------------------
void Renderer::DestroyAllRenderTargets ()
{
    RenderTargetMap::iterator iter = mRenderTargets.begin();
    RenderTargetMap::iterator end = mRenderTargets.end();
    for (/**/; iter != end; ++iter)
    {
        PdrRenderTarget* pdrRenderTarget = iter->second;
        delete0(pdrRenderTarget);
    }
}
//----------------------------------------------------------------------------
void Renderer::DestroyAllVertexShaders ()
{
    VertexShaderMap::iterator iter = mVertexShaders.begin();
    VertexShaderMap::iterator end = mVertexShaders.end();
    for (/**/; iter != end; ++iter)
    {
        PdrVertexShader* pdrVShader = iter->second;
        delete0(pdrVShader);
    }
}
//----------------------------------------------------------------------------
void Renderer::DestroyAllPixelShaders ()
{
    PixelShaderMap::iterator iter = mPixelShaders.begin();
    PixelShaderMap::iterator end = mPixelShaders.end();
    for (/**/; iter != end; ++iter)
    {
        PdrPixelShader* pdrPShader = iter->second;
        delete0(pdrPShader);
    }
}
//----------------------------------------------------------------------------
PdrVertexFormat* Renderer::GetResource (const VertexFormat* vformat)
{
    VertexFormatMap::iterator iter = mVertexFormats.find(vformat);
    if (iter != mVertexFormats.end())
    {
        return iter->second;
    }
    return 0;
}
//----------------------------------------------------------------------------
PdrVertexBuffer* Renderer::GetResource (const VertexBuffer* vbuffer)
{
    VertexBufferMap::iterator iter = mVertexBuffers.find(vbuffer);
    if (iter != mVertexBuffers.end())
    {
        return iter->second;
    }
    return 0;
}
//----------------------------------------------------------------------------
PdrIndexBuffer* Renderer::GetResource (const IndexBuffer* ibuffer)
{
    IndexBufferMap::iterator iter = mIndexBuffers.find(ibuffer);
    if (iter != mIndexBuffers.end())
    {
        return iter->second;
    }
    return 0;
}
//----------------------------------------------------------------------------
PdrTexture1D* Renderer::GetResource (const Texture1D* texture)
{
    Texture1DMap::iterator iter = mTexture1Ds.find(texture);
    if (iter != mTexture1Ds.end())
    {
        return iter->second;
    }
    return 0;
}
//----------------------------------------------------------------------------
PdrTexture2D* Renderer::GetResource (const Texture2D* texture)
{
    Texture2DMap::iterator iter = mTexture2Ds.find(texture);
    if (iter != mTexture2Ds.end())
    {
        return iter->second;
    }
    return 0;
}
//----------------------------------------------------------------------------
PdrTexture3D* Renderer::GetResource (const Texture3D* texture)
{
    Texture3DMap::iterator iter = mTexture3Ds.find(texture);
    if (iter != mTexture3Ds.end())
    {
        return iter->second;
    }
    return 0;
}
//----------------------------------------------------------------------------
PdrTextureCube* Renderer::GetResource (const TextureCube* texture)
{
    TextureCubeMap::iterator iter = mTextureCubes.find(texture);
    if (iter != mTextureCubes.end())
    {
        return iter->second;
    }
    return 0;
}
//----------------------------------------------------------------------------
PdrRenderTarget* Renderer::GetResource (const RenderTarget* renderTarget)
{
    RenderTargetMap::iterator iter = mRenderTargets.find(renderTarget);
    if (iter != mRenderTargets.end())
    {
        return iter->second;
    }
    return 0;
}
//----------------------------------------------------------------------------
PdrVertexShader* Renderer::GetResource (const VertexShader* vshader)
{
    VertexShaderMap::iterator iter = mVertexShaders.find(vshader);
    if (iter != mVertexShaders.end())
    {
        return iter->second;
    }
    return 0;
}
//----------------------------------------------------------------------------
PdrPixelShader* Renderer::GetResource (const PixelShader* pshader)
{
    PixelShaderMap::iterator iter = mPixelShaders.find(pshader);
    if (iter != mPixelShaders.end())
    {
        return iter->second;
    }
    return 0;
}
//----------------------------------------------------------------------------
