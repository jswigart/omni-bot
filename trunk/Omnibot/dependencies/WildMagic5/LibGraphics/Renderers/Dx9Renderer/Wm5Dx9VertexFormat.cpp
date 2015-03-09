// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/02)

#include "Wm5GraphicsPCH.h"
#include "Wm5Dx9VertexFormat.h"
#include "Wm5Dx9Mapping.h"
#include "Wm5Dx9RendererData.h"
#include "Wm5Renderer.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PdrVertexFormat::PdrVertexFormat (Renderer* renderer,
    const VertexFormat* vformat)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;

    const int numAttributes = vformat->GetNumAttributes();
    for (int i = 0; i < numAttributes; ++i)
    {
        // Get the platform-independent attribute.
        unsigned int streamIndex;
        unsigned int offset;
        VertexFormat::AttributeType type;
        VertexFormat::AttributeUsage usage;
        unsigned int usageIndex;
        vformat->GetAttribute(i, streamIndex, offset, type, usage,
            usageIndex);

        // Set the DX9 attribute.
        D3DVERTEXELEMENT9 &element = mElements[i];
        element.Stream = (WORD)streamIndex;
        element.Offset = (WORD)offset;
        element.Type = gDX9AttributeType[type];
        element.Method = D3DDECLMETHOD_DEFAULT;
        element.Usage = gDX9AttributeUsage[usage];
        element.UsageIndex = (BYTE)usageIndex;
    }

    // The last DX9 element must be D3DDECL_END().
    D3DVERTEXELEMENT9 &element = mElements[numAttributes];
    element.Stream = 0xFF;
    element.Offset = 0;
    element.Type = D3DDECLTYPE_UNUSED;
    element.Method = 0;
    element.Usage = 0;
    element.UsageIndex = 0;

    HRESULT hr = device->CreateVertexDeclaration(mElements, &mDeclaration);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to create vertex declaration: %s\n",
        DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
PdrVertexFormat::~PdrVertexFormat ()
{
    mDeclaration->Release();
}
//----------------------------------------------------------------------------
void PdrVertexFormat::Enable (Renderer* renderer)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;

    // Enable the declaration by setting the state.
    HRESULT hr = device->SetVertexDeclaration(mDeclaration);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to enable vertex declaration: %s\n",
        DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void PdrVertexFormat::Disable (Renderer* renderer)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;
    WM5_UNUSED(device);
    HRESULT hr;
    WM5_UNUSED(hr);

#ifdef WM5_PDR_DEBUG
    // Verify that the active declaration is the one making the disable
    // request.
    IDirect3DVertexDeclaration9* activeDeclaration = 0;
    hr = device->GetVertexDeclaration(&activeDeclaration);
    assertion(hr == D3D_OK, "Failed to get vertex declaration: %s\n",
        DXGetErrorString(hr));
    assertion(activeDeclaration == mDeclaration,
        "Mismatched vertex declarations\n");
    activeDeclaration->Release();
#endif

#if 0
    // Disable the declaration by clearing the state.  TODO:  DirectX9
    // using debug drivers warns that the vertex declaration is set to
    // null.  For now, let's not call it.
    hr = device->SetVertexDeclaration(0);
    assertion(hr == D3D_OK, "Failed to set vertex declaration: %s\n",
        DXGetErrorString(hr));
#endif
}
//----------------------------------------------------------------------------
