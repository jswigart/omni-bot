// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRARC2CIRCLE2_H
#define WM5INTRARC2CIRCLE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Arc2.h"
#include "Wm5Circle2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrArc2Circle2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrArc2Circle2 (const Arc2<Real>& arc, const Circle2<Real>& circle);

    // Object access.
    const Arc2<Real>& GetArc () const;
    const Circle2<Real>& GetCircle () const;

    // Static find-intersection query.
    virtual bool Find ();

    // Intersection set for static find-intersection query.  The quantity Q is
    // 0, 1, or 2.  When Q > 0, the interpretation depends on the intersection
    // type.
    //   IT_POINT:  Q distinct points of intersection
    //   IT_OTHER:  The arc is already on the circle.  The arc is the full
    //              intersection set, returned by GetIntersectionArc.  Q is
    //              invalid.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;
    const Arc2<Real>& GetIntersectionArc () const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_OTHER;
    using Intersector<Real,Vector2<Real> >::mIntersectionType;

    // The objects to intersect.
    const Arc2<Real>* mArc;
    const Circle2<Real>* mCircle;

    // Points of intersection.
    int mQuantity;
    Vector2<Real> mPoint[2];
};

typedef IntrArc2Circle2<float> IntrArc2Circle2f;
typedef IntrArc2Circle2<double> IntrArc2Circle2d;

}

#endif
