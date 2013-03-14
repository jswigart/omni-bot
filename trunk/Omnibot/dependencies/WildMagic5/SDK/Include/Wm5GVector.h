// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/03/27)

#ifndef WM5GVECTOR_H
#define WM5GVECTOR_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Math.h"
#include "Wm5Memory.h"
#include <cstring>

namespace Wm5
{

template <typename Real>
class GVector
{
public:
    // Construction and destruction.
    GVector (int size = 0);
    GVector (int size, const Real* tuple);
    GVector (const GVector& vec);
    ~GVector ();

    // Coordinate access.
    void SetSize (int size);
    inline int GetSize () const;
    inline operator const Real* () const;
    inline operator Real* ();
    inline const Real& operator[] (int i) const;
    inline Real& operator[] (int i);

    // Assignment.
    GVector& operator= (const GVector& vec);

    // Comparison (for use by STL containers).
    bool operator== (const GVector& vec) const;
    bool operator!= (const GVector& vec) const;
    bool operator<  (const GVector& vec) const;
    bool operator<= (const GVector& vec) const;
    bool operator>  (const GVector& vec) const;
    bool operator>= (const GVector& vec) const;

    // Arithmetic operations.
    GVector operator+ (const GVector& vec) const;
    GVector operator- (const GVector& vec) const;
    GVector operator* (Real scalar) const;
    GVector operator/ (Real scalar) const;
    GVector operator- () const;

    friend GVector operator* (Real scalar, const GVector& vec)
    {
        return vec*scalar;
    }

    // Arithmetic updates.
    GVector& operator+= (const GVector& vec);
    GVector& operator-= (const GVector& vec);
    GVector& operator*= (Real scalar);
    GVector& operator/= (Real scalar);

    // Vector operations.
    Real Length () const;
    Real SquaredLength () const;
    Real Dot (const GVector& vec) const;
    Real Normalize (Real epsilon = Math<Real>::ZERO_TOLERANCE);

protected:
    int mSize;
    Real* mTuple;
};

#include "Wm5GVector.inl"

typedef GVector<float> GVectorf;
typedef GVector<double> GVectord;

}

#endif
