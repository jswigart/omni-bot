// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5ASSERT_H
#define WM5ASSERT_H

#include "Wm5CoreLIB.h"

#ifdef WM5_USE_ASSERT
//----------------------------------------------------------------------------
// Use WM5 asserts with file/line tracking.
//----------------------------------------------------------------------------
namespace Wm5
{

class WM5_CORE_ITEM Assert
{
public:
    // Construction and destruction.
    Assert (bool condition, const char* file, int line, const char* format,
        ...);

    ~Assert ();

private:
    enum { MAX_MESSAGE_BYTES = 1024 };
    static const char* msDebugPrompt;
    static const size_t msDebugPromptLength;
    static const char* msMessagePrefix;

#ifdef WM5_USE_ASSERT_WRITE_TO_MESSAGE_BOX
    static const char* msMessageBoxTitle;
#endif
};

}

#define assertion(condition, format, ...) \
    Wm5::Assert(condition, __FILE__, __LINE__, format, __VA_ARGS__)
//----------------------------------------------------------------------------
#else
//----------------------------------------------------------------------------
// Use standard asserts.
//----------------------------------------------------------------------------
#define assertion(condition, format, ...) assert(condition)
//----------------------------------------------------------------------------
#endif

#endif
