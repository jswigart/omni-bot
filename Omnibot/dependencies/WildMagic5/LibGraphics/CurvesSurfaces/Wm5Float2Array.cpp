// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Float2Array.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, Float2Array);
WM5_IMPLEMENT_STREAM(Float2Array);
WM5_IMPLEMENT_FACTORY(Float2Array);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, Float2Array);

//----------------------------------------------------------------------------
Float2Array::Float2Array (int numElements, Float2* elements)
    :
    mNumElements(numElements),
    mElements(elements)
{
}
//----------------------------------------------------------------------------
Float2Array::~Float2Array ()
{
    delete1(mElements);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
Float2Array::Float2Array (LoadConstructor value)
    :
    Object(value),
    mNumElements(0),
    mElements(0)
{
}
//----------------------------------------------------------------------------
void Float2Array::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadAggregateRR(mNumElements, mElements);

    WM5_END_DEBUG_STREAM_LOAD(Float2Array, source);
}
//----------------------------------------------------------------------------
void Float2Array::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void Float2Array::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool Float2Array::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void Float2Array::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WriteAggregateW(mNumElements, mElements);

    WM5_END_DEBUG_STREAM_SAVE(Float2Array, target);
}
//----------------------------------------------------------------------------
int Float2Array::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += sizeof(mNumElements);
    size += mNumElements*sizeof(mElements[0]);
    return size;
}
//----------------------------------------------------------------------------
