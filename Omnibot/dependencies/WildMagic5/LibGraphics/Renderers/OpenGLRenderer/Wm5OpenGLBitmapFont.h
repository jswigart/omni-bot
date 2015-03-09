// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5OPENGLBITMAPFONT_H
#define WM5OPENGLBITMAPFONT_H

#include "Wm5OpenGLRendererLIB.h"

namespace Wm5
{

class BitmapFontChar
{
public:
    BitmapFontChar (int xOrigin, int yOrigin, int xSize, int ySize,
        const unsigned char* bitmap)
    {
        mXOrigin = xOrigin;
        mYOrigin = yOrigin;
        mXSize = xSize;
        mYSize = ySize;
        mBitmap = bitmap;
    }

    int mXOrigin, mYOrigin, mXSize, mYSize;
    const unsigned char* mBitmap;
};

class BitmapFont
{
public:
    BitmapFont (const char* name, int numCharacters,
        const BitmapFontChar* const* characters)
    {
        mName = name;
        mNumCharacters = numCharacters;
        mCharacters = characters;
    }

    const char* mName;
    int mNumCharacters;
    const BitmapFontChar* const* mCharacters;
};

extern const BitmapFont gVerdanaS16B0I0;

}

#endif
