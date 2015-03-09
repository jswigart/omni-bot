// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int PdeFilter3::GetXBound () const
{
    return mXBound;
}
//----------------------------------------------------------------------------
inline int PdeFilter3::GetYBound () const
{
    return mYBound;
}
//----------------------------------------------------------------------------
inline int PdeFilter3::GetZBound () const
{
    return mZBound;
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetXSpacing () const
{
    return mXSpacing;
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetYSpacing () const
{
    return mYSpacing;
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetZSpacing () const
{
    return mZSpacing;
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetU (int x, int y, int z) const
{
    return mSrc[z+1][y+1][x+1];
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUx (int x, int y, int z) const
{
    int iYp1 = y + 1, iZp1 = z + 1;
    return mHalfInvDx*(mSrc[iZp1][iYp1][x+2] - mSrc[iZp1][iYp1][x]);
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUy (int x, int y, int z) const
{
    int iXp1 = x + 1, iZp1 = z + 1;
    return mHalfInvDy*(mSrc[iZp1][y+2][iXp1] - mSrc[iZp1][y][iXp1]);
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUz (int x, int y, int z) const
{
    int iXp1 = x + 1, iYp1 = y + 1;
    return mHalfInvDz*(mSrc[z+2][iYp1][iXp1] - mSrc[z][iYp1][iXp1]);
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUxx (int x, int y, int z) const
{
    int iYp1 = y + 1, iZp1 = z + 1;
    return mInvDxDx*(mSrc[iZp1][iYp1][x+2] -
        2.0f*mSrc[iZp1][iYp1][x+1] + mSrc[iZp1][iYp1][x]);
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUxy (int x, int y, int z) const
{
    int iXp2 = x + 2, iYp2 = y + 2, iZp1 = z + 1;
    return mFourthInvDxDy*(mSrc[iZp1][y][x] -
        mSrc[iZp1][y][iXp2] + mSrc[iZp1][iYp2][iXp2] -
        mSrc[iZp1][iYp2][x]);
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUxz (int x, int y, int z) const
{
    int iXp2 = x + 2, iYp1 = y + 1, iZp2 = z + 2;
    return mFourthInvDxDz*(mSrc[z][iYp1][x] -
        mSrc[z][iYp1][iXp2] + mSrc[iZp2][iYp1][iXp2] -
        mSrc[iZp2][iYp1][x]);
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUyy (int x, int y, int z) const
{
    int iXp1 = x + 1, iZp1 = z + 1;
    return mInvDyDy*(mSrc[iZp1][y+2][iXp1] -
        2.0f*mSrc[iZp1][y+1][iXp1] + mSrc[iZp1][y][iXp1]);
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUyz (int x, int y, int z) const
{
    int iXp1 = x + 1, iYp2 = y + 2, iZp2 = z + 2;
    return mFourthInvDyDz*(mSrc[z][y][iXp1] -
        mSrc[z][iYp2][iXp1] + mSrc[iZp2][iYp2][iXp1] -
        mSrc[iZp2][y][iXp1]);
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUzz (int x, int y, int z) const
{
    int iXp1 = x + 1, iYp1 = y + 1;
    return mInvDzDz*(mSrc[z+2][iYp1][iXp1] -
        2.0f*mSrc[z+1][iYp1][iXp1] + mSrc[z][iYp1][iXp1]);
}
//----------------------------------------------------------------------------
inline bool PdeFilter3::GetMask (int x, int y, int z) const
{
    return (mMask ? mMask[z+1][y+1][x+1] : true);
}
//----------------------------------------------------------------------------
