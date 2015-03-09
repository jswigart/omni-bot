// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/02)

#include "Wm5GraphicsPCH.h"
#include "Wm5DX9VertexBuffer.h"
#include "Wm5Dx9Mapping.h"
#include "Wm5Dx9RendererData.h"
#include "Wm5Renderer.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PdrVertexBuffer::PdrVertexBuffer (Renderer* renderer,
    const VertexBuffer* vbuffer)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;

    UINT numBytes = (UINT)vbuffer->GetNumBytes();
    DWORD usage = gDX9BufferUsage[vbuffer->GetUsage()];
    HRESULT hr = device->CreateVertexBuffer(numBytes, usage, 0,
        D3DPOOL_DEFAULT, &mBuffer, 0);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to create vertex buffer: %s\n",
        DXGetErrorString(hr));

    void* data = Lock(Buffer::BL_WRITE_ONLY);
    memcpy(data, vbuffer->GetData(), vbuffer->GetNumBytes());
    Unlock();
}
//----------------------------------------------------------------------------
PdrVertexBuffer::~PdrVertexBuffer ()
{
    mBuffer->Release();
}
//----------------------------------------------------------------------------
void PdrVertexBuffer::Enable (Renderer* renderer, unsigned int vertexSize,
    unsigned int streamIndex, unsigned int offset)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;

    // Enable the buffer by setting the state.
    HRESULT hr = device->SetStreamSource(streamIndex, mBuffer, offset,
        vertexSize);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to enable vertex buffer: %s\n",
        DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void PdrVertexBuffer::Disable (Renderer* renderer, unsigned int streamIndex)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;
    HRESULT hr;
    WM5_UNUSED(hr);

#ifdef WM5_PDR_DEBUG
    // Verify that the active buffer is the one making the disable request.
    IDirect3DVertexBuffer9 *activeBuffer = 0;
    unsigned int activeOffset = 0, activeStride = 0;
    hr = device->GetStreamSource(streamIndex, &activeBuffer,
        &activeOffset, &activeStride);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to get stream source: %s\n",
        DXGetErrorString(hr));
    assertion(activeBuffer == mBuffer, "Mismatched vertex buffers\n");
    activeBuffer->Release();
#endif

    // Disable the buffer by clearing the state.
    hr = device->SetStreamSource(streamIndex, 0, 0, 0);
    assertion(hr == D3D_OK, "Failed to set stream source: %s\n",
        DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void* PdrVertexBuffer::Lock (Buffer::Locking mode)
{
    void *videoMemory = 0;
    HRESULT hr = mBuffer->Lock(0, 0, &videoMemory, gDX9BufferLocking[mode]);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to lock vertex buffer: %s\n",
        DXGetErrorString(hr));
    return videoMemory;
}
//----------------------------------------------------------------------------
void PdrVertexBuffer::Unlock ()
{
    HRESULT hr = mBuffer->Unlock();
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to unlock vertex buffer: %s\n",
        DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
