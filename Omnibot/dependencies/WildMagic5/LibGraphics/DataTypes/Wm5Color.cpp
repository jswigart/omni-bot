// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5Color.h"
#include "Wm5HalfFloat.h"
using namespace Wm5;

//----------------------------------------------------------------------------
// Convert from the specified format to 32-bit RGBA color.
//----------------------------------------------------------------------------
void Color::ConvertFromR5G6B5 (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const unsigned short* src = (const unsigned short*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++src, ++trg)
    {
        (*trg)[2] = (float)((*src & 0x001Fu));
        (*trg)[1] = (float)((*src & 0x07E0u) >>  5);
        (*trg)[0] = (float)((*src & 0xF800u) >> 11);
        (*trg)[3] = 0.0f;
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromA1R5G5B5 (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const unsigned short* src = (const unsigned short*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++src, ++trg)
    {
        (*trg)[2] = (float)((*src & 0x001Fu));
        (*trg)[1] = (float)((*src & 0x03E0u) >>  5);
        (*trg)[0] = (float)((*src & 0x7C00u) >> 10);
        (*trg)[3] = (float)((*src & 0x8000u) >> 15);
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromA4R4G4B4 (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const unsigned short* src = (const unsigned short*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++src, ++trg)
    {
        (*trg)[2] = (float)((*src & 0x000Fu));
        (*trg)[1] = (float)((*src & 0x00F0u) >>  4);
        (*trg)[0] = (float)((*src & 0x0F00u) >>  8);
        (*trg)[3] = (float)((*src & 0xF000u) >> 12);
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromA8 (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const unsigned char* src = (const unsigned char*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++trg)
    {
        (*trg)[0] = 0.0f;
        (*trg)[1] = 0.0f;
        (*trg)[2] = 0.0f;
        (*trg)[3] = (float)(*src++);
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromL8 (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const unsigned char* src = (const unsigned char*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++trg)
    {
        float luminance = (float)(*src++);
        (*trg)[0] = luminance;
        (*trg)[1] = luminance;
        (*trg)[2] = luminance;
        (*trg)[3] = 255.0f;
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromA8L8 (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const unsigned char* src = (const unsigned char*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++trg)
    {
        float luminance = (float)(*src++);
        float alpha = (float)(*src++);
        (*trg)[0] = luminance;
        (*trg)[1] = luminance;
        (*trg)[2] = luminance;
        (*trg)[3] = alpha;
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromR8G8B8 (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const unsigned char* src = (const unsigned char*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++trg)
    {
        (*trg)[2] = (float)(*src++);
        (*trg)[1] = (float)(*src++);
        (*trg)[0] = (float)(*src++);
        (*trg)[3] = 0.0f;
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromA8R8G8B8 (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const unsigned char* src = (const unsigned char*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++trg)
    {
        (*trg)[2] = (float)(*src++);
        (*trg)[1] = (float)(*src++);
        (*trg)[0] = (float)(*src++);
        (*trg)[3] = (float)(*src++);
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromA8B8G8R8 (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const unsigned char* src = (const unsigned char*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++trg)
    {
        (*trg)[0] = (float)(*src++);
        (*trg)[1] = (float)(*src++);
        (*trg)[2] = (float)(*src++);
        (*trg)[3] = (float)(*src++);
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromL16 (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const unsigned short* src = (const unsigned short*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++trg)
    {
        float luminance = (float)(*src++);
        (*trg)[0] = luminance;
        (*trg)[1] = luminance;
        (*trg)[2] = luminance;
        (*trg)[3] = 65535.0f;
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromG16R16 (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const unsigned short* src = (const unsigned short*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++trg)
    {
        (*trg)[0] = (float)(*src++);
        (*trg)[1] = (float)(*src++);
        (*trg)[2] = 0.0f;
        (*trg)[3] = 0.0f;
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromA16B16G16R16 (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const unsigned short* src = (const unsigned short*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++trg)
    {
        (*trg)[0] = (float)(*src++);
        (*trg)[1] = (float)(*src++);
        (*trg)[2] = (float)(*src++);
        (*trg)[3] = (float)(*src++);
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromR16F (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const HalfFloat* src = (const HalfFloat*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++trg)
    {
        (*trg)[0] = ToFloat(*src++);
        (*trg)[1] = 0.0f;
        (*trg)[2] = 0.0f;
        (*trg)[3] = 0.0f;
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromG16R16F (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const HalfFloat* src = (const HalfFloat*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++trg)
    {
        (*trg)[0] = ToFloat(*src++);
        (*trg)[1] = ToFloat(*src++);
        (*trg)[2] = 0.0f;
        (*trg)[3] = 0.0f;
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromA16B16G16R16F (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const HalfFloat* src = (const HalfFloat*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++trg)
    {
        (*trg)[0] = ToFloat(*src++);
        (*trg)[1] = ToFloat(*src++);
        (*trg)[2] = ToFloat(*src++);
        (*trg)[3] = ToFloat(*src++);
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromR32F (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const float* src = (const float*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++trg)
    {
        (*trg)[0] = *src++;
        (*trg)[1] = 0.0f;
        (*trg)[2] = 0.0f;
        (*trg)[3] = 0.0f;
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromG32R32F (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const float* src = (const float*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++trg)
    {
        (*trg)[0] = *src++;
        (*trg)[1] = *src++;
        (*trg)[2] = 0.0f;
        (*trg)[3] = 0.0f;
    }
}
//----------------------------------------------------------------------------
void Color::ConvertFromA32B32G32R32F (int numTexels, const char* inTexels,
    Float4* outTexels)
{
    const float* src = (const float*)inTexels;
    Float4* trg = outTexels;
    for (int i = 0; i < numTexels; ++i, ++trg)
    {
        (*trg)[0] = *src++;
        (*trg)[1] = *src++;
        (*trg)[2] = *src++;
        (*trg)[3] = *src++;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Convert to the specified format from 32-bit RGBA color.
//----------------------------------------------------------------------------
void Color::ConvertToR5G6B5 (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    unsigned short* trg = (unsigned short*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        unsigned short r = (unsigned short)(*src)[0];
        unsigned short g = (unsigned short)(*src)[1];
        unsigned short b = (unsigned short)(*src)[2];
        *trg++ = b | (g << 5) | (r << 11);
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToA1R5G5B5 (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    unsigned short* trg = (unsigned short*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        unsigned short r = (unsigned short)(*src)[0];
        unsigned short g = (unsigned short)(*src)[1];
        unsigned short b = (unsigned short)(*src)[2];
        unsigned short a = (unsigned short)(*src)[3];
        *trg++ = b | (g << 5) | (r << 10) | (a << 15);
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToA4R4G4B4 (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    unsigned short* trg = (unsigned short*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        unsigned short r = (unsigned short)(*src)[0];
        unsigned short g = (unsigned short)(*src)[1];
        unsigned short b = (unsigned short)(*src)[2];
        unsigned short a = (unsigned short)(*src)[3];
        *trg++ = b | (g << 4) | (r << 8) | (a << 12);
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToA8 (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    unsigned char* trg = (unsigned char*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        *trg++ = (unsigned char)(*src)[3];
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToL8 (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    unsigned char* trg = (unsigned char*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        *trg++ = (unsigned char)(*src)[0];
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToA8L8 (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    unsigned char* trg = (unsigned char*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        *trg++ = (unsigned char)(*src)[0];
        *trg++ = (unsigned char)(*src)[3];
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToR8G8B8 (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    unsigned char* trg = (unsigned char*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        *trg++ = (unsigned char)(*src)[2];
        *trg++ = (unsigned char)(*src)[1];
        *trg++ = (unsigned char)(*src)[0];
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToA8R8G8B8 (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    unsigned char* trg = (unsigned char*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        *trg++ = (unsigned char)(*src)[2];
        *trg++ = (unsigned char)(*src)[1];
        *trg++ = (unsigned char)(*src)[0];
        *trg++ = (unsigned char)(*src)[3];
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToA8B8G8R8 (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    unsigned char* trg = (unsigned char*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        *trg++ = (unsigned char)(*src)[0];
        *trg++ = (unsigned char)(*src)[1];
        *trg++ = (unsigned char)(*src)[2];
        *trg++ = (unsigned char)(*src)[3];
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToL16 (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    unsigned short* trg = (unsigned short*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        *trg++ = (unsigned short)(*src)[0];
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToG16R16 (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    unsigned short* trg = (unsigned short*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        *trg++ = (unsigned short)(*src)[0];
        *trg++ = (unsigned short)(*src)[1];
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToA16B16G16R16 (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    unsigned short* trg = (unsigned short*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        *trg++ = (unsigned short)(*src)[0];
        *trg++ = (unsigned short)(*src)[1];
        *trg++ = (unsigned short)(*src)[2];
        *trg++ = (unsigned short)(*src)[3];
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToR16F (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    HalfFloat* trg = (HalfFloat*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        *trg++ = ToHalf((*src)[0]);
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToG16R16F (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    HalfFloat* trg = (HalfFloat*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        *trg++ = ToHalf((*src)[0]);
        *trg++ = ToHalf((*src)[1]);
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToA16B16G16R16F (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    HalfFloat* trg = (HalfFloat*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        *trg++ = ToHalf((*src)[0]);
        *trg++ = ToHalf((*src)[1]);
        *trg++ = ToHalf((*src)[2]);
        *trg++ = ToHalf((*src)[3]);
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToR32F (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    float* trg = (float*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        *trg++ = (*src)[0];
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToG32R32F (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    float* trg = (float*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        *trg++ = (*src)[0];
        *trg++ = (*src)[1];
    }
}
//----------------------------------------------------------------------------
void Color::ConvertToA32B32G32R32F (int numTexels, const Float4* inTexels,
    char* outTexels)
{
    const Float4* src = inTexels;
    float* trg = (float*)outTexels;
    for (int i = 0; i < numTexels; ++i, ++src)
    {
        *trg++ = (*src)[0];
        *trg++ = (*src)[1];
        *trg++ = (*src)[2];
        *trg++ = (*src)[3];
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
Color::ConvertFromFunction Color::FromFunction[CF_QUANTITY] =
{
    0,
    &ConvertFromR5G6B5,
    &ConvertFromA1R5G5B5,
    &ConvertFromA4R4G4B4,
    &ConvertFromA8,
    &ConvertFromL8,
    &ConvertFromA8L8,
    &ConvertFromR8G8B8,
    &ConvertFromA8R8G8B8,
    &ConvertFromA8B8G8R8,
    &ConvertFromL16,
    &ConvertFromG16R16,
    &ConvertFromA16B16G16R16,
    &ConvertFromR16F,
    &ConvertFromG16R16F,
    &ConvertFromA16B16G16R16F,
    &ConvertFromR32F,
    &ConvertFromG32R32F,
    &ConvertFromA32B32G32R32F,
    0,
    0,
    0,
    0
};
//----------------------------------------------------------------------------
Color::ConvertToFunction Color::ToFunction[CF_QUANTITY] =
{
    0,
    &ConvertToR5G6B5,
    &ConvertToA1R5G5B5,
    &ConvertToA4R4G4B4,
    &ConvertToA8,
    &ConvertToL8,
    &ConvertToA8L8,
    &ConvertToR8G8B8,
    &ConvertToA8R8G8B8,
    &ConvertToA8B8G8R8,
    &ConvertToL16,
    &ConvertToG16R16,
    &ConvertToA16B16G16R16,
    &ConvertToR16F,
    &ConvertToG16R16F,
    &ConvertToA16B16G16R16F,
    &ConvertToR32F,
    &ConvertToG32R32F,
    &ConvertToA32B32G32R32F,
    0,
    0,
    0,
    0
};
//----------------------------------------------------------------------------
