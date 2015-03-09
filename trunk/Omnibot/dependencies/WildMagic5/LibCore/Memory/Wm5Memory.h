// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/10/01)

#ifndef WM5MEMORY_H
#define WM5MEMORY_H

#include "Wm5CoreLIB.h"
#include "Wm5Assert.h"
#include "Wm5Mutex.h"

#ifdef WM5_USE_MEMORY
// Wild Magic memory tracking for debugging.

namespace Wm5
{

class WM5_CORE_ITEM Memory
{
public:
    // Allow users to provide their own allocator and deallocator.
    typedef void* (*Allocator)(size_t numBytes, const char* file, int line);
    typedef void (*Deallocator)(void* memblock, const char* file, int line);

    // The initialization and termination functions are called in 'main'
    // in the file Wm5Application.cpp.  To specify your own allocator and
    // deallocator, modify the Memory::Initialize call in 'main'.
    static void Initialize (
        Allocator allocator = &DefaultAllocator,
        Deallocator deallocator = &DefaultDeallocator
    );

    static void Terminate (const std::string& filename);

public_internal:
    inline Memory (const char* file, int line) : mFile(file), mLine(line) {}
    inline ~Memory () {}

    inline static bool HasMap () { return msMap != 0; }
    void* CreateBlock (size_t numBytes, int numDimensions) const;

    // For 1D arrays:  data[bound0]
    template <typename T>
    T* New1 (const size_t bound0);

    // For 2D arrays:  data[bound1][bound0]
    template <typename T>
    T** New2 (const size_t bound0, const size_t bound1);

    // For 3D arrays:  data[bound2][bound1][bound0]
    template <typename T>
    T*** New3 (const size_t bound0, const size_t bound1, const size_t bound2);

    // For 4D arrays:  data[bound3][bound2][bound1][bound0]
    template <typename T>
    T**** New4 (const size_t bound0, const size_t bound1, const size_t bound2,
        const size_t bound3);


    // For singletons.
    template <typename T>
    void Delete0 (T*& data);

    // For 1D arrays:  data[bound0]
    template <typename T>
    void Delete1 (T*& data);

    // For 2D arrays:  data[bound1][bound0]
    template <typename T>
    void Delete2 (T**& data);

    // For 3D arrays:  data[bound2][bound1][bound0]
    template <typename T>
    void Delete3 (T***& data);

    // For 4D arrays:  data[bound3][bound2][bound1][bound0]
    template <typename T>
    void Delete4 (T****& data);

private_internal:
    static void* DefaultAllocator (size_t numBytes, const char* file,
        int line);

    static void DefaultDeallocator (void* memBlock, const char* file,
        int line);

    class Information
    {
    public:
        inline Information () {}

        inline Information (size_t numBytes, int numDimensions,
            const char* file, int line)
            :
            mNumBytes(numBytes),
            mNumDimensions(numDimensions),
            mFile(file),
            mLine(line),
            mUniqueID(++msUniqueID)
        {
        }

        size_t mNumBytes;
        int mNumDimensions;
        const char* mFile;
        int mLine;
        unsigned int mUniqueID;
        static unsigned int msUniqueID;
    };

    typedef std::map<void*,Information> MemoryMap;
    typedef std::map<unsigned int, std::pair<void*,Information> > SortedMap;

    const char* mFile;
    int mLine;

    // The msMutex is used to prevent concurrent access to msMap.  However,
    // the order of declaration and initialization is as shown because the
    // function CreateBlock tests msMap first to warn about pre-main
    // allocations.
    static MemoryMap* msMap;
    static Mutex msMutex;
    static Allocator msAllocator;
    static Deallocator msDeallocator;
};

#include "Wm5Memory.inl"

}

//----------------------------------------------------------------------------
inline void* operator new (size_t numBytes, const Wm5::Memory& memory)
{
    if (memory.HasMap())
    {
        return memory.CreateBlock(numBytes, 0);
    }
    else
    {
#ifdef WM5_USE_MEMORY_ASSERT_ON_PREINIT_POSTTERM_OPERATIONS
        assertion(false, "Allocations are not tracked outside a "
            "Memory::Initialize/Memory::Terminate block.\n");
#endif
        return malloc(numBytes);
    }
}
//----------------------------------------------------------------------------
inline void operator delete (void*, const Wm5::Memory&)
{
    // Only called during exception handling.
}
//----------------------------------------------------------------------------

#define new0 new(Wm5::Memory(__FILE__,__LINE__))
#define new1 Wm5::Memory(__FILE__,__LINE__).New1
#define new2 Wm5::Memory(__FILE__,__LINE__).New2
#define new3 Wm5::Memory(__FILE__,__LINE__).New3
#define new4 Wm5::Memory(__FILE__,__LINE__).New4
#define delete0 Wm5::Memory(__FILE__,__LINE__).Delete0
#define delete1 Wm5::Memory(__FILE__,__LINE__).Delete1
#define delete2 Wm5::Memory(__FILE__,__LINE__).Delete2
#define delete3 Wm5::Memory(__FILE__,__LINE__).Delete3
#define delete4 Wm5::Memory(__FILE__,__LINE__).Delete4

#else

// Standard memory management.
#define new0 new

// For 1D arrays:  data[bound0]
template <typename T>
T* new1 (const size_t bound0);

// For 2D arrays:  data[bound1][bound0]
template <typename T>
T** new2 (const size_t bound0, const size_t bound1);

// For 3D arrays:  data[bound2][bound1][bound0]
template <typename T>
T*** new3 (const size_t bound0, const size_t bound1, const size_t bound2);

// For 4D arrays:  data[bound3][bound2][bound1][bound0]
template <typename T>
T**** new4 (const size_t bound0, const size_t bound1, const size_t bound2,
    const size_t bound3);

// For singletons.
template <typename T>
void delete0 (T*& data);

// For 1D arrays:  data[bound0]
template <typename T>
void delete1 (T*& data);

// For 2D arrays:  data[bound1][bound0]
template <typename T>
void delete2 (T**& data);

// For 3D arrays:  data[bound2][bound1][bound0]
template <typename T>
void delete3 (T***& data);

// For 4D arrays:  data[bound3][bound2][bound1][bound0]
template <typename T>
void delete4 (T****& data);

#include "Wm5Memory.inl"

#endif
#endif
