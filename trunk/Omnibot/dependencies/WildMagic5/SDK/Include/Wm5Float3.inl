// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/07)

//----------------------------------------------------------------------------
inline Float3::Float3 ()
{
}
//----------------------------------------------------------------------------
inline Float3::~Float3 ()
{
}
//----------------------------------------------------------------------------
inline Float3::Float3 (float f0, float f1, float f2)
{
    mTuple[0] = f0;
    mTuple[1] = f1;
    mTuple[2] = f2;
}
//----------------------------------------------------------------------------
inline Float3::Float3 (const Float3& tuple)
{
    mTuple[0] = tuple.mTuple[0];
    mTuple[1] = tuple.mTuple[1];
    mTuple[2] = tuple.mTuple[2];
}
//----------------------------------------------------------------------------
inline Float3& Float3::operator= (const Float3& tuple)
{
    mTuple[0] = tuple.mTuple[0];
    mTuple[1] = tuple.mTuple[1];
    mTuple[2] = tuple.mTuple[2];
    return *this;
}
//----------------------------------------------------------------------------
