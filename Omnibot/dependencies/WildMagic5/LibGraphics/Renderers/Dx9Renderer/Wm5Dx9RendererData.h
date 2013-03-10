// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5DX9RENDERERDATA_H
#define WM5DX9RENDERERDATA_H

#include "Wm5Dx9RendererLIB.h"
#include "Wm5Dx9RendererInput.h"
#include "Wm5AlphaState.h"
#include "Wm5CullState.h"
#include "Wm5DepthState.h"
#include "Wm5OffsetState.h"
#include "Wm5StencilState.h"
#include "Wm5WireState.h"
#include "Wm5Texture.h"

namespace Wm5
{

class RendererData
{
public:
    // Construction and destruction.
    RendererData (RendererInput& input, int width, int height,
        Texture::Format colorFormat, Texture::Format depthStencilFormat,
        int numMultisamples);

    ~RendererData ();

    // Support for construction, destruction, and device resetting.
    void CreateUniqueFont ();
    void DestroyUniqueFont ();
    void SetDefaultStates (const AlphaState* astate, const CullState* cstate,
        const DepthState* dstate, const OffsetState* ostate,
        const StencilState* sstate, const WireState* wstate);

    // Maintain current render states to avoid redundant state changes.
    class RenderState
    {
    public:
        RenderState ();

        void Initialize (IDirect3DDevice9* device, const AlphaState* astate,
            const CullState* cstate, const DepthState* dstate,
            const OffsetState* ostate, const StencilState* sstate,
            const WireState* wstate);

        // AlphaState
        DWORD mAlphaBlendEnable;
        DWORD mAlphaSrcBlend;
        DWORD mAlphaDstBlend;
        DWORD mAlphaTestEnable;
        DWORD mAlphaFunc;
        DWORD mAlphaRef;
        D3DCOLOR mBlendFactor;

        // CullState
        DWORD mCullMode;

        // DepthState
        DWORD mZEnable;
        DWORD mZFunc;
        DWORD mZWriteEnable;

        // OffsetState
        DWORD mSlopeScaleDepthBias;
        DWORD mDepthBias;

        // StencilState
        DWORD mStencilEnable;
        DWORD mStencilFunc;
        DWORD mStencilRef;
        DWORD mStencilMask;
        DWORD mStencilWriteMask;
        DWORD mStencilFail;
        DWORD mStencilZFail;
        DWORD mStencilPass;

        // WireState
        DWORD mFillMode;
    };

    // Maintain current sampler states to avoid redundant state changes.
    enum
    {
        MAX_NUM_VSAMPLERS = 4,  // VSModel 3 has 4, VSModel 2 has 0.
        MAX_NUM_PSAMPLERS = 16  // PSModel 2 and PSModel 3 have 16.
    };

    class SamplerState
    {
    public:
        SamplerState ();

        float mAnisotropy;
        float mLodBias;
        DWORD mMagFilter;
        DWORD mMinFilter;
        DWORD mMipFilter;
        D3DCOLOR mBorderColor;
        DWORD mWrap[3];
    };

    // Platform-dependent data.
    IDirect3DDevice9* mDevice;
    HWND mWindowHandle;
    D3DPRESENT_PARAMETERS mPresent;
    LPD3DXFONT mFont;
    bool mDeviceLost;

    // State data.
    RenderState mCurrentRS;
    SamplerState mCurrentVSState[MAX_NUM_VSAMPLERS];
    SamplerState mCurrentPSState[MAX_NUM_PSAMPLERS];

    // Capabilities (queried at run time).
    int mMaxVShaderImages;
    int mMaxPShaderImages;
    int mMaxTCoords;
};

}

#endif
