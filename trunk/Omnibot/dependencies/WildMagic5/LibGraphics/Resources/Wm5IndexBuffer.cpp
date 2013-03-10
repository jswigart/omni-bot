// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5IndexBuffer.h"
#include "Wm5Renderer.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Buffer, IndexBuffer);
WM5_IMPLEMENT_STREAM(IndexBuffer);
WM5_IMPLEMENT_FACTORY(IndexBuffer);
WM5_IMPLEMENT_DEFAULT_NAMES(Buffer, IndexBuffer);

//----------------------------------------------------------------------------
IndexBuffer::IndexBuffer (int numIndices, int indexSize, Usage usage)
    :
    Buffer(numIndices, indexSize, usage),
    mOffset(0)
{
}
//----------------------------------------------------------------------------
IndexBuffer::~IndexBuffer ()
{
    Renderer::UnbindAll(this);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
IndexBuffer::IndexBuffer (LoadConstructor value)
    :
    Buffer(value),
    mOffset(0)
{
}
//----------------------------------------------------------------------------
void IndexBuffer::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Buffer::Load(source);

    source.Read(mOffset);

    WM5_END_DEBUG_STREAM_LOAD(IndexBuffer, source);
}
//----------------------------------------------------------------------------
void IndexBuffer::Link (InStream& source)
{
    Buffer::Link(source);
}
//----------------------------------------------------------------------------
void IndexBuffer::PostLink ()
{
    Buffer::PostLink();
}
//----------------------------------------------------------------------------
bool IndexBuffer::Register (OutStream& target) const
{
    return Buffer::Register(target);
}
//----------------------------------------------------------------------------
void IndexBuffer::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Buffer::Save(target);

    target.Write(mOffset);

    WM5_END_DEBUG_STREAM_SAVE(IndexBuffer, target);
}
//----------------------------------------------------------------------------
int IndexBuffer::GetStreamingSize () const
{
    int size = Buffer::GetStreamingSize();
    size += sizeof(mOffset);
    return size;
}
//----------------------------------------------------------------------------
