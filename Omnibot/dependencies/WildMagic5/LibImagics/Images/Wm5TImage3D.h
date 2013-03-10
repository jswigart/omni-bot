// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5TIMAGE3D_H
#define WM5TIMAGE3D_H

#include "Wm5ImagicsLIB.h"
#include "Wm5TImage.h"

namespace Wm5
{

template <typename T>
class TImage3D : public TImage<T>
{
public:
    // Construction and destruction.  TImage3D accepts responsibility for
    // deleting the input data array.
    TImage3D (int xBound, int yBound, int zBound, T* data = 0);
    TImage3D (const TImage3D& image);
    TImage3D (const char* filename);

    // Data access.
    T& operator() (int x, int y, int z) const;

    // Conversion between 3D coordinates and 1D indexing.
    int GetIndex (int x, int y, int z) const;
    void GetCoordinates (int index, int& x, int& y, int& z) const;

    // Assignment.
    TImage3D& operator= (const TImage3D& image);
    TImage3D& operator= (T value);

protected:
    using TImage<T>::SetBounds;
    using TImage<T>::SetData;
    using TImage<T>::mBounds;
    using TImage<T>::mData;
};

#include "Wm5TImage3D.inl"

}

#endif
