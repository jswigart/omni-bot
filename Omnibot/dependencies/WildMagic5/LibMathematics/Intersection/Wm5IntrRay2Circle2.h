// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRRAY2CIRCLE2_H
#define WM5INTRRAY2CIRCLE2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Ray2.h"
#include "Wm5Circle2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrRay2Circle2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrRay2Circle2 (const Ray2<Real>& ray, const Circle2<Real>& circle);

    // Object access.
    const Ray2<Real>& GetRay () const;
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
    const Ray2<Real>* mRay;
    const Circle2<Real>* mCircle;

    // Information about the intersection set.
    int mQuantity;
    Vector2<Real> mPoint[2];
};

typedef IntrRay2Circle2<float> IntrRay2Circle2f;
typedef IntrRay2Circle2<double> IntrRay2Circle2d;

}

#endif
