// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5CorePCH.h"
#include "Wm5BufferIO.h"
#include "Wm5Endian.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
BufferIO::BufferIO ()
    :
    mBuffer(0),
    mNumBytesTotal(0),
    mNumBytesProcessed(0),
    mMode(BM_NONE)
{
}
//----------------------------------------------------------------------------
BufferIO::BufferIO (int numBytesTotal, char* buffer, int mode)
    :
    mBuffer(0),
    mNumBytesTotal(0),
    mNumBytesProcessed(0),
    mMode(BM_NONE)
{
    Open(numBytesTotal, buffer, mode);
}
//----------------------------------------------------------------------------
BufferIO::~BufferIO ()
{
    if (mMode != BM_NONE)
    {
        Close();
    }
}
//----------------------------------------------------------------------------
bool BufferIO::Open (int numBytesTotal, char* buffer, int mode)
{
    if (mMode == BM_NONE)
    {
        if (buffer && numBytesTotal > 0)
        {
            mBuffer = buffer;
            mNumBytesTotal = numBytesTotal;
            mNumBytesProcessed = 0;
            mMode = mode;
            return true;
        }

        assertion(false, "Failed to open buffer\n");
    }
    else
    {
        assertion(false, "Buffer %s is already open\n");
    }

    return false;
}
//----------------------------------------------------------------------------
bool BufferIO::Close ()
{
    mBuffer = 0;
    mNumBytesTotal = 0;
    mNumBytesProcessed = 0;
    mMode = BM_NONE;
    return false;
}
//----------------------------------------------------------------------------
bool BufferIO::IncrementNumBytesProcessed (int numBytes)
{
#ifdef WM5_BUFFERIO_VALIDATE_OPERATION
    if (numBytes <= 0)
    {
        assertion(false, "Increment must be positive\n");
        return false;
    }
#endif

    int nextBytesProcessed = mNumBytesProcessed + numBytes;
    if (nextBytesProcessed <= mNumBytesTotal)
    {
        mNumBytesProcessed = nextBytesProcessed;
        return true;
    }

#ifdef WM5_BUFFERIO_VALIDATE_OPERATION
    assertion(false, "Increment exceeds buffer size\n");
#endif

    mNumBytesProcessed = mNumBytesTotal;
    return false;
}
//----------------------------------------------------------------------------
bool BufferIO::Read (size_t itemSize, void* datum)
{
#ifdef WM5_BUFFERIO_VALIDATE_OPERATION
    if ((mMode != BM_READ && mMode != BM_READ_AND_SWAP) || !datum
    ||  (itemSize != 1 && itemSize != 2 && itemSize != 4 && itemSize != 8))
    {
        assertion(false, "Invalid BufferIO::Read\n");
        return false;
    }
#endif

    int nextBytesProcessed = mNumBytesProcessed + (int)itemSize;
    if (nextBytesProcessed <= mNumBytesTotal)
    {
        char* source = mBuffer + mNumBytesProcessed;
        mNumBytesProcessed = nextBytesProcessed;
        memcpy(datum, source, itemSize);
        if (mMode == BM_READ_AND_SWAP && itemSize > 1)
        {
            Endian::Swap(itemSize, datum);
        }
        return true;
    }

#ifdef WM5_BUFFERIO_VALIDATE_OPERATION
    assertion(false, "Invalid BufferIO::Read\n");
#endif

    mNumBytesProcessed = mNumBytesTotal;
    return false;
}
//----------------------------------------------------------------------------
bool BufferIO::Read (size_t itemSize, int numItems, void* data)
{
#ifdef WM5_BUFFERIO_VALIDATE_OPERATION
    if ((mMode != BM_READ && mMode != BM_READ_AND_SWAP) || numItems <= 0
    ||  !data
    ||  (itemSize != 1 && itemSize != 2 && itemSize != 4 && itemSize != 8))
    {
        assertion(false, "Invalid BufferIO::Read\n");
        return false;
    }
#endif

    int numToCopy = ((int)itemSize)*numItems;
    int nextBytesProcessed = mNumBytesProcessed + numToCopy;
    if (nextBytesProcessed <= mNumBytesTotal)
    {
        char* source = mBuffer + mNumBytesProcessed;
        mNumBytesProcessed = nextBytesProcessed;
        memcpy(data, source, (size_t)numToCopy);
        if (mMode == BM_READ_AND_SWAP && itemSize > 1)
        {
            Endian::Swap(itemSize, numItems, data);
        }
        return true;
    }

#ifdef WM5_BUFFERIO_VALIDATE_OPERATION
    assertion(false, "Invalid BufferIO::Read\n");
#endif

    mNumBytesProcessed = mNumBytesTotal;
    return false;
}
//----------------------------------------------------------------------------
bool BufferIO::Write (size_t itemSize, const void* datum)
{
#ifdef WM5_BUFFERIO_VALIDATE_OPERATION
    if ((mMode != BM_WRITE && mMode != BM_WRITE_AND_SWAP) || !datum
    ||  (itemSize != 1 && itemSize != 2 && itemSize != 4 && itemSize != 8))
    {
        assertion(false, "Invalid BufferIO::Write\n");
        return false;
    }
#endif

    int nextBytesProcessed = mNumBytesProcessed + (int)itemSize;
    if (nextBytesProcessed <= mNumBytesTotal)
    {
        char* target = mBuffer + mNumBytesProcessed;
        mNumBytesProcessed = nextBytesProcessed;
        memcpy(target, datum, itemSize);
        if (mMode == BM_WRITE_AND_SWAP && itemSize > 1)
        {
            Endian::Swap(itemSize, target);
        }
        return true;
    }

#ifdef WM5_BUFFERIO_VALIDATE_OPERATION
    assertion(false, "Invalid BufferIO::Write\n");
#endif

    mNumBytesProcessed = mNumBytesTotal;
    return false;
}
//----------------------------------------------------------------------------
bool BufferIO::Write (size_t itemSize, int numItems, const void* data)
{
#ifdef WM5_BUFFERIO_VALIDATE_OPERATION
    if ((mMode != BM_WRITE && mMode != BM_WRITE_AND_SWAP) || numItems <= 0
    ||  !data
    ||  (itemSize != 1 && itemSize != 2 && itemSize != 4 && itemSize != 8))
    {
        assertion(false, "Invalid BufferIO::Write\n");
        return false;
    }
#endif

    int numToCopy = ((int)itemSize)*numItems;
    int nextBytesProcessed = mNumBytesProcessed + numToCopy;
    if (nextBytesProcessed <= mNumBytesTotal)
    {
        char* target = mBuffer + mNumBytesProcessed;
        mNumBytesProcessed = nextBytesProcessed;
        memcpy(target, data, (size_t)numToCopy);
        if (mMode == BM_WRITE_AND_SWAP && itemSize > 1)
        {
            Endian::Swap(itemSize, numItems, target);
        }
        return true;
    }

#ifdef WM5_BUFFERIO_VALIDATE_OPERATION
    assertion(false, "Invalid BufferIO::Write\n");
#endif

    mNumBytesProcessed = mNumBytesTotal;
    return false;
}
//----------------------------------------------------------------------------
