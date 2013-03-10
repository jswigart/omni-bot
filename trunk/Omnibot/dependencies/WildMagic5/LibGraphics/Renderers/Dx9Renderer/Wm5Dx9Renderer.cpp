// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.4 (2011/06/27)

#include "Wm5GraphicsPCH.h"
#include "Wm5Renderer.h"
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
#include "Wm5Dx9Mapping.h"
#include "Wm5Dx9RendererData.h"
#include "Wm5Dx9RendererInput.h"
#include "Wm5Polypoint.h"
#include "Wm5Polysegment.h"
#include "Wm5TriFan.h"
#include "Wm5TriStrip.h"
using namespace Wm5;

//----------------------------------------------------------------------------
Renderer::Renderer (RendererInput& input, int width, int height,
    Texture::Format colorFormat, Texture::Format depthStencilFormat,
    int numMultisamples)
{
    Initialize(width, height, colorFormat, depthStencilFormat,
        numMultisamples);

    mData = new0 RendererData(input, width, height, colorFormat,
        depthStencilFormat, numMultisamples);

    // Set the default render states and sampler states.
    mData->SetDefaultStates(mDefaultAlphaState, mDefaultCullState,
        mDefaultDepthState, mDefaultOffsetState, mDefaultStencilState,
        mDefaultWireState);
}
//----------------------------------------------------------------------------
Renderer::~Renderer ()
{
    delete0(mData);

    Terminate();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Visual state management.
//----------------------------------------------------------------------------
void Renderer::SetAlphaState (const AlphaState* alphaState)
{
    if (!mOverrideAlphaState)
    {
        mAlphaState = alphaState;
    }
    else
    {
        mAlphaState = mOverrideAlphaState;
    }

    HRESULT hr;
    if (mAlphaState->BlendEnabled)
    {
        if (TRUE != mData->mCurrentRS.mAlphaBlendEnable)
        {
            mData->mCurrentRS.mAlphaBlendEnable = TRUE;
            hr = mData->mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }

        DWORD srcBlend = gDX9AlphaSrcBlend[mAlphaState->SrcBlend];
        DWORD srcConstantColor = 0;
        if (srcBlend > 0)
        {
            srcConstantColor = 0;
        }
        else  // CONSTANT format
        {
            switch (mAlphaState->SrcBlend)
            {
            case AlphaState::SBM_CONSTANT_COLOR:
                srcBlend = D3DBLEND_BLENDFACTOR;
                srcConstantColor = D3DCOLOR_COLORVALUE(
                    mAlphaState->ConstantColor[0],
                    mAlphaState->ConstantColor[1],
                    mAlphaState->ConstantColor[2],
                    mAlphaState->ConstantColor[3]);
                break;
            case AlphaState::SBM_ONE_MINUS_CONSTANT_COLOR:
                srcBlend = D3DBLEND_INVBLENDFACTOR;
                srcConstantColor = D3DCOLOR_COLORVALUE(
                    mAlphaState->ConstantColor[0],
                    mAlphaState->ConstantColor[1],
                    mAlphaState->ConstantColor[2],
                    mAlphaState->ConstantColor[3]);
                break;
            case AlphaState::SBM_CONSTANT_ALPHA:
                srcBlend = D3DBLEND_BLENDFACTOR;
                srcConstantColor = D3DCOLOR_COLORVALUE(
                    mAlphaState->ConstantColor[3],
                    mAlphaState->ConstantColor[3],
                    mAlphaState->ConstantColor[3],
                    mAlphaState->ConstantColor[3]);
                break;
            case AlphaState::SBM_ONE_MINUS_CONSTANT_ALPHA:
                srcBlend = D3DBLEND_INVBLENDFACTOR;
                srcConstantColor = D3DCOLOR_COLORVALUE(
                    mAlphaState->ConstantColor[3],
                    mAlphaState->ConstantColor[3],
                    mAlphaState->ConstantColor[3],
                    mAlphaState->ConstantColor[3]);
                break;
            }
        }

        DWORD dstBlend = gDX9AlphaDstBlend[mAlphaState->DstBlend];
        DWORD dstConstantColor = 0;
        if (dstBlend > 0)
        {
            dstConstantColor = 0;
        }
        else  // CONSTANT format
        {
            switch (mAlphaState->DstBlend)
            {
            case AlphaState::DBM_CONSTANT_COLOR:
                dstBlend = D3DBLEND_BLENDFACTOR;
                dstConstantColor = D3DCOLOR_COLORVALUE(
                    mAlphaState->ConstantColor[0],
                    mAlphaState->ConstantColor[1],
                    mAlphaState->ConstantColor[2],
                    mAlphaState->ConstantColor[3]);
                break;
            case AlphaState::DBM_ONE_MINUS_CONSTANT_COLOR:
                dstBlend = D3DBLEND_INVBLENDFACTOR;
                dstConstantColor = D3DCOLOR_COLORVALUE(
                    mAlphaState->ConstantColor[0],
                    mAlphaState->ConstantColor[1],
                    mAlphaState->ConstantColor[2],
                    mAlphaState->ConstantColor[3]);
                break;
            case AlphaState::DBM_CONSTANT_ALPHA:
                dstBlend = D3DBLEND_BLENDFACTOR;
                dstConstantColor = D3DCOLOR_COLORVALUE(
                    mAlphaState->ConstantColor[3],
                    mAlphaState->ConstantColor[3],
                    mAlphaState->ConstantColor[3],
                    mAlphaState->ConstantColor[3]);
                break;
            case AlphaState::DBM_ONE_MINUS_CONSTANT_ALPHA:
                dstBlend = D3DBLEND_INVBLENDFACTOR;
                dstConstantColor = D3DCOLOR_COLORVALUE(
                    mAlphaState->ConstantColor[3],
                    mAlphaState->ConstantColor[3],
                    mAlphaState->ConstantColor[3],
                    mAlphaState->ConstantColor[3]);
                break;
            }
        }

        // DirectX appears not to support the OpenGL modes GL_CONSTANT_ALPHA
        // and GL_ONE_MINUS_CONSTANT_ALPHA.  In these cases, the constant
        // color is being set explicitly to the alpha values.  If the source
        // uses constant color (constant alpha) and the destination uses
        // constant alpha (constant color), then the blending results will be
        // incorrect.
#ifdef _DEBUG
        if (srcBlend == D3DBLEND_BLENDFACTOR
        ||  srcBlend == D3DBLEND_INVBLENDFACTOR)
        {
            if (dstBlend == D3DBLEND_BLENDFACTOR
            ||  dstBlend == D3DBLEND_INVBLENDFACTOR)
            {
                assertion(srcConstantColor == dstConstantColor,
                    "Src and dst colors should be the same\n");
            }
        }
#endif

        if (srcBlend != mData->mCurrentRS.mAlphaSrcBlend)
        {
            mData->mCurrentRS.mAlphaSrcBlend = srcBlend;
            hr = mData->mDevice->SetRenderState(D3DRS_SRCBLEND, srcBlend);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }

        if (dstBlend != mData->mCurrentRS.mAlphaDstBlend)
        {
            mData->mCurrentRS.mAlphaDstBlend = dstBlend;
            hr = mData->mDevice->SetRenderState(D3DRS_DESTBLEND, dstBlend);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }

        if (srcConstantColor != mData->mCurrentRS.mBlendFactor)
        {
            mData->mCurrentRS.mBlendFactor = srcConstantColor;
            hr = mData->mDevice->SetRenderState(D3DRS_BLENDFACTOR,
                srcConstantColor);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }
    }
    else
    {
        if (FALSE != mData->mCurrentRS.mAlphaBlendEnable)
        {
            mData->mCurrentRS.mAlphaBlendEnable = FALSE;
            hr = mData->mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,
                FALSE);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }
    }

    if (mAlphaState->CompareEnabled)
    {
        if (TRUE != mData->mCurrentRS.mAlphaTestEnable)
        {
            mData->mCurrentRS.mAlphaTestEnable = TRUE;
            hr = mData->mDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }

        DWORD alphaFunc = gDX9AlphaCompare[mAlphaState->Compare];
        if (alphaFunc != mData->mCurrentRS.mAlphaFunc)
        {
            mData->mCurrentRS.mAlphaFunc = alphaFunc;
            hr = mData->mDevice->SetRenderState(D3DRS_ALPHAFUNC, alphaFunc);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }

        DWORD alphaRef = (DWORD)(255.0f*mAlphaState->Reference);
        if (alphaRef != mData->mCurrentRS.mAlphaRef)
        {
            mData->mCurrentRS.mAlphaRef = alphaRef;
            hr = mData->mDevice->SetRenderState(D3DRS_ALPHAREF, alphaRef);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }
    }
    else
    {
        if (FALSE != mData->mCurrentRS.mAlphaTestEnable)
        {
            mData->mCurrentRS.mAlphaTestEnable = FALSE;
            hr = mData->mDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::SetCullState (const CullState* cullState)
{
    if (!mOverrideCullState)
    {
        mCullState = cullState;
    }
    else
    {
        mCullState = mOverrideCullState;
    }

    HRESULT hr;
    if (mCullState->Enabled)
    {
        int index = (mCullState->CCWOrder ? 1 : 0);
        if (mReverseCullOrder)
        {
            index = 1 - index;
        }

        DWORD cullMode = gDX9CullOrder[mCullState->CCWOrder ? 1 : 0];
        if (cullMode != mData->mCurrentRS.mCullMode)
        {
            mData->mCurrentRS.mCullMode = cullMode;
            hr = mData->mDevice->SetRenderState(D3DRS_CULLMODE, cullMode);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }
    }
    else
    {
        if (D3DCULL_NONE != mData->mCurrentRS.mCullMode)
        {
            mData->mCurrentRS.mCullMode = D3DCULL_NONE;
            hr = mData->mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::SetDepthState (const DepthState* depthState)
{
    if (!mOverrideDepthState)
    {
        mDepthState = depthState;
    }
    else
    {
        mDepthState = mOverrideDepthState;
    }

    HRESULT hr;
    if (mDepthState->Enabled)
    {
        if (D3DZB_TRUE != mData->mCurrentRS.mZEnable)
        {
            mData->mCurrentRS.mZEnable = D3DZB_TRUE;
            hr = mData->mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }

        DWORD zFunc = gDX9DepthCompare[mDepthState->Compare];
        if (zFunc != mData->mCurrentRS.mZFunc)
        {
            mData->mCurrentRS.mZFunc = zFunc;
            hr = mData->mDevice->SetRenderState(D3DRS_ZFUNC, zFunc);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }
    }
    else
    {
        if (D3DZB_FALSE != mData->mCurrentRS.mZEnable)
        {
            mData->mCurrentRS.mZEnable = D3DZB_FALSE;
            hr = mData->mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }
    }

    if (mDepthState->Writable)
    {
        if (TRUE != mData->mCurrentRS.mZWriteEnable)
        {
            mData->mCurrentRS.mZWriteEnable = TRUE;
            hr = mData->mDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }
    }
    else
    {
        if (FALSE != mData->mCurrentRS.mZWriteEnable)
        {
            mData->mCurrentRS.mZWriteEnable = FALSE;
            hr = mData->mDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::SetOffsetState (const OffsetState* offsetState)
{
    if (!mOverrideOffsetState)
    {
        mOffsetState = offsetState;
    }
    else
    {
        mOffsetState = mOverrideOffsetState;
    }

    // The LineEnabled and PointEnabled members are ignored by the DX9
    // renderer since DX9 does not support polygon offset for those
    // primitives.

    HRESULT hr;
    if (mOffsetState->FillEnabled)
    {
        DWORD slopeScaleDepthBias = *(DWORD*)&mOffsetState->Scale;
        if (slopeScaleDepthBias != mData->mCurrentRS.mSlopeScaleDepthBias)
        {
            mData->mCurrentRS.mSlopeScaleDepthBias = slopeScaleDepthBias;
            hr = mData->mDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,
                slopeScaleDepthBias);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }

        // TODO.  The renderer currently always creates a 24-bit depth
        // buffer.  If the precision changes, the adjustment to depth bias
        // must depend on the bits of precision.  The divisor is 2^n for n
        // bits of precision.
        float bias = mOffsetState->Bias/16777216.0f;
        DWORD depthBias = *(DWORD*)&bias;
        if (depthBias != mData->mCurrentRS.mDepthBias)
        {
            mData->mCurrentRS.mDepthBias = depthBias;
            hr = mData->mDevice->SetRenderState(D3DRS_DEPTHBIAS, depthBias);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }
    }
    else
    {
        if (0 != mData->mCurrentRS.mSlopeScaleDepthBias)
        {
            mData->mCurrentRS.mSlopeScaleDepthBias = 0;
            hr = mData->mDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }

        if (0 != mData->mCurrentRS.mDepthBias)
        {
            mData->mCurrentRS.mDepthBias = 0;
            hr = mData->mDevice->SetRenderState(D3DRS_DEPTHBIAS, 0);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::SetStencilState (const StencilState* stencilState)
{
    if (!mOverrideStencilState)
    {
        mStencilState = stencilState;
    }
    else
    {
        mStencilState = mOverrideStencilState;
    }

    HRESULT hr;
    if (mStencilState->Enabled)
    {
        if (TRUE != mData->mCurrentRS.mStencilEnable)
        {
            mData->mCurrentRS.mStencilEnable = TRUE;
            hr = mData->mDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }

        DWORD stencilFunc = gDX9StencilCompare[mStencilState->Compare];
        if (stencilFunc != mData->mCurrentRS.mStencilFunc)
        {
            mData->mCurrentRS.mStencilFunc = stencilFunc;
            hr = mData->mDevice->SetRenderState(D3DRS_STENCILFUNC,
                stencilFunc);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }

        DWORD stencilRef = (DWORD)mStencilState->Reference;
        if (stencilRef != mData->mCurrentRS.mStencilRef)
        {
            mData->mCurrentRS.mStencilRef = stencilRef;
            hr = mData->mDevice->SetRenderState(D3DRS_STENCILREF, stencilRef);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }

        DWORD stencilMask = (DWORD)mStencilState->Mask;
        if (stencilMask != mData->mCurrentRS.mStencilMask)
        {
            mData->mCurrentRS.mStencilMask = stencilMask;
            hr = mData->mDevice->SetRenderState(D3DRS_STENCILMASK,
                stencilMask);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }

        DWORD stencilWriteMask = (DWORD)mStencilState->WriteMask;
        if (stencilWriteMask != mData->mCurrentRS.mStencilWriteMask)
        {
            mData->mCurrentRS.mStencilWriteMask = stencilWriteMask;
            hr = mData->mDevice->SetRenderState(D3DRS_STENCILWRITEMASK,
                stencilWriteMask);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }

        DWORD stencilFail = gDX9StencilOperation[mStencilState->OnFail];
        if (stencilFail != mData->mCurrentRS.mStencilFail)
        {
            mData->mCurrentRS.mStencilFail = stencilFail;
            hr = mData->mDevice->SetRenderState(D3DRS_STENCILFAIL,
                stencilFail);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }

        DWORD stencilZFail = gDX9StencilOperation[mStencilState->OnZFail];
        if (stencilZFail != mData->mCurrentRS.mStencilZFail)
        {
            mData->mCurrentRS.mStencilZFail = stencilZFail;
            hr = mData->mDevice->SetRenderState(D3DRS_STENCILZFAIL,
                stencilZFail);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }

        DWORD stencilPass = gDX9StencilOperation[mStencilState->OnZPass];
        if (stencilPass != mData->mCurrentRS.mStencilPass)
        {
            mData->mCurrentRS.mStencilPass = stencilPass;
            hr = mData->mDevice->SetRenderState(D3DRS_STENCILPASS,
                stencilPass);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }
    }
    else
    {
        if (FALSE != mData->mCurrentRS.mStencilEnable)
        {
            mData->mCurrentRS.mStencilEnable = FALSE;
            hr = mData->mDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }
    }
}
//----------------------------------------------------------------------------
void Renderer::SetWireState (const WireState* wireState)
{
    if (!mOverrideWireState)
    {
        mWireState = wireState;
    }
    else
    {
        mWireState = mOverrideWireState;
    }

    HRESULT hr;
    if (mWireState->Enabled)
    {
        if (D3DFILL_WIREFRAME != mData->mCurrentRS.mFillMode)
        {
            mData->mCurrentRS.mFillMode = D3DFILL_WIREFRAME;
            hr = mData->mDevice->SetRenderState(D3DRS_FILLMODE,
                D3DFILL_WIREFRAME);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }
    }
    else
    {
        if (D3DFILL_SOLID != mData->mCurrentRS.mFillMode)
        {
            mData->mCurrentRS.mFillMode = D3DFILL_SOLID;
            hr = mData->mDevice->SetRenderState(D3DRS_FILLMODE,
                D3DFILL_SOLID);
            assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
                DXGetErrorString(hr));
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Viewport management.
//----------------------------------------------------------------------------
void Renderer::SetViewport (int xPosition, int yPosition, int width,
    int height)
{
    D3DVIEWPORT9 viewport;
    HRESULT hr = mData->mDevice->GetViewport(&viewport);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to get old viewport: %s\n",
        DXGetErrorString(hr));

    // DirectX viewports are in left-handed screen coordinates.  The origin
    // is the upper-left corner, the y-axis points downward, and the x-axis
    // points upward.  We need a transformation in y to meet the Wild Magic
    // convention of a right-handed viewport with origin in the lower-left
    // corner.
    viewport.X = (DWORD)xPosition;
    viewport.Y = (DWORD)(mHeight - height - yPosition);
    viewport.Width = (DWORD)width;
    viewport.Height = (DWORD)height;
    hr = mData->mDevice->SetViewport(&viewport);
    assertion(hr == D3D_OK, "Failed to set new viewport: %s\n",
        DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void Renderer::GetViewport (int& xPosition, int& yPosition,
    int& width, int& height) const
{
    D3DVIEWPORT9 viewport;
    HRESULT hr = mData->mDevice->GetViewport(&viewport);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to get viewport: %s\n",
        DXGetErrorString(hr));

    // DirectX viewports are in left-handed screen coordinates.  The origin
    // is the upper-left corner, the y-axis points downward, and the x-axis
    // points upward.  We need a transformation in y to meet the Wild Magic
    // convention of a right-handed viewport with origin in the lower-left
    // corner.
    xPosition = (int)viewport.X;
    yPosition = mHeight - (int)viewport.Height - (int)viewport.Y;
    width = (int)viewport.Width;
    height = (int)viewport.Height;
}
//----------------------------------------------------------------------------
void Renderer::SetDepthRange (float zMin, float zMax)
{
    D3DVIEWPORT9 viewport;
    HRESULT hr = mData->mDevice->GetViewport(&viewport);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to get old viewport: %s\n",
        DXGetErrorString(hr));

    viewport.MinZ = zMin;
    viewport.MaxZ = zMax;
    hr = mData->mDevice->SetViewport(&viewport);
    assertion(hr == D3D_OK, "Failed to set new viewport: %s\n",
        DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void Renderer::GetDepthRange (float& zMin, float& zMax) const
{
    D3DVIEWPORT9 viewport;
    HRESULT hr = mData->mDevice->GetViewport(&viewport);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to get viewport: %s\n",
        DXGetErrorString(hr));

    zMin = viewport.MinZ;
    zMax = viewport.MaxZ;
}
//----------------------------------------------------------------------------
void Renderer::Resize (int width, int height)
{
    mWidth = width;
    mHeight = height;

    D3DVIEWPORT9 viewport;
    HRESULT hr = mData->mDevice->GetViewport(&viewport);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Failed to get old viewport: %s\n",
        DXGetErrorString(hr));

    viewport.Width = (DWORD)width;
    viewport.Height = (DWORD)height;
    hr = mData->mDevice->SetViewport(&viewport);
    assertion(hr == D3D_OK, "Failed to set new viewport: %s\n",
        DXGetErrorString(hr));
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Support for clearing the color, depth, and stencil buffers.
//----------------------------------------------------------------------------
void Renderer::ClearColorBuffer ()
{
    DWORD clearColor = D3DCOLOR_COLORVALUE(mClearColor[0], mClearColor[1],
        mClearColor[2], mClearColor[3]);

    HRESULT hr = mData->mDevice->Clear(0, 0, D3DCLEAR_TARGET, clearColor,
        1.0f, 0);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Clear failed: %s\n", DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void Renderer::ClearDepthBuffer ()
{
    HRESULT hr = mData->mDevice->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, mClearDepth,
        0);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Clear failed: %s\n", DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void Renderer::ClearStencilBuffer ()
{
    HRESULT hr = mData->mDevice->Clear(0, 0, D3DCLEAR_STENCIL, 0, 1.0f,
        (DWORD)mClearStencil);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Clear failed: %s\n", DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void Renderer::ClearBuffers ()
{
    DWORD clearColor = D3DCOLOR_COLORVALUE(mClearColor[0], mClearColor[1],
        mClearColor[2], mClearColor[3]);

    HRESULT hr = mData->mDevice->Clear(0, 0,
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
        clearColor, mClearDepth, (DWORD)mClearStencil);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Clear failed: %s\n", DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void Renderer::ClearColorBuffer (int x, int y, int w, int h)
{
    D3DRECT rect;
    rect.x1 = (long)x;
    rect.y1 = (long)y;
    rect.x2 = (long)(x + w - 1);
    rect.y2 = (long)(y + h - 1);

    DWORD clearColor = D3DCOLOR_COLORVALUE(mClearColor[0], mClearColor[1],
        mClearColor[2], mClearColor[3]);

    HRESULT hr = mData->mDevice->Clear(1, &rect, D3DCLEAR_TARGET, clearColor,
        1.0f, 0);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Clear failed: %s\n", DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void Renderer::ClearDepthBuffer (int x, int y, int w, int h)
{
    D3DRECT rect;
    rect.x1 = (long)x;
    rect.y1 = (long)y;
    rect.x2 = (long)(x + w - 1);
    rect.y2 = (long)(y + h - 1);

    HRESULT hr = mData->mDevice->Clear(1, &rect, D3DCLEAR_ZBUFFER, 0,
        mClearDepth, 0);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Clear failed: %s\n", DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void Renderer::ClearStencilBuffer (int x, int y, int w, int h)
{
    D3DRECT rect;
    rect.x1 = (long)x;
    rect.y1 = (long)y;
    rect.x2 = (long)(x + w - 1);
    rect.y2 = (long)(y + h - 1);

    HRESULT hr = mData->mDevice->Clear(1, &rect, D3DCLEAR_STENCIL, 0, 1.0f,
        (DWORD)mClearStencil);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Clear failed: %s\n", DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void Renderer::ClearBuffers (int x, int y, int w, int h)
{
    D3DRECT rect;
    rect.x1 = (long)x;
    rect.y1 = (long)y;
    rect.x2 = (long)(x + w - 1);
    rect.y2 = (long)(y + h - 1);

    DWORD clearColor = D3DCOLOR_COLORVALUE(mClearColor[0], mClearColor[1],
        mClearColor[2], mClearColor[3]);

    HRESULT hr = mData->mDevice->Clear(1, &rect,
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
        clearColor, mClearDepth, (DWORD)mClearStencil);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Clear failed: %s\n", DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void Renderer::DisplayColorBuffer ()
{
    HRESULT hr = mData->mDevice->Present(0, 0, 0, 0);
    if (hr != D3DERR_DEVICELOST)
    {
        assertion(hr == D3D_OK, "Present failed: %s\n",
            DXGetErrorString(hr));
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Support for masking color channels.
//----------------------------------------------------------------------------
void Renderer::SetColorMask (bool allowRed, bool allowGreen,
    bool allowBlue, bool allowAlpha)
{
    mAllowRed = allowRed;
    mAllowGreen = allowGreen;
    mAllowBlue = allowBlue;
    mAllowAlpha = allowAlpha;

    DWORD mask = 0;

    if (mAllowRed)
    {
        mask |= D3DCOLORWRITEENABLE_RED;
    }

    if (mAllowGreen)
    {
        mask |= D3DCOLORWRITEENABLE_GREEN;
    }

    if (mAllowBlue)
    {
        mask |= D3DCOLORWRITEENABLE_BLUE;
    }

    if (mAllowAlpha)
    {
        mask |= D3DCOLORWRITEENABLE_ALPHA;
    }

    HRESULT hr = mData->mDevice->SetRenderState(D3DRS_COLORWRITEENABLE, mask);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Support for predraw and postdraw semantics.
//----------------------------------------------------------------------------
template <typename Resource, typename PdrResource>
void DestroyResources (
    std::map<const Resource*, PdrResource*>& rmap,
    std::set<const Resource*>& save)
{
    std::map<const Resource*, PdrResource*>::iterator iter = rmap.begin();
    std::map<const Resource*, PdrResource*>::iterator end = rmap.end();
    for (/**/; iter != end; ++iter)
    {
        delete0(iter->second);
        save.insert(iter->first);
    }
    rmap.clear();
}
//----------------------------------------------------------------------------
template <typename Resource>
void RecreateResources (Renderer* renderer,
    const std::set<const Resource*>& save)
{
    std::set<const Resource*>::const_iterator iter = save.begin();
    std::set<const Resource*>::const_iterator end = save.end();
    for (/**/; iter != end; ++iter)
    {
        renderer->Bind(*iter);
    }
}
//----------------------------------------------------------------------------
bool Renderer::PreDraw ()
{
    HRESULT hr = mData->mDevice->TestCooperativeLevel();
    if (hr == D3DERR_DEVICELOST)
    {
        if (!mData->mDeviceLost)
        {
            // This is the first time we noticed that the device was lost.
            mData->mDeviceLost = true;
            mData->mFont->OnLostDevice();
        }
        return false;
    }
    else if (hr == D3DERR_DEVICENOTRESET)
    {
        // Recreate all D3DPOOL_DEFAULT resources.  Shaders do not have to be
        // recreated in DX9.  DX9 textures are managed, so they do not have to
        // be recreated.  However, let's just recreate everything.  Glad to
        // see the useless device_lost/reset mechanism disappears on DX10 and
        // later.

        // Destroy the resources.
        std::set<const VertexFormat*> saveVertexFormats;
        std::set<const VertexBuffer*> saveVertexBuffers;
        std::set<const IndexBuffer*> saveIndexBuffers;
        std::set<const Texture1D*> saveTexture1Ds;
        std::set<const Texture2D*> saveTexture2Ds;
        std::set<const Texture3D*> saveTexture3Ds;
        std::set<const TextureCube*> saveTextureCubes;
        std::set<const RenderTarget*> saveRenderTargets;
        std::set<const VertexShader*> saveVertexShaders;
        std::set<const PixelShader*> savePixelShaders;

        DestroyResources(mVertexFormats, saveVertexFormats);
        DestroyResources(mVertexBuffers, saveVertexBuffers);
        DestroyResources(mIndexBuffers, saveIndexBuffers);
        DestroyResources(mTexture1Ds, saveTexture1Ds);
        DestroyResources(mTexture2Ds, saveTexture2Ds);
        DestroyResources(mTexture3Ds, saveTexture3Ds);
        DestroyResources(mTextureCubes, saveTextureCubes);
        DestroyResources(mRenderTargets, saveRenderTargets);
        DestroyResources(mVertexShaders, saveVertexShaders);
        DestroyResources(mPixelShaders, savePixelShaders);
        mData->DestroyUniqueFont();

        // Reset the device.
        hr = mData->mDevice->Reset(&mData->mPresent);
        if (hr != D3D_OK)
        {
            assertion(false, "Reset failed: %s\n", DXGetErrorString(hr));
            return false;
        }

        mData->SetDefaultStates(mDefaultAlphaState, mDefaultCullState,
            mDefaultDepthState, mDefaultOffsetState, mDefaultStencilState,
            mDefaultWireState);

        // Recreate the resources.
        mData->CreateUniqueFont();
        RecreateResources(this, saveVertexFormats);
        RecreateResources(this, saveVertexBuffers);
        RecreateResources(this, saveIndexBuffers);
        RecreateResources(this, saveTexture1Ds);
        RecreateResources(this, saveTexture2Ds);
        RecreateResources(this, saveTexture3Ds);
        RecreateResources(this, saveTextureCubes);
        RecreateResources(this, saveRenderTargets);
        RecreateResources(this, saveVertexShaders);
        RecreateResources(this, savePixelShaders);
    }

    // TODO.  Go-to-fullscreen needs to be supported here.
    hr = mData->mDevice->BeginScene();
    if (hr == D3D_OK)
    {
        return true;
    }

    assertion(false, "BeginScene failed: %s\n", DXGetErrorString(hr));
    return false;
}
//----------------------------------------------------------------------------
void Renderer::PostDraw ()
{
    HRESULT hr = mData->mDevice->EndScene();
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "EndScene failed: %s\n", DXGetErrorString(hr));
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Drawing routines.
//----------------------------------------------------------------------------
void Renderer::Draw (const unsigned char* screenBuffer, bool reflectY)
{
    if (!screenBuffer)
    {
        assertion(false, "Incoming screen buffer is null.\n");
        return;
    }

    IDirect3DSurface9* backBuffer = 0;
    HRESULT hr = mData->mDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO,
        &backBuffer);
    if (hr != D3D_OK || !backBuffer)
    {
        assertion(false, "GetBackBuffer failed: %s\n",
            DXGetErrorString(hr));
        return;
    }

    // TODO:  The backbuffer for now is D3DFMT_A8R8G8B8, so the 4*width is the
    // correct choice.  However, we need a gDX9TextureFormatSize[] array to
    // use here, and we need to infer the "UINT srcPitch" value from it.
    IDirect3DSurface9* surface = 0;
    hr = mData->mDevice->CreateOffscreenPlainSurface(mWidth, mHeight,
        D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surface, 0);
    if (hr != D3D_OK)
    {
        assertion(false, "Failed to create surface: %s\n",
            DXGetErrorString(hr));
        backBuffer->Release();
        return;
    }

    D3DLOCKED_RECT lockedRect;
    hr = surface->LockRect(&lockedRect, 0, D3DLOCK_DISCARD);
    if (hr != D3D_OK || !lockedRect.pBits)
    {
        assertion(false, "Failed to lock surface: %s\n",
            DXGetErrorString(hr));
        surface->Release();
        backBuffer->Release();
        return;
    }

    if (!reflectY)
    {
        memcpy(lockedRect.pBits, screenBuffer, 4*mWidth*mHeight);
    }
    else
    {
        size_t numRowBytes = 4*mWidth;  // 4 = sizeof(A8R8G8B8)
        const unsigned char* src = screenBuffer;
        unsigned char* trg = (unsigned char*)lockedRect.pBits +
            numRowBytes*(mHeight - 1);
        for (int i = 0; i < mHeight; ++i)
        {
            memcpy(trg, src, numRowBytes);
            src += numRowBytes;
            trg -= numRowBytes;
        }
    }

    hr = surface->UnlockRect();
    if (hr != D3D_OK)
    {
        assertion(false, "Failed to unlock surface: %s\n",
            DXGetErrorString(hr));
        surface->Release();
        backBuffer->Release();
        return;
    }

    hr = mData->mDevice->UpdateSurface(surface, 0, backBuffer, 0);
    if (hr != D3D_OK)
    {
        assertion(false, "Failed to update backbuffer: %s\n",
            DXGetErrorString(hr));
        surface->Release();
        backBuffer->Release();
        return;
    }

    surface->Release();
    backBuffer->Release();
}
//----------------------------------------------------------------------------
void Renderer::Draw (int x, int y, const Float4& color,
    const std::string& message)
{
    D3DCOLOR dx9color = D3DCOLOR_COLORVALUE(color[0], color[1], color[2],
        color[3]);

    // Compute the bounding rectangle of the text.
    RECT textRect;
    textRect.bottom = y;
    textRect.top = y;
    textRect.left = x;
    textRect.right = x;

    // Set default render states, except for depth buffering that must be
    // disabled because text is always overlayed.
    SetAlphaState(mDefaultAlphaState);
    SetCullState(mDefaultCullState);
    SetOffsetState(mDefaultOffsetState);
    SetStencilState(mDefaultStencilState);
    SetWireState(mDefaultWireState);

    mData->mCurrentRS.mZEnable = D3DZB_FALSE;
    HRESULT hr = mData->mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));

    mData->mFont->DrawText(
        0,                                  // pSprite
        message.c_str(),                    // pString
        -1,                                 // Count
        &textRect,                          // pRect
        DT_LEFT | DT_BOTTOM | DT_CALCRECT,  // Format
        dx9color);                          // Color

    // Draw the text.
    mData->mFont->DrawText(
        0,
        message.c_str(),
        -1,
        &textRect,
        DT_LEFT | DT_BOTTOM,
        dx9color);

#ifdef WM5_RESET_STATE_AFTER_DRAW
    // Restore visual state.  Only depth buffering state varied from the
    // default state.
    mData->mCurrentRS.mZEnable = D3DZB_TRUE;
    hr = mData->mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
#endif
}
//----------------------------------------------------------------------------
#ifdef WM5_QUERY_PIXEL_COUNT
static IDirect3DQuery9* BeginQuery (IDirect3DDevice9* device)
{
    IDirect3DQuery9* query = 0;
    HRESULT hr = device->CreateQuery(D3DQUERYTYPE_OCCLUSION, &query);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "CreateQuery failed: %s\n",
        DXGetErrorString(hr));
    hr = query->Issue(D3DISSUE_BEGIN);
    assertion(hr == D3D_OK, "Issue (begin) failed: %s\n",
        DXGetErrorString(hr));
    return query;
}

static DWORD EndQuery (IDirect3DQuery9* query)
{
    HRESULT hr = query->Issue(D3DISSUE_END);
    WM5_UNUSED(hr);
    assertion(hr == D3D_OK, "Issue (end) failed: %s\n",
        DXGetErrorString(hr));
    DWORD numPixelsDrawn = 0;
    while (S_FALSE == query->GetData(&numPixelsDrawn, sizeof(DWORD),
        D3DGETDATA_FLUSH))
    {
        // Wait until query finishes.
    }
    hr = query->Release();
    assertion(hr == D3D_OK, "Release failed: %s\n",
        DXGetErrorString(hr));
    return numPixelsDrawn;
}

#define WM5_BEGIN_QUERY(device, query) \
    IDirect3DQuery9* query = BeginQuery(device)

#define WM5_END_QUERY(query, numPixelsDrawn) \
    numPixelsDrawn = EndQuery(query)
#else
#define WM5_BEGIN_QUERY(device, query)
#define WM5_END_QUERY(query, numPixelsDrawn)
#endif
//----------------------------------------------------------------------------
void Renderer::DrawPrimitive (const Visual* visual)
{
    Visual::PrimitiveType type = visual->GetPrimitiveType();
    const VertexBuffer* vbuffer = visual->GetVertexBuffer();
    const IndexBuffer* ibuffer = visual->GetIndexBuffer();

    HRESULT hr;
    WM5_UNUSED(hr);
    int numPrimitives;
    DWORD numPixelsDrawn;
    WM5_UNUSED(numPixelsDrawn);

    if (type == Visual::PT_TRIMESH
    ||  type == Visual::PT_TRISTRIP
    ||  type == Visual::PT_TRIFAN)
    {
        int numVertices = vbuffer->GetNumElements();
        numPrimitives = StaticCast<Triangles>(visual)->GetNumTriangles();
        if (numVertices > 0 && numPrimitives > 0)
        {
            WM5_BEGIN_QUERY(mData->mDevice, query);

            int indicesOffset = ibuffer->GetOffset();
            hr = mData->mDevice->DrawIndexedPrimitive(gDX9PrimitiveType[type],
                0, 0, numVertices, indicesOffset, numPrimitives);
            assertion(hr == D3D_OK, "DrawIndexedPrimitive failed: %s\n",
                DXGetErrorString(hr));

            WM5_END_QUERY(query, numPixelsDrawn);
        }
    }
    else if (type == Visual::PT_POLYSEGMENTS_CONTIGUOUS)
    {
        numPrimitives = StaticCast<Polysegment>(visual)->GetNumSegments();
        if (numPrimitives > 0)
        {
            WM5_BEGIN_QUERY(mData->mDevice, query);

            hr = mData->mDevice->DrawPrimitive(D3DPT_LINESTRIP, 0,
                numPrimitives);
            assertion(hr == D3D_OK, "DrawPrimitive failed: %s\n",
                DXGetErrorString(hr));

            WM5_END_QUERY(query, numPixelsDrawn);
        }
    }
    else if (type == Visual::PT_POLYSEGMENTS_DISJOINT)
    {
        numPrimitives = StaticCast<Polysegment>(visual)->GetNumSegments();
        if (numPrimitives > 0)
        {
            WM5_BEGIN_QUERY(mData->mDevice, query);

            hr = mData->mDevice->DrawPrimitive(D3DPT_LINELIST, 0,
                numPrimitives);
            assertion(hr == D3D_OK, "DrawPrimitive failed: %s\n",
                DXGetErrorString(hr));

            WM5_END_QUERY(query, numPixelsDrawn);
        }
    }
    else if (type == Visual::PT_POLYPOINT)
    {
        numPrimitives = StaticCast<Polypoint>(visual)->GetNumPoints();
        if (numPrimitives > 0)
        {
            WM5_BEGIN_QUERY(mData->mDevice, query);

            hr = mData->mDevice->DrawPrimitive(D3DPT_POINTLIST, 0,
                numPrimitives);
            assertion(hr == D3D_OK, "DrawPrimitive failed: %s\n",
                DXGetErrorString(hr));

            WM5_END_QUERY(query, numPixelsDrawn);
        }
    }
    else
    {
        assertion(false, "Invalid type.\n");
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// RendererData
//----------------------------------------------------------------------------
RendererData::RendererData (RendererInput& input, int width, int height,
    Texture::Format colorFormat, Texture::Format depthStencilFormat,
    int numMultisamples)
    :
    mWindowHandle(input.mWindowHandle),
    mDeviceLost(false)
{
    D3DFORMAT dxColorFormat = gDX9TextureFormat[colorFormat];
    D3DFORMAT dxDepthStencilFormat = gDX9TextureFormat[depthStencilFormat];

    mPresent.BackBufferWidth = width;
    mPresent.BackBufferHeight = height;
    mPresent.BackBufferFormat = dxColorFormat;
    mPresent.BackBufferCount = 1;
    mPresent.hDeviceWindow = mWindowHandle;
    mPresent.Windowed = TRUE;
    mPresent.Flags = 0;
    mPresent.FullScreen_RefreshRateInHz = 0;
    mPresent.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Request depth and stencil buffers.
    mPresent.EnableAutoDepthStencil = TRUE;
    mPresent.AutoDepthStencilFormat = dxDepthStencilFormat;

    // Request multisampling.
    mPresent.SwapEffect = D3DSWAPEFFECT_FLIP; 
    mPresent.MultiSampleType = D3DMULTISAMPLE_NONE;
    mPresent.MultiSampleQuality = 0;
    HRESULT hr;
    if (numMultisamples == 2)
    {
        hr = input.mDriver->CheckDeviceMultiSampleType(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            dxColorFormat,
            FALSE,
            D3DMULTISAMPLE_2_SAMPLES,
            NULL);

        assertion(hr == D3D_OK, "Two multisamples not supported: %s\n",
            DXGetErrorString(hr));

        if (hr == D3D_OK)
        {
            mPresent.SwapEffect = D3DSWAPEFFECT_DISCARD; 
            mPresent.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
        }
    }
    else if (numMultisamples == 4)
    {
        hr = input.mDriver->CheckDeviceMultiSampleType(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            dxColorFormat,
            FALSE,
            D3DMULTISAMPLE_4_SAMPLES,
            NULL);

        assertion(hr == D3D_OK, "Two multisamples not supported: %s\n",
            DXGetErrorString(hr));

        if (hr == D3D_OK)
        {
            mPresent.SwapEffect = D3DSWAPEFFECT_DISCARD; 
            mPresent.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
        }
    }

    hr = input.mDriver->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        mWindowHandle,
        D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,
        &mPresent,
        &mDevice);

    assertion(hr == D3D_OK, "Failed to create device: %s\n",
        DXGetErrorString(hr));

    CreateUniqueFont();

    // Query the device for its capabilities.
    D3DCAPS9 deviceCaps;
    hr = input.mDriver->GetDeviceCaps(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        &deviceCaps);

    assertion(hr == D3D_OK, "GetDeviceCaps failed: %s\n",
        DXGetErrorString(hr));

    DWORD vsModel = deviceCaps.VertexShaderVersion;
    if (vsModel < D3DVS_VERSION(1,1))
    {
        assertion(false, "Vertex Shader Model must be at least 1.1.\n");
        vsModel = D3DVS_VERSION(1,1);
    }
    if (vsModel > D3DVS_VERSION(3,0))
    {
        assertion(false, "Vertex Shader Model must be at most 3.0.\n");
        vsModel = D3DVS_VERSION(3,0);
    }
    if (vsModel < D3DVS_VERSION(2,0))
    {
        VertexShader::SetProfile(VertexShader::VP_VS_1_1);
    }
    else if (vsModel < D3DVS_VERSION(3,0))
    {
        VertexShader::SetProfile(VertexShader::VP_VS_2_0);
    }
    else
    {
        VertexShader::SetProfile(VertexShader::VP_VS_3_0);
    }

    DWORD psModel = deviceCaps.PixelShaderVersion;
    if (psModel < D3DPS_VERSION(1,1))
    {
        assertion(false, "Pixel Shader Model must be at least 1.1.\n");
        psModel = D3DPS_VERSION(1,1);
    }
    if (psModel > D3DPS_VERSION(3,0))
    {
        assertion(false, "Pixel Shader Model must be at most 3.0.\n");
        psModel = D3DPS_VERSION(3,0);
    }
    if (psModel < D3DPS_VERSION(2,0))
    {
        PixelShader::SetProfile(PixelShader::PP_PS_1_1);
    }
    else if (psModel < D3DPS_VERSION(3,0))
    {
        PixelShader::SetProfile(PixelShader::PP_PS_2_0);
    }
    else
    {
        PixelShader::SetProfile(PixelShader::PP_PS_3_0);
    }

    mMaxVShaderImages = (vsModel < D3DVS_VERSION(3,0) ? 0 : 4);
    mMaxPShaderImages = 16;
    mMaxTCoords = 8;
}
//----------------------------------------------------------------------------
RendererData::~RendererData ()
{
    DestroyUniqueFont();
    mDevice->Release();
}
//----------------------------------------------------------------------------
void RendererData::CreateUniqueFont ()
{
    // TODO:  For now, let's create a single font.  Later, let more fonts and
    // switching among them be possible.  OR use a bitmapped font system
    // such as the OpenGL one that will work with any back-end renderer.
    HRESULT hr = D3DXCreateFont(
        mDevice,              // pDevice
        18,                   // Height
        0,                    // Width
        FW_REGULAR,           // Weight
        0,                    // MipLevels
        0,                    // Italic
        DEFAULT_CHARSET,      // CharSet
        OUT_DEFAULT_PRECIS,   // OutputPrecision
        ANTIALIASED_QUALITY,  // Quality
        VARIABLE_PITCH,       // PitchAndFamily
        "Arial",              // pFaceName
        &mFont);              // ppFont

    WM5_UNUSED(hr);

    assertion(hr == D3D_OK, "D3DXCreateFont failed: %s\n",
        DXGetErrorString(hr));
}
//----------------------------------------------------------------------------
void RendererData::DestroyUniqueFont ()
{
    mFont->Release();
}
//----------------------------------------------------------------------------
void RendererData::SetDefaultStates (const AlphaState* astate,
    const CullState* cstate, const DepthState* dstate,
    const OffsetState* ostate, const StencilState* sstate,
    const WireState* wstate)
{
    // Set the default render states.
    mCurrentRS.Initialize(mDevice, astate, cstate, dstate, ostate, sstate,
        wstate);

    // Set the default vertex sampler states.
    HRESULT hr;
    for (DWORD i = 0; i < MAX_NUM_VSAMPLERS; ++i)
    {
        mCurrentVSState[i] = SamplerState();
        SamplerState& state = mCurrentVSState[i];

        hr = mDevice->SetSamplerState(i, D3DSAMP_MAXANISOTROPY,
            (DWORD)state.mAnisotropy);
        assertion(hr == D3D_OK, "Failed to set anisotropy: %s\n",
            DXGetErrorString(hr));

        hr = mDevice->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS,
            *(DWORD*)&state.mLodBias);
        assertion(hr == D3D_OK, "Failed to set LOD bias: %s\n",
            DXGetErrorString(hr));

        hr = mDevice->SetSamplerState(i, D3DSAMP_MAGFILTER,
            state.mMagFilter);
        assertion(hr == D3D_OK, "Failed to set magfilter: %s\n",
            DXGetErrorString(hr));

        hr = mDevice->SetSamplerState(i, D3DSAMP_MINFILTER,
            state.mMinFilter);
        assertion(hr == D3D_OK, "Failed to set minfilter: %s\n",
            DXGetErrorString(hr));

        hr = mDevice->SetSamplerState(i, D3DSAMP_MIPFILTER,
            state.mMipFilter);
        assertion(hr == D3D_OK, "Failed to set mipfilter: %s\n",
            DXGetErrorString(hr));

        hr = mDevice->SetSamplerState(i, D3DSAMP_BORDERCOLOR,
            state.mBorderColor);
        assertion(hr == D3D_OK, "Failed to set border color: %s\n",
            DXGetErrorString(hr));

        hr = mDevice->SetSamplerState(i, D3DSAMP_ADDRESSU,
            state.mWrap[0]);
        assertion(hr == D3D_OK, "Failed to set wrap0: %s\n",
            DXGetErrorString(hr));

        hr = mDevice->SetSamplerState(i, D3DSAMP_ADDRESSV,
            state.mWrap[1]);
        assertion(hr == D3D_OK, "Failed to set wrap1: %s\n",
            DXGetErrorString(hr));

        hr = mDevice->SetSamplerState(i, D3DSAMP_ADDRESSW,
            state.mWrap[2]);
        assertion(hr == D3D_OK, "Failed to set wrap2: %s\n",
            DXGetErrorString(hr));
    }

    // Set the default pixel sampler states.
    for (DWORD i = 0; i < MAX_NUM_PSAMPLERS; ++i)
    {
        mCurrentPSState[i] = SamplerState();
        SamplerState& state = mCurrentPSState[i];

        hr = mDevice->SetSamplerState(i, D3DSAMP_MAXANISOTROPY,
            (DWORD)state.mAnisotropy);
        assertion(hr == D3D_OK, "Failed to set anisotropy: %s\n",
            DXGetErrorString(hr));

        hr = mDevice->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS,
            *(DWORD*)&state.mLodBias);
        assertion(hr == D3D_OK, "Failed to set LOD bias: %s\n",
            DXGetErrorString(hr));

        hr = mDevice->SetSamplerState(i, D3DSAMP_MAGFILTER,
            state.mMagFilter);
        assertion(hr == D3D_OK, "Failed to set magfilter: %s\n",
            DXGetErrorString(hr));

        hr = mDevice->SetSamplerState(i, D3DSAMP_MINFILTER,
            state.mMinFilter);
        assertion(hr == D3D_OK, "Failed to set minfilter: %s\n",
            DXGetErrorString(hr));

        hr = mDevice->SetSamplerState(i, D3DSAMP_MIPFILTER,
            state.mMipFilter);
        assertion(hr == D3D_OK, "Failed to set mipfilter: %s\n",
            DXGetErrorString(hr));

        hr = mDevice->SetSamplerState(i, D3DSAMP_BORDERCOLOR,
            state.mBorderColor);
        assertion(hr == D3D_OK, "Failed to set border color: %s\n",
            DXGetErrorString(hr));

        hr = mDevice->SetSamplerState(i, D3DSAMP_ADDRESSU,
            state.mWrap[0]);
        assertion(hr == D3D_OK, "Failed to set wrap0: %s\n",
            DXGetErrorString(hr));

        hr = mDevice->SetSamplerState(i, D3DSAMP_ADDRESSV,
            state.mWrap[1]);
        assertion(hr == D3D_OK, "Failed to set wrap1: %s\n",
            DXGetErrorString(hr));

        hr = mDevice->SetSamplerState(i, D3DSAMP_ADDRESSW,
            state.mWrap[2]);
        assertion(hr == D3D_OK, "Failed to set wrap2: %s\n",
            DXGetErrorString(hr));
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// RendererData::RenderState (Render state information to avoid redundant
// state changes.)
//----------------------------------------------------------------------------
RendererData::RenderState::RenderState ()
{
    // Initialization must be deferred until a DirectX9 device has been
    // created.
}
//----------------------------------------------------------------------------
void RendererData::RenderState::Initialize (IDirect3DDevice9* device,
    const AlphaState* astate, const CullState* cstate,
    const DepthState* dstate, const OffsetState* ostate,
    const StencilState* sstate, const WireState* wstate)
{
    HRESULT hr;
    WM5_UNUSED(hr);

    // AlphaState
    mAlphaBlendEnable = astate->BlendEnabled ? TRUE : FALSE;
    mAlphaSrcBlend = gDX9AlphaSrcBlend[astate->SrcBlend];
    mAlphaDstBlend = gDX9AlphaDstBlend[astate->DstBlend];
    mAlphaTestEnable = astate->CompareEnabled ? TRUE : FALSE;
    mAlphaFunc = gDX9AlphaCompare[astate->Compare];
    mAlphaRef = (DWORD)(255.0f*astate->Reference);
    mBlendFactor = D3DCOLOR_COLORVALUE(
        astate->ConstantColor[0],
        astate->ConstantColor[1],
        astate->ConstantColor[2],
        astate->ConstantColor[3]);

    hr = device->SetRenderState(D3DRS_ALPHABLENDENABLE, mAlphaBlendEnable);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
    hr = device->SetRenderState(D3DRS_SRCBLEND, mAlphaSrcBlend);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
    hr = device->SetRenderState(D3DRS_DESTBLEND, mAlphaDstBlend);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
    hr = device->SetRenderState(D3DRS_ALPHATESTENABLE, mAlphaTestEnable);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
    hr = device->SetRenderState(D3DRS_ALPHAFUNC, mAlphaFunc);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
    hr = device->SetRenderState(D3DRS_ALPHAREF, mAlphaRef);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
    hr = device->SetRenderState(D3DRS_BLENDFACTOR, mBlendFactor);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));

    // CullState
    mCullMode = cstate->Enabled ? gDX9CullOrder[cstate->CCWOrder ? 1 : 0] :
        D3DCULL_NONE;

    hr = device->SetRenderState(D3DRS_CULLMODE, mCullMode);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));

    // DepthState
    mZEnable = dstate->Enabled ? D3DZB_TRUE : D3DZB_FALSE;
    mZFunc = gDX9DepthCompare[dstate->Compare];
    mZWriteEnable = dstate->Writable ? TRUE : FALSE;

    hr = device->SetRenderState(D3DRS_ZENABLE, mZEnable);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
    hr = device->SetRenderState(D3DRS_ZFUNC, mZFunc);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
    hr = device->SetRenderState(D3DRS_ZWRITEENABLE, mZWriteEnable);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));

    // OffsetState
    if (ostate->FillEnabled)
    {
        mSlopeScaleDepthBias = *(DWORD*)&ostate->Scale;
        float bias = ostate->Bias/16777216.0f;
        mDepthBias = *(DWORD*)&bias;
    }
    else
    {
        mSlopeScaleDepthBias = 0;
        mDepthBias = 0;
    }

    hr = device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,
        mSlopeScaleDepthBias);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
    hr = device->SetRenderState(D3DRS_DEPTHBIAS, mDepthBias);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));

    // StencilState
    mStencilEnable = sstate->Enabled ? TRUE : FALSE;
    mStencilFunc = gDX9StencilCompare[sstate->Compare];
    mStencilRef = (DWORD)sstate->Reference;
    mStencilMask = (DWORD)sstate->Mask;
    mStencilWriteMask = (DWORD)sstate->WriteMask;
    mStencilFail = gDX9StencilOperation[sstate->OnFail];
    mStencilZFail = gDX9StencilOperation[sstate->OnZFail];
    mStencilPass = gDX9StencilOperation[sstate->OnZPass];

    hr = device->SetRenderState(D3DRS_STENCILENABLE, mStencilEnable);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
    hr = device->SetRenderState(D3DRS_STENCILFUNC, mStencilFunc);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
    hr = device->SetRenderState(D3DRS_STENCILREF, mStencilRef);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
    hr = device->SetRenderState(D3DRS_STENCILMASK, mStencilMask);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
    hr = device->SetRenderState(D3DRS_STENCILWRITEMASK, mStencilWriteMask);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
    hr = device->SetRenderState(D3DRS_STENCILFAIL, mStencilFail);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
    hr = device->SetRenderState(D3DRS_STENCILZFAIL, mStencilZFail);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
    hr = device->SetRenderState(D3DRS_STENCILPASS, mStencilPass);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));

    // WireState
    mFillMode = wstate->Enabled ? D3DFILL_WIREFRAME : D3DFILL_SOLID;

    hr = device->SetRenderState(D3DRS_FILLMODE, mFillMode);
    assertion(hr == D3D_OK, "SetRenderState failed: %s\n",
        DXGetErrorString(hr));
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// RendererData::SamplerState (Sampler state information to avoid redundant
// state changes.)
//----------------------------------------------------------------------------
RendererData::SamplerState::SamplerState ()
{
    mAnisotropy = 1.0f;
    mLodBias = 0.0f;
    mMagFilter = D3DTEXF_POINT;
    mMinFilter = D3DTEXF_POINT;
    mMipFilter = D3DTEXF_NONE;
    mBorderColor = 0;
    mWrap[0] = D3DTADDRESS_WRAP;
    mWrap[1] = D3DTADDRESS_WRAP;
    mWrap[2] = D3DTADDRESS_WRAP;
}
//----------------------------------------------------------------------------
