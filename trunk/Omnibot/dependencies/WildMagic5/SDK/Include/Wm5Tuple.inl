// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2012/03/09)

//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
Tuple<DIMENSION,TYPE>::Tuple ()
{
    // Uninitialized for native data.  Initialized for class data as long as
    // TYPE's default constructor initializes its own data.
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
Tuple<DIMENSION,TYPE>::Tuple (const Tuple& tuple)
{
    for (int i = 0; i < DIMENSION; ++i)
    {
        mTuple[i] = tuple.mTuple[i];
    }
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
Tuple<DIMENSION,TYPE>::~Tuple ()
{
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
inline Tuple<DIMENSION,TYPE>::operator const TYPE* () const
{
    return mTuple;
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
inline Tuple<DIMENSION,TYPE>::operator TYPE* ()
{
    return mTuple;
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
inline const TYPE& Tuple<DIMENSION,TYPE>::operator[] (int i) const
{
    return mTuple[i];
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
inline TYPE& Tuple<DIMENSION,TYPE>::operator[] (int i)
{
    return mTuple[i];
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
Tuple<DIMENSION,TYPE>& Tuple<DIMENSION,TYPE>::operator= (const Tuple& tuple)
{
    for (int i = 0; i < DIMENSION; ++i)
    {
        mTuple[i] = tuple.mTuple[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
bool Tuple<DIMENSION,TYPE>::operator== (const Tuple& tuple) const
{
    for (int i = 0; i < DIMENSION; ++i)
    {
        if (mTuple[i] != tuple.mTuple[i])
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
bool Tuple<DIMENSION,TYPE>::operator!= (const Tuple& tuple) const
{
    for (int i = 0; i < DIMENSION; ++i)
    {
        if (mTuple[i] != tuple.mTuple[i])
        {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
bool Tuple<DIMENSION,TYPE>::operator< (const Tuple& tuple) const
{
    for (int i = 0; i < DIMENSION; ++i)
    {
        if (mTuple[i] < tuple.mTuple[i])
        {
            return true;
        }

        if (mTuple[i] > tuple.mTuple[i])
        {
            return false;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
bool Tuple<DIMENSION,TYPE>::operator<= (const Tuple& tuple) const
{
    for (int i = 0; i < DIMENSION; ++i)
    {
        if (mTuple[i] < tuple.mTuple[i])
        {
            return true;
        }

        if (mTuple[i] > tuple.mTuple[i])
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
bool Tuple<DIMENSION,TYPE>::operator> (const Tuple& tuple) const
{
    for (int i = 0; i < DIMENSION; ++i)
    {
        if (mTuple[i] > tuple.mTuple[i])
        {
            return true;
        }

        if (mTuple[i] < tuple.mTuple[i])
        {
            return false;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
template <int DIMENSION, typename TYPE>
bool Tuple<DIMENSION,TYPE>::operator>= (const Tuple& tuple) const
{
    for (int i = 0; i < DIMENSION; ++i)
    {
        if (mTuple[i] > tuple.mTuple[i])
        {
            return true;
        }

        if (mTuple[i] < tuple.mTuple[i])
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
