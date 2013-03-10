// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRRAY2RAY2_H
#define WM5INTRRAY2RAY2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Ray2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrRay2Ray2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrRay2Ray2 (const Ray2<Real>& ray0, const Ray2<Real>& ray1);

    // Object access.
    const Ray2<Real>& GetRay0 () const;
    const Ray2<Real>& GetRay1 () const;

    // Static intersection query.
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set.  If the rays do not intersect, GetQuantity()
    // returns 0.  If the rays intersect in a single point, GetQuantity()
    // returns 1, in which case GetPoint() returns the point of intersection
    // and Intersector::GetIntersectionType() returns IT_POINT.  If the rayd
    // are collinear, GetQuantity() returns INT_MAX and
    // Intersector::GetIntersectionType() returns IT_RAY.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint () const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_RAY;
    using Intersector<Real,Vector2<Real> >::mIntersectionType;

    // Determine the relationship between the lines that contain the rays.
    int Classify (Real* s, Vector2<Real>* diff, Vector2<Real>* diffN);

    // The objects to intersect.
    const Ray2<Real>* mRay0;
    const Ray2<Real>* mRay1;

    // Information about the intersection set.
    int mQuantity;
    Vector2<Real> mPoint;
};

typedef IntrRay2Ray2<float> IntrRay2Ray2f;
typedef IntrRay2Ray2<double> IntrRay2Ray2d;

}

#endif
