// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5VERTEXBUFFER_H
#define WM5VERTEXBUFFER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Buffer.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM VertexBuffer : public Buffer
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(VertexBuffer);

public:
    // Construction and destruction.
    VertexBuffer (int numVertices, int vertexSize, Usage usage = BU_STATIC);
    virtual ~VertexBuffer ();

    // Access the buffer data.
    inline char* GetData () const;
};

WM5_REGISTER_STREAM(VertexBuffer);
typedef Pointer0<VertexBuffer> VertexBufferPtr;
#include "Wm5VertexBuffer.inl"

}

#endif
