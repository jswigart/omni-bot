// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/07)

//----------------------------------------------------------------------------
inline Float2::Float2 ()
{
}
//----------------------------------------------------------------------------
inline Float2::~Float2 ()
{
}
//----------------------------------------------------------------------------
inline Float2::Float2 (float f0, float f1)
{
    mTuple[0] = f0;
    mTuple[1] = f1;
}
//----------------------------------------------------------------------------
inline Float2::Float2 (const Float2& tuple)
{
    mTuple[0] = tuple.mTuple[0];
    mTuple[1] = tuple.mTuple[1];
}
//----------------------------------------------------------------------------
inline Float2& Float2::operator= (const Float2& tuple)
{
    mTuple[0] = tuple.mTuple[0];
    mTuple[1] = tuple.mTuple[1];
    return *this;
}
//----------------------------------------------------------------------------
