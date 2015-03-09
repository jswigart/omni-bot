// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5VISUALPASS_H
#define WM5VISUALPASS_H

#include "Wm5GraphicsLIB.h"
#include "Wm5VertexShader.h"
#include "Wm5PixelShader.h"
#include "Wm5AlphaState.h"
#include "Wm5CullState.h"
#include "Wm5DepthState.h"
#include "Wm5OffsetState.h"
#include "Wm5StencilState.h"
#include "Wm5WireState.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM VisualPass : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(VisualPass);

public:
    // Construction and destruction.
    VisualPass ();
    virtual ~VisualPass ();

    // Support for deferred construction.
    inline void SetVertexShader (VertexShader* vshader);
    inline void SetPixelShader (PixelShader* pshader);
    inline void SetAlphaState (AlphaState* alphaState);
    inline void SetCullState (CullState* cullState);
    inline void SetDepthState (DepthState* depthState);
    inline void SetOffsetState (OffsetState* offsetState);
    inline void SetStencilState (StencilState* stencilState);
    inline void SetWireState (WireState* wireState);

    // Member access.
    inline VertexShader* GetVertexShader () const;
    inline PixelShader* GetPixelShader () const;
    inline AlphaState* GetAlphaState () const;
    inline CullState* GetCullState () const;
    inline DepthState* GetDepthState () const;
    inline OffsetState* GetOffsetState () const;
    inline StencilState* GetStencilState () const;
    inline WireState* GetWireState () const;

protected:
    VertexShaderPtr mVShader;
    PixelShaderPtr mPShader;
    AlphaStatePtr mAlphaState;
    CullStatePtr mCullState;
    DepthStatePtr mDepthState;
    OffsetStatePtr mOffsetState;
    StencilStatePtr mStencilState;
    WireStatePtr mWireState;
};

WM5_REGISTER_STREAM(VisualPass);
typedef Pointer0<VisualPass> VisualPassPtr;
#include "Wm5VisualPass.inl"

}

#endif
