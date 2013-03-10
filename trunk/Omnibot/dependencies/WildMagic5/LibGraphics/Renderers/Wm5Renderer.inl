// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int Renderer::GetWidth () const
{
    return mWidth;
}
//----------------------------------------------------------------------------
inline int Renderer::GetHeight () const
{
    return mHeight;
}
//----------------------------------------------------------------------------
inline Texture::Format Renderer::GetColorFormat () const
{
    return mColorFormat;
}
//----------------------------------------------------------------------------
inline Texture::Format Renderer::GetDepthStencilFormat () const
{
    return mDepthStencilFormat;
}
//----------------------------------------------------------------------------
inline int Renderer::GetNumMultisamples () const
{
    return mNumMultisamples;
}
//----------------------------------------------------------------------------
inline const AlphaState* Renderer::GetAlphaState () const
{
    return mAlphaState;
}
//----------------------------------------------------------------------------
inline const CullState* Renderer::GetCullState () const
{
    return mCullState;
}
//----------------------------------------------------------------------------
inline const DepthState* Renderer::GetDepthState () const
{
    return mDepthState;
}
//----------------------------------------------------------------------------
inline const OffsetState* Renderer::GetOffsetState () const
{
    return mOffsetState;
}
//----------------------------------------------------------------------------
inline const StencilState* Renderer::GetStencilState () const
{
    return mStencilState;
}
//----------------------------------------------------------------------------
inline const WireState* Renderer::GetWireState () const
{
    return mWireState;
}
//----------------------------------------------------------------------------
inline void Renderer::SetOverrideAlphaState (const AlphaState* alphaState)
{
    mOverrideAlphaState = alphaState;
    if (alphaState)
    {
        SetAlphaState(alphaState);
    }
    else
    {
        SetAlphaState(mDefaultAlphaState);
    }
}
//----------------------------------------------------------------------------
inline void Renderer::SetOverrideCullState (const CullState* cullState)
{
    mOverrideCullState = cullState;
    if (cullState)
    {
        SetCullState(cullState);
    }
    else
    {
        SetCullState(mDefaultCullState);
    }
}
//----------------------------------------------------------------------------
inline void Renderer::SetOverrideDepthState (const DepthState* depthState)
{
    mOverrideDepthState = depthState;
    if (depthState)
    {
        SetDepthState(depthState);
    }
    else
    {
        SetDepthState(mDefaultDepthState);
    }
}
//----------------------------------------------------------------------------
inline void Renderer::SetOverrideOffsetState (const OffsetState* offsetState)
{
    mOverrideOffsetState = offsetState;
    if (offsetState)
    {
        SetOffsetState(offsetState);
    }
    else
    {
        SetOffsetState(mDefaultOffsetState);
    }
}
//----------------------------------------------------------------------------
inline void Renderer::SetOverrideStencilState (const StencilState* stencilState)
{
    mOverrideStencilState = stencilState;
    if (stencilState)
    {
        SetStencilState(stencilState);
    }
    else
    {
        SetStencilState(mDefaultStencilState);
    }
}
//----------------------------------------------------------------------------
inline void Renderer::SetOverrideWireState (const WireState* wireState)
{
    mOverrideWireState = wireState;
    if (wireState)
    {
        SetWireState(wireState);
    }
    else
    {
        SetWireState(mDefaultWireState);
    }
}
//----------------------------------------------------------------------------
inline const AlphaState* Renderer::GetOverrideAlphaState () const
{
    return mOverrideAlphaState;
}
//----------------------------------------------------------------------------
inline const CullState* Renderer::GetOverrideCullState () const
{
    return mOverrideCullState;
}
//----------------------------------------------------------------------------
inline const DepthState* Renderer::GetOverrideDepthState () const
{
    return mOverrideDepthState;
}
//----------------------------------------------------------------------------
inline const OffsetState* Renderer::GetOverrideOffsetState () const
{
    return mOverrideOffsetState;
}
//----------------------------------------------------------------------------
inline const StencilState* Renderer::GetOverrideStencilState () const
{
    return mOverrideStencilState;
}
//----------------------------------------------------------------------------
inline const WireState* Renderer::GetOverrideWireState () const
{
    return mOverrideWireState;
}
//----------------------------------------------------------------------------
inline void Renderer::SetReverseCullOrder (bool reverseCullOrder)
{
    mReverseCullOrder = reverseCullOrder;
}
//----------------------------------------------------------------------------
inline bool Renderer::GetReverseCullOrder () const
{
    return mReverseCullOrder;
}
//----------------------------------------------------------------------------
inline void Renderer::SetCamera (Camera* camera)
{
    mCamera = camera;
}
//----------------------------------------------------------------------------
inline Camera* Renderer::GetCamera ()
{
    return mCamera;
}
//----------------------------------------------------------------------------
inline const Camera* Renderer::GetCamera () const
{
    return mCamera;
}
//----------------------------------------------------------------------------
inline const HMatrix& Renderer::GetViewMatrix () const
{
    return mCamera->GetViewMatrix();
}
//----------------------------------------------------------------------------
inline const HMatrix& Renderer::GetProjectionMatrix () const
{
    return mCamera->GetProjectionMatrix();
}
//----------------------------------------------------------------------------
inline const HMatrix& Renderer::GetPostProjectionMatrix () const
{
    return mCamera->GetPostProjectionMatrix();
}
//----------------------------------------------------------------------------
inline void Renderer::SetClearColor (const Float4& clearColor)
{
    mClearColor = clearColor;
}
//----------------------------------------------------------------------------
inline const Float4& Renderer::GetClearColor () const
{
    return mClearColor;
}
//----------------------------------------------------------------------------
inline void Renderer::SetClearDepth (float clearDepth)
{
    mClearDepth = clearDepth;
}
//----------------------------------------------------------------------------
inline float Renderer::GetClearDepth () const
{
    return mClearDepth;
}
//----------------------------------------------------------------------------
inline void Renderer::SetClearStencil (unsigned int clearStencil)
{
    mClearStencil = clearStencil;
}
//----------------------------------------------------------------------------
inline unsigned int Renderer::GetClearStencil () const
{
    return mClearStencil;
}
//----------------------------------------------------------------------------
inline void Renderer::GetColorMask (bool& allowRed, bool& allowGreen,
    bool& allowBlue, bool& allowAlpha) const
{
    allowRed = mAllowRed;
    allowGreen = mAllowGreen;
    allowBlue = mAllowBlue;
    allowAlpha = mAllowAlpha;
}
//----------------------------------------------------------------------------
inline bool Renderer::InTexture2DMap (const Texture2D* texture)
{
    return mTexture2Ds.find(texture) != mTexture2Ds.end();
}
//----------------------------------------------------------------------------
inline void Renderer::InsertInTexture2DMap (const Texture2D* texture,
    PdrTexture2D* platformTexture)
{
    mTexture2Ds[texture] = platformTexture;
}
//----------------------------------------------------------------------------
