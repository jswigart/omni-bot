// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5AVECTOR_H
#define WM5AVECTOR_H

#include "Wm5MathematicsLIB.h"
#include "Wm5HPoint.h"
#include "Wm5Float3.h"
#include "Wm5Vector3.h"

namespace Wm5
{

class WM5_MATHEMATICS_ITEM AVector : public HPoint
{
public:
    // Construction and destruction.  AVector represents an affine vector of
    // the form (x,y,z,0).  The destructor hides the HPoint destructor, which
    // is not a problem because there are no side effects that must occur in
    // the base class.
    AVector ();  // default (0,0,0,0)
    AVector (const AVector& vec);
    AVector (float x, float y, float z);
    AVector (const Float3& tuple);
    AVector (const Vector3f& vec);
    ~AVector ();

    // Implicit conversions.
    inline operator const Float3& () const;
    inline operator Float3& ();
    inline operator const Vector3f& () const;
    inline operator Vector3f& ();

    // Assignment.
    AVector& operator= (const AVector& vec);

    // Arithmetic operations.
    AVector operator+ (const AVector& vec) const;
    AVector operator- (const AVector& vec) const;
    AVector operator* (float scalar) const;
    AVector operator/ (float scalar) const;
    AVector operator- () const;

    WM5_MATHEMATICS_ITEM
    friend AVector operator* (float scalar, const AVector& vec);

    // Arithmetic updates.
    AVector& operator+= (const AVector& vec);
    AVector& operator-= (const AVector& vec);
    AVector& operator*= (float scalar);
    AVector& operator/= (float scalar);

    // Vector operations.
    float Length () const;
    float SquaredLength () const;
    float Dot (const AVector& vec) const;
    float Normalize (const float epsilon = Mathf::ZERO_TOLERANCE);
    AVector Cross (const AVector& vec) const;
    AVector UnitCross (const AVector& vec) const;

    // Inputs must be initialized nonzero vectors.
    static void Orthonormalize (AVector& vec0, AVector& vec1, AVector& vec2);
    static void Orthonormalize (AVector* vec);

    // Input vec2 must be a nonzero vector. The output is an orthonormal
    // basis {vec0,vec1,vec2}.  The input vec2 is normalized by this function.
    // If you know that vec2 is already unit length, use the function
    // GenerateComplementBasis to compute vec0 and vec1.
    static void GenerateOrthonormalBasis (AVector& vec0, AVector& vec1,
        AVector& vec2);

    // Input vec0 must be a unit-length vector.  The output vectors
    // {vec0,vec1} are unit length and mutually perpendicular, and
    // {vec0,vec1,vec2} is an orthonormal basis.
    static void GenerateComplementBasis (AVector& vec0, AVector& vec1,
        const AVector& vec2);

    // Special vectors.
    static const AVector ZERO;    // (0,0,0,0)
    static const AVector UNIT_X;  // (1,0,0,0)
    static const AVector UNIT_Y;  // (0,1,0,0)
    static const AVector UNIT_Z;  // (0,0,1,0)
};

#include "Wm5AVector.inl"

}

#endif
