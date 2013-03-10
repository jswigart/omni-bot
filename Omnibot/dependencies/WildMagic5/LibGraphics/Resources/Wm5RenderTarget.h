// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5RENDERTARGET_H
#define WM5RENDERTARGET_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Texture2D.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM RenderTarget : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(RenderTarget);

public:
    // Construction and destruction.  The number of supported targets
    // depends on the graphics hardware and drivers.  'numTargets' must
    // be at least 1.
    RenderTarget (int numTargets, Texture::Format tformat, int width,
        int height, bool hasMipmaps, bool hasDepthStencil);

    virtual ~RenderTarget ();

    // Member access.
    inline int GetNumTargets () const;
    inline Texture::Format GetFormat () const;
    inline int GetWidth () const;
    inline int GetHeight () const;
    inline Texture2D* GetColorTexture (int i) const;
    inline Texture2D* GetDepthStencilTexture () const;
    inline bool HasMipmaps () const;
    inline bool HasDepthStencil () const;

protected:
    int mNumTargets;
    Texture2DPtr* mColorTextures;
    Texture2DPtr mDepthStencilTexture;
    bool mHasMipmaps;
};

WM5_REGISTER_STREAM(RenderTarget);
typedef Pointer0<RenderTarget> RenderTargetPtr;
#include "Wm5RenderTarget.inl"

}

#endif
