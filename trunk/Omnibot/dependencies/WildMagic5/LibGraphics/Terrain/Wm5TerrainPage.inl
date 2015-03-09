// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int TerrainPage::GetSize () const
{
    return mSize;
}
//----------------------------------------------------------------------------
inline const unsigned short* TerrainPage::GetHeights () const
{
    return mHeights;
}
//----------------------------------------------------------------------------
inline const Float2& TerrainPage::GetOrigin () const
{
    return mOrigin;
}
//----------------------------------------------------------------------------
inline float TerrainPage::GetMinElevation () const
{
    return mMinElevation;
}
//----------------------------------------------------------------------------
inline float TerrainPage::GetMaxElevation () const
{
    return mMaxElevation;
}
//----------------------------------------------------------------------------
inline float TerrainPage::GetSpacing () const
{
    return mSpacing;
}
//----------------------------------------------------------------------------
inline float TerrainPage::GetX (int x) const
{
    return mOrigin[0] + mSpacing*(float)x;
}
//----------------------------------------------------------------------------
inline float TerrainPage::GetY (int y) const
{
    return mOrigin[1] + mSpacing*(float)y;
}
//----------------------------------------------------------------------------
inline float TerrainPage::GetHeight (int index) const
{
    return mMinElevation + mMultiplier*(float)mHeights[index];
}
//----------------------------------------------------------------------------
