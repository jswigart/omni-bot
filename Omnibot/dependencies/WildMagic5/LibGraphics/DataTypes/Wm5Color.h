// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5COLOR_H
#define WM5COLOR_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Float4.h"

// The Color class contains only static functions.

namespace Wm5
{
class WM5_GRAPHICS_ITEM Color
{
public:
    // Make a 32-bit RGB color from 8-bit channels.  The alpha channel is
    // set to 255.
    inline static unsigned int MakeR8G8B8 (unsigned char red,
        unsigned char green, unsigned char blue);

    // Make a 32-bit RGB color from 8-bit channels.
    inline static unsigned int MakeR8G8B8A8 (unsigned char red,
        unsigned char green, unsigned char blue, unsigned char alpha);

    // Extract 8-bit channels from a 32-bit RGBA color.  The alpha channel
    // is ignored.
    inline static void ExtractR8G8B8 (unsigned int color,
        unsigned char& red, unsigned char& green, unsigned char& blue);

    // Extract 8-bit channels from a 32-bit RGBA color.
    inline static void ExtractR8G8B8A8 (unsigned int color,
        unsigned char& red, unsigned char& green, unsigned char& blue,
        unsigned char& alpha);

    enum
    {
        CF_QUANTITY = 23  // Same as Texture::TF_QUANTITY
    };

    // Convert from the specified format to 32-bit RGBA.
    static void ConvertFromR5G6B5 (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromA1R5G5B5 (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromA4R4G4B4 (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromA8 (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromL8 (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromA8L8 (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromR8G8B8 (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromA8R8G8B8 (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromA8B8G8R8 (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromL16 (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromG16R16 (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromA16B16G16R16 (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromR16F (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromG16R16F (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromA16B16G16R16F (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromR32F (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromG32R32F (int numTexels, const char* inTexels,
        Float4* outTexels);

    static void ConvertFromA32B32G32R32F (int numTexels, const char* inTexels,
        Float4* outTexels);

    // Convert to the specified format from 32-bit RGBA.
    static void ConvertToR5G6B5 (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToA1R5G5B5 (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToA4R4G4B4 (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToA8 (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToL8 (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToA8L8 (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToR8G8B8 (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToA8R8G8B8 (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToA8B8G8R8 (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToL16 (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToG16R16 (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToA16B16G16R16 (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToR16F (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToG16R16F (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToA16B16G16R16F (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToR32F (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToG32R32F (int numTexels, const Float4* inTexels,
        char* outTexels);

    static void ConvertToA32B32G32R32F (int numTexels, const Float4* inTexels,
        char* outTexels);

    // For the convenience of applications when looking up functions.
    typedef void (*ConvertFromFunction)(int, const char*, Float4*);
    static ConvertFromFunction FromFunction[CF_QUANTITY];

    typedef void (*ConvertToFunction)(int, const Float4*, char*);
    static ConvertToFunction ToFunction[CF_QUANTITY];
};

#include "Wm5Color.inl"
}

#endif
