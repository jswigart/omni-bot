// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename T>
TImage2D<T>::TImage2D (int xBound, int yBound, T* data)
    :
    TImage<T>(2)
{
    int* bounds = new1<int>(2);
    bounds[0] = xBound;
    bounds[1] = yBound;
    SetBounds(bounds);
    SetData(data);
}
//----------------------------------------------------------------------------
template <typename T>
TImage2D<T>::TImage2D (const TImage2D& image)
    :
    TImage<T>(image)
{
}
//----------------------------------------------------------------------------
template <typename T>
TImage2D<T>::TImage2D (const char* filename)
    :
    TImage<T>(filename)
{
}
//----------------------------------------------------------------------------
template <typename T>
T& TImage2D<T>::operator() (int x, int y) const
{
    return mData[x + mBounds[0]*y];
}
//----------------------------------------------------------------------------
template <typename T>
int TImage2D<T>::GetIndex (int x, int y) const
{
    return x + mBounds[0]*y;
}
//----------------------------------------------------------------------------
template <typename T>
void TImage2D<T>::GetCoordinates (int index, int& x, int& y) const
{
    x = index % mBounds[0];
    y = index / mBounds[0];
}
//----------------------------------------------------------------------------
template <typename T>
TImage2D<T>& TImage2D<T>::operator= (const TImage2D& image)
{
    return (TImage2D<T>&) TImage<T>::operator=(image);
}
//----------------------------------------------------------------------------
template <typename T>
TImage2D<T>& TImage2D<T>::operator= (T value)
{
    return (TImage2D<T>&) TImage<T>::operator=(value);
}
//----------------------------------------------------------------------------
