// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5BitHacks.h"

namespace Wm5
{
//----------------------------------------------------------------------------
bool IsPowerOfTwo (unsigned int value)
{
    return (value > 0) && ((value & (value - 1)) == 0);
}
//----------------------------------------------------------------------------
bool IsPowerOfTwo (int value)
{
    return (value > 0) && ((value & (value - 1)) == 0);
}
//----------------------------------------------------------------------------
unsigned int Log2OfPowerOfTwo (unsigned int powerOfTwo)
{
    unsigned int log2 = (powerOfTwo & 0xAAAAAAAA) != 0;
    log2 |= ((powerOfTwo & 0xFFFF0000) != 0) << 4;
    log2 |= ((powerOfTwo & 0xFF00FF00) != 0) << 3;
    log2 |= ((powerOfTwo & 0xF0F0F0F0) != 0) << 2;
    log2 |= ((powerOfTwo & 0xCCCCCCCC) != 0) << 1;
    return log2;
}
//----------------------------------------------------------------------------
int Log2OfPowerOfTwo (int powerOfTwo)
{
    unsigned int log2 = (powerOfTwo & 0xAAAAAAAA) != 0;
    log2 |= ((powerOfTwo & 0xFFFF0000) != 0) << 4;
    log2 |= ((powerOfTwo & 0xFF00FF00) != 0) << 3;
    log2 |= ((powerOfTwo & 0xF0F0F0F0) != 0) << 2;
    log2 |= ((powerOfTwo & 0xCCCCCCCC) != 0) << 1;
    return (int)log2;
}
//----------------------------------------------------------------------------
}
