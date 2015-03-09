// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/03/27)

#ifndef WM5IVECTOR_H
#define WM5IVECTOR_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Assert.h"

namespace Wm5
{

template <int VSIZE>
class IVector
{
public:
    // Construction.
    IVector ();
    IVector (const IVector& vec);

    // Coordinate access.
    inline operator const int64_t* () const;
    inline operator int64_t* ();
    inline const int64_t& operator[] (int i) const;
    inline int64_t& operator[] (int i);

    // Assignment.
    IVector& operator= (const IVector& vec);

    // Comparison.
    bool operator== (const IVector& vec) const;
    bool operator!= (const IVector& vec) const;
    bool operator<  (const IVector& vec) const;
    bool operator<= (const IVector& vec) const;
    bool operator>  (const IVector& vec) const;
    bool operator>= (const IVector& vec) const;

    // Arithmetic operations.
    IVector operator+ (const IVector& vec) const;
    IVector operator- (const IVector& vec) const;
    IVector operator* (const int64_t& scalar) const;
    IVector operator/ (const int64_t& scalar) const;
    IVector operator- () const;

    // Arithmetic updates.
    IVector& operator+= (const IVector& vec);
    IVector& operator-= (const IVector& vec);
    IVector& operator*= (const int64_t& scalar);
    IVector& operator/= (const int64_t& scalar);

    // Vector operations.
    int64_t SquaredLength () const;
    int64_t Dot (const IVector& vec) const;

protected:
    // Support for comparisons.
    int CompareArrays (const IVector& vec) const;

    int64_t mTuple[VSIZE];
};

template <int VSIZE>
IVector<VSIZE> operator* (const int64_t& scalar, const IVector<VSIZE>& vec);

#include "Wm5IVector.inl"

}

#endif
