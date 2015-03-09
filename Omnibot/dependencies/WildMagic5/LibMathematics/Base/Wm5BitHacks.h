// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

// Bit hacks are available at
//     http://graphics.stanford.edu/~seander/bithacks.html

#ifndef WM5BITHACKS_H
#define WM5BITHACKS_H

#include "Wm5MathematicsLIB.h"

namespace Wm5
{

WM5_MATHEMATICS_ITEM bool IsPowerOfTwo (unsigned int value);
WM5_MATHEMATICS_ITEM bool IsPowerOfTwo (int value);

WM5_MATHEMATICS_ITEM unsigned int Log2OfPowerOfTwo (unsigned int powerOfTwo);
WM5_MATHEMATICS_ITEM int Log2OfPowerOfTwo (int powerOfTwo);

// Fast conversion from a IEEE 32-bit floating point number F in [0,1] to a
// a 32-bit integer I in [0,2^P-1].  The input 'value' is F, the input
// 'power' is P, and the return value of the function is I.
inline int ScaledFloatToInt (float value, int power);

#include "Wm5BitHacks.inl"

}

#endif
