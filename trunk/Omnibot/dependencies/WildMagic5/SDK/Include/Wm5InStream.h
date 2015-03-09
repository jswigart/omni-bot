// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2010/10/01)

#ifndef WM5INSTREAM_H
#define WM5INSTREAM_H

#include "Wm5CoreLIB.h"
#include "Wm5Stream.h"

namespace Wm5
{

class Object;

class WM5_CORE_ITEM InStream : public Stream
{
public:
    // Construction and destruction.
    InStream ();
    virtual ~InStream ();

    // The top-level objects that were loaded.
    int GetNumObjects () const;
    Object* GetObjectAt (int i) const;

    // Read the objects from a memory block.  The caller must manage the input
    // buffer to Load; that is, if you created the buffer with new1, then you
    // must delete the buffer with delete1 when you are finished with it.
    // TODO:  The caller does not know how much space to allocate for 'buffer'.
    void Load (int bufferSize, char* buffer,
        int mode = BufferIO::BM_DEFAULT_READ);

    // Read the objects from a disk file.  The version string is read first
    // and compared to the parameter 'version'.  If these strings are the
    // same, then the top-level objects are read.  The function returns 'true'
    // iff the operation was successful.
    bool Load (const std::string& name,
        std::string version = "WM_VERSION_5_1",
        int mode = BufferIO::BM_DEFAULT_READ);

public_internal:
    // Functions for reading arrays come in three flavors: VV, VR, and RR.  V
    // is 'value' and R is reference'.
    //   numElements:
    //     V = known by the caller
    //     R = read from the buffer
    //   data
    //     V = array allocated by the caller
    //     R = array allocated by the function and read from the buffer

    // Native read functions.
    template <typename T> bool Read (T& datum);
    template <typename T> bool ReadVV (int numElements, T* data);
    template <typename T> bool ReadVR (int numElements, T*& data);
    template <typename T> bool ReadRR (int& numElements, T*& data);

    // Read bools as 4-byte quantities.
    bool ReadBool (bool& datum);
    bool ReadBoolVV (int numElements, bool* data);
    bool ReadBoolVR (int numElements, bool*& data);
    bool ReadBoolRR (int& numElements, bool*& data);

    // Read strings as 4-bytes for length followed by the characters without
    // a null terminator.
    bool ReadString (std::string& datum);
    bool ReadStringVV (int numElements, std::string* data);
    bool ReadStringVR (int numElements, std::string*& data);
    bool ReadStringRR (int& numElements, std::string*& data);

    // Read enums as 4-byte quantities.
    template <typename T> bool ReadEnum (T& datum);
    template <typename T> bool ReadEnumVV (int numElements, T* data);
    template <typename T> bool ReadEnumVR (int numElements, T*& data);
    template <typename T> bool ReadEnumRR (int& numElements, T*& data);

    // Read object pointers (uniqueID on disk to [invalid] memory pointer).
    template <typename T> bool ReadPointer (T*& object);
    template <typename T> bool ReadPointerVV (int numElements, T** objects);
    template <typename T> bool ReadPointerVR (int numElements, T**& objects);
    template <typename T> bool ReadPointerRR (int& numElements, T**& objects);

    template <typename T> bool ReadPointer (Pointer0<T>& object);
    template <typename T> bool ReadPointerVV (int numElements,
        Pointer0<T>* objects);
    template <typename T> bool ReadPointerVR (int numElements,
        Pointer0<T>*& objects);
    template <typename T> bool ReadPointerRR (int& numElements,
        Pointer0<T>*& objects);

    // These template functions are for aggregrate types.  The functions must
    // be specialized, which is enforced by not providing a default
    // implementation.
    template <typename T> bool ReadAggregate (T& datum);
    template <typename T> bool ReadAggregateVV (int numElements, T* data);
    template <typename T> bool ReadAggregateVR (int numElements, T*& data);
    template <typename T> bool ReadAggregateRR (int& numElements, T*& data);

    // Helper functions.
    int GetBytesRead () const;
    void ReadUniqueID (Object* object);
    template <typename T> void ResolveLink (T*& object);
    template <typename T> void ResolveLink (int numElements, T** object);
    template <typename T> void ResolveLink (Pointer0<T>& object);
    template <typename T> void ResolveLink (int numElements,
        Pointer0<T>* objects);

private:
    typedef std::map<uint64_t, Object*> LinkMap;
    typedef std::vector<Object*> LinkArray;

    // The top-level objects for the stream.
    std::vector<Object*> mTopLevel;

    // Storage for the objects during a load of the graphs of the top-level
    // objects.
    LinkMap mLinked;
    LinkArray mOrdered;
    BufferIO mSource;
};

#include "Wm5InStream.inl"

}

#endif
