// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2010/09/19)

#include "Wm5CorePCH.h"
#include "Wm5Memory.h"

#ifdef WM5_USE_MEMORY

using namespace Wm5;

// The msMutex is used to prevent concurrent access to msMap.  However, the
// order of declaration and initialization is as shown because the function
// CreateBlock tests msMap first to warn about pre-main allocations.
Memory::MemoryMap* Memory::msMap = 0;
Mutex Memory::msMutex;
Memory::Allocator Memory::msAllocator = &Memory::DefaultAllocator;
Memory::Deallocator Memory::msDeallocator = &Memory::DefaultDeallocator;
unsigned int Memory::Information::msUniqueID = 0;

//----------------------------------------------------------------------------
void Memory::Initialize (Allocator allocator, Deallocator deallocator)
{
    msMutex.Enter();

    msAllocator = allocator;
    msDeallocator = deallocator;

    // You must use 'new', not 'new0', otherwise you have an infinite
    // loop trying to allocate msMap (over and over and over...).
    assertion(msMap == 0, "The map already exists.  Did you call"
        " Memory::Initialize twice in a row?\n");
    if (msMap == 0)
    {
        msMap = new MemoryMap();
    }

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void Memory::Terminate (const std::string& filename)
{
    msMutex.Enter();

    assertion(msMap != 0, "The map does not exist.  Did you call"
        "Memory::Terminate twice in a row?\n");
    if (!msMap)
    {
        msMutex.Leave();
        return;
    }

    FILE* outFile = fopen(filename.c_str(), "wt");
    if (!outFile)
    {
        msMutex.Leave();
        return;
    }

    // Create a sorted map from the memory map based on unique ID.
    SortedMap sorted;
    MemoryMap::const_iterator mapIter = msMap->begin();
    MemoryMap::const_iterator mapEnd = msMap->end();
    for (/**/; mapIter != mapEnd; ++mapIter)
    {
        sorted[mapIter->second.mUniqueID] =
            std::make_pair(mapIter->first, mapIter->second);
    }

    // Create the format for the output.
    std::string format = "uid = %8u , ";
    format += "addr = %p , ";
    format += "numbytes = %13d , ";
    format += "numdims = %d , ";
    format += "%s (%4d)\n";

    // Save the sorted map to disk.
    SortedMap::const_iterator sortedIter = sorted.begin();
    SortedMap::const_iterator sortedEnd = sorted.end();
    for (/**/; sortedIter != sortedEnd; ++sortedIter)
    {
        unsigned int uniqueID = sortedIter->first;
        void* address = sortedIter->second.first;
        Information info = sortedIter->second.second;

        fprintf(outFile, format.c_str(), uniqueID, address, info.mNumBytes,
            info.mNumDimensions, info.mFile, info.mLine);
    }

    fclose(outFile);

    // You must use 'delete', not 'delete0'.  See the comments in the
    // Memory::Initialize functoin.
    delete msMap;
    msMap = 0;

    msMutex.Leave();
}
//----------------------------------------------------------------------------
void* Memory::CreateBlock (size_t numBytes, int numDimensions) const
{
    msMutex.Enter();

    void* memBlock = msAllocator(numBytes, mFile, mLine);
    (*msMap)[memBlock] = Information(numBytes, numDimensions, mFile, mLine);

    msMutex.Leave();
    return memBlock;
}
//----------------------------------------------------------------------------
void* Memory::DefaultAllocator (size_t numBytes, const char*, int)
{
    return malloc(numBytes);
}
//----------------------------------------------------------------------------
void Memory::DefaultDeallocator (void* memBlock, const char*, int)
{
    free(memBlock);
}
//----------------------------------------------------------------------------

#endif
