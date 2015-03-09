// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5VisualPass.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, VisualPass);
WM5_IMPLEMENT_STREAM(VisualPass);
WM5_IMPLEMENT_FACTORY(VisualPass);

//----------------------------------------------------------------------------
VisualPass::VisualPass ()
{
}
//----------------------------------------------------------------------------
VisualPass::~VisualPass ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* VisualPass::GetObjectByName (const std::string& name)
{
    Object* found = Object::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mVShader, name, found);
    WM5_GET_OBJECT_BY_NAME(mPShader, name, found);
    WM5_GET_OBJECT_BY_NAME(mAlphaState, name, found);
    WM5_GET_OBJECT_BY_NAME(mCullState, name, found);
    WM5_GET_OBJECT_BY_NAME(mDepthState, name, found);
    WM5_GET_OBJECT_BY_NAME(mOffsetState, name, found);
    WM5_GET_OBJECT_BY_NAME(mStencilState, name, found);
    WM5_GET_OBJECT_BY_NAME(mWireState, name, found);

    return 0;
}
//----------------------------------------------------------------------------
void VisualPass::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Object::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mVShader, name, objects);
    WM5_GET_ALL_OBJECTS_BY_NAME(mPShader, name, objects);
    WM5_GET_ALL_OBJECTS_BY_NAME(mAlphaState, name, objects);
    WM5_GET_ALL_OBJECTS_BY_NAME(mCullState, name, objects);
    WM5_GET_ALL_OBJECTS_BY_NAME(mDepthState, name, objects);
    WM5_GET_ALL_OBJECTS_BY_NAME(mOffsetState, name, objects);
    WM5_GET_ALL_OBJECTS_BY_NAME(mStencilState, name, objects);
    WM5_GET_ALL_OBJECTS_BY_NAME(mWireState, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
VisualPass::VisualPass (LoadConstructor value)
    :
    Object(value)
{
}
//----------------------------------------------------------------------------
void VisualPass::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadPointer(mVShader);
    source.ReadPointer(mPShader);
    source.ReadPointer(mAlphaState);
    source.ReadPointer(mCullState);
    source.ReadPointer(mDepthState);
    source.ReadPointer(mOffsetState);
    source.ReadPointer(mStencilState);
    source.ReadPointer(mWireState);

    WM5_END_DEBUG_STREAM_LOAD(VisualPass, source);
}
//----------------------------------------------------------------------------
void VisualPass::Link (InStream& source)
{
    Object::Link(source);

    source.ResolveLink(mVShader);
    source.ResolveLink(mPShader);
    source.ResolveLink(mAlphaState);
    source.ResolveLink(mCullState);
    source.ResolveLink(mDepthState);
    source.ResolveLink(mOffsetState);
    source.ResolveLink(mStencilState);
    source.ResolveLink(mWireState);
}
//----------------------------------------------------------------------------
void VisualPass::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool VisualPass::Register (OutStream& target) const
{
    if (Object::Register(target))
    {
        target.Register(mVShader);
        target.Register(mPShader);
        target.Register(mAlphaState);
        target.Register(mCullState);
        target.Register(mDepthState);
        target.Register(mOffsetState);
        target.Register(mStencilState);
        target.Register(mWireState);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void VisualPass::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WritePointer(mVShader);
    target.WritePointer(mPShader);
    target.WritePointer(mAlphaState);
    target.WritePointer(mCullState);
    target.WritePointer(mDepthState);
    target.WritePointer(mOffsetState);
    target.WritePointer(mStencilState);
    target.WritePointer(mWireState);

    WM5_END_DEBUG_STREAM_SAVE(VisualPass, target);
}
//----------------------------------------------------------------------------
int VisualPass::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += WM5_POINTERSIZE(mVShader);
    size += WM5_POINTERSIZE(mPShader);
    size += WM5_POINTERSIZE(mAlphaState);
    size += WM5_POINTERSIZE(mCullState);
    size += WM5_POINTERSIZE(mDepthState);
    size += WM5_POINTERSIZE(mOffsetState);
    size += WM5_POINTERSIZE(mStencilState);
    size += WM5_POINTERSIZE(mWireState);
    return size;
}
//----------------------------------------------------------------------------
