// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5CullState.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, CullState);
WM5_IMPLEMENT_STREAM(CullState);
WM5_IMPLEMENT_FACTORY(CullState);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, CullState);

//----------------------------------------------------------------------------
CullState::CullState ()
    :
    Enabled(true),
    CCWOrder(true)
{
}
//----------------------------------------------------------------------------
CullState::~CullState ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
CullState::CullState (LoadConstructor value)
    :
    Object(value),
    Enabled(false),
    CCWOrder(false)
{
}
//----------------------------------------------------------------------------
void CullState::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadBool(Enabled);
    source.ReadBool(CCWOrder);

    WM5_END_DEBUG_STREAM_LOAD(CullState, source);
}
//----------------------------------------------------------------------------
void CullState::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void CullState::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool CullState::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void CullState::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WriteBool(Enabled);
    target.WriteBool(CCWOrder);

    WM5_END_DEBUG_STREAM_SAVE(CullState, target);
}
//----------------------------------------------------------------------------
int CullState::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += WM5_BOOLSIZE(Enabled);
    size += WM5_BOOLSIZE(CCWOrder);
    return size;
}
//----------------------------------------------------------------------------
