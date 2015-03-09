// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5MUTEXTYPE_H
#define WM5MUTEXTYPE_H

#include "Wm5CoreLIB.h"

#if defined(WIN32)
//----------------------------------------------------------------------------
// Mutex type for Microsoft Windows.  It is technically a HANDLE, but
// including <windows.h> here exposes Microsoft Windows symbols in the
// application layer.  To avoid this, void* is used and typecasts are
// applied in Wm5Mutex.cpp.
//----------------------------------------------------------------------------
namespace Wm5
{
    typedef void* MutexType;
}
//----------------------------------------------------------------------------
#elif defined(__LINUX__) || defined(__APPLE__)
//----------------------------------------------------------------------------
// Mutex type for Linux/Apple.  The file pthread.h exposes only native data
// types, so including it here does not suck in lots of extra stuff.
//----------------------------------------------------------------------------
#include <pthread.h>
namespace Wm5
{
    typedef struct
    {
        pthread_mutexattr_t Attribute;
        pthread_mutex_t Mutex;
    }
    MutexType;
}
#else
//----------------------------------------------------------------------------
// TODO: Mutex types for other platforms.
//----------------------------------------------------------------------------
#error Other platforms not yet implemented.
#endif

#endif
