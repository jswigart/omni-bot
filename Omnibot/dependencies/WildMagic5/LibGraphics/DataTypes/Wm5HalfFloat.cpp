// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5GraphicsPCH.h"
#include "Wm5HalfFloat.h"

namespace Wm5
{
//----------------------------------------------------------------------------
HalfFloat ToHalf (float value)
{
    unsigned int bits = *(unsigned int*)&value;
    unsigned short biasExp = (unsigned short)((bits & 0x7F800000) >> 23);
    if (biasExp >= 0x0071)
    {
        if (biasExp <= 0x008E)
        {
            if (biasExp != 0)
            {
                // Truncate 23-bit mantissa to 10 bits.
                unsigned short signBit =
                    (unsigned short)((bits & 0x80000000) >> 16);
                unsigned int mantissa = (bits & 0x007FFFFF) >> 13;
                biasExp = (biasExp - 0x0070) << 10;
                return (HalfFloat)(signBit | biasExp | mantissa);
            }
            else
            {
                // E = 0 (alternate encoding of zero, M does not matter)
                return (unsigned short)0;
            }
        }
        else
        {
            // E = 30, M = 1023 (largest magnitude half-float)
            unsigned short signBit =
                (unsigned short)((bits & 0x80000000) >> 16);
            return signBit | (unsigned short)0x7BFF;
        }
    }
    else
    {
        // E = 1, M = 0 (smallest magnitude half-float)
        unsigned short signBit = (unsigned short)((bits & 0x80000000) >> 16);
        return signBit | (unsigned short)0x0400;
    }
}
//----------------------------------------------------------------------------
float ToFloat (HalfFloat value)
{
    unsigned int biasExp  = (unsigned int)(value & 0x7C00) >> 10;
    if (biasExp != 0)
    {
        unsigned int signBit  = (unsigned int)(value & 0x8000) << 16;
        unsigned int mantissa = (unsigned int)(value & 0x03FF) << 13;
        biasExp = (biasExp + 0x0070) << 23;
        unsigned int result = signBit | biasExp | mantissa;
        return *(float*)&result;
    }
    else
    {
        // E = 0 (alternate encoding of zero, M does not matter)
        return 0.0f;
    }
}
//----------------------------------------------------------------------------

}
