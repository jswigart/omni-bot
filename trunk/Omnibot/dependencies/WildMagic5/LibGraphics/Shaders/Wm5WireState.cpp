// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5WireState.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, WireState);
WM5_IMPLEMENT_STREAM(WireState);
WM5_IMPLEMENT_FACTORY(WireState);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, WireState);

//----------------------------------------------------------------------------
WireState::WireState ()
    :
    Enabled(false)
{
}
//----------------------------------------------------------------------------
WireState::~WireState ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
WireState::WireState (LoadConstructor value)
    :
    Object(value),
    Enabled(false)
{
}
//----------------------------------------------------------------------------
void WireState::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadBool(Enabled);

    WM5_END_DEBUG_STREAM_LOAD(WireState, source);
}
//----------------------------------------------------------------------------
void WireState::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void WireState::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool WireState::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void WireState::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WriteBool(Enabled);

    WM5_END_DEBUG_STREAM_SAVE(WireState, target);
}
//----------------------------------------------------------------------------
int WireState::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += WM5_BOOLSIZE(Enabled);
    return size;
}
//----------------------------------------------------------------------------
