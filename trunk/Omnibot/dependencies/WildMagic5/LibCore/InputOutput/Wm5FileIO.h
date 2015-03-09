// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5FILEIO_H
#define WM5FILEIO_H

#include "Wm5CoreLIB.h"

// Support for reading or writing a binary file.  If you open the file for
// reading, calling the writing routines is an error.  If you open the file
// for writing, calling the reading routines is an error.  This class does
// not support read-write access.  The class wraps FILE operations of fopen,
// fclose, fread, and fwrite.  A hint is passed to fopen to optimize caching
// for sequential access from disk.
//
// The nested enumeration Mode supports endianness mismatches between the
// platform that created the file and the platform that is using it.  The
// flags are defined as follows:
//
// FM_READ
//   Read the data from disk as is (no byte swapping).
//
// FM_WRITE
//   Write the data to disk as is (no byte swapping).
//
// FM_READ_AND_SWAP
//   Read from disk and swap 2-, 4-, and 8-byte inputs.
//
// FM_WRITE_AND_SWAP
//   Swap 2-, 4-, and 8-byte inputs and write to disk.

namespace Wm5
{

class WM5_CORE_ITEM FileIO
{
public:
    enum
    {
        FM_NONE,
        FM_READ,
        FM_WRITE,
        FM_READ_AND_SWAP,
        FM_WRITE_AND_SWAP,

        // All data files are in little endian format, because most platforms
        // these days are little endian.
#ifdef WM5_LITTLE_ENDIAN
        FM_DEFAULT_READ = FM_READ,
        FM_DEFAULT_WRITE = FM_WRITE
#else
        FM_DEFAULT_READ = FM_READ_AND_SWAP,
        FM_DEFAULT_WRITE = FM_WRITE_AND_SWAP
#endif
    };

    // Construction and destruction.
    FileIO ();
    FileIO (const std::string& filename, int mode);
    ~FileIO ();

    bool Open (const std::string& filename, int mode);
    bool Close ();

    // Implicit conversion to allow testing for successful file open.
    operator bool () const;

    // Member access.
    inline int GetMode () const;

    // The return value is 'true' if and only if the operation was
    // successful, in which case the number of bytes read or written is
    // the item size times number of items.
    bool Read (size_t itemSize, void* datum);
    bool Read (size_t itemSize, int numItems, void* data);
    bool Write (size_t itemSize, const void* datum);
    bool Write (size_t itemSize, int numItems, const void* data);

    // Load the entire file into a buffer.
    static bool Load (const std::string& filename, bool binaryFile,
        int& bufferSize, char*& buffer);

    // Save a buffer into a file.
    static bool Save (const std::string& filename, bool binaryFile,
        int bufferSize, const char* buffer);

    // Append the buffer to a file.
    static bool Append (const std::string& filename, bool binaryFile,
        int bufferSize, const char* buffer);

private:
    FILE* mFile;
    int mMode;
};

#include "Wm5FileIO.inl"

}

#endif
