// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5TIMAGE2D_H
#define WM5TIMAGE2D_H

#include "Wm5ImagicsLIB.h"
#include "Wm5TImage.h"

namespace Wm5
{

template <typename T>
class TImage2D : public TImage<T>
{
public:
    // Construction and destruction.  TImage2D accepts responsibility for
    // deleting the input data array.
    TImage2D (int xBound, int yBound, T* data = 0);
    TImage2D (const TImage2D& image);
    TImage2D (const char* filename);

    // Data access.
    T& operator() (int x, int y) const;

    // Conversion between 2D coordinates and 1D indexing.
    int GetIndex (int x, int y) const;
    void GetCoordinates (int index, int& x, int& y) const;

    // Assignment.
    TImage2D& operator= (const TImage2D& image);
    TImage2D& operator= (T value);

protected:
    using TImage<T>::SetBounds;
    using TImage<T>::SetData;
    using TImage<T>::mBounds;
    using TImage<T>::mData;
};

#include "Wm5TImage2D.inl"

}

#endif
