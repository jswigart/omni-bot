// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5DX9VERTEXBUFFER_H
#define WM5DX9VERTEXBUFFER_H

#include "Wm5Dx9RendererLIB.h"
#include "Wm5VertexBuffer.h"

namespace Wm5
{

class Renderer;

class PdrVertexBuffer
{
public:
    // Construction and destruction.
    PdrVertexBuffer (Renderer* renderer, const VertexBuffer* vbuffer);
    ~PdrVertexBuffer ();

    // Vertex buffer operations.
    void Enable (Renderer* renderer, unsigned int vertexSize,
        unsigned int streamIndex, unsigned int offset);
    void Disable (Renderer* renderer, unsigned int streamIndex);
    void* Lock (Buffer::Locking mode);
    void Unlock ();

private:
    IDirect3DVertexBuffer9* mBuffer;
};

}

#endif
