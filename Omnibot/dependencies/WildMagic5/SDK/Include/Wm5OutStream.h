// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/10/01)

#ifndef WM5OUTSTREAM_H
#define WM5OUTSTREAM_H

#include "Wm5CoreLIB.h"
#include "Wm5Stream.h"

namespace Wm5
{

class Object;

class WM5_CORE_ITEM OutStream : public Stream
{
public:
    // Construction and destruction.
    OutStream ();
    virtual ~OutStream ();

    // The top-level objects to be saved.
    bool Insert (Object* object);
    bool IsTopLevel (const Object* object) const;

    // Write the objects to a memory block.  The output buffer is allocated
    // with new1, so you must release it with delete1 when you are finished
    // with it.  The function returns 'true' iff the operation was successful.
    void Save (int& bufferSize, char*& buffer,
        int mode = BufferIO::BM_DEFAULT_WRITE);

    // Write the objects to a disk file.  The version string is written first
    // before the top-level objects.  The function returns 'true' iff the
    // operation was successful.
    bool Save (const std::string& name,
        std::string version = "WM_VERSION_5_1",
        int mode = BufferIO::BM_DEFAULT_WRITE);

public_internal:
    // Functions for writing arrays come in two flavors:  W and N.  W means
    // write numElements to the buffer.  N means do not write numElements to
    // the buffer.

    // Native write functions.
    template <typename T> bool Write (T datum);
    template <typename T> bool WriteW (int numElements, const T* data);
    template <typename T> bool WriteN (int numElements, const T* data);

    // Write bools as 4-byte quantities.
    bool WriteBool (const bool datum);
    bool WriteBoolW (int numElements, const bool* data);
    bool WriteBoolN (int numElements, const bool* data);

    // Write strings as 4-bytes for length followed by the characters without
    // a null terminator.
    bool WriteString (const std::string& datum);
    bool WriteStringW (int numElements, const std::string* data);
    bool WriteStringN (int numElements, const std::string* data);

    // Write enums as 4-byte quantities.
    template <typename T> bool WriteEnum (const T datum);
    template <typename T> bool WriteEnumW (int numElements, const T* data);
    template <typename T> bool WriteEnumN (int numElements, const T* data);

    // Read object pointers (uniqueID on disk to [invalid] memory pointer).
    template <typename T> bool WritePointer (const T* object);
    template <typename T> bool WritePointerW (int numElements,
        T* const* objects);
    template <typename T> bool WritePointerN (int numElements,
        T* const* objects);

    template <typename T> bool WritePointer (const Pointer0<T>& object);

    template <typename T> bool WritePointerW (int numElements,
        Pointer0<T> const* objects);

    template <typename T> bool WritePointerN (int numElements,
        Pointer0<T> const* objects);

    // These template functions are for aggregrate types.  The functions must
    // be specialized, which is enforced by not providing a default
    // implementation.
    template <typename T> bool WriteAggregate (const T& datum);
    template <typename T> bool WriteAggregateW (int numElements,
        const T* data);
    template <typename T> bool WriteAggregateN (int numElements,
        const T* data);

    // Helper functions.
    int GetBytesWritten () const;
    bool RegisterRoot (const Object* object);
    void WriteUniqueID (const Object* object);

    template <typename T> void Register (const T* object);
    template <typename T> void Register (int numElements, T* const* objects);
    template <typename T> void Register (const Pointer0<T>& object);
    template <typename T> void Register (int numElements,
        Pointer0<T> const* objects);

private:
    typedef std::map<const Object*, unsigned int> RegisterMap;
    typedef std::vector<const Object*> RegisterArray;

    // The top-level objects for the stream.
    std::vector<Object*> mTopLevel;

    // Storage for the objects during a depth-first traversal of the graphs
    // of the top-level objects.
    RegisterMap mRegistered;
    RegisterArray mOrdered;
    BufferIO mTarget;
};

#include "Wm5OutStream.inl"

}

#endif
