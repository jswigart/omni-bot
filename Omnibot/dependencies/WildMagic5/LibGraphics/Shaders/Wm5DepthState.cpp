// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5DepthState.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, DepthState);
WM5_IMPLEMENT_STREAM(DepthState);
WM5_IMPLEMENT_FACTORY(DepthState);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, DepthState);

//----------------------------------------------------------------------------
DepthState::DepthState ()
    :
    Enabled(true),
    Writable(true),
    Compare(CM_LEQUAL)
{
}
//----------------------------------------------------------------------------
DepthState::~DepthState ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
DepthState::DepthState (LoadConstructor value)
    :
    Object(value),
    Enabled(false),
    Writable(false),
    Compare(CM_NEVER)
{
}
//----------------------------------------------------------------------------
void DepthState::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadBool(Enabled);
    source.ReadBool(Writable);
    source.ReadEnum(Compare);

    WM5_END_DEBUG_STREAM_LOAD(DepthState, source);
}
//----------------------------------------------------------------------------
void DepthState::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void DepthState::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool DepthState::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void DepthState::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WriteBool(Enabled);
    target.WriteBool(Writable);
    target.WriteEnum(Compare);

    WM5_END_DEBUG_STREAM_SAVE(DepthState, target);
}
//----------------------------------------------------------------------------
int DepthState::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += WM5_BOOLSIZE(Enabled);
    size += WM5_BOOLSIZE(Writable);
    size += WM5_ENUMSIZE(Compare);
    return size;
}
//----------------------------------------------------------------------------
