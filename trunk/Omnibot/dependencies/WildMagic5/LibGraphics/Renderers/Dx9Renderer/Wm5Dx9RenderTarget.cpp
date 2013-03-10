// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/10/02)

#include "Wm5GraphicsPCH.h"
#include "Wm5Dx9RenderTarget.h"
#include "Wm5Dx9Mapping.h"
#include "Wm5Dx9RendererData.h"
#include "Wm5Dx9Texture2D.h"
#include "Wm5Renderer.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PdrRenderTarget::PdrRenderTarget (Renderer* renderer,
    const RenderTarget* renderTarget)
    :
    mSaveColorSurface(0),
    mSaveDepthStencilSurface(0)
{
    HRESULT hr;
    WM5_UNUSED(hr);

    mNumTargets = renderTarget->GetNumTargets();
    assertion(mNumTargets >= 1,
        "Number of render targets must be at least one.\n");

    mWidth = renderTarget->GetWidth();
    mHeight = renderTarget->GetHeight();
    mFormat = renderTarget->GetFormat();
    mHasDepthStencil = renderTarget->HasDepthStencil();

    mColorTextures = new1<IDirect3DTexture9*>(mNumTargets);
    mColorSurfaces = new1<IDirect3DSurface9*>(mNumTargets);
    for (int i = 0; i < mNumTargets; ++i)
    {
        Texture2D* colorTexture = renderTarget->GetColorTexture(i);
        assertion(!renderer->InTexture2DMap(colorTexture),
            "Texture should not yet exist.\n");

        PdrTexture2D* dxColorTexture = new0 PdrTexture2D(renderer, true,
            colorTexture, renderTarget->HasMipmaps());
        renderer->InsertInTexture2DMap(colorTexture, dxColorTexture);
        mColorTextures[i] = dxColorTexture->mTexture;
        mColorTextures[i]->AddRef();

        hr = mColorTextures[i]->GetSurfaceLevel(0, &mColorSurfaces[i]);
        assertion(hr == D3D_OK, "Failed to get rt %d color surface: %s\n",
            i, DXGetErrorString(hr));
    }

    if (mHasDepthStencil)
    {
        Texture2D* depthStencilTexture;
        PdrTexture2D* dxDepthStencilTexture;

        depthStencilTexture = renderTarget->GetDepthStencilTexture();
        dxDepthStencilTexture = new0 PdrTexture2D(renderer, false,
            depthStencilTexture, false);
        renderer->InsertInTexture2DMap(depthStencilTexture,
            dxDepthStencilTexture);
        mDepthStencilTexture = dxDepthStencilTexture->mTexture;
        mDepthStencilTexture->AddRef();

        hr = mDepthStencilTexture->GetSurfaceLevel(0, &mDepthStencilSurface);
        assertion(hr == D3D_OK,
            "Failed to get rt depthstencil surface: %s\n",
            DXGetErrorString(hr));
    }
    else
    {
        mDepthStencilTexture = 0;
        mDepthStencilSurface = 0;
    }
}
//----------------------------------------------------------------------------
PdrRenderTarget::~PdrRenderTarget ()
{
    int i;
    for (i = 0; i < mNumTargets; ++i)
    {
        mColorSurfaces[i]->Release();
        mColorTextures[i]->Release();
    }

    delete1(mColorTextures);
    delete1(mColorSurfaces);

    if (mHasDepthStencil)
    {
        if (mDepthStencilSurface)
        {
            mDepthStencilSurface->Release();
        }

        if (mDepthStencilTexture)
        {
            mDepthStencilTexture->Release();
        }
    }
}
//----------------------------------------------------------------------------
void PdrRenderTarget::Enable (Renderer* renderer)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;
    HRESULT hr;
    WM5_UNUSED(hr);

    hr = device->GetRenderTarget(0, &mSaveColorSurface);
    assertion(hr == D3D_OK, "Failed to get old rt 0 color surface: %s\n",
        DXGetErrorString(hr));

    // The viewport is automatically set by this call.
    hr = device->SetRenderTarget(0, mColorSurfaces[0]);
    assertion(hr == D3D_OK, "Failed to set new rt 0 color surface: %s\n",
        DXGetErrorString(hr));

    for (int i = 1; i < mNumTargets; ++i)
    {
        hr = device->SetRenderTarget((DWORD)i, mColorSurfaces[i]);
        assertion(hr == D3D_OK, "Failed to set new rt %d color surface: %s\n",
            i, DXGetErrorString(hr));
    }

    if (mHasDepthStencil)
    {
        hr = device->GetDepthStencilSurface(&mSaveDepthStencilSurface);
        assertion(hr == D3D_OK,
            "Failed to get old rt depthstencil surface: %s\n",
            DXGetErrorString(hr));

        hr = device->SetDepthStencilSurface(mDepthStencilSurface);
        assertion(hr == D3D_OK,
            "Failed to set new rt depthstencil surface: %s\n",
            DXGetErrorString(hr));
    }
}
//----------------------------------------------------------------------------
void PdrRenderTarget::Disable (Renderer* renderer)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;
    HRESULT hr;
    WM5_UNUSED(hr);

    // The viewport is automatically restored by this call.
    hr = device->SetRenderTarget(0, mSaveColorSurface);
    assertion(hr == D3D_OK, "Failed to set old rt 0 color surface: %s\n",
        DXGetErrorString(hr));

    mSaveColorSurface->Release();

    for (int i = 1; i < mNumTargets; ++i)
    {
        // The viewport is automatically restored by this call.
        hr = device->SetRenderTarget((DWORD)i, 0);
        assertion(hr == D3D_OK,
            "Failed to set old rt %d color surface: %s\n", i,
            DXGetErrorString(hr));
    }

    if (mHasDepthStencil)
    {
        hr = device->SetDepthStencilSurface(mSaveDepthStencilSurface);
        assertion(hr == D3D_OK,
            "Failed to set old rt 0 depthstencil surface: %s\n",
            DXGetErrorString(hr));

        mSaveDepthStencilSurface->Release();
    }
}
//----------------------------------------------------------------------------
void PdrRenderTarget::ReadColor (int i, Renderer* renderer,
    Texture2D*& texture)
{
    if (i < 0 || i >= mNumTargets)
    {
        assertion(false, "Invalid target index.\n");
        return;
    }

    IDirect3DDevice9* device = renderer->mData->mDevice;
    HRESULT hr;
    WM5_UNUSED(hr);

    // Enable the input render target surface.
    if (i == 0)
    {
        hr = device->GetRenderTarget(0, &mSaveColorSurface);
        assertion(hr == D3D_OK, "Failed to get old rt 0 color surface: %s\n",
            DXGetErrorString(hr));
    }

    hr = device->SetRenderTarget((DWORD)i, mColorSurfaces[i]);
    assertion(hr == D3D_OK,
        "Failed to set new rt %d color surface: %s\n", i,
        DXGetErrorString(hr));

    // Make a duplicate in system memory.
    IDirect3DTexture9* copyTexture = 0;
    hr = D3DXCreateTexture
    (
        device,
        (UINT)mWidth,
        (UINT)mHeight,
        0,
        0,
        gDX9TextureFormat[mFormat],
        D3DPOOL_SYSTEMMEM,
        &copyTexture
    );
    assertion(hr == D3D_OK,
        "Failed to create copy texture: %s\n",
        DXGetErrorString(hr));

    // Get the surface associated with the copy.
    IDirect3DSurface9* copySurface = 0;
    hr = copyTexture->GetSurfaceLevel(0, &copySurface);
    assertion(hr == D3D_OK,
        "Failed to get surface level for copy texture: %s\n",
        DXGetErrorString(hr));

    // Copy the render target surface.
    hr = device->GetRenderTargetData(mColorSurfaces[i], copySurface);
    assertion(hr == D3D_OK,
        "Failed to copy the rt %d surface: %s\n", i,
        DXGetErrorString(hr));

    // Get the data to write to disk.
    D3DLOCKED_RECT rect;
    hr = copySurface->LockRect(&rect, 0, 0);
    assertion(hr == D3D_OK,
        "Failed to lock copy surface: %s\n",
        DXGetErrorString(hr));

    if (texture)
    {
        if (texture->GetFormat() != mFormat ||
            texture->GetWidth() != (int)mWidth ||
            texture->GetHeight() != (int)mHeight)
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
    memcpy(texture->GetData(0), rect.pBits, texture->GetNumLevelBytes(0));

    hr = copySurface->UnlockRect();
    assertion(hr == D3D_OK,
        "Failed to unlock copy surface: %s\n",
        DXGetErrorString(hr));

    copySurface->Release();
    copyTexture->Release();

    // Restore the previous render target surface.
    if (i == 0)
    {
        hr = device->SetRenderTarget(0, mSaveColorSurface);
        assertion(hr == D3D_OK,
            "Failed to set old rt 0 color surface: %s\n",
            DXGetErrorString(hr));

        mSaveColorSurface->Release();
    }
}
//----------------------------------------------------------------------------
