// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/07)

//----------------------------------------------------------------------------
inline Float4::Float4 ()
{
}
//----------------------------------------------------------------------------
inline Float4::~Float4 ()
{
}
//----------------------------------------------------------------------------
inline Float4::Float4 (float f0, float f1, float f2, float f3)
{
    mTuple[0] = f0;
    mTuple[1] = f1;
    mTuple[2] = f2;
    mTuple[3] = f3;
}
//----------------------------------------------------------------------------
inline Float4::Float4 (const Float4& tuple)
{
    mTuple[0] = tuple.mTuple[0];
    mTuple[1] = tuple.mTuple[1];
    mTuple[2] = tuple.mTuple[2];
    mTuple[3] = tuple.mTuple[3];
}
//----------------------------------------------------------------------------
inline Float4& Float4::operator= (const Float4& tuple)
{
    mTuple[0] = tuple.mTuple[0];
    mTuple[1] = tuple.mTuple[1];
    mTuple[2] = tuple.mTuple[2];
    mTuple[3] = tuple.mTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
