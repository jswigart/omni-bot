// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5UTILITY_H
#define WM5UTILITY_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Float4.h"

// The Utility class contains only static functions.

namespace Wm5
{
class WM5_GRAPHICS_ITEM Utility
{
public:
    static void ReflectX (int xSize, int ySize, int numBytesPerPixel,
        char* pixels);

    static void ReflectY (int xSize, int ySize, int numBytesPerPixel,
        char* pixels);
};

}

#endif
