// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5THREAD_H
#define WM5THREAD_H

#include "Wm5CoreLIB.h"
#include "Wm5ThreadType.h"

namespace Wm5
{

class WM5_CORE_ITEM Thread
{
public:
    // Construction and destruction.
    Thread (void* function, void* userData, unsigned int processorNumber = 0,
        unsigned int stackSize = 0);

    ~Thread ();

    // Start and stop the thread.
    void Resume ();
    void Suspend ();

private:
    ThreadType mThread;
    unsigned int mThreadID;
    void* mFunction;
    void* mUserData;
    unsigned int mProcessorNumber;
    unsigned int mStackSize;
};

}

#endif
