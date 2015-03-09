// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Buffer.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Object, Buffer);
WM5_IMPLEMENT_STREAM(Buffer);
WM5_IMPLEMENT_ABSTRACT_FACTORY(Buffer);
WM5_IMPLEMENT_DEFAULT_NAMES(Object, Buffer);

//----------------------------------------------------------------------------
Buffer::Buffer ()
    :
    mNumElements(0),
    mElementSize(0),
    mUsage(Buffer::BU_QUANTITY),
    mNumBytes(0),
    mData(0)
{
}
//----------------------------------------------------------------------------
Buffer::Buffer (int numElements, int elementSize, Usage usage)
    :
    mNumElements(numElements),
    mElementSize(elementSize),
    mUsage(usage),
    mNumBytes(numElements*elementSize)
{
    assertion(mNumElements > 0, "Number of elements must be positive\n");
    assertion(mElementSize > 0, "Element size must be positive\n");

    mData = new1<char>(mNumBytes);
}
//----------------------------------------------------------------------------
Buffer::~Buffer ()
{
    delete1(mData);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
Buffer::Buffer (LoadConstructor value)
    :
    Object(value),
    mNumElements(0),
    mElementSize(0),
    mUsage(Buffer::BU_QUANTITY),
    mNumBytes(0),
    mData(0)
{
}
//----------------------------------------------------------------------------
void Buffer::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Object::Load(source);

    source.Read(mNumElements);
    source.Read(mElementSize);
    source.ReadEnum(mUsage);
    source.ReadRR(mNumBytes, mData);

    WM5_END_DEBUG_STREAM_LOAD(Buffer, source);
}
//----------------------------------------------------------------------------
void Buffer::Link (InStream& source)
{
    Object::Link(source);
}
//----------------------------------------------------------------------------
void Buffer::PostLink ()
{
    Object::PostLink();
}
//----------------------------------------------------------------------------
bool Buffer::Register (OutStream& target) const
{
    return Object::Register(target);
}
//----------------------------------------------------------------------------
void Buffer::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Object::Save(target);

    target.Write(mNumElements);
    target.Write(mElementSize);
    target.WriteEnum(mUsage);
    target.WriteW(mNumBytes, mData);

    WM5_END_DEBUG_STREAM_SAVE(Buffer, target);
}
//----------------------------------------------------------------------------
int Buffer::GetStreamingSize () const
{
    int size = Object::GetStreamingSize();
    size += sizeof(mNumElements);
    size += sizeof(mElementSize);
    size += WM5_ENUMSIZE(mUsage);
    size += sizeof(mNumBytes);
    size += mNumBytes*sizeof(mData[0]);
    return size;
}
//----------------------------------------------------------------------------
