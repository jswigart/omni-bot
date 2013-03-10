// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRCIRCLE3PLANE3_H
#define WM5INTRCIRCLE3PLANE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Circle3.h"
#include "Wm5Plane3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrCircle3Plane3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrCircle3Plane3 (const Circle3<Real>& circle,
        const Plane3<Real>& plane);

    // Object access.
    const Circle3<Real>& GetCircle () const;
    const Plane3<Real>& GetPlane () const;

    // Static intersection queries.
    virtual bool Test ();
    virtual bool Find ();

    // Information about the intersection set.  Only get the specific object
    // of intersection corresponding to the intersection type.  If the type
    // is IT_POINT, use GetPoint(i).  If the type is IT_OTHER, the set is a
    // circle, so use GetIntersectionCircle(), which returns the circle
    // object.
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;
    const Circle3<Real>& GetIntersectionCircle () const;

protected:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_PLANE;
    using Intersector<Real,Vector3<Real> >::IT_OTHER;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;

    // The objects to intersect.
    const Circle3<Real>* mCircle;
    const Plane3<Real>* mPlane;

    // Information about the intersection set.
    int mQuantity;
    Vector3<Real> mPoint[2];
};

typedef IntrCircle3Plane3<float> IntrCircle3Plane3f;
typedef IntrCircle3Plane3<double> IntrCircle3Plane3d;

}

#endif
