// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/03/27)

//----------------------------------------------------------------------------
template <int VSIZE>
IVector<VSIZE>::IVector ()
{
    // For efficiency in construction of large arrays of vectors, the
    // default constructor does not initialize the vector.
}
//----------------------------------------------------------------------------
template <int VSIZE>
IVector<VSIZE>::IVector (const IVector& vec)
{
    for (int i = 0; i < VSIZE; ++i)
    {
        mTuple[i] = vec.mTuple[i];
    }
}
//----------------------------------------------------------------------------
template <int VSIZE>
inline IVector<VSIZE>::operator const int64_t* () const
{
    return mTuple;
}
//----------------------------------------------------------------------------
template <int VSIZE>
inline IVector<VSIZE>::operator int64_t* ()
{
    return mTuple;
}
//----------------------------------------------------------------------------
template <int VSIZE>
inline const int64_t& IVector<VSIZE>::operator[] (int i) const
{
    assertion(0 <= i && i < VSIZE, "Invalid index\n");
    return mTuple[i];
}
//----------------------------------------------------------------------------
template <int VSIZE>
inline int64_t& IVector<VSIZE>::operator[] (int i)
{
    assertion(0 <= i && i < VSIZE, "Invalid index\n");
    return mTuple[i];
}
//----------------------------------------------------------------------------
template <int VSIZE>
IVector<VSIZE>& IVector<VSIZE>::operator= (const IVector& vec)
{
    for (int i = 0; i < VSIZE; ++i)
    {
        mTuple[i] = vec.mTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int VSIZE>
bool IVector<VSIZE>::operator== (const IVector& vec) const
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
template <int VSIZE>
bool IVector<VSIZE>::operator!= (const IVector& vec) const
{
    return !operator==(vec);
}
//----------------------------------------------------------------------------
template <int VSIZE>
int IVector<VSIZE>::CompareArrays (const IVector& vec) const
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
template <int VSIZE>
bool IVector<VSIZE>::operator< (const IVector& vec) const
{
    return CompareArrays(vec) < 0;
}
//----------------------------------------------------------------------------
template <int VSIZE>
bool IVector<VSIZE>::operator<= (const IVector& vec) const
{
    return CompareArrays(vec) <= 0;
}
//----------------------------------------------------------------------------
template <int VSIZE>
bool IVector<VSIZE>::operator> (const IVector& vec) const
{
    return CompareArrays(vec) > 0;
}
//----------------------------------------------------------------------------
template <int VSIZE>
bool IVector<VSIZE>::operator>= (const IVector& vec) const
{
    return CompareArrays(vec) >= 0;
}
//----------------------------------------------------------------------------
template <int VSIZE>
IVector<VSIZE> IVector<VSIZE>::operator+ (const IVector& vec) const
{
    IVector<VSIZE> iSum;
    for (int i = 0; i < VSIZE; ++i)
    {
        iSum.mTuple[i] = mTuple[i] + vec.mTuple[i];
    }
    return iSum;
}
//----------------------------------------------------------------------------
template <int VSIZE>
IVector<VSIZE> IVector<VSIZE>::operator- (const IVector& vec) const
{
    IVector<VSIZE> diff;
    for (int i = 0; i < VSIZE; ++i)
    {
        diff.mTuple[i] = mTuple[i] - vec.mTuple[i];
    }
    return diff;
}
//----------------------------------------------------------------------------
template <int VSIZE>
IVector<VSIZE> IVector<VSIZE>::operator* (const int64_t& scalar) const
{
    IVector<VSIZE> prod;
    for (int i = 0; i < VSIZE; ++i)
    {
        prod.mTuple[i] = scalar*mTuple[i];
    }
    return prod;
}
//----------------------------------------------------------------------------
template <int VSIZE>
IVector<VSIZE> IVector<VSIZE>::operator/ (const int64_t& scalar) const
{
    assertion(scalar != 0, "Division by zero\n");

    IVector<VSIZE> div;
    for (int i = 0; i < VSIZE; ++i)
    {
        div.mTuple[i] = mTuple[i]/scalar;
    }

    return div;
}
//----------------------------------------------------------------------------
template <int VSIZE>
IVector<VSIZE> IVector<VSIZE>::operator- () const
{
    IVector<VSIZE> neg;
    for (int i = 0; i < VSIZE; ++i)
    {
        neg.mTuple[i] = -mTuple[i];
    }
    return neg;
}
//----------------------------------------------------------------------------
template <int VSIZE>
IVector<VSIZE> operator* (const int64_t& scalar, const IVector<VSIZE>& vec)
{
    IVector<VSIZE> prod;
    for (int i = 0; i < VSIZE; ++i)
    {
        prod[i] = scalar*vec[i];
    }
    return prod;
}
//----------------------------------------------------------------------------
template <int VSIZE>
IVector<VSIZE>& IVector<VSIZE>::operator+= (const IVector& vec)
{
    for (int i = 0; i < VSIZE; ++i)
    {
        mTuple[i] += vec.mTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int VSIZE>
IVector<VSIZE>& IVector<VSIZE>::operator-= (const IVector& vec)
{
    for (int i = 0; i < VSIZE; ++i)
    {
        mTuple[i] -= vec.mTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int VSIZE>
IVector<VSIZE>& IVector<VSIZE>::operator*= (const int64_t& scalar)
{
    for (int i = 0; i < VSIZE; ++i)
    {
        mTuple[i] *= scalar;
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int VSIZE>
IVector<VSIZE>& IVector<VSIZE>::operator/= (const int64_t& scalar)
{
    assertion(scalar != 0, "Division by zero\n");

    for (int i = 0; i < VSIZE; ++i)
    {
        mTuple[i] /= scalar;
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int VSIZE>
int64_t IVector<VSIZE>::SquaredLength () const
{
    int64_t sqrLen = 0;
    for (int i = 0; i < VSIZE; ++i)
    {
        sqrLen += mTuple[i]*mTuple[i];
    }
    return sqrLen;
}
//----------------------------------------------------------------------------
template <int VSIZE>
int64_t IVector<VSIZE>::Dot (const IVector& vec) const
{
    int64_t dot = 0;
    for (int i = 0; i < VSIZE; ++i)
    {
        dot += mTuple[i]*vec.mTuple[i];
    }
    return dot;
}
//----------------------------------------------------------------------------
