// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline unsigned short GetColor16 (unsigned char r, unsigned char g,
    unsigned char b)
{
    return (unsigned short)((unsigned(b) >> 3) 
        |  ((unsigned(g) >> 3) << 5 ) 
        |  ((unsigned(r) >> 3) << 10));
}
//----------------------------------------------------------------------------
inline unsigned char GetBlue16 (unsigned short color)
{ 
    return (unsigned char)(color & 0x001F); 
}
//----------------------------------------------------------------------------
inline unsigned char GetGreen16 (unsigned short color)
{ 
    return (unsigned char)((color & 0x03E0) >> 5); 
}
//----------------------------------------------------------------------------
inline unsigned char GetRed16 (unsigned short color)
{ 
    return (unsigned char)((color & 0x7C00) >> 10);
}
//----------------------------------------------------------------------------
inline unsigned int GetColor24 (unsigned char r, unsigned char g,
    unsigned char b)
{
    return (unsigned(b))
        |  ((unsigned(g)) << 8)
        |  ((unsigned(r)) << 16);
}
//----------------------------------------------------------------------------
inline unsigned char GetBlue24 (unsigned int color)
{ 
    return (unsigned char)(color & 0x000000FF); 
}
//----------------------------------------------------------------------------
inline unsigned char GetGreen24 (unsigned int color)
{ 
    return (unsigned char)((color & 0x0000FF00) >> 8); 
}
//----------------------------------------------------------------------------
inline unsigned char GetRed24 (unsigned int color)
{ 
    return (unsigned char)((color & 0x00FF0000) >> 16);
}
//----------------------------------------------------------------------------
