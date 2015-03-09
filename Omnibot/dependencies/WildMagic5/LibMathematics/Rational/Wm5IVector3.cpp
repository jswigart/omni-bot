// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IVector3.h"
using namespace Wm5;

//----------------------------------------------------------------------------
IVector3::IVector3 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
IVector3::IVector3 (const IVector3& vec)
{
    mTuple[0] = vec.mTuple[0];
    mTuple[1] = vec.mTuple[1];
    mTuple[2] = vec.mTuple[2];
}
//----------------------------------------------------------------------------
IVector3::IVector3 (const IVector<3>& vec)
{
    mTuple[0] = vec[0];
    mTuple[1] = vec[1];
    mTuple[2] = vec[2];
}
//----------------------------------------------------------------------------
IVector3::IVector3 (const int64_t& x, const int64_t& y, const int64_t& z)
{
    mTuple[0] = x;
    mTuple[1] = y;
    mTuple[2] = z;
}
//----------------------------------------------------------------------------
IVector3& IVector3::operator= (const IVector3& vec)
{
    mTuple[0] = vec.mTuple[0];
    mTuple[1] = vec.mTuple[1];
    mTuple[2] = vec.mTuple[2];
    return *this;
}
//----------------------------------------------------------------------------
IVector3& IVector3::operator= (const IVector<3>& vec)
{
    mTuple[0] = vec[0];
    mTuple[1] = vec[1];
    mTuple[2] = vec[2];
    return *this;
}
//----------------------------------------------------------------------------
int64_t IVector3::Dot (const IVector3& vec) const
{
    return mTuple[0]*vec.mTuple[0] + mTuple[1]*vec.mTuple[1] +
        mTuple[2]*vec.mTuple[2];
}
//----------------------------------------------------------------------------
IVector3 IVector3::Cross (const IVector3& vec) const
{
    return IVector3(
        mTuple[1]*vec.mTuple[2] - mTuple[2]*vec.mTuple[1],
        mTuple[2]*vec.mTuple[0] - mTuple[0]*vec.mTuple[2],
        mTuple[0]*vec.mTuple[1] - mTuple[1]*vec.mTuple[0]);
}
//----------------------------------------------------------------------------
int64_t IVector3::TripleScalar (const IVector3& U, const IVector3& V) const
{
    return Dot(U.Cross(V));
}
//----------------------------------------------------------------------------
