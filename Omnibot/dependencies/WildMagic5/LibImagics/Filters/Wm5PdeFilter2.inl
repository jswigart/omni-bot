// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int PdeFilter2::GetXBound () const
{
    return mXBound;
}
//----------------------------------------------------------------------------
inline int PdeFilter2::GetYBound () const
{
    return mYBound;
}
//----------------------------------------------------------------------------
inline float PdeFilter2::GetXSpacing () const
{
    return mXSpacing;
}
//----------------------------------------------------------------------------
inline float PdeFilter2::GetYSpacing () const
{
    return mYSpacing;
}
//----------------------------------------------------------------------------
inline float PdeFilter2::GetU (int x, int y) const
{
    return mSrc[y+1][x+1];
}
//----------------------------------------------------------------------------
inline float PdeFilter2::GetUx (int x, int y) const
{
    int iYp1 = y + 1;
    return mHalfInvDx*(mSrc[iYp1][x+2] - mSrc[iYp1][x]);
}
//----------------------------------------------------------------------------
inline float PdeFilter2::GetUy (int x, int y) const
{
    int iXp1 = x + 1;
    return mHalfInvDy*(mSrc[y+2][iXp1] - mSrc[y][iXp1]);
}
//----------------------------------------------------------------------------
inline float PdeFilter2::GetUxx (int x, int y) const
{
    int iYp1 = y + 1;
    return mInvDxDx*(mSrc[iYp1][x+2] - 2.0f*mSrc[iYp1][x+1] +
        mSrc[iYp1][x]);
}
//----------------------------------------------------------------------------
inline float PdeFilter2::GetUxy (int x, int y) const
{
    int iXp2 = x + 2, iYp2 = y + 2;
    return mFourthInvDxDy*(mSrc[y][x] - mSrc[y][iXp2] +
        mSrc[iYp2][iXp2] - mSrc[iYp2][x]);
}
//----------------------------------------------------------------------------
inline float PdeFilter2::GetUyy (int x, int y) const
{
    int iXp1 = x + 1;
    return mInvDyDy*(mSrc[y+2][iXp1] - 2.0f*mSrc[y+1][iXp1] +
        mSrc[y][iXp1]);
}
//----------------------------------------------------------------------------
inline bool PdeFilter2::GetMask (int x, int y) const
{
    return (mMask ? mMask[y+1][x+1] : true);
}
//----------------------------------------------------------------------------
