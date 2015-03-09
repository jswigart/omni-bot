// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline void FastMarch::SetTime (int i, float time)
{
    mTimes[i] = time;
}
//----------------------------------------------------------------------------
inline float FastMarch::GetTime (int i) const
{
    return mTimes[i];
}
//----------------------------------------------------------------------------
inline int FastMarch::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
inline bool FastMarch::IsValid (int i) const
{
    return 0.0f <= mTimes[i] && mTimes[i] < Mathf::MAX_REAL;
}
//----------------------------------------------------------------------------
inline bool FastMarch::IsTrial (int i) const
{
    return mTrials[i] != 0;
}
//----------------------------------------------------------------------------
inline bool FastMarch::IsFar (int i) const
{
    return mTimes[i] == Mathf::MAX_REAL;
}
//----------------------------------------------------------------------------
inline bool FastMarch::IsZeroSpeed (int i) const
{
    return mTimes[i] == -Mathf::MAX_REAL;
}
//----------------------------------------------------------------------------
inline bool FastMarch::IsInterior (int i) const
{
    return IsValid(i) && !IsTrial(i);
}
//----------------------------------------------------------------------------
