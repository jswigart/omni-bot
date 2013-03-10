// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/09/07)

#include "Wm5CorePCH.h"
#include "Wm5Mutex.h"
#include "Wm5Assert.h"
using namespace Wm5;

#if defined(WIN32)

// The system mutex is very slow and are not needed for the single-process
// applications of Wild Magic.  Critical sections are faster.  In case you
// relied on the system mutex, you can comment out this define to get the
// old behavior
#define WM5_USE_CRITICAL_SECTIONS

//----------------------------------------------------------------------------
#ifdef WM5_USING_VC80
// See the comments at
// http://msdn.microsoft.com/en-us/library/ms683476(VS.85).aspx
// about why these #defines expose InitializeCriticalSectionAndSpinCount.
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500
#endif
#include <windows.h>
//----------------------------------------------------------------------------
Mutex::Mutex ()
{
#ifdef WM5_USE_CRITICAL_SECTIONS
    // The MSVS2010 documentation mentioned that the heap manager spin lock
    // was set to approximately 4000.  Let's see how 4096 does.
    mMutex = new CRITICAL_SECTION;
    BOOL success = InitializeCriticalSectionAndSpinCount(
        (CRITICAL_SECTION*)mMutex, 4096);
    assertion(success == TRUE, "Failed to initialize critical section.\n");
    WM5_UNUSED(success);
#else
    mMutex = CreateMutex(NULL, FALSE, NULL);
    assertion(mMutex != 0, "Failed to create mutex\n");
#endif
}
//----------------------------------------------------------------------------
Mutex::~Mutex ()
{
#ifdef WM5_USE_CRITICAL_SECTIONS
    DeleteCriticalSection((CRITICAL_SECTION*)mMutex);
    delete mMutex;
#else
    BOOL closed = CloseHandle((HANDLE)mMutex);
    assertion(closed == TRUE, "Failed to destroy mutex\n");
    WM5_UNUSED(closed);
#endif
}
//----------------------------------------------------------------------------
void Mutex::Enter ()
{
#ifdef WM5_USE_CRITICAL_SECTIONS
    EnterCriticalSection((CRITICAL_SECTION*)mMutex);
#else
    DWORD result = WaitForSingleObject((HANDLE)mMutex, INFINITE);
    WM5_UNUSED(result);
    // result:
    //   WAIT_ABANDONED (0x00000080)
    //   WAIT_OBJECT_0  (0x00000000), signaled
    //   WAIT_TIMEOUT   (0x00000102), [not possible with INFINITE]
    //   WAIT_FAILED    (0xFFFFFFFF), not signaled
#endif
}
//----------------------------------------------------------------------------
void Mutex::Leave ()
{
#ifdef WM5_USE_CRITICAL_SECTIONS
    LeaveCriticalSection((CRITICAL_SECTION*)mMutex);
#else
    BOOL released = ReleaseMutex((HANDLE)mMutex);
    WM5_UNUSED(released);
#endif
}
//----------------------------------------------------------------------------
#elif defined(__LINUX__) || defined(__APPLE__)
//----------------------------------------------------------------------------
Mutex::Mutex ()
{
    int result;
    WM5_UNUSED(result);
    
    result = pthread_mutexattr_init(&mMutex.Attribute);
    // successful = 0
    // errors = ENOMEM

    result = pthread_mutexattr_settype(&mMutex.Attribute,
         PTHREAD_MUTEX_RECURSIVE);
    // successful = 0
    
    result = pthread_mutex_init(&mMutex.Mutex, &mMutex.Attribute);
    // successful = 0
    // errors = EAGAIN, ENOMEM, EPERM, EBUSY, EINVAL
}
//----------------------------------------------------------------------------
Mutex::~Mutex ()
{
    int result;
    WM5_UNUSED(result);

    result = pthread_mutex_destroy(&mMutex.Mutex);
    // successful = 0
    // errors = EINVAL

    result = pthread_mutexattr_destroy(&mMutex.Attribute);
    // successful = 0
    // errors = EBUSY, EINVAL
}
//----------------------------------------------------------------------------
void Mutex::Enter ()
{
    int result = pthread_mutex_lock(&mMutex.Mutex);
    WM5_UNUSED(result);
    // successful = 0
    // errors = EINVAL, EDEADLK
}
//----------------------------------------------------------------------------
void Mutex::Leave ()
{
    int result = pthread_mutex_unlock(&mMutex.Mutex);
    WM5_UNUSED(result);
    // successful = 0
    // errors = EINVAL, EPERM
}
//----------------------------------------------------------------------------
#else
#error Other platforms not yet implemented.
#endif
//----------------------------------------------------------------------------
