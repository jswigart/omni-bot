// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <int ISIZE>
RVector2<ISIZE>::RVector2 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <int ISIZE>
RVector2<ISIZE>::RVector2 (const RVector2& vec)
{
    mTuple[0] = vec.mTuple[0];
    mTuple[1] = vec.mTuple[1];
}
//----------------------------------------------------------------------------
template <int ISIZE>
RVector2<ISIZE>::RVector2 (const RVector<2,ISIZE>& vec)
{
    mTuple[0] = vec[0];
    mTuple[1] = vec[1];
}
//----------------------------------------------------------------------------
template <int ISIZE>
RVector2<ISIZE>::RVector2 (const Rational<ISIZE>& x, const Rational<ISIZE>& y)
{
    mTuple[0] = x;
    mTuple[1] = y;
}
//----------------------------------------------------------------------------
template <int ISIZE>
RVector2<ISIZE>& RVector2<ISIZE>::operator= (const RVector2& vec)
{
    mTuple[0] = vec.mTuple[0];
    mTuple[1] = vec.mTuple[1];
    return *this;
}
//----------------------------------------------------------------------------
template <int ISIZE>
RVector2<ISIZE>& RVector2<ISIZE>::operator= (const RVector<2,ISIZE>& vec)
{
    mTuple[0] = vec[0];
    mTuple[1] = vec[1];
    return *this;
}
//----------------------------------------------------------------------------
template <int ISIZE>
inline Rational<ISIZE> RVector2<ISIZE>::X () const
{
    return mTuple[0];
}
//----------------------------------------------------------------------------
template <int ISIZE>
inline Rational<ISIZE>& RVector2<ISIZE>::X ()
{
    return mTuple[0];
}
//----------------------------------------------------------------------------
template <int ISIZE>
inline Rational<ISIZE> RVector2<ISIZE>::Y () const
{
    return mTuple[1];
}
//----------------------------------------------------------------------------
template <int ISIZE>
inline Rational<ISIZE>& RVector2<ISIZE>::Y ()
{
    return mTuple[1];
}
//----------------------------------------------------------------------------
template <int ISIZE>
Rational<ISIZE> RVector2<ISIZE>::Dot (const RVector2& vec) const
{
    return mTuple[0]*vec.mTuple[0] + mTuple[1]*vec.mTuple[1];
}
//----------------------------------------------------------------------------
template <int ISIZE>
RVector2<ISIZE> RVector2<ISIZE>::Perp () const
{
    return RVector2<ISIZE>(mTuple[1], -mTuple[0]);
}
//----------------------------------------------------------------------------
template <int ISIZE>
Rational<ISIZE> RVector2<ISIZE>::DotPerp (const RVector2& vec) const
{
    return mTuple[0]*vec.mTuple[1] - mTuple[1]*vec.mTuple[0];
}
//----------------------------------------------------------------------------
