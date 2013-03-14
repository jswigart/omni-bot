// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline APoint::operator const Float3& () const
{
    return *(const Float3*)mTuple;
}
//----------------------------------------------------------------------------
inline APoint::operator Float3& ()
{
    return *(Float3*)mTuple;
}
//----------------------------------------------------------------------------
inline APoint::operator const Vector3f& () const
{
    return *(const Vector3f*)mTuple;
}
//----------------------------------------------------------------------------
inline APoint::operator Vector3f& ()
{
    return *(Vector3f*)mTuple;
}
//----------------------------------------------------------------------------
