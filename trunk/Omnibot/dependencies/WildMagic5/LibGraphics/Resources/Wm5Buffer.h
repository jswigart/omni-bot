// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BUFFER_H
#define WM5BUFFER_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Buffer : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(Buffer);

public:
    // Usage flags for vertex buffers, index buffers, and textures.
    enum WM5_GRAPHICS_ITEM Usage
    {
        BU_STATIC,
        BU_DYNAMIC,
        BU_RENDERTARGET,
        BU_DEPTHSTENCIL,
        BU_TEXTURE,
        BU_QUANTITY
    };

    // Locking modes for accessing video memory for a buffer or texture.
    enum WM5_GRAPHICS_ITEM Locking
    {
        BL_READ_ONLY,
        BL_WRITE_ONLY,
        BL_READ_WRITE,
        BL_QUANTITY
    };

protected:
    // Abstract base class.  Construction and destruction.
    Buffer ();
    Buffer (int numElements, int elementSize, Usage usage);
public:
    virtual ~Buffer ();

    // Member access.
    inline int GetNumElements () const;
    inline int GetElementSize () const;
    inline Usage GetUsage () const;

    // An application might want to vary the active number of indices.  Use
    // this function to do so.  It does not change the data storage, only the
    // Buffer::mNumElements member.  The caller is responsible for saving the
    // original number of indices and resetting this when finished with the
    // index buffer.  The caller also should not pass in a number of indices
    // that is larger than the original number of indices.
    inline void SetNumElements (int numElements);

    // Manage a copy of the buffer in system memory.  Queries of the system
    // memory occur in IndexBuffer and VertexBuffer.  The GetData() accessor
    // should be for read-only data.  Do not typecast as "char*" and write,
    // otherwise the renderers will not know the data has been modified.
    inline int GetNumBytes () const;
    inline const char* GetData () const;

protected:
    int mNumElements;
    int mElementSize;
    Usage mUsage;
    int mNumBytes;
    char* mData;
};

WM5_REGISTER_STREAM(Buffer);
typedef Pointer0<Buffer> BufferPtr;
#include "Wm5Buffer.inl"

}

#endif
