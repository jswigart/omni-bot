// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5APOINT_H
#define WM5APOINT_H

#include "Wm5MathematicsLIB.h"
#include "Wm5AVector.h"

namespace Wm5
{

class WM5_MATHEMATICS_ITEM APoint : public HPoint
{
public:
    // Construction and destruction.  APoint represents an affine point of the
    // form (x,y,z,1).  The destructor hides the HPoint destructor, which is
    // not a problem because there are no side effects that must occur in the
    // base class.
    APoint ();  // default (0,0,0,1)
    APoint (const APoint& pnt);
    APoint (float x, float y, float z);
    APoint (const Float3& tuple);
    APoint (const Vector3f& pnt);
    ~APoint ();

    // Implicit conversions.
    inline operator const Float3& () const;
    inline operator Float3& ();
    inline operator const Vector3f& () const;
    inline operator Vector3f& ();

    // Assignment.
    APoint& operator= (const APoint& pnt);

    // Arithmetic operations supported by affine algebra.

    // A point minus a point is a vector.
    AVector operator- (const APoint& pnt) const;

    // A point plus or minus a vector is a point.
    APoint operator+ (const AVector& vec) const;
    APoint operator- (const AVector& vec) const;
    APoint& operator+= (const AVector& vec);
    APoint& operator-= (const AVector& vec);

    // In affine algebra, points cannot be added arbitrarily.  However,
    // affine sums and affine differences are allowed.  You are responsible
    // for ensuring that you are computing one or the other.
    //
    // An affine sum is of the form
    //   r = s1*p1 + s2*p2 + ... + sn*pn
    // where p1 through pn are homogeneous points (w-values are 1) and
    // s1 through sn are scalars for which s1 + s2 + ... + sn = 1.  The
    // result r is a homogenous point.
    //
    // An affine difference is of the form
    //   r = d1*p1 + d2*p2 + ... + dn*pn
    // where p1 through pn are homogeneous points (w-values are 1) and
    // d1 through dn are scalars for which d1 + d2 + ... + dn = 0.  The
    // result r is a homogeneous vector.  NOTE:  The arithemtic operations
    // of this class return APoint objects, but the affine difference needs
    // to be an HVector object.  The following code shows how to accomplish
    // this:
    //   APoint p1, p2, p3;  // initialized to whatever
    //   HVector r = 1.5f*p1 + (-0.2f)*p2 + (-0.3f)*p3;
    // The right-hand side is computed using APoint operations, so it is an
    // APoint object.  HVector has a constructor that takes a 'const float*'.
    // APoint has an implicit conversion to 'const float*'.  The code
    //   APoint somePoint;  // initialized to whatever
    //   HVector r = somePoint;
    // involves an APoint implicit conversion to 'const float*' followed by
    // an HVector(const float*) constructor call.  The latter copies only the
    // x, y, and z components and sets the w component to zero.
    APoint operator+ (const APoint& pnt) const;
    APoint operator* (float scalar) const;
    APoint operator/ (float scalar) const;

    WM5_MATHEMATICS_ITEM
    friend APoint operator* (float scalar, const APoint& pnt);

    APoint& operator+= (const APoint& pnt);
    APoint& operator-= (const APoint& pnt);
    APoint& operator*= (float scalar);
    APoint& operator/= (float scalar);
    APoint operator- () const;

    // The dot product between a point and a vector is not allowed in affine
    // algebra.  However, it is convenient to have one defined when dealing
    // with planes.  Specifically, a plane is Dot(N,X-P) = 0, where N is a
    // vector, P is a specific point on the plane, and X is any point on the
    // plane.  The difference X-P is a vector, so Dot(N,X-P) is well defined.
    // If the plane is rewritten as Dot(N,X) = Dot(N,P), this is not supported
    // by affine algebra, but we sometimes need to compute Dot(N,P) anyway.
    // In the following, the w-component of the APoint is ignored, which means
    // the APoint is treated as a vector.
    float Dot (const AVector& vec) const;

    // Special vector.
    static const APoint ORIGIN;  // (0,0,0,1)
};

#include "Wm5APoint.inl"

}

#endif
