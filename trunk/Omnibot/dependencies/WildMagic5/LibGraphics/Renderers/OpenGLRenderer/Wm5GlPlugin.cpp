// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GlExtensions.h"
#include <cassert>
#include <cstdio>

// Disable the MSVC8 warnings about deprecated functions.
#pragma warning( push )
#pragma warning( disable : 4996 )

//----------------------------------------------------------------------------
void ReportNullFunction (const char*)
{
    assert(false);
}
//----------------------------------------------------------------------------
void ReportGLError (const char* acFunction)
{
    GLenum code = glGetError();
    while (code != GL_NO_ERROR)
    {
        const char* errorString = GTGetErrorString(code);
        (void)errorString;
        assert(false);
        code = glGetError();
    }
    (void)acFunction;
}
//----------------------------------------------------------------------------

#pragma warning( pop )
