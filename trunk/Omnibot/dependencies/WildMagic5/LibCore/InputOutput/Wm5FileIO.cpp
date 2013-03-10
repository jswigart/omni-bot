// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5CorePCH.h"
#include "Wm5FileIO.h"
#include "Wm5Endian.h"
#include "Wm5Memory.h"

// TODO.  Add the file handling for Macintosh.
#include <sys/stat.h>
using namespace Wm5;

//----------------------------------------------------------------------------
FileIO::FileIO ()
    :
    mFile(0),
    mMode(FM_NONE)
{
}
//----------------------------------------------------------------------------
FileIO::FileIO (const std::string& filename, int mode)
    :
    mFile(0),
    mMode(FM_NONE)
{
    Open(filename, mode);
}
//----------------------------------------------------------------------------
FileIO::~FileIO ()
{
    if (mMode != FM_NONE)
    {
        Close();
    }
}
//----------------------------------------------------------------------------
bool FileIO::Open (const std::string& filename, int mode)
{
    if (mMode == FM_NONE)
    {
        if (mode == FM_READ || mode == FM_READ_AND_SWAP)
        {
            mFile = fopen(filename.c_str(), "rbS");
        }
        else
        {
            mFile = fopen(filename.c_str(), "wbS");
        }

        if (mFile)
        {
            mMode = mode;
            return true;
        }

        assertion(false, "Failed to open file %s\n", filename.c_str());
    }
    else
    {
        assertion(false, "File %s is already open\n", filename.c_str());
    }

    return false;
}
//----------------------------------------------------------------------------
bool FileIO::Close ()
{
    mMode = FM_NONE;
    if (fclose(mFile) == 0)
    {
        return true;
    }

    assertion(false, "Failed to close file\n");
    return false;
}
//----------------------------------------------------------------------------
FileIO::operator bool () const
{
    return mMode != FM_NONE;
}
//----------------------------------------------------------------------------
bool FileIO::Read (size_t itemSize, void* datum)
{
#ifdef WM5_FILEIO_VALIDATE_OPERATION
    if ((mMode != FM_READ && mMode != FM_READ_AND_SWAP) || !mFile || !datum
    ||  (itemSize != 1 && itemSize != 2 && itemSize != 4 && itemSize != 8))
    {
        assertion(false, "Invalid FileIO::Read\n");
        return false;
    }
#endif

    int numRead = (int)fread(datum, itemSize, 1, mFile);

#ifdef WM5_FILEIO_VALIDATE_OPERATION
    if (numRead != 1)
    {
        assertion(false, "Invalid FileIO::Read\n");
        return false;
    }
#else
    WM5_UNUSED(numRead);
#endif

    if (mMode == FM_READ_AND_SWAP && itemSize > 1)
    {
        Endian::Swap(itemSize, datum);
    }

    return true;
}
//----------------------------------------------------------------------------
bool FileIO::Read (size_t itemSize, int numItems, void* data)
{
#ifdef WM5_FILEIO_VALIDATE_OPERATION
    if ((mMode != FM_READ && mMode != FM_READ_AND_SWAP) || !mFile
    ||  numItems <= 0 || !data
    ||  (itemSize != 1 && itemSize != 2 && itemSize != 4 && itemSize != 8))
    {
        assertion(false, "Invalid FileIO::Read\n");
        return false;
    }
#endif

    int numRead = (int)fread(data, itemSize, numItems, mFile);

#ifdef WM5_FILEIO_VALIDATE_OPERATION
    if (numRead != numItems)
    {
        assertion(false, "Invalid FileIO::Read\n");
        return false;
    }
#else
    WM5_UNUSED(numRead);
#endif

    if (mMode == FM_READ_AND_SWAP && itemSize > 1)
    {
        Endian::Swap(itemSize, numItems, data);
    }

    return true;
}
//----------------------------------------------------------------------------
bool FileIO::Write (size_t itemSize, const void* datum)
{
#ifdef WM5_FILEIO_VALIDATE_OPERATION
    if ((mMode != FM_WRITE && mMode != FM_WRITE_AND_SWAP) || !mFile || !datum
    ||  (itemSize != 1 && itemSize != 2 && itemSize != 4 && itemSize != 8))
    {
        assertion(false, "Invalid FileIO::Write\n");
        return false;
    }
#endif

    int numWritten;
    if (mMode == FM_WRITE_AND_SWAP && itemSize > 1)
    {
        if (itemSize == 2)
        {
            short tmpData = *(const short*)datum;
            Endian::Swap(itemSize, &tmpData);
            numWritten = (int)fwrite(&tmpData, itemSize, 1, mFile);
        }
        else if (itemSize == 4)
        {
            int tmpData = *(const int*)datum;
            Endian::Swap(itemSize, &tmpData);
            numWritten = (int)fwrite(&tmpData, itemSize, 1, mFile);
        }
        else if (itemSize == 8)
        {
            double tmpData = *(const double*)datum;
            Endian::Swap(itemSize, &tmpData);
            numWritten = (int)fwrite(&tmpData, itemSize, 1, mFile);
        }
        else
        {
            numWritten = 0;
#ifdef WM5_FILEIO_VALIDATE_OPERATION
            assertion(false, "Invalid FileIO::Write\n");
#endif
        }
    }
    else
    {
        numWritten = (int)fwrite(datum, itemSize, 1, mFile);
    }


#ifdef WM5_FILEIO_VALIDATE_OPERATION
    if (numWritten != 1)
    {
        assertion(false, "Invalid FileIO::Write\n");
        return false;
    }
#else
    WM5_UNUSED(numWritten);
#endif

    return true;
}
//----------------------------------------------------------------------------
bool FileIO::Write (size_t itemSize, int numItems, const void* data)
{
#ifdef WM5_FILEIO_VALIDATE_OPERATION
    if ((mMode != FM_WRITE && mMode != FM_WRITE_AND_SWAP) || !mFile
    ||  numItems <= 0 || !data
    ||  (itemSize != 1 && itemSize != 2 && itemSize != 4 && itemSize != 8))
    {
        assertion(false, "Invalid FileIO::Write\n");
        return false;
    }
#endif

    int numWritten;
    if (mMode == FM_WRITE_AND_SWAP && itemSize > 1)
    {
        numWritten = 0;
        if (itemSize == 2)
        {
            const short* tmpData = (const short*)data;
            for (int i = 0; i < numItems; ++i, ++tmpData)
            {
                short temp = *tmpData;
                Endian::Swap(itemSize, &temp);
                numWritten += (int)fwrite(&temp, itemSize, 1, mFile);
            }
        }
        else if (itemSize == 4)
        {
            const int* tmpData = (const int*)data;
            for (int i = 0; i < numItems; ++i, ++tmpData)
            {
                int temp = *tmpData;
                Endian::Swap(itemSize, &temp);
                numWritten += (int)fwrite(&temp, itemSize, 1, mFile);
            }
        }
        else if (itemSize == 8)
        {
            const double* tmpData = (const double*)data;
            for (int i = 0; i < numItems; ++i, ++tmpData)
            {
                double temp = *tmpData;
                Endian::Swap(itemSize, &temp);
                numWritten += (int)fwrite(&temp, itemSize, 1, mFile);
            }
        }
        else
        {
#ifdef WM5_FILEIO_VALIDATE_OPERATION
            assertion(false, "Invalid FileIO::Write\n");
#endif
        }
    }
    else
    {
        numWritten = (int)fwrite(data, itemSize, numItems, mFile);
    }


#ifdef WM5_FILEIO_VALIDATE_OPERATION
    if (numWritten != numItems)
    {
        assertion(false, "Invalid FileIO::Write\n");
        return false;
    }
#else
    WM5_UNUSED(numWritten);
#endif

    return true;
}
//----------------------------------------------------------------------------
bool FileIO::Load (const std::string& filename, bool binaryFile,
    int& bufferSize, char*& buffer)
{
    struct stat statistics;
    if (stat(filename.c_str(), &statistics) != 0)
    {
        // The file does not exist.
        assertion(false, "Failed to open file %s\n", filename.c_str());
        buffer = 0;
        bufferSize = 0;
        return false;
    }

    FILE* inFile;
    if (binaryFile)
    {
        inFile = fopen(filename.c_str(), "rb");
    }
    else
    {
        inFile = fopen(filename.c_str(), "rt");
    }
    if (!inFile)
    {
        // The file cannot be opened for reading. 
        assertion(false, "Failed to open file %s\n", filename.c_str());
        buffer = 0;
        bufferSize = 0;
        return false;
    }

    bufferSize = statistics.st_size;
    buffer = new1<char>(bufferSize);
    int numRead = (int)fread(buffer, sizeof(char), bufferSize, inFile);
    if (fclose(inFile) != 0 || numRead != bufferSize)
    {
        assertion(false, "Failed to read or close file %s\n",
            filename.c_str());
        delete1(buffer);
        buffer = 0;
        bufferSize = 0;
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
bool FileIO::Save (const std::string& filename, bool binaryFile,
    int bufferSize, const char* buffer)
{
    if (!buffer || bufferSize <= 0)
    {
        // The input buffer must exist.  It is not possible to verify that
        // the buffer has the specified number of bytes.
        assertion(false, "Invalid inputs, file %s\n", filename.c_str());
        return false;
    }

    FILE* outFile;
    if (binaryFile)
    {
        outFile = fopen(filename.c_str(), "wb");
    }
    else
    {
        outFile = fopen(filename.c_str(), "wt");
    }
    if (!outFile)
    {
        // The file cannot be opened for writing.
        assertion(false, "Failed to open file %s\n", filename.c_str());
        return false;
    }

    int numWritten = (int)fwrite(buffer, sizeof(char), bufferSize, outFile);
    if (fclose(outFile) != 0 || numWritten != bufferSize)
    {
        assertion(false, "Failed to write or close file %s\n",
            filename.c_str());
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
bool FileIO::Append (const std::string& filename, bool binaryFile,
    int bufferSize, const char* buffer)
{
    if (!buffer || bufferSize <= 0)
    {
        // The input buffer must exist.  It is not possible to verify that
        // the buffer has the specified number of bytes.
        assertion(false, "Invalid inputs, file %s\n", filename.c_str());
        return false;
    }

    FILE* outFile;
    if (binaryFile)
    {
        outFile = fopen(filename.c_str(), "ab");
    }
    else
    {
        outFile = fopen(filename.c_str(), "at");
    }
    if (!outFile)
    {
        // The file cannot be opened for appending.
        assertion(false, "Failed to open file %s\n", filename.c_str());
        return false;
    }

    int numWritten = (int)fwrite(buffer, sizeof(char), bufferSize, outFile);
    if (fclose(outFile) != 0 || numWritten != bufferSize)
    {
        assertion(false, "Failed to write or close file %s\n",
            filename.c_str());
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
