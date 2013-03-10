// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5VECTOR4_H
#define WM5VECTOR4_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Tuple.h"
#include "Wm5Math.h"

namespace Wm5
{

template <typename Real>
class Vector4 : public Tuple<4,Real>
{
public:
    // Construction.
    Vector4 ();  // uninitialized
    Vector4 (const Vector4& vec);
    Vector4 (const Tuple<4,Real>& tuple);
    Vector4 (Real x, Real y, Real z, Real w);

    // Assignment.
    Vector4& operator= (const Vector4& vec);
    Vector4& operator= (const Tuple<4,Real>& tuple);

    // Coordinate access.
    inline Real X () const;
    inline Real& X ();
    inline Real Y () const;
    inline Real& Y ();
    inline Real Z () const;
    inline Real& Z ();
    inline Real W () const;
    inline Real& W ();

    // Arithmetic operations.
    inline Vector4 operator+ (const Vector4& vec) const;
    inline Vector4 operator- (const Vector4& vec) const;
    inline Vector4 operator* (Real scalar) const;
    inline Vector4 operator/ (Real scalar) const;
    inline Vector4 operator- () const;

    // Arithmetic updates.
    inline Vector4& operator+= (const Vector4& vec);
    inline Vector4& operator-= (const Vector4& vec);
    inline Vector4& operator*= (Real scalar);
    inline Vector4& operator/= (Real scalar);

    // Vector operations.
    inline Real Length () const;
    inline Real SquaredLength () const;
    inline Real Dot (const Vector4& vec) const;
    inline Real Normalize (const Real epsilon = Math<Real>::ZERO_TOLERANCE);

    // Compute the axis-aligned bounding box of the points.
    static void ComputeExtremes (int numVectors, const Vector4* vectors,
        Vector4& vmin, Vector4& vmax);

    // Special vectors.
    WM5_MATHEMATICS_ITEM static const Vector4 ZERO;
    WM5_MATHEMATICS_ITEM static const Vector4 UNIT_X;  // (1,0,0,0)
    WM5_MATHEMATICS_ITEM static const Vector4 UNIT_Y;  // (0,1,0,0)
    WM5_MATHEMATICS_ITEM static const Vector4 UNIT_Z;  // (0,0,1,0)
    WM5_MATHEMATICS_ITEM static const Vector4 UNIT_W;  // (0,0,0,1)
    WM5_MATHEMATICS_ITEM static const Vector4 ONE;     // (1,1,1,1)

protected:
    using Tuple<4,Real>::mTuple;
};

// Arithmetic operations.
template <typename Real>
inline Vector4<Real> operator* (Real scalar, const Vector4<Real>& vec);

// debugging output
template <typename Real>
std::ostream& operator<< (std::ostream& outFile, const Vector4<Real>& vec);

#include "Wm5Vector4.inl"

typedef Vector4<float> Vector4f;
typedef Vector4<double> Vector4d;

}

#endif
