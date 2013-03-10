// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/05/07)

#include "Wm5CorePCH.h"
#include "Wm5Stream.h"
using namespace Wm5;

//----------------------------------------------------------------------------
Stream::Stream ()
{
}
//----------------------------------------------------------------------------
Stream::~Stream ()
{
}
//----------------------------------------------------------------------------
int Stream::GetStreamingSize (const std::string& input)
{
    // The string is written to disk, length first and characters second,
    // but with no terminating null character.  It is padded to a multiple of
    // four bytes.
    int length = (int)input.length();
    int padding = 0;
    if (length > 0)
    {
        padding = (length % 4);
        if (padding > 0)
        {
            padding = 4 - padding;
        }
    }
    return sizeof(int) + length*sizeof(char) + padding;
}
//----------------------------------------------------------------------------
