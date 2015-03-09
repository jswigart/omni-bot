// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5HALFFLOAT_H
#define WM5HALFFLOAT_H

#include "Wm5GraphicsLIB.h"

namespace Wm5
{

//----------------------------------------------------------------------------
// A 16-bit floating-point number has a 1-bit sign (S), a 5-bit
// exponent (E), and a 10-bit mantissa (M).  The value of a 16-bit
// floating-point number is determined by the following:
//    (-1)^S * 0.0,                        if E == 0 and M == 0,
//    (-1)^S * 2^-14 * (M / 2^10),         if E == 0 and M != 0,
//    (-1)^S * 2^(E-15) * (1 + M/2^10),    if 0 < E < 31,
//    (-1)^S * INF,                        if E == 31 and M == 0, or
//    NaN,                                 if E == 31 and M != 0,
// where
//    S = floor((N mod 65536) / 32768),
//    E = floor((N mod 32768) / 1024), and
//    M = N mod 1024.
// Implementations are also allowed to use any of the following
// alternative encodings:
//    (-1)^S * 0.0,                        if E == 0 and M != 0,
//    (-1)^S * 2^(E-15) * (1 + M/2^10),    if E == 31 and M == 0, or
//    (-1)^S * 2^(E-15) * (1 + M/2^10),    if E == 31 and M != 0,
//
// This table explains the conversions in ToHalf and ToFloat.
// ---------------------------
// bias16  exp32  bias32
// ---------------------------
//     0    -127      0 (0x00)
//     :       :      :
//     0     -15    112 (0x70)
// ---------------------------
//     1     -14    113 (0x71)
//     :       :      :
//    15       0    127 (0x7F)
//     :       :      :
//    30      15    142 (0x8E)
// ---------------------------
//    31      16    143 (0x8F)
//     :       :      :
//    31     128    255 (0xFF)
// ---------------------------
//----------------------------------------------------------------------------

typedef unsigned short HalfFloat;

WM5_GRAPHICS_ITEM HalfFloat ToHalf (float value);
WM5_GRAPHICS_ITEM float ToFloat (HalfFloat value);

}

#endif
