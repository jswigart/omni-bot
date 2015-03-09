// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5FloatArray.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, FloatArray);
WM5_IMPLEMENT_STREAM(FloatArray);
WM5_IMPLEMENT_FACTORY(FloatArray);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, FloatArray);

//----------------------------------------------------------------------------
FloatArray::FloatArray (int numElements, float* elements)
    :
    mNumElements(numElements),
    mElements(elements)
{
}
//----------------------------------------------------------------------------
FloatArray::~FloatArray ()
{
    delete1(mElements);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
FloatArray::FloatArray (LoadConstructor value)
    :
    Object(value),
    mNumElements(0),
    mElements(0)
{
}
//----------------------------------------------------------------------------
void FloatArray::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.ReadRR(mNumElements, mElements);

    WM5_END_DEBUG_STREAM_LOAD(FloatArray, source);
}
//----------------------------------------------------------------------------
void FloatArray::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void FloatArray::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool FloatArray::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void FloatArray::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.WriteW(mNumElements, mElements);

    WM5_END_DEBUG_STREAM_SAVE(FloatArray, target);
}
//----------------------------------------------------------------------------
int FloatArray::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += sizeof(mNumElements);
    size += mNumElements*sizeof(mElements[0]);
    return size;
}
//----------------------------------------------------------------------------
