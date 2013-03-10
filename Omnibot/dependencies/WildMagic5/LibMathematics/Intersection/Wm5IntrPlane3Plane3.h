// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRPLANE3PLANE3_H
#define WM5INTRPLANE3PLANE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Plane3.h"
#include "Wm5Line3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrPlane3Plane3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrPlane3Plane3 (const Plane3<Real>& plane0, const Plane3<Real>& plane1);

    // Object access.
    const Plane3<Real>& GetPlane0 () const;
    const Plane3<Real>& GetPlane1 () const;

    // Static intersection queries.
    virtual bool Test ();
    virtual bool Find ();

    // Dynamic intersection queries.
    virtual bool Test (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);
    virtual bool Find (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Information about the intersection set.  Only get the specific object
    // of intersection corresponding to the intersection type (IT_LINE or
    // IT_PLANE).
    const Line3<Real>& GetIntersectionLine () const;
    const Plane3<Real>& GetIntersectionPlane () const;

protected:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_LINE;
    using Intersector<Real,Vector3<Real> >::IT_PLANE;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;
    using Intersector<Real,Vector3<Real> >::mContactTime;

    // The objects to intersect.
    const Plane3<Real>* mPlane0;
    const Plane3<Real>* mPlane1;

    // Information about the intersection set.
    Line3<Real> mIntrLine;
    Plane3<Real> mIntrPlane;
};

typedef IntrPlane3Plane3<float> IntrPlane3Plane3f;
typedef IntrPlane3Plane3<double> IntrPlane3Plane3d;

}

#endif
