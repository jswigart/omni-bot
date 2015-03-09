// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5SCOPEDCS_H
#define WM5SCOPEDCS_H

#include "Wm5CoreLIB.h"
#include "Wm5Mutex.h"

namespace Wm5
{

// Scoped critial section:  This provides for a mutex that is entered at the
// beginning of a scope.  The mutex leaves when the ScopedCS object is
// automatically destroyed when leaving the scope, due to the usual C++
// semantics of calling destructors when exiting a scope.

class WM5_CORE_ITEM ScopedCS
{
public:
    // Construction and destruction.
    ScopedCS (Mutex* mutex);
    ~ScopedCS ();

private:
    Mutex* mMutex;
};

}

#endif
