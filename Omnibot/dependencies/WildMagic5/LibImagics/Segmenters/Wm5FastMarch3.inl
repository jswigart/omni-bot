// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int FastMarch3::GetXBound () const
{
    return mXBound;
}
//----------------------------------------------------------------------------
inline int FastMarch3::GetYBound () const
{
    return mYBound;
}
//----------------------------------------------------------------------------
inline int FastMarch3::GetZBound () const
{
    return mZBound;
}
//----------------------------------------------------------------------------
inline float FastMarch3::GetXSpacing () const
{
    return mXSpacing;
}
//----------------------------------------------------------------------------
inline float FastMarch3::GetYSpacing () const
{
    return mYSpacing;
}
//----------------------------------------------------------------------------
inline float FastMarch3::GetZSpacing () const
{
    return mZSpacing;
}
//----------------------------------------------------------------------------
inline int FastMarch3::Index (int x, int y, int z) const
{
    return x + mXBound*(y + mYBound*z);
}
//----------------------------------------------------------------------------
