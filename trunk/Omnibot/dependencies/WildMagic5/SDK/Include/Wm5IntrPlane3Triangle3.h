// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRPLANE3TRIANGLE3_H
#define WM5INTRPLANE3TRIANGLE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Plane3.h"
#include "Wm5Triangle3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrPlane3Triangle3
    : public Intersector<Real,Vector3<Real> >
{
public:
    // If you want a fuzzy determination, set the epsilon value to a small
    // positive number.
    IntrPlane3Triangle3 (const Plane3<Real>& plane,
        const Triangle3<Real>& triangle, Real epsilon = (Real)0);

    // Object access.
    const Plane3<Real>& GetPlane () const;
    const Triangle3<Real>& GetTriangle () const;

    // Static intersection queries.
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set, which is empty, a point, a line segment, or the
    // entire triangle (GetQuantity returns 0, 1, 2, or 3).
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

protected:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector3<Real> >::IT_POLYGON;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;

    // The objects to intersect.
    const Plane3<Real>* mPlane;
    const Triangle3<Real>* mTriangle;

    // Information about the intersection set.
    int mQuantity;
    Vector3<Real> mPoint[3];

    // For fuzzy arithmetic.
    Real mEpsilon;
};

typedef IntrPlane3Triangle3<float> IntrPlane3Triangle3f;
typedef IntrPlane3Triangle3<double> IntrPlane3Triangle3d;

}

#endif
