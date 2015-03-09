// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRBOX2CIRCLE2_H
#define WM5INTRBOX2CIRCLE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Box2.h"
#include "Wm5Circle2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrBox2Circle2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrBox2Circle2 (const Box2<Real>& box, const Circle2<Real>& circle);

    // Object access.
    const Box2<Real>& GetBox () const;
    const Circle2<Real>& GetCircle () const;

    // Static test-intersection query.
    virtual bool Test ();

    // Dynamic find-intersection query.
    virtual bool Find (Real tmax, const Vector2<Real>& velocity0,
        const Vector2<Real>& velocity1);

    // Intersection set for dynamic find-intersection query.
    const Vector2<Real>& GetContactPoint () const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_OTHER;
    using Intersector<Real,Vector2<Real> >::mIntersectionType;
    using Intersector<Real,Vector2<Real> >::mContactTime;

    // Support for dynamic Find.  Both functions return -1 if the objects are
    // initially intersecting, 0 if no intersection, or +1 if they intersect
    // at some positive time.
    int TestVertexRegion (Real cx, Real cy, Real vx, Real vy, Real ex,
        Real ey, Real& ix, Real& iy);

    int TestEdgeRegion (Real cx, Real cy, Real vx, Real vy, Real ex, Real ey,
        Real& ix, Real& iy);

    // The objects to intersect.
    const Box2<Real>* mBox;
    const Circle2<Real>* mCircle;

    // Point of intersection.
    Vector2<Real> mContactPoint;
};

typedef IntrBox2Circle2<float> IntrBox2Circle2f;
typedef IntrBox2Circle2<double> IntrBox2Circle2d;

}

#endif
