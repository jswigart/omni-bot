// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/02)

#include "Wm5GraphicsPCH.h"
#include "Wm5Dx9PixelShader.h"
#include "Wm5Renderer.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PdrPixelShader::PdrPixelShader (Renderer* renderer,
    const PixelShader* pshader)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;

    // Compile the shader to assembly code.
    const char* programText =
        pshader->GetProgram(PixelShader::GetProfile())->c_str();
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
        assertion(false, "Failed to assemble pixel shader.\n");
    }
#endif
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK && compiledShader,
        "Failed to assemble pixel shader: %s\n", DXGetErrorString(hr));

    // Create the pixel shader.
    hr = device->CreatePixelShader( 
        (DWORD*)(compiledShader->GetBufferPointer()), &mShader);
    assertion(hr == D3D_OK, "Failed to create pixel shader\n");

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
PdrPixelShader::~PdrPixelShader ()
{
    mShader->Release();
}
//----------------------------------------------------------------------------
void PdrPixelShader::Enable (Renderer* renderer, const PixelShader* pshader,
    const ShaderParameters* parameters)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;

    // Enable the buffer by setting the state.
    HRESULT hr = device->SetPixelShader(mShader);
    assertion(hr == D3D_OK, "Failed to enable pixel shader: %s\n",
        DXGetErrorString(hr));

    // Set the shader constants.
    int profile = PixelShader::GetProfile();
    const int numConstants = pshader->GetNumConstants();
    int i;
    for (i = 0; i < numConstants; ++i)
    {
        hr = device->SetPixelShaderConstantF(
            pshader->GetBaseRegister(profile, i),
            parameters->GetConstant(i)->GetData(),
            pshader->GetNumRegistersUsed(i));
        assertion(hr == D3D_OK, "Failed to set shader constant: %s\n",
            DXGetErrorString(hr));
    }

    SetSamplerState(renderer, pshader, profile, parameters,
        renderer->mData->mMaxPShaderImages, 0,
        renderer->mData->mCurrentPSState);
}
//----------------------------------------------------------------------------
void PdrPixelShader::Disable (Renderer* renderer, const PixelShader* pshader,
    const ShaderParameters* parameters)
{
    IDirect3DDevice9* device = renderer->mData->mDevice;
    HRESULT hr;
    WM5_UNUSED(hr);

#ifdef WM5_PDR_DEBUG
    // Verify that the active shader is the one making the disable request.
    IDirect3DPixelShader9* activePShader = 0;
    hr = device->GetPixelShader(&activePShader);
    assertion(hr == D3D_OK, "Failed to get pixel shader: %s\n",
        DXGetErrorString(hr));
    assertion(activePShader == mShader, "Mismatched pixel shaders\n");
    activePShader->Release();
#endif

    int profile = PixelShader::GetProfile();
    DisableTextures(renderer, pshader, profile, parameters,
        renderer->mData->mMaxPShaderImages, 0);

    // Disable the shader by clearing the state.
    hr = device->SetPixelShader(0);
    assertion(hr == D3D_OK, "Failed to set pixel shader: %s\n",
        DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
