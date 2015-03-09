// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/03/27)

//----------------------------------------------------------------------------
inline HPlane::operator HPoint ()
{
    return mTuple;
}
//----------------------------------------------------------------------------
inline HPlane::operator HPoint () const
{
    return mTuple;
}
//----------------------------------------------------------------------------
inline HPlane::operator const float* () const
{
    return (const float*)mTuple;
}
//----------------------------------------------------------------------------
inline HPlane::operator float* ()
{
    return (float*)mTuple;
}
//----------------------------------------------------------------------------
inline const float& HPlane::operator[] (int i) const
{
    return mTuple[i];
}
//----------------------------------------------------------------------------
inline float& HPlane::operator[] (int i)
{
    return mTuple[i];
}
//----------------------------------------------------------------------------
inline void HPlane::SetNormal (const AVector& normal)
{
    mTuple[0] = normal[0];
    mTuple[1] = normal[1];
    mTuple[2] = normal[2];
}
//----------------------------------------------------------------------------
inline void HPlane::SetConstant (float constant)
{
    mTuple[3] = -constant;
}
//----------------------------------------------------------------------------
inline AVector HPlane::GetNormal () const
{
    return AVector(mTuple[0], mTuple[1], mTuple[2]);
}
//----------------------------------------------------------------------------
inline float HPlane::GetConstant () const
{
    return -mTuple[3];
}
//----------------------------------------------------------------------------
