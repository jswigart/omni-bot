// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/03/27)

//----------------------------------------------------------------------------
inline HQuaternion::operator const float* () const
{
    return mTuple;
}
//----------------------------------------------------------------------------
inline HQuaternion::operator float* ()
{
    return mTuple;
}
//----------------------------------------------------------------------------
inline const float& HQuaternion::operator[] (int i) const
{
    return mTuple[i];
}
//----------------------------------------------------------------------------
inline float& HQuaternion::operator[] (int i)
{
    return mTuple[i];
}
//----------------------------------------------------------------------------
inline float HQuaternion::W () const
{
    return mTuple[0];
}
//----------------------------------------------------------------------------
inline float& HQuaternion::W ()
{
    return mTuple[0];
}
//----------------------------------------------------------------------------
inline float HQuaternion::X () const
{
    return mTuple[1];
}
//----------------------------------------------------------------------------
inline float& HQuaternion::X ()
{
    return mTuple[1];
}
//----------------------------------------------------------------------------
inline float HQuaternion::Y () const
{
    return mTuple[2];
}
//----------------------------------------------------------------------------
inline float& HQuaternion::Y ()
{
    return mTuple[2];
}
//----------------------------------------------------------------------------
inline float HQuaternion::Z () const
{
    return mTuple[3];
}
//----------------------------------------------------------------------------
inline float& HQuaternion::Z ()
{
    return mTuple[3];
}
//----------------------------------------------------------------------------
