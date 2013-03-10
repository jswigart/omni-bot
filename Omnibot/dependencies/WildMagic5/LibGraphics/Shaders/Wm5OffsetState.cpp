// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5OffsetState.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, OffsetState);
WM5_IMPLEMENT_STREAM(OffsetState);
WM5_IMPLEMENT_FACTORY(OffsetState);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, OffsetState);

//----------------------------------------------------------------------------
OffsetState::OffsetState ()
    :
    FillEnabled(false),
    LineEnabled(false),
    PointEnabled(false),
    Scale(0.0f),
    Bias(0.0f)
{
}
//----------------------------------------------------------------------------
OffsetState::~OffsetState ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
OffsetState::OffsetState (LoadConstructor value)
    :
    Object(value),
    FillEnabled(false),
    LineEnabled(false),
    PointEnabled(false),
    Scale(0.0f),
    Bias(0.0f)
{
}
//----------------------------------------------------------------------------
void OffsetState::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadBool(FillEnabled);
    source.ReadBool(LineEnabled);
    source.ReadBool(PointEnabled);
    source.Read(Scale);
    source.Read(Bias);

    WM5_END_DEBUG_STREAM_LOAD(OffsetState, source);
}
//----------------------------------------------------------------------------
void OffsetState::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void OffsetState::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool OffsetState::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void OffsetState::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WriteBool(FillEnabled);
    target.WriteBool(LineEnabled);
    target.WriteBool(PointEnabled);
    target.Write(Scale);
    target.Write(Bias);

    WM5_END_DEBUG_STREAM_SAVE(OffsetState, target);
}
//----------------------------------------------------------------------------
int OffsetState::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += WM5_BOOLSIZE(FillEnabled);
    size += WM5_BOOLSIZE(LineEnabled);
    size += WM5_BOOLSIZE(PointEnabled);
    size += sizeof(Scale);
    size += sizeof(Bias);
    return size;
}
//----------------------------------------------------------------------------
