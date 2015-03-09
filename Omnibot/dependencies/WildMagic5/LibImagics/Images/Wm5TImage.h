// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5TIMAGE_H
#define WM5TIMAGE_H

#include "Wm5ImagicsLIB.h"
#include "Wm5ImageConvert.h"
#include "Wm5Lattice.h"

namespace Wm5
{

// The typename T is intended to be a wrapper of native data (int, float,
// char, etc.).  The code uses memcpy, memcmp, and memset on the array of T
// values.  Typename T must have the following member functions:
//   T::T ()
//   T& T::operator= (T)
//   static const char* GetRTTI ()
// The static member function returns a string that is used for streaming.


template <typename T>
class TImage : public Lattice
{
public:
    // Construction and destruction.  TImage accepts responsibility for
    // deleting the input arrays.
    TImage (int numDimensions, int* bounds, T* data = 0);
    TImage (const TImage& image);
    TImage (const char* filename);
    virtual ~TImage ();

    // Data access.
    T* GetData () const;
    T& operator[] (int i) const;

    // Assignment.
    TImage& operator= (const TImage& image);
    TImage& operator= (T value);

    // Comparison.
    bool operator== (const TImage& image) const;
    bool operator!= (const TImage& image) const;

    // Streaming.
    bool Load (const char* filename);
    bool Save (const char* filename) const;

protected:
    // For deferred creation of bounds.
    TImage (int numDimensions);
    void SetData (T* data);

    T* mData;
};

#include "Wm5TImage.inl"

}

#endif
