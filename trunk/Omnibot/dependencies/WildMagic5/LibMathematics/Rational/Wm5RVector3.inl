// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <int ISIZE>
RVector3<ISIZE>::RVector3 ()
{
    // the vector is uninitialized
}
//----------------------------------------------------------------------------
template <int ISIZE>
RVector3<ISIZE>::RVector3 (const RVector3& vec)
{
    mTuple[0] = vec.mTuple[0];
    mTuple[1] = vec.mTuple[1];
    mTuple[2] = vec.mTuple[2];
}
//----------------------------------------------------------------------------
template <int ISIZE>
RVector3<ISIZE>::RVector3 (const RVector<3,ISIZE>& vec)
{
    mTuple[0] = vec[0];
    mTuple[1] = vec[1];
    mTuple[2] = vec[2];
}
//----------------------------------------------------------------------------
template <int ISIZE>
RVector3<ISIZE>::RVector3 (const Rational<ISIZE>& x, const Rational<ISIZE>& y,
    const Rational<ISIZE>& z)
{
    mTuple[0] = x;
    mTuple[1] = y;
    mTuple[2] = z;
}
//----------------------------------------------------------------------------
template <int ISIZE>
RVector3<ISIZE>& RVector3<ISIZE>::operator= (const RVector3& vec)
{
    mTuple[0] = vec.mTuple[0];
    mTuple[1] = vec.mTuple[1];
    mTuple[2] = vec.mTuple[2];
    return *this;
}
//----------------------------------------------------------------------------
template <int ISIZE>
RVector3<ISIZE>& RVector3<ISIZE>::operator= (const RVector<3,ISIZE>& vec)
{
    mTuple[0] = vec[0];
    mTuple[1] = vec[1];
    mTuple[2] = vec[2];
    return *this;
}
//----------------------------------------------------------------------------
template <int ISIZE>
inline Rational<ISIZE> RVector3<ISIZE>::X () const
{
    return mTuple[0];
}
//----------------------------------------------------------------------------
template <int ISIZE>
inline Rational<ISIZE>& RVector3<ISIZE>::X ()
{
    return mTuple[0];
}
//----------------------------------------------------------------------------
template <int ISIZE>
inline Rational<ISIZE> RVector3<ISIZE>::Y () const
{
    return mTuple[1];
}
//----------------------------------------------------------------------------
template <int ISIZE>
inline Rational<ISIZE>& RVector3<ISIZE>::Y ()
{
    return mTuple[1];
}
//----------------------------------------------------------------------------
template <int ISIZE>
inline Rational<ISIZE> RVector3<ISIZE>::Z () const
{
    return mTuple[2];
}
//----------------------------------------------------------------------------
template <int ISIZE>
inline Rational<ISIZE>& RVector3<ISIZE>::Z ()
{
    return mTuple[2];
}
//----------------------------------------------------------------------------
template <int ISIZE>
Rational<ISIZE> RVector3<ISIZE>::Dot (const RVector3& vec) const
{
    return mTuple[0]*vec.mTuple[0] + mTuple[1]*vec.mTuple[1] +
        mTuple[2]*vec.mTuple[2];
}
//----------------------------------------------------------------------------
template <int ISIZE>
RVector3<ISIZE> RVector3<ISIZE>::Cross (const RVector3& vec) const
{
    return RVector3<ISIZE>(
        mTuple[1]*vec.mTuple[2] - mTuple[2]*vec.mTuple[1],
        mTuple[2]*vec.mTuple[0] - mTuple[0]*vec.mTuple[2],
        mTuple[0]*vec.mTuple[1] - mTuple[1]*vec.mTuple[0]);
}
//----------------------------------------------------------------------------
template <int ISIZE>
Rational<ISIZE> RVector3<ISIZE>::TripleScalar (const RVector3& U,
    const RVector3& V) const
{
    return Dot(U.Cross(V));
}
//----------------------------------------------------------------------------
