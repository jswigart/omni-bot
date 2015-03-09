// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5StencilState.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, StencilState);
WM5_IMPLEMENT_STREAM(StencilState);
WM5_IMPLEMENT_FACTORY(StencilState);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, StencilState);

//----------------------------------------------------------------------------
StencilState::StencilState ()
    :
    Enabled(false),
    Compare(CM_NEVER),
    Reference(0),
    Mask(UINT_MAX),
    WriteMask(UINT_MAX),
    OnFail(OT_KEEP),
    OnZFail(OT_KEEP),
    OnZPass(OT_KEEP)
{
}
//----------------------------------------------------------------------------
StencilState::~StencilState ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
StencilState::StencilState (LoadConstructor value)
    :
    Object(value),
    Enabled(false),
    Compare(CM_NEVER),
    Reference(0),
    Mask(0),
    WriteMask(0),
    OnFail(OT_KEEP),
    OnZFail(OT_KEEP),
    OnZPass(OT_KEEP)
{
}
//----------------------------------------------------------------------------
void StencilState::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadBool(Enabled);
    source.ReadEnum(Compare);
    source.Read(Reference);
    source.Read(Mask);
    source.Read(WriteMask);
    source.ReadEnum(OnFail);
    source.ReadEnum(OnZFail);
    source.ReadEnum(OnZPass);

    WM5_END_DEBUG_STREAM_LOAD(StencilState, source);
}
//----------------------------------------------------------------------------
void StencilState::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void StencilState::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool StencilState::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void StencilState::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WriteBool(Enabled);
    target.WriteEnum(Compare);
    target.Write(Reference);
    target.Write(Mask);
    target.Write(WriteMask);
    target.WriteEnum(OnFail);
    target.WriteEnum(OnZFail);
    target.WriteEnum(OnZPass);

    WM5_END_DEBUG_STREAM_SAVE(StencilState, target);
}
//----------------------------------------------------------------------------
int StencilState::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += WM5_BOOLSIZE(Enabled);
    size += WM5_ENUMSIZE(Compare);
    size += sizeof(Reference);
    size += sizeof(Mask);
    size += sizeof(WriteMask);
    size += WM5_ENUMSIZE(OnFail);
    size += WM5_ENUMSIZE(OnZFail);
    size += WM5_ENUMSIZE(OnZPass);
    return size;
}
//----------------------------------------------------------------------------
