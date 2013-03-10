// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline void Bound::SetCenter (const APoint& center)
{
    mCenter = center;
}
//----------------------------------------------------------------------------
inline void Bound::SetRadius (float radius)
{
    mRadius = radius;
}
//----------------------------------------------------------------------------
inline const APoint& Bound::GetCenter () const
{
    return mCenter;
}
//----------------------------------------------------------------------------
inline float Bound::GetRadius () const
{
    return mRadius;
}
//----------------------------------------------------------------------------
inline int Bound::GetStreamingSize ()
{
    return (int)(sizeof(APoint) + sizeof(float));
}
//----------------------------------------------------------------------------
