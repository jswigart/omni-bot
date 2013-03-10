// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRSEGMENT2CIRCLE2_H
#define WM5INTRSEGMENT2CIRCLE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Segment2.h"
#include "Wm5Circle2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrSegment2Circle2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrSegment2Circle2 (const Segment2<Real>& segment,
        const Circle2<Real>& circle);

    // Object access.
    const Segment2<Real>& GetSegment () const;
    const Circle2<Real>& GetCircle () const;

    // Static intersection query.
    virtual bool Find ();

    // The intersection set.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::mIntersectionType;

    // The objects to intersect.
    const Segment2<Real>* mSegment;
    const Circle2<Real>* mCircle;

    // Information about the intersection set.
    int mQuantity;
    Vector2<Real> mPoint[2];
};

typedef IntrSegment2Circle2<float> IntrSegment2Circle2f;
typedef IntrSegment2Circle2<double> IntrSegment2Circle2d;

}

#endif
