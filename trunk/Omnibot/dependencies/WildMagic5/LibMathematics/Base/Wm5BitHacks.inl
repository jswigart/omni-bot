// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int ScaledFloatToInt (float value, int power)
{
    int result;

    int shift = 150 - power - ((*(int*)(&value) >> 23) & 0xFF);
    if (shift < 24)
    {
        result = ((*(int*)(&value) & 0x007FFFFF) | 0x00800000) >> shift;
        if (result == (1 << power))
        {
            --result;
        }
    }
    else
    {
        result = 0;
    }

    return result;
}
//----------------------------------------------------------------------------
