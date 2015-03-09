// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IVector2.h"
using namespace Wm5;

//----------------------------------------------------------------------------
IVector2::IVector2 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
IVector2::IVector2 (const IVector2& vec)
{
    mTuple[0] = vec.mTuple[0];
    mTuple[1] = vec.mTuple[1];
}
//----------------------------------------------------------------------------
IVector2::IVector2 (const IVector<2>& vec)
{
    mTuple[0] = vec[0];
    mTuple[1] = vec[1];
}
//----------------------------------------------------------------------------
IVector2::IVector2 (const int64_t& x, const int64_t& y)
{
    mTuple[0] = x;
    mTuple[1] = y;
}
//----------------------------------------------------------------------------
IVector2& IVector2::operator= (const IVector2& vec)
{
    mTuple[0] = vec.mTuple[0];
    mTuple[1] = vec.mTuple[1];
    return *this;
}
//----------------------------------------------------------------------------
IVector2& IVector2::operator= (const IVector<2>& vec)
{
    mTuple[0] = vec[0];
    mTuple[1] = vec[1];
    return *this;
}
//----------------------------------------------------------------------------
int64_t IVector2::Dot (const IVector2& vec) const
{
    return mTuple[0]*vec.mTuple[0] + mTuple[1]*vec.mTuple[1];
}
//----------------------------------------------------------------------------
IVector2 IVector2::Perp () const
{
    return IVector2(mTuple[1], -mTuple[0]);
}
//----------------------------------------------------------------------------
int64_t IVector2::DotPerp (const IVector2& vec) const
{
    return mTuple[0]*vec.mTuple[1] - mTuple[1]*vec.mTuple[0];
}
//----------------------------------------------------------------------------
