// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5ImagicsPCH.h"
#include "Wm5Element.h"
#include "Wm5ImageConvert.h"

namespace Wm5
{
//----------------------------------------------------------------------------
#define WM5_IMAGE_CONVERTER(SrcType,TrgType) \
static void SrcType##To##TrgType (int quantity, void* src, void* trg) \
{ \
    SrcType* tSrc = (SrcType*)src; \
    TrgType* tTrg = (TrgType*)trg; \
    for (int i = 0; i < quantity; ++i, ++tSrc, ++tTrg) \
    { \
        *tTrg = (TrgType)(*tSrc); \
    } \
}
//----------------------------------------------------------------------------
WM5_IMAGE_CONVERTER(char, uchar)
WM5_IMAGE_CONVERTER(char, short)
WM5_IMAGE_CONVERTER(char, ushort)
WM5_IMAGE_CONVERTER(char, int)
WM5_IMAGE_CONVERTER(char, uint)
WM5_IMAGE_CONVERTER(char, long)
WM5_IMAGE_CONVERTER(char, ulong)
WM5_IMAGE_CONVERTER(char, float)
WM5_IMAGE_CONVERTER(char, double)
WM5_IMAGE_CONVERTER(char, rgb5)
WM5_IMAGE_CONVERTER(char, rgb8)

WM5_IMAGE_CONVERTER(uchar, char)
WM5_IMAGE_CONVERTER(uchar, short)
WM5_IMAGE_CONVERTER(uchar, ushort)
WM5_IMAGE_CONVERTER(uchar, int)
WM5_IMAGE_CONVERTER(uchar, uint)
WM5_IMAGE_CONVERTER(uchar, long)
WM5_IMAGE_CONVERTER(uchar, ulong)
WM5_IMAGE_CONVERTER(uchar, float)
WM5_IMAGE_CONVERTER(uchar, double)
WM5_IMAGE_CONVERTER(uchar, rgb5)
WM5_IMAGE_CONVERTER(uchar, rgb8)

WM5_IMAGE_CONVERTER(short, char)
WM5_IMAGE_CONVERTER(short, uchar)
WM5_IMAGE_CONVERTER(short, ushort)
WM5_IMAGE_CONVERTER(short, int)
WM5_IMAGE_CONVERTER(short, uint)
WM5_IMAGE_CONVERTER(short, long)
WM5_IMAGE_CONVERTER(short, ulong)
WM5_IMAGE_CONVERTER(short, float)
WM5_IMAGE_CONVERTER(short, double)
WM5_IMAGE_CONVERTER(short, rgb5)
WM5_IMAGE_CONVERTER(short, rgb8)

WM5_IMAGE_CONVERTER(ushort, char)
WM5_IMAGE_CONVERTER(ushort, uchar)
WM5_IMAGE_CONVERTER(ushort, short)
WM5_IMAGE_CONVERTER(ushort, int)
WM5_IMAGE_CONVERTER(ushort, uint)
WM5_IMAGE_CONVERTER(ushort, long)
WM5_IMAGE_CONVERTER(ushort, ulong)
WM5_IMAGE_CONVERTER(ushort, float)
WM5_IMAGE_CONVERTER(ushort, double)
WM5_IMAGE_CONVERTER(ushort, rgb5)
WM5_IMAGE_CONVERTER(ushort, rgb8)

WM5_IMAGE_CONVERTER(int, char)
WM5_IMAGE_CONVERTER(int, uchar)
WM5_IMAGE_CONVERTER(int, short)
WM5_IMAGE_CONVERTER(int, ushort)
WM5_IMAGE_CONVERTER(int, uint)
WM5_IMAGE_CONVERTER(int, long)
WM5_IMAGE_CONVERTER(int, ulong)
WM5_IMAGE_CONVERTER(int, float)
WM5_IMAGE_CONVERTER(int, double)
WM5_IMAGE_CONVERTER(int, rgb5)
WM5_IMAGE_CONVERTER(int, rgb8)

WM5_IMAGE_CONVERTER(uint, char)
WM5_IMAGE_CONVERTER(uint, uchar)
WM5_IMAGE_CONVERTER(uint, short)
WM5_IMAGE_CONVERTER(uint, ushort)
WM5_IMAGE_CONVERTER(uint, int)
WM5_IMAGE_CONVERTER(uint, long)
WM5_IMAGE_CONVERTER(uint, ulong)
WM5_IMAGE_CONVERTER(uint, float)
WM5_IMAGE_CONVERTER(uint, double)
WM5_IMAGE_CONVERTER(uint, rgb5)
WM5_IMAGE_CONVERTER(uint, rgb8)

WM5_IMAGE_CONVERTER(long, char)
WM5_IMAGE_CONVERTER(long, uchar)
WM5_IMAGE_CONVERTER(long, short)
WM5_IMAGE_CONVERTER(long, ushort)
WM5_IMAGE_CONVERTER(long, int)
WM5_IMAGE_CONVERTER(long, uint)
WM5_IMAGE_CONVERTER(long, ulong)
WM5_IMAGE_CONVERTER(long, float)
WM5_IMAGE_CONVERTER(long, double)
WM5_IMAGE_CONVERTER(long, rgb5)
WM5_IMAGE_CONVERTER(long, rgb8)

WM5_IMAGE_CONVERTER(ulong, char)
WM5_IMAGE_CONVERTER(ulong, uchar)
WM5_IMAGE_CONVERTER(ulong, short)
WM5_IMAGE_CONVERTER(ulong, ushort)
WM5_IMAGE_CONVERTER(ulong, int)
WM5_IMAGE_CONVERTER(ulong, uint)
WM5_IMAGE_CONVERTER(ulong, long)
WM5_IMAGE_CONVERTER(ulong, float)
WM5_IMAGE_CONVERTER(ulong, double)
WM5_IMAGE_CONVERTER(ulong, rgb5)
WM5_IMAGE_CONVERTER(ulong, rgb8)

WM5_IMAGE_CONVERTER(float, char)
WM5_IMAGE_CONVERTER(float, uchar)
WM5_IMAGE_CONVERTER(float, short)
WM5_IMAGE_CONVERTER(float, ushort)
WM5_IMAGE_CONVERTER(float, int)
WM5_IMAGE_CONVERTER(float, uint)
WM5_IMAGE_CONVERTER(float, long)
WM5_IMAGE_CONVERTER(float, ulong)
WM5_IMAGE_CONVERTER(float, double)
WM5_IMAGE_CONVERTER(float, rgb5)
WM5_IMAGE_CONVERTER(float, rgb8)

WM5_IMAGE_CONVERTER(double, char)
WM5_IMAGE_CONVERTER(double, uchar)
WM5_IMAGE_CONVERTER(double, short)
WM5_IMAGE_CONVERTER(double, ushort)
WM5_IMAGE_CONVERTER(double, int)
WM5_IMAGE_CONVERTER(double, uint)
WM5_IMAGE_CONVERTER(double, long)
WM5_IMAGE_CONVERTER(double, ulong)
WM5_IMAGE_CONVERTER(double, float)
WM5_IMAGE_CONVERTER(double, rgb5)
WM5_IMAGE_CONVERTER(double, rgb8)

WM5_IMAGE_CONVERTER(rgb5, char)
WM5_IMAGE_CONVERTER(rgb5, uchar)
WM5_IMAGE_CONVERTER(rgb5, short)
WM5_IMAGE_CONVERTER(rgb5, ushort)
WM5_IMAGE_CONVERTER(rgb5, int)
WM5_IMAGE_CONVERTER(rgb5, uint)
WM5_IMAGE_CONVERTER(rgb5, long)
WM5_IMAGE_CONVERTER(rgb5, ulong)
WM5_IMAGE_CONVERTER(rgb5, float)
WM5_IMAGE_CONVERTER(rgb5, double)
WM5_IMAGE_CONVERTER(rgb5, rgb8)

WM5_IMAGE_CONVERTER(rgb8, char)
WM5_IMAGE_CONVERTER(rgb8, uchar)
WM5_IMAGE_CONVERTER(rgb8, short)
WM5_IMAGE_CONVERTER(rgb8, ushort)
WM5_IMAGE_CONVERTER(rgb8, int)
WM5_IMAGE_CONVERTER(rgb8, uint)
WM5_IMAGE_CONVERTER(rgb8, long)
WM5_IMAGE_CONVERTER(rgb8, ulong)
WM5_IMAGE_CONVERTER(rgb8, float)
WM5_IMAGE_CONVERTER(rgb8, double)
WM5_IMAGE_CONVERTER(rgb8, rgb5)
//----------------------------------------------------------------------------
#define WM5_IMAGE_ENTRY(SrcType,TrgType) SrcType##To##TrgType
//----------------------------------------------------------------------------
typedef void (*Converter)(int,void*,void*);
static Converter gsConvert[gNumImageElements][gNumImageElements] =
{
    { 0,
      WM5_IMAGE_ENTRY(char, uchar),
      WM5_IMAGE_ENTRY(char, short),
      WM5_IMAGE_ENTRY(char, ushort),
      WM5_IMAGE_ENTRY(char, int),
      WM5_IMAGE_ENTRY(char, uint),
      WM5_IMAGE_ENTRY(char, long),
      WM5_IMAGE_ENTRY(char, ulong),
      WM5_IMAGE_ENTRY(char, float),
      WM5_IMAGE_ENTRY(char, double),
      WM5_IMAGE_ENTRY(char, rgb5),
      WM5_IMAGE_ENTRY(char, rgb8) },

    { WM5_IMAGE_ENTRY(uchar, char),
      0,
      WM5_IMAGE_ENTRY(uchar, short),
      WM5_IMAGE_ENTRY(uchar, ushort),
      WM5_IMAGE_ENTRY(uchar, int),
      WM5_IMAGE_ENTRY(uchar, uint),
      WM5_IMAGE_ENTRY(uchar, long),
      WM5_IMAGE_ENTRY(uchar, ulong),
      WM5_IMAGE_ENTRY(uchar, float),
      WM5_IMAGE_ENTRY(uchar, double),
      WM5_IMAGE_ENTRY(uchar, rgb5),
      WM5_IMAGE_ENTRY(uchar, rgb8) },

    { WM5_IMAGE_ENTRY(short, char),
      WM5_IMAGE_ENTRY(short, uchar),
      0,
      WM5_IMAGE_ENTRY(short, ushort),
      WM5_IMAGE_ENTRY(short, int),
      WM5_IMAGE_ENTRY(short, uint),
      WM5_IMAGE_ENTRY(short, long),
      WM5_IMAGE_ENTRY(short, ulong),
      WM5_IMAGE_ENTRY(short, float),
      WM5_IMAGE_ENTRY(short, double),
      WM5_IMAGE_ENTRY(short, rgb5),
      WM5_IMAGE_ENTRY(short, rgb8) },

    { WM5_IMAGE_ENTRY(ushort, char),
      WM5_IMAGE_ENTRY(ushort, uchar),
      WM5_IMAGE_ENTRY(ushort, short),
      0,
      WM5_IMAGE_ENTRY(ushort, int),
      WM5_IMAGE_ENTRY(ushort, uint),
      WM5_IMAGE_ENTRY(ushort, long),
      WM5_IMAGE_ENTRY(ushort, ulong),
      WM5_IMAGE_ENTRY(ushort, float),
      WM5_IMAGE_ENTRY(ushort, double),
      WM5_IMAGE_ENTRY(ushort, rgb5),
      WM5_IMAGE_ENTRY(ushort, rgb8) },

    { WM5_IMAGE_ENTRY(int, char),
      WM5_IMAGE_ENTRY(int, uchar),
      WM5_IMAGE_ENTRY(int, short),
      WM5_IMAGE_ENTRY(int, ushort),
      0,
      WM5_IMAGE_ENTRY(int, uint),
      WM5_IMAGE_ENTRY(int, long),
      WM5_IMAGE_ENTRY(int, ulong),
      WM5_IMAGE_ENTRY(int, float),
      WM5_IMAGE_ENTRY(int, double),
      WM5_IMAGE_ENTRY(int, rgb5),
      WM5_IMAGE_ENTRY(int, rgb8) },

    { WM5_IMAGE_ENTRY(uint, char),
      WM5_IMAGE_ENTRY(uint, uchar),
      WM5_IMAGE_ENTRY(uint, short),
      WM5_IMAGE_ENTRY(uint, ushort),
      WM5_IMAGE_ENTRY(uint, int),
      0,
      WM5_IMAGE_ENTRY(uint, long),
      WM5_IMAGE_ENTRY(uint, ulong),
      WM5_IMAGE_ENTRY(uint, float),
      WM5_IMAGE_ENTRY(uint, double),
      WM5_IMAGE_ENTRY(uint, rgb5),
      WM5_IMAGE_ENTRY(uint, rgb8) },

    { WM5_IMAGE_ENTRY(long, char),
      WM5_IMAGE_ENTRY(long, uchar),
      WM5_IMAGE_ENTRY(long, short),
      WM5_IMAGE_ENTRY(long, ushort),
      WM5_IMAGE_ENTRY(long, int),
      WM5_IMAGE_ENTRY(long, uint),
      0,
      WM5_IMAGE_ENTRY(long, ulong),
      WM5_IMAGE_ENTRY(long, float),
      WM5_IMAGE_ENTRY(long, double),
      WM5_IMAGE_ENTRY(long, rgb5),
      WM5_IMAGE_ENTRY(long, rgb8) },

    { WM5_IMAGE_ENTRY(ulong, char),
      WM5_IMAGE_ENTRY(ulong, uchar),
      WM5_IMAGE_ENTRY(ulong, short),
      WM5_IMAGE_ENTRY(ulong, ushort),
      WM5_IMAGE_ENTRY(ulong, int),
      WM5_IMAGE_ENTRY(ulong, uint),
      WM5_IMAGE_ENTRY(ulong, long),
      0,
      WM5_IMAGE_ENTRY(ulong, float),
      WM5_IMAGE_ENTRY(ulong, double),
      WM5_IMAGE_ENTRY(ulong, rgb5),
      WM5_IMAGE_ENTRY(ulong, rgb8) },

    { WM5_IMAGE_ENTRY(float, char),
      WM5_IMAGE_ENTRY(float, uchar),
      WM5_IMAGE_ENTRY(float, short),
      WM5_IMAGE_ENTRY(float, ushort),
      WM5_IMAGE_ENTRY(float, int),
      WM5_IMAGE_ENTRY(float, uint),
      WM5_IMAGE_ENTRY(float, long),
      WM5_IMAGE_ENTRY(float, ulong),
      0,
      WM5_IMAGE_ENTRY(float, double),
      WM5_IMAGE_ENTRY(float, rgb5),
      WM5_IMAGE_ENTRY(float, rgb8) },

    { WM5_IMAGE_ENTRY(double, char),
      WM5_IMAGE_ENTRY(double, uchar),
      WM5_IMAGE_ENTRY(double, short),
      WM5_IMAGE_ENTRY(double, ushort),
      WM5_IMAGE_ENTRY(double, int),
      WM5_IMAGE_ENTRY(double, uint),
      WM5_IMAGE_ENTRY(double, long),
      WM5_IMAGE_ENTRY(double, ulong),
      WM5_IMAGE_ENTRY(double, float),
      0,
      WM5_IMAGE_ENTRY(double, rgb5),
      WM5_IMAGE_ENTRY(double, rgb8) },

    { WM5_IMAGE_ENTRY(rgb5, char),
      WM5_IMAGE_ENTRY(rgb5, uchar),
      WM5_IMAGE_ENTRY(rgb5, short),
      WM5_IMAGE_ENTRY(rgb5, ushort),
      WM5_IMAGE_ENTRY(rgb5, int),
      WM5_IMAGE_ENTRY(rgb5, uint),
      WM5_IMAGE_ENTRY(rgb5, long),
      WM5_IMAGE_ENTRY(rgb5, ulong),
      WM5_IMAGE_ENTRY(rgb5, float),
      WM5_IMAGE_ENTRY(rgb5, double),
      0,
      WM5_IMAGE_ENTRY(rgb5, rgb8) },

    { WM5_IMAGE_ENTRY(rgb8, char),
      WM5_IMAGE_ENTRY(rgb8, uchar),
      WM5_IMAGE_ENTRY(rgb8, short),
      WM5_IMAGE_ENTRY(rgb8, ushort),
      WM5_IMAGE_ENTRY(rgb8, int),
      WM5_IMAGE_ENTRY(rgb8, uint),
      WM5_IMAGE_ENTRY(rgb8, long),
      WM5_IMAGE_ENTRY(rgb8, ulong),
      WM5_IMAGE_ENTRY(rgb8, float),
      WM5_IMAGE_ENTRY(rgb8, double),
      WM5_IMAGE_ENTRY(rgb8, rgb5),
      0 }
};
//----------------------------------------------------------------------------
void ImageConvert (int quantity, int srcRTTI, void* srcData,
    int trgRTTI, void* trgData)
{
    Converter convert = gsConvert[srcRTTI][trgRTTI];
    convert(quantity, srcData, trgData);
}
//----------------------------------------------------------------------------
}
