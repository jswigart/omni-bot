// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/02)

#include "Wm5GraphicsPCH.h"
#include "Wm5Dx9IndexBuffer.h"
#include "Wm5Dx9Mapping.h"
#include "Wm5Dx9RendererData.h"
#include "Wm5Renderer.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PdrIndexBuffer::PdrIndexBuffer (Renderer* renderer,
    const IndexBuffer* ibuffer)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;

    UINT numBytes = (UINT)ibuffer->GetNumBytes();
    DWORD usage = gDX9BufferUsage[ibuffer->GetUsage()];
    D3DFORMAT format =
        (ibuffer->GetElementSize() == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32);
    HRESULT hr = device->CreateIndexBuffer(numBytes, usage, format,
        D3DPOOL_DEFAULT, &mBuffer, 0);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to create index buffer: %s\n",
        DXGetErrorString(hr));

    void* data = Lock(Buffer::BL_WRITE_ONLY);
    memcpy(data, ibuffer->GetData(), ibuffer->GetNumBytes());
    Unlock();
}
//----------------------------------------------------------------------------
PdrIndexBuffer::~PdrIndexBuffer ()
{
    mBuffer->Release();
}
//----------------------------------------------------------------------------
void PdrIndexBuffer::Enable (Renderer* renderer)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;

    // Enable the buffer by setting the state.
    HRESULT hr = device->SetIndices(mBuffer);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to enable index buffer: %s\n",
        DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void PdrIndexBuffer::Disable (Renderer* renderer)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;
    HRESULT hr;
    WM5_UNUSED(hr);

#ifdef WM5_PDR_DEBUG
    // Verify that the active buffer is the one making the disable request.
    IDirect3DIndexBuffer9 *activeBuffer = 0;
    hr = device->GetIndices(&activeBuffer);
    assertion(hr == D3D_OK, "Failed to get indices: %s\n",
        DXGetErrorString(hr));
    assertion(activeBuffer == mBuffer, "Mismatched index buffers\n");
    activeBuffer->Release();
#endif

    // Disable the buffer by clearing the state.
    hr = device->SetIndices(0);
    assertion(hr == D3D_OK, "Failed to set indices: %s\n",
        DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void* PdrIndexBuffer::Lock (Buffer::Locking mode)
{
    void *videoMemory = 0;
    HRESULT hr = mBuffer->Lock(0, 0, &videoMemory, gDX9BufferLocking[mode]);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to lock index buffer: %s\n",
        DXGetErrorString(hr));
    return videoMemory;
}
//----------------------------------------------------------------------------
void PdrIndexBuffer::Unlock ()
{
    HRESULT hr = mBuffer->Unlock();
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to unlock index buffer: %s\n",
        DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
