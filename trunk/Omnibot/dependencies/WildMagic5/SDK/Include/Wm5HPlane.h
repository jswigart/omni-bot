// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2011/03/27)

#ifndef WM5HPLANE_H
#define WM5HPLANE_H

#include "Wm5MathematicsLIB.h"
#include "Wm5APoint.h"

namespace Wm5
{

class WM5_MATHEMATICS_ITEM HPlane
{
public:
    // The plane is represented as Dot(N,X) - c = 0, where N = (n0,n1,n2,0)
    // is a unit-length normal vector, c is the plane constant, and
    // X = (x0,x1,x2,1) is any point on the plane.  The user must ensure
    // that the normal vector is unit length.  The storage as a 4-tuple is
    // (n0,n1,n2,-c).

    // Construction and destruction.
    HPlane ();  // uninitialized
    HPlane (const HPlane& plane);
    ~HPlane ();

    // Specify N and c directly.
    HPlane (float normal0, float normal1, float normal2, float constant);
    HPlane (const AVector& normal, float constant);

    // N is specified, c = Dot(N,P) where P = (p0,p1,p2,1) is a point on the
    // plane.
    HPlane (const AVector& normal, const APoint& p);

    // N = Cross(P1-P0,P2-P0)/Length(Cross(P1-P0,P2-P0)), c = Dot(N,P0) where
    // P0, P1, P2 are points on the plane.
    HPlane (const APoint& p0, const APoint& p1, const APoint& p2);

    // Specify the entire (n0,n1,n2,-c) tuple.
    HPlane (const HPoint& tuple);

    // Implicit conversion to HPoint.
    inline operator HPoint ();
    inline operator HPoint () const;

    // Coordinate access.
    inline operator const float* () const;
    inline operator float* ();
    inline const float& operator[] (int i) const;
    inline float& operator[] (int i);

    // Assignment.
    HPlane& operator= (const HPlane& plane);

    // Comparison (for use by STL containers).
    bool operator== (const HPlane& plane) const;
    bool operator!= (const HPlane& plane) const;
    bool operator<  (const HPlane& plane) const;
    bool operator<= (const HPlane& plane) const;
    bool operator>  (const HPlane& plane) const;
    bool operator>= (const HPlane& plane) const;

    // Access to individual components.
    inline void SetNormal (const AVector& normal);
    inline void SetConstant (float constant);
    inline AVector GetNormal () const;
    inline float GetConstant () const;

    // Compute L = Length(n0,n1,n2) and set the plane to (n0,n1,n2,-c)/L.
    // This is useful when transforming planes by homogeneous matrices, where
    // the unit-length normal is not guaranteed.  The function returns L.
    float Normalize (const float epsilon = Mathf::ZERO_TOLERANCE);

    // Compute d = Dot(N,P)-c where N is the plane normal and c is the plane
    // constant.  This is a signed distance.  The sign of the return value is
    // positive if the point is on the positive side of the plane, negative if
    // the point is on the negative side, and zero if the point is on the
    // plane.
    float DistanceTo (const APoint& p) const;

    // The "positive side" of the plane is the half space to which the plane
    // normal points.  The "negative side" is the other half space.  The
    // function returns +1 when P is on the positive side, -1 when P is on the
    // the negative side, or 0 when P is on the plane.
    int WhichSide (const APoint& p) const;

private:
    // Storage is (n0,n1,n2,-c).
    HPoint mTuple;
};

#include "Wm5HPlane.inl"

}

#endif
