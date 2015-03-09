// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5RenderTarget.h"
#include "Wm5Renderer.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, RenderTarget);
WM5_IMPLEMENT_STREAM(RenderTarget);
WM5_IMPLEMENT_FACTORY(RenderTarget);

//----------------------------------------------------------------------------
RenderTarget::RenderTarget (int numTargets, Texture::Format tformat,
    int width, int height, bool hasMipmaps, bool hasDepthStencil)
    :
    mNumTargets(numTargets),
    mHasMipmaps(hasMipmaps)
{
    assertion(mNumTargets > 0, "Number of targets must be at least one.\n");

    mColorTextures = new1<Texture2DPtr>(mNumTargets);
    int i;
    for (i = 0; i < mNumTargets; ++i)
    {
        mColorTextures[i] = new0 Texture2D(tformat, width, height,
            (hasMipmaps ? 0 : 1), Buffer::BU_RENDERTARGET);
    }

    if (hasDepthStencil)
    {
        mDepthStencilTexture = new0 Texture2D(Texture::TF_D24S8,
            width, height, 1, Buffer::BU_DEPTHSTENCIL);
    }
}
//----------------------------------------------------------------------------
RenderTarget::~RenderTarget ()
{
    Renderer::UnbindAll(this);

    delete1(mColorTextures);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* RenderTarget::GetObjectByName (const std::string& name)
{
    Object* found = Object::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    for (int i = 0; i < mNumTargets; ++i)
    {
        WM5_GET_OBJECT_BY_NAME(mColorTextures[i], name, found);
    }
    WM5_GET_OBJECT_BY_NAME(mDepthStencilTexture, name, found);

    return 0;
}
//----------------------------------------------------------------------------
void RenderTarget::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Object::GetAllObjectsByName(name, objects);

    for (int i = 0; i < mNumTargets; ++i)
    {
        WM5_GET_ALL_OBJECTS_BY_NAME(mColorTextures[i], name, objects);
    }
    WM5_GET_ALL_OBJECTS_BY_NAME(mDepthStencilTexture, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
RenderTarget::RenderTarget (LoadConstructor value)
    :
    Object(value),
    mNumTargets(0),
    mColorTextures(0),
    mHasMipmaps(false)
{
}
//----------------------------------------------------------------------------
void RenderTarget::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadPointerRR(mNumTargets, mColorTextures);
    source.ReadPointer(mDepthStencilTexture);
    source.ReadBool(mHasMipmaps);

    WM5_END_DEBUG_STREAM_LOAD(RenderTarget, source);
}
//----------------------------------------------------------------------------
void RenderTarget::Link (InStream& source)
{
    Object::Link(source);

    for (int i = 0; i < mNumTargets; ++i)
    {
        source.ResolveLink(mColorTextures[i]);
    }
    source.ResolveLink(mDepthStencilTexture);
}
//----------------------------------------------------------------------------
void RenderTarget::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool RenderTarget::Register (OutStream& target) const
{
    if (Object::Register(target))
    {
        for (int i = 0; i < mNumTargets; ++i)
        {
            target.Register(mColorTextures[i]);
        }
        target.Register(mDepthStencilTexture);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void RenderTarget::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WritePointerW(mNumTargets, mColorTextures);
    target.WritePointer(mDepthStencilTexture);
    target.WriteBool(mHasMipmaps);

    WM5_END_DEBUG_STREAM_SAVE(RenderTarget, target);
}
//----------------------------------------------------------------------------
int RenderTarget::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += mNumTargets*WM5_POINTERSIZE(mColorTextures[0]);
    size += WM5_POINTERSIZE(mDepthStencilTexture);
    size += WM5_BOOLSIZE(mHasMipmaps);
    return size;
}
//----------------------------------------------------------------------------
