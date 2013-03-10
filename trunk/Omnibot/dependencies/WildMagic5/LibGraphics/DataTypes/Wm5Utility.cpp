// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Utility.h"
using namespace Wm5;

//----------------------------------------------------------------------------
void Utility::ReflectX (int xSize, int ySize, int numBytesPerPixel,
    char* pixels)
{
    const int xMax = xSize/2;
    const int xSizeM1 = xSize - 1;
    for (int x = 0; x < xMax; ++x)
    {
        int reflectX = xSizeM1 - x;
        for (int y = 0; y < ySize; ++y)
        {
            int src = numBytesPerPixel*(x + xSize*y);
            int trg = numBytesPerPixel*(reflectX + xSize*y);
            for (int i = 0; i < numBytesPerPixel; ++i)
            {
                char save = pixels[src + i];
                pixels[src + i] = pixels[trg + i];
                pixels[trg + i] = save;
            }
        }
    }
}
//----------------------------------------------------------------------------
void Utility::ReflectY (int xSize, int ySize, int numBytesPerPixel,
    char* pixels)
{
    const int yMax = ySize/2;
    const int ySizeM1 = ySize - 1;
    for (int y = 0; y < yMax; ++y)
    {
        int reflectY = ySizeM1 - y;
        for (int x = 0; x < xSize; ++x)
        {
            int src = numBytesPerPixel*(x + xSize*y);
            int trg = numBytesPerPixel*(x + xSize*reflectY);
            for (int i = 0; i < numBytesPerPixel; ++i)
            {
                char save = pixels[src + i];
                pixels[src + i] = pixels[trg + i];
                pixels[trg + i] = save;
            }
        }
    }
}
//----------------------------------------------------------------------------
