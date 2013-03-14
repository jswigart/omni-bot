// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/07)

//----------------------------------------------------------------------------
inline Float1::Float1 ()
{
}
//----------------------------------------------------------------------------
inline Float1::~Float1 ()
{
}
//----------------------------------------------------------------------------
inline Float1::Float1 (float f0)
{
    mTuple[0] = f0;
}
//----------------------------------------------------------------------------
inline Float1::Float1 (const Float1& tuple)
{
    mTuple[0] = tuple.mTuple[0];
}
//----------------------------------------------------------------------------
inline Float1& Float1::operator= (const Float1& tuple)
{
    mTuple[0] = tuple.mTuple[0];
    return *this;
}
//----------------------------------------------------------------------------
