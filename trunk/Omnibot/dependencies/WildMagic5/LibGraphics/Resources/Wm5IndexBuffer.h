// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INDEXBUFFER_H
#define WM5INDEXBUFFER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Buffer.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM IndexBuffer : public Buffer
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(IndexBuffer);

public:
    // Construction and destruction.
    IndexBuffer (int numIndices, int indexSize, Usage usage = BU_STATIC);
    virtual ~IndexBuffer ();

    // Access the buffer data.
    inline char* GetData () const;

    // The offset into the indices is used by the renderer for drawing.  The
    // ability to set this is useful when multiple geometric primitives share
    // an index buffer, each primitive using a continguous set of indices.  In
    // this case, SetNumElements and SetOffset will be called dynamically by
    // the application for each such geometric primitive.
    inline void SetOffset (int offset);
    inline int GetOffset () const;

protected:
    int mOffset;
};

WM5_REGISTER_STREAM(IndexBuffer);
typedef Pointer0<IndexBuffer> IndexBufferPtr;
#include "Wm5IndexBuffer.inl"

}

#endif
