// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/02)

#include "Wm5GraphicsPCH.h"
#include "Wm5Dx9VertexShader.h"
#include "Wm5Renderer.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PdrVertexShader::PdrVertexShader (Renderer* renderer,
    const VertexShader* vshader)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;

    // Compile the shader to assembly code.
    const char* programText =
        vshader->GetProgram(VertexShader::GetProfile())->c_str();
    int programLength = (int)strlen(programText);
    LPD3DXBUFFER compiledShader = 0;
    LPD3DXBUFFER errors = 0;
    HRESULT hr = D3DXAssembleShader(programText, programLength, 0, 0, 0,
        &compiledShader, &errors);
#ifdef _DEBUG
    if (errors)
    {
        DWORD size = errors->GetBufferSize();
        WM5_UNUSED(size);
        char* data = (char*)errors->GetBufferPointer();
        WM5_UNUSED(data);
        assertion(false, "Failed to assemble vertex shader.\n");
    }
#endif
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK && compiledShader,
        "Failed to assemble vertex shader: %s\n", DXGetErrorString(hr));

    // Create the vertex shader.
    hr = device->CreateVertexShader(
        (DWORD*)(compiledShader->GetBufferPointer()), &mShader);
    assertion(hr == D3D_OK, "Failed to create vertex shader\n");

    // Release buffers, if necessary.
    if (compiledShader)
    {
        compiledShader->Release();
    }
    if (errors)
    {
        errors->Release();
    }
}
//----------------------------------------------------------------------------
PdrVertexShader::~PdrVertexShader ()
{
    mShader->Release();
}
//----------------------------------------------------------------------------
void PdrVertexShader::Enable (Renderer* renderer,
    const VertexShader* vshader, const ShaderParameters* parameters)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;

    // Enable the buffer by setting the state.
    HRESULT hr = device->SetVertexShader(mShader);
    assertion(hr == D3D_OK, "Failed to enable vertex shader: %s\n",
        DXGetErrorString(hr));

    // Set the shader constants.
    int profile = VertexShader::GetProfile();
    const int numConstants = vshader->GetNumConstants();
    int i;
    for (i = 0; i < numConstants; ++i)
    {
        hr = device->SetVertexShaderConstantF(
            vshader->GetBaseRegister(profile, i),
            parameters->GetConstant(i)->GetData(),
            vshader->GetNumRegistersUsed(i));
        assertion(hr == D3D_OK, "Failed to set shader constant: %s\n",
            DXGetErrorString(hr));
    }

    SetSamplerState(renderer, vshader, profile, parameters,
        renderer->mData->mMaxVShaderImages, D3DVERTEXTEXTURESAMPLER0,
        renderer->mData->mCurrentVSState);
}
//----------------------------------------------------------------------------
void PdrVertexShader::Disable (Renderer* renderer,
    const VertexShader* vshader, const ShaderParameters* parameters)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;
    HRESULT hr;
    WM5_UNUSED(hr);

#ifdef WM5_PDR_DEBUG
    // Verify that the active shader is the one making the disable request.
    IDirect3DVertexShader9* activeVShader = 0;
    hr = device->GetVertexShader(&activeVShader);
    assertion(hr == D3D_OK, "Failed to get vertex shader: %s\n",
        DXGetErrorString(hr));
    assertion(activeVShader == mShader, "Mismatched vertex shaders\n");
    activeVShader->Release();
#endif

    int profile = PixelShader::GetProfile();
    DisableTextures(renderer, vshader, profile, parameters,
        renderer->mData->mMaxVShaderImages, D3DVERTEXTEXTURESAMPLER0);

    // Disable the shader by clearing the state.
    hr = device->SetVertexShader(0);
    assertion(hr == D3D_OK, "Failed to set vertex shader: %s\n",
        DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
