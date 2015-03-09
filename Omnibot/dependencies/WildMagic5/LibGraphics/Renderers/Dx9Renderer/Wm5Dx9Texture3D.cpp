// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/02)

#include "Wm5GraphicsPCH.h"
#include "Wm5Dx9Texture3D.h"
#include "Wm5Dx9Mapping.h"
#include "Wm5Dx9RendererData.h"
#include "Wm5Renderer.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PdrTexture3D::PdrTexture3D (Renderer* renderer, const Texture3D* texture)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;

    int numLevels = texture->GetNumLevels();
    HRESULT hr = device->CreateVolumeTexture((UINT)texture->GetWidth(),
        (UINT)texture->GetHeight(), (UINT)texture->GetThickness(),
        (UINT)numLevels, gDX9BufferUsage[texture->GetUsage()],
        gDX9TextureFormat[texture->GetFormat()], D3DPOOL_MANAGED,
        &mTexture, 0);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to create 3D texture: %s\n",
        DXGetErrorString(hr));

    for (int level = 0; level < numLevels; ++level)
    {
        void* data = Lock(level, Buffer::BL_WRITE_ONLY);
        memcpy(data, texture->GetData(level),
            texture->GetNumLevelBytes(level));
        Unlock(level);
    }
}
//----------------------------------------------------------------------------
PdrTexture3D::~PdrTexture3D ()
{
    mTexture->Release();
}
//----------------------------------------------------------------------------
void PdrTexture3D::Enable (Renderer* renderer, int textureUnit)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;

    HRESULT hr = device->SetTexture(textureUnit, mTexture);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to enable 3D texture, unit %d: %s\n",
        textureUnit, DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void PdrTexture3D::Disable (Renderer* renderer, int textureUnit)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;
    HRESULT hr;
    WM5_UNUSED(hr);

#ifdef WM5_PDR_DEBUG
    // Verify that the active texture is the one making the disable request.
    IDirect3DBaseTexture9 *activeTexture = 0;
    hr = device->GetTexture(textureUnit, &activeTexture);
    assertion(hr == D3D_OK, "Failed to get 3D texture, unit %d: %s\n",
        textureUnit, DXGetErrorString(hr));
    assertion(activeTexture == mTexture, "Mismatched textures\n");
    activeTexture->Release();
#endif

    // Disable the texture by clearing the state.
    hr = device->SetTexture(textureUnit, 0);
    assertion(hr == D3D_OK, "Failed to disable 3D texture, unit %d: %s\n",
        textureUnit, DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void* PdrTexture3D::Lock (int level, Buffer::Locking mode)
{
    D3DLOCKED_BOX box;
    HRESULT hr = mTexture->LockBox((UINT)level, &box, 0,
        gDX9BufferLocking[mode]);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to lock level %u of 3D texture: %s\n",
        level, DXGetErrorString(hr));
    return box.pBits;
}
//----------------------------------------------------------------------------
void PdrTexture3D::Unlock (int level)
{
    HRESULT hr = mTexture->UnlockBox((UINT)level);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to unlock level %u of 3D texture: %s\n",
        level, DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
