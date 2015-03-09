// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BUFFERIO_H
#define WM5BUFFERIO_H

// Support for reading or writing a binary buffer.  If you open the buffer for
// reading, calling the writing routines is an error.  If you open the buffer
// for writing, calling the reading routines is an error.  This class does
// not support read-write access.  A common use for BufferIO is to read an
// entire file using FileIO::Load and then read the fields using BufferIO.
// Similarly, BufferIO may be used to write fields to a buffer followed by
// writing the entire file using FileIO::Save.
//
// The nested enumeration Mode supports endianness mismatches between the
// platform that created the file and the platform that is using it.  The
// flags are defined as follows:
//
// BM_READ
//   Read the data from the buffer as is (no byte swapping).
//
// BM_WRITE
//   Write the data to the buffer as is (no byte swapping).
//
// BM_READ_AND_SWAP
//   Read from the buffer and swap 2-, 4-, and 8-byte inputs.
//
// BM_WRITE_AND_SWAP
//   Write to the buffer and swap 2-, 4-, and 8-byte inputs.

#include "Wm5CoreLIB.h"

namespace Wm5
{

class WM5_CORE_ITEM BufferIO
{
public:
    enum
    {
        BM_NONE,
        BM_READ,
        BM_WRITE,
        BM_READ_AND_SWAP,
        BM_WRITE_AND_SWAP,

        // All data files are in little endian format, because most platforms
        // these days are little endian.  The BufferIO class is designed for a
        // block load of an entire file followed by parsing the buffer.
#ifdef WM5_LITTLE_ENDIAN
        BM_DEFAULT_READ = BM_READ,
        BM_DEFAULT_WRITE = BM_WRITE
#else
        BM_DEFAULT_READ = BM_READ_AND_SWAP,
        BM_DEFAULT_WRITE = BM_WRITE_AND_SWAP
#endif
    };

    // Construction and destruction.  The caller provides the memory for the
    // buffer, whether reading or writing, and is responsible for managing
    // that memory.  The buffer does not have to be dynamically allocated.
    // TODO:  Support writing where the total number of bytes to be written is
    // unknown.  This requires dynamic reallocation of the input buffer, and
    // accessors must be added, say, GetBuffer() and GetNumBytes().
    BufferIO ();
    BufferIO (int numBytesTotal, char* buffer, int mode);
    ~BufferIO ();

    bool Open (int numBytesTotal, char* buffer, int mode);
    bool Close ();

    // Implicit conversion to allow testing for successful construction.
    inline operator bool () const;

    // Member access.
    inline const char* GetBuffer () const;
    inline int GetNumBytesTotal () const;
    inline int GetNumBytesProcessed () const;
    inline int GetMode () const;
    bool IncrementNumBytesProcessed (int numBytes);

    // The return value is 'true' if and only if the operation was
    // successful, in which case the number of bytes read or written is
    // the item size times number of items.
    bool Read (size_t itemSize, void* datum);
    bool Read (size_t itemSize, int numItems, void* data);
    bool Write (size_t itemSize, const void* datum);
    bool Write (size_t itemSize, int numItems, const void* data);

private:
    char* mBuffer;
    int mNumBytesTotal;
    int mNumBytesProcessed;
    int mMode;
};

#include "Wm5BufferIO.inl"

}

#endif
