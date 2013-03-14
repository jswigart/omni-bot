// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRSEGMENT2TRIANGLE2_H
#define WM5INTRSEGMENT2TRIANGLE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Segment2.h"
#include "Wm5Triangle2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrSegment2Triangle2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrSegment2Triangle2 (const Segment2<Real>& segment,
        const Triangle2<Real>& triangle);

    // Object access.
    const Segment2<Real>& GetSegment () const;
    const Triangle2<Real>& GetTriangle () const;

    // Static intersection query.
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set.  If the segment and triangle do not intersect,
    // GetQuantity() returns 0, in which case the intersection type is
    // IT_EMPTY.  If the segment and triangle intersect in a single point,
    // GetQuantity() returns 1, in which case the intersection type is
    // IT_POINT and GetPoint() returns the intersection point.  If the segment
    // and triangle intersect in a segment, GetQuantity() returns 2, in which
    // case the intersection type is IT_SEGMENT and GetPoint() returns the
    // segment endpoints.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector2<Real> >::mIntersectionType;

    // The objects to intersect.
    const Segment2<Real>* mSegment;
    const Triangle2<Real>* mTriangle;

    // Information about the intersection set.
    int mQuantity;
    Vector2<Real> mPoint[2];
};

typedef IntrSegment2Triangle2<float> IntrSegment2Triangle2f;
typedef IntrSegment2Triangle2<double> IntrSegment2Triangle2d;

}

#endif
