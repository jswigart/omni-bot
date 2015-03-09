// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/03/27)

//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
RVector<VSIZE,ISIZE>::RVector ()
{
    // For efficiency in construction of large arrays of vectors, the
    // default constructor does not initialize the vector.
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
RVector<VSIZE,ISIZE>::RVector (const RVector& vec)
{
    for (int i = 0; i < VSIZE; ++i)
    {
        mTuple[i] = vec.mTuple[i];
    }
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
inline RVector<VSIZE,ISIZE>::operator const Rational<ISIZE>* () const
{
    return mTuple;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
inline RVector<VSIZE,ISIZE>::operator Rational<ISIZE>* ()
{
    return mTuple;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
inline const Rational<ISIZE>& RVector<VSIZE,ISIZE>::operator[] (int i) const
{
    assertion(0 <= i && i < VSIZE, "Invalid input.\n");
    return mTuple[i];
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
inline Rational<ISIZE>& RVector<VSIZE,ISIZE>::operator[] (int i)
{
    assertion(0 <= i && i < VSIZE, "Invalid input.\n");
    return mTuple[i];
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
RVector<VSIZE,ISIZE>& RVector<VSIZE,ISIZE>::operator= (const RVector& vec)
{
    for (int i = 0; i < VSIZE; ++i)
    {
        mTuple[i] = vec.mTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
bool RVector<VSIZE,ISIZE>::operator== (const RVector& vec) const
{
    for (int i = 0; i < VSIZE; ++i)
    {
        if (mTuple[i] != vec.mTuple[i])
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
bool RVector<VSIZE,ISIZE>::operator!= (const RVector& vec) const
{
    return !operator==(vec);
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
int RVector<VSIZE,ISIZE>::CompareArrays (const RVector& vec) const
{
    for (int i = 0; i < VSIZE; ++i)
    {
        if (mTuple[i] < vec.mTuple[i])
        {
            return -1;
        }
        if (mTuple[i] > vec.mTuple[i])
        {
            return +1;
        }
    }
    return 0;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
bool RVector<VSIZE,ISIZE>::operator< (const RVector& vec) const
{
    return CompareArrays(vec) < 0;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
bool RVector<VSIZE,ISIZE>::operator<= (const RVector& vec) const
{
    return CompareArrays(vec) <= 0;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
bool RVector<VSIZE,ISIZE>::operator> (const RVector& vec) const
{
    return CompareArrays(vec) > 0;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
bool RVector<VSIZE,ISIZE>::operator>= (const RVector& vec) const
{
    return CompareArrays(vec) >= 0;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
RVector<VSIZE,ISIZE> RVector<VSIZE,ISIZE>::operator+ (const RVector& vec)
    const
{
    RVector<VSIZE,ISIZE> sum;
    for (int i = 0; i < VSIZE; ++i)
    {
        sum.mTuple[i] = mTuple[i] + vec.mTuple[i];
    }
    return sum;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
RVector<VSIZE,ISIZE> RVector<VSIZE,ISIZE>::operator- (const RVector& vec)
    const
{
    RVector<VSIZE,ISIZE> diff;
    for (int i = 0; i < VSIZE; ++i)
    {
        diff.mTuple[i] = mTuple[i] - vec.mTuple[i];
    }
    return diff;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
RVector<VSIZE,ISIZE> RVector<VSIZE,ISIZE>::operator*
    (const Rational<ISIZE>& scalar) const
{
    RVector<VSIZE,ISIZE> prod;
    for (int i = 0; i < VSIZE; ++i)
    {
        prod.mTuple[i] = scalar*mTuple[i];
    }
    return prod;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
RVector<VSIZE,ISIZE> RVector<VSIZE,ISIZE>::operator/
    (const Rational<ISIZE>& scalar) const
{
    assertion(scalar != 0, "Division by zero\n");

    RVector<VSIZE,ISIZE> div;
    for (int i = 0; i < VSIZE; ++i)
    {
        div.mTuple[i] = mTuple[i]/scalar;
    }

    return div;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
RVector<VSIZE,ISIZE> RVector<VSIZE,ISIZE>::operator- () const
{
    RVector<VSIZE,ISIZE> neg;
    for (int i = 0; i < VSIZE; ++i)
    {
        neg.mTuple[i] = -mTuple[i];
    }
    return neg;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
RVector<VSIZE,ISIZE> operator* (const Rational<ISIZE>& scalar,
    const RVector<VSIZE,ISIZE>& vec)
{
    RVector<VSIZE,ISIZE> prod;
    for (int i = 0; i < VSIZE; ++i)
    {
        prod[i] = scalar*vec[i];
    }
    return prod;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
RVector<VSIZE,ISIZE>& RVector<VSIZE,ISIZE>::operator+= (const RVector& vec)
{
    for (int i = 0; i < VSIZE; ++i)
    {
        mTuple[i] += vec.mTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
RVector<VSIZE,ISIZE>& RVector<VSIZE,ISIZE>::operator-= (const RVector& vec)
{
    for (int i = 0; i < VSIZE; ++i)
    {
        mTuple[i] -= vec.mTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
RVector<VSIZE,ISIZE>& RVector<VSIZE,ISIZE>::operator*=
    (const Rational<ISIZE>& scalar)
{
    for (int i = 0; i < VSIZE; ++i)
    {
        mTuple[i] *= scalar;
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
RVector<VSIZE,ISIZE>& RVector<VSIZE,ISIZE>::operator/=
    (const Rational<ISIZE>& scalar)
{
    assertion(scalar != 0, "Division by zero\n");

    for (int i = 0; i < VSIZE; ++i)
    {
        mTuple[i] /= scalar;
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
Rational<ISIZE> RVector<VSIZE,ISIZE>::SquaredLength () const
{
    Rational<ISIZE> sqrLen = 0;
    for (int i = 0; i < VSIZE; ++i)
    {
        sqrLen += mTuple[i]*mTuple[i];
    }
    return sqrLen;
}
//----------------------------------------------------------------------------
template <int VSIZE, int ISIZE>
Rational<ISIZE> RVector<VSIZE,ISIZE>::Dot (const RVector& vec) const
{
    Rational<ISIZE> dot = 0;
    for (int i = 0; i < VSIZE; ++i)
    {
        dot += mTuple[i]*vec.mTuple[i];
    }
    return dot;
}
//----------------------------------------------------------------------------
