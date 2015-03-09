// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BOUND_H
#define WM5BOUND_H

#include "Wm5GraphicsLIB.h"
#include "Wm5HPlane.h"
#include "Wm5Transform.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM Bound
{
public:
    // Construction and destruction.  The Bound represents a sphere of the
    // specified center and radius.
    Bound ();
    ~Bound ();

    // Assignment.
    Bound& operator= (const Bound& bound);

    // Member access.  Any Bound must define a center and a radius.
    inline void SetCenter (const APoint& center);
    inline void SetRadius (float radius);
    inline const APoint& GetCenter () const;
    inline float GetRadius () const;

    // Operations on bounding volumes.
    int WhichSide (const HPlane& plane) const;
    void GrowToContain (const Bound& bound);
    void TransformBy (const Transform& transform, Bound& bound);
    void ComputeFromData (int numElements, int stride, const char* data);

    // Test for intersection of linear component and bound (points of
    // intersection not computed).  The linear component is parameterized by
    // P + t*D, where P is a point on the component (the origin) and D is a
    // unit-length direction vector.  The interval [tmin,tmax] is
    //   line:     tmin = -Mathf::MAX_REAL, tmax = Mathf::MAX_REAL
    //   ray:      tmin = 0.0f, tmax = Mathf::MAX_REAL
    //   segment:  tmin >= 0.0f, tmax > tmin
    bool TestIntersection (const APoint& origin, const AVector& direction,
        float tmin, float tmax) const;

    // Test for intersection of the two stationary bounds.
    bool TestIntersection (const Bound& bound) const;

    // Test for intersection of the two moving bounds.  Velocity0 is that of
    // the calling Bound and velocity1 is that of the input bound.
    bool TestIntersection (const Bound& bound, float tmax,
        const AVector& velocity0, const AVector& velocity1) const;

    // Streaming support.
    inline static int GetStreamingSize ();

private:
    APoint mCenter;
    float mRadius;
};

#include "Wm5Bound.inl"

}

#endif
