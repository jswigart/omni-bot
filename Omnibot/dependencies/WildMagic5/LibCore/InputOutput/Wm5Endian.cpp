// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5CorePCH.h"
#include "Wm5Assert.h"
#include "Wm5Endian.h"
using namespace Wm5;

static int gsOne = 1;
bool Endian::msIsLittle = (*(char*)&gsOne != 0);

//----------------------------------------------------------------------------
bool Endian::IsBig ()
{
    return !msIsLittle;
}
//----------------------------------------------------------------------------
bool Endian::IsLittle ()
{
    return msIsLittle;
}
//----------------------------------------------------------------------------
void Endian::Swap2 (void* datum)
{
    char* bytes = (char*) datum;
    char save = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = save;
}
//----------------------------------------------------------------------------
void Endian::Swap2 (int numItems, void* data)
{
    char* bytes = (char*) data;
    for (int i = 0; i < numItems; ++i, bytes += 2)
    {
        char save = bytes[0];
        bytes[0] = bytes[1];
        bytes[1] = save;
    }
}
//----------------------------------------------------------------------------
void Endian::Swap4 (void* datum)
{
    char* bytes = (char*) datum;
    char save = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = save;
    save = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = save;
}
//----------------------------------------------------------------------------
void Endian::Swap4 (int numItems, void* data)
{
    char* bytes = (char*) data;
    for (int i = 0; i < numItems; ++i, bytes += 4)
    {
        char save = bytes[0];
        bytes[0] = bytes[3];
        bytes[3] = save;
        save = bytes[1];
        bytes[1] = bytes[2];
        bytes[2] = save;
    }
}
//----------------------------------------------------------------------------
void Endian::Swap8 (void* datum)
{
    char* bytes = (char*) datum;
    char save = bytes[0];
    bytes[0] = bytes[7];
    bytes[7] = save;
    save = bytes[1];
    bytes[1] = bytes[6];
    bytes[6] = save;
    save = bytes[2];
    bytes[2] = bytes[5];
    bytes[5] = save;
    save = bytes[3];
    bytes[3] = bytes[4];
    bytes[4] = save;
}
//----------------------------------------------------------------------------
void Endian::Swap8 (int numItems, void* data)
{
    char* bytes = (char*) data;
    for (int i = 0; i < numItems; ++i, bytes += 8)
    {
        char save = bytes[0];
        bytes[0] = bytes[7];
        bytes[7] = save;
        save = bytes[1];
        bytes[1] = bytes[6];
        bytes[6] = save;
        save = bytes[2];
        bytes[2] = bytes[5];
        bytes[5] = save;
        save = bytes[3];
        bytes[3] = bytes[4];
        bytes[4] = save;
    }
}
//----------------------------------------------------------------------------
void Endian::Swap (size_t itemSize, void* data)
{
    assertion(itemSize == 2 || itemSize == 4 || itemSize == 8,
        "Size must be 2, 4, or 8\n");

    int size = (int)itemSize;
    char* bytes = (char*) data;
    for (int i0 = 0, i1 = size - 1; i0 < size/2; ++i0, --i1)
    {
        char save = bytes[i0];
        bytes[i0] = bytes[i1];
        bytes[i1] = save;
    }
}
//----------------------------------------------------------------------------
void Endian::Swap (size_t itemSize, int numItems, void* data)
{
    assertion(itemSize == 2 || itemSize == 4 || itemSize == 8,
        "Size must be 2, 4, or 8\n");

    int size = (int)itemSize;
    char* bytes = (char*) data;
    for (int i = 0; i < numItems; ++i, bytes += itemSize)
    {
        for (int i0 = 0, i1 = size - 1; i0 < size/2; ++i0, --i1)
        {
            char save = bytes[i0];
            bytes[i0] = bytes[i1];
            bytes[i1] = save;
        }
    }
}
//----------------------------------------------------------------------------
