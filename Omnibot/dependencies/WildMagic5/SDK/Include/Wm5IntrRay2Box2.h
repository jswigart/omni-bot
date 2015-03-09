// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRRAY2BOX2_H
#define WM5INTRRAY2BOX2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Ray2.h"
#include "Wm5Box2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrRay2Box2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrRay2Box2 (const Ray2<Real>& ray, const Box2<Real>& box);

    // Object access.
    const Ray2<Real>& GetRay () const;
    const Box2<Real>& GetBox () const;

    // Static intersection queries.
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector2<Real> >::mIntersectionType;

    // The objects to intersect.
    const Ray2<Real>* mRay;
    const Box2<Real>* mBox;

    // Information about the intersection set.
    int mQuantity;
    Vector2<Real> mPoint[2];
};

typedef IntrRay2Box2<float> IntrRay2Box2f;
typedef IntrRay2Box2<double> IntrRay2Box2d;

}

#endif
