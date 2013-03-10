// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/03/27)

#ifndef WM5RVECTOR_H
#define WM5RVECTOR_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Rational.h"

namespace Wm5
{

template <int VSIZE, int ISIZE>
class RVector
{
public:
    // Construction.
    RVector ();
    RVector (const RVector& vec);

    // Coordinate access.
    inline operator const Rational<ISIZE>* () const;
    inline operator Rational<ISIZE>* ();
    inline const Rational<ISIZE>& operator[] (int i) const;
    inline Rational<ISIZE>& operator[] (int i);

    // Assignment.
    RVector& operator= (const RVector& vec);

    // Comparison.
    bool operator== (const RVector& vec) const;
    bool operator!= (const RVector& vec) const;
    bool operator<  (const RVector& vec) const;
    bool operator<= (const RVector& vec) const;
    bool operator>  (const RVector& vec) const;
    bool operator>= (const RVector& vec) const;

    // Arithmetic operations.
    RVector operator+ (const RVector& vec) const;
    RVector operator- (const RVector& vec) const;
    RVector operator* (const Rational<ISIZE>& scalar) const;
    RVector operator/ (const Rational<ISIZE>& scalar) const;
    RVector operator- () const;

    // Arithmetic updates.
    RVector& operator+= (const RVector& vec);
    RVector& operator-= (const RVector& vec);
    RVector& operator*= (const Rational<ISIZE>& scalar);
    RVector& operator/= (const Rational<ISIZE>& scalar);

    // Vector operations.
    Rational<ISIZE> SquaredLength () const;
    Rational<ISIZE> Dot (const RVector& vec) const;

protected:
    // Support for comparisons.
    int CompareArrays (const RVector& vec) const;

    Rational<ISIZE> mTuple[VSIZE];
};

template <int VSIZE, int ISIZE>
RVector<VSIZE,ISIZE> operator* (const Rational<ISIZE>& scalar,
    const RVector<VSIZE,ISIZE>& vec);

#include "Wm5RVector.inl"

}

#endif
