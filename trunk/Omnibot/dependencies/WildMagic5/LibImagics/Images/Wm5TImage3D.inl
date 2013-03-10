// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename T>
TImage3D<T>::TImage3D (int xBound, int yBound, int zBound, T* data)
    :
    TImage<T>(3)
{
    int* bounds = new1<int>(3);
    bounds[0] = xBound;
    bounds[1] = yBound;
    bounds[2] = zBound;
    SetBounds(bounds);
    SetData(data);
}
//----------------------------------------------------------------------------
template <typename T>
TImage3D<T>::TImage3D (const TImage3D& image)
    :
    TImage<T>(image)
{
}
//----------------------------------------------------------------------------
template <typename T>
TImage3D<T>::TImage3D (const char* filename)
    :
    TImage<T>(filename)
{
}
//----------------------------------------------------------------------------
template <typename T>
T& TImage3D<T>::operator() (int x, int y, int z) const
{
    return mData[x + mBounds[0]*(y + mBounds[1]*z)];
}
//----------------------------------------------------------------------------
template <typename T>
int TImage3D<T>::GetIndex (int x, int y, int z) const
{
    return x + mBounds[0]*(y + mBounds[1]*z);
}
//----------------------------------------------------------------------------
template <typename T>
void TImage3D<T>::GetCoordinates (int index, int& x, int& y, int& z) const
{
    x = index % mBounds[0];
    index /= mBounds[0];
    y = index % mBounds[1];
    z = index / mBounds[1];
}
//----------------------------------------------------------------------------
template <typename T>
TImage3D<T>& TImage3D<T>::operator= (const TImage3D& image)
{
    return (TImage3D<T>&) TImage<T>::operator=(image);
}
//----------------------------------------------------------------------------
template <typename T>
TImage3D<T>& TImage3D<T>::operator= (T value)
{
    return (TImage3D<T>&) TImage<T>::operator=(value);
}
//----------------------------------------------------------------------------
