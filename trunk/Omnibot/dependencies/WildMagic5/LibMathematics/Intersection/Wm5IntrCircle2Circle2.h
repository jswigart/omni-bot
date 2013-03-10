// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRCIRCLE2CIRCLE2_H
#define WM5INTRCIRCLE2CIRCLE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Circle2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrCircle2Circle2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrCircle2Circle2 (const Circle2<Real>& circle0,
        const Circle2<Real>& circle1);

    // Object access.
    const Circle2<Real>& GetCircle0 () const;
    const Circle2<Real>& GetCircle1 () const;

    // Static find-intersection query.
    virtual bool Find ();

    // Intersection set for static find-intersection query.  The quantity Q is
    // 0, 1, or 2.  When Q > 0, the interpretation depends on the intersection
    // type.
    //   IT_POINT:  Q distinct points of intersection
    //   IT_OTHER:  The circles are the same.  One of the circle objects is
    //              returned by GetIntersectionCircle.  Q is invalid.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;
    const Circle2<Real>& GetIntersectionCircle () const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_OTHER;
    using Intersector<Real,Vector2<Real> >::mIntersectionType;

    // The objects to intersect.
    const Circle2<Real>* mCircle0;
    const Circle2<Real>* mCircle1;

    // Points of intersection.
    int mQuantity;
    Vector2<Real> mPoint[2];
};

typedef IntrCircle2Circle2<float> IntrCircle2Circle2f;
typedef IntrCircle2Circle2<double> IntrCircle2Circle2d;

}

#endif
