// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int FastMarch2::GetXBound () const
{
    return mXBound;
}
//----------------------------------------------------------------------------
inline int FastMarch2::GetYBound () const
{
    return mYBound;
}
//----------------------------------------------------------------------------
inline float FastMarch2::GetXSpacing () const
{
    return mXSpacing;
}
//----------------------------------------------------------------------------
inline float FastMarch2::GetYSpacing () const
{
    return mYSpacing;
}
//----------------------------------------------------------------------------
inline int FastMarch2::Index (int x, int y) const
{
    return x + mXBound*y;
}
//----------------------------------------------------------------------------
