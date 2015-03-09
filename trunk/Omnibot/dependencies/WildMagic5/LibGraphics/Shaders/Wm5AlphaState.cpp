// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5AlphaState.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, AlphaState);
WM5_IMPLEMENT_STREAM(AlphaState);
WM5_IMPLEMENT_FACTORY(AlphaState);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, AlphaState);

//----------------------------------------------------------------------------
AlphaState::AlphaState ()
    :
    BlendEnabled(false),
    SrcBlend(SBM_SRC_ALPHA),
    DstBlend(DBM_ONE_MINUS_SRC_ALPHA),
    CompareEnabled(false),
    Compare(CM_ALWAYS),
    Reference(0.0f),
    ConstantColor(0.0f, 0.0f, 0.0f, 0.0f)
{
}
//----------------------------------------------------------------------------
AlphaState::~AlphaState ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
AlphaState::AlphaState (LoadConstructor value)
    :
    Object(value),
    BlendEnabled(false),
    SrcBlend(SBM_ZERO),
    DstBlend(DBM_ZERO),
    CompareEnabled(false),
    Compare(CM_NEVER),
    Reference(0.0f),
    ConstantColor(0.0f, 0.0f, 0.0f, 0.0f)
{
}
//----------------------------------------------------------------------------
void AlphaState::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadBool(BlendEnabled);
    source.ReadEnum(SrcBlend);
    source.ReadEnum(DstBlend);
    source.ReadBool(CompareEnabled);
    source.ReadEnum(Compare);
    source.Read(Reference);
    source.ReadAggregate(ConstantColor);

    WM5_END_DEBUG_STREAM_LOAD(AlphaState, source);
}
//----------------------------------------------------------------------------
void AlphaState::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void AlphaState::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool AlphaState::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void AlphaState::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WriteBool(BlendEnabled);
    target.WriteEnum(SrcBlend);
    target.WriteEnum(DstBlend);
    target.WriteBool(CompareEnabled);
    target.WriteEnum(Compare);
    target.Write(Reference);
    target.WriteAggregate(ConstantColor);

    WM5_END_DEBUG_STREAM_SAVE(AlphaState, target);
}
//----------------------------------------------------------------------------
int AlphaState::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += WM5_BOOLSIZE(BlendEnabled);
    size += WM5_ENUMSIZE(SrcBlend);
    size += WM5_ENUMSIZE(DstBlend);
    size += WM5_BOOLSIZE(CompareEnabled);
    size += WM5_ENUMSIZE(Compare);
    size += sizeof(Reference);
    size += sizeof(ConstantColor);
    return size;
}
//----------------------------------------------------------------------------
