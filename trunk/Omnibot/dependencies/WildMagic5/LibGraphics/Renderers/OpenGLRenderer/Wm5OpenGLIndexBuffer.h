// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5OPENGLINDEXBUFFER_H
#define WM5OPENGLINDEXBUFFER_H

#include "Wm5OpenGLRendererLIB.h"
#include "Wm5IndexBuffer.h"

namespace Wm5
{

class Renderer;

class PdrIndexBuffer
{
public:
    // Construction and destruction.
    PdrIndexBuffer (Renderer* renderer, const IndexBuffer* ibuffer);
    ~PdrIndexBuffer ();

    // Buffer operations.
    void Enable (Renderer* renderer);
    void Disable (Renderer* renderer);
    void* Lock (Buffer::Locking mode);
    void Unlock ();

private:
    GLuint mBuffer;
};

}

#endif
