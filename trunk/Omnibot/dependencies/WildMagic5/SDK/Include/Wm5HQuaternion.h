// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/03/27)

#ifndef WM5HQUATERNION_H
#define WM5HQUATERNION_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Float4.h"
#include "Wm5HMatrix.h"

namespace Wm5
{

class WM5_MATHEMATICS_ITEM HQuaternion
{
public:
    // A quaternion is q = w + x*i + y*j + z*k where (w,x,y,z) is not
    // necessarily a unit-length vector in 4D.

    // Construction.
    HQuaternion ();  // uninitialized
    HQuaternion (float w, float x, float y, float z);
    HQuaternion (const HQuaternion& q);

    // HQuaternion for the input rotation matrix.
    HQuaternion (const HMatrix& rot);

    // HQuaternion for the rotation of the axis-angle pair.
    HQuaternion (const AVector& axis, float angle);

    // Coordinate access as an array:  0 = w, 1 = x, 2 = y, 3 = z.
    inline operator const float* () const;
    inline operator float* ();
    inline const float& operator[] (int i) const;
    inline float& operator[] (int i);
    inline float W () const;
    inline float& W ();
    inline float X () const;
    inline float& X ();
    inline float Y () const;
    inline float& Y ();
    inline float Z () const;
    inline float& Z ();

    // Assignment.
    HQuaternion& operator= (const HQuaternion& q);

    // Comparison (for use by STL containers).
    bool operator== (const HQuaternion& q) const;
    bool operator!= (const HQuaternion& q) const;
    bool operator<  (const HQuaternion& q) const;
    bool operator<= (const HQuaternion& q) const;
    bool operator>  (const HQuaternion& q) const;
    bool operator>= (const HQuaternion& q) const;

    // Arithmetic operations.
    HQuaternion operator+ (const HQuaternion& q) const;
    HQuaternion operator- (const HQuaternion& q) const;
    HQuaternion operator* (const HQuaternion& q) const;
    HQuaternion operator* (float scalar) const;
    HQuaternion operator/ (float scalar) const;
    HQuaternion operator- () const;

    WM5_MATHEMATICS_ITEM
    friend HQuaternion operator* (float scalar, const HQuaternion& q);

    // Arithmetic updates.
    HQuaternion& operator+= (const HQuaternion& q);
    HQuaternion& operator-= (const HQuaternion& q);
    HQuaternion& operator*= (float scalar);
    HQuaternion& operator/= (float scalar);

    // Conversion between quaternions, matrices, and axis-angle.
    void FromRotationMatrix (const HMatrix& rot);
    void ToRotationMatrix (HMatrix& rot) const;
    void FromAxisAngle (const AVector& axis, float angle);
    void ToAxisAngle (AVector& axis, float& angle) const;

    // Functions of a quaternion.
    float Length () const;  // length of 4-tuple
    float SquaredLength () const;  // squared length of 4-tuple
    float Dot (const HQuaternion& q) const;  // dot product of 4-tuples
    float Normalize (const float epsilon = Mathf::ZERO_TOLERANCE);
    HQuaternion Inverse () const;  // apply to non-zero quaternion
    HQuaternion Conjugate () const;  // negate x, y, and z terms
    HQuaternion Exp () const;  // apply to quaternion with w = 0
    HQuaternion Log () const;  // apply to unit-length quaternion

    // Rotation of a vector by a quaternion.
    AVector Rotate (const AVector& vec) const;

    // Spherical linear interpolation.
    HQuaternion& Slerp (float t, const HQuaternion& p, const HQuaternion& q);

    // Intermediate terms for spherical quadratic interpolation.
    HQuaternion& Intermediate (const HQuaternion& q0, const HQuaternion& q1,
        const HQuaternion& q2);

    // Spherical quadratic interpolation.
    HQuaternion& Squad (float t, const HQuaternion& q0, const HQuaternion& a0,
        const HQuaternion& a1, const HQuaternion& q1);

    // Special quaternions.
    static const HQuaternion ZERO;
    static const HQuaternion IDENTITY;

private:
    // Order of storage is (w,x,y,z).
    float mTuple[4];
};

#include "Wm5HQuaternion.inl"

}

#endif
