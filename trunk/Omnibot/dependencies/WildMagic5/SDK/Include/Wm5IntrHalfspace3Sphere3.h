// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRHALFSPACE3SPHERE3_H
#define WM5INTRHALFSPACE3SPHERE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Plane3.h"
#include "Wm5Sphere3.h"

// A halfspace is the set of points on the side of a plane to which the plane
// normal points.  The queries here are for intersection of a sphere and a
// halfspace.  In the dynamice find query, if the sphere is already
// intersecting the halfspace, the return value is 'false'.  The idea is to
// find first time of contact, in which case there is a single point of
// contacct.

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrHalfspace3Sphere3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrHalfspace3Sphere3 (const Plane3<Real>& halfspace,
        const Sphere3<Real>& sphere);

    // Object access.
    const Plane3<Real>& GetHalfspace () const;
    const Sphere3<Real>& GetSphere () const;

    // Static query.
    virtual bool Test ();

    // Dynamic queries.
    virtual bool Test (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    virtual bool Find (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // The intersection set is empty or a point.  GetPoint() returns a
    // valid result only when Find(...) returns 'true'.
    const Vector3<Real>& GetPoint () const;

protected:
    using Intersector<Real,Vector3<Real> >::mContactTime;

    // The objects to intersect.
    const Plane3<Real>* mHalfspace;
    const Sphere3<Real>* mSphere;

    // Information about the intersection set.
    Vector3<Real> mPoint;
};

typedef IntrHalfspace3Sphere3<float> IntrHalfspace3Sphere3f;
typedef IntrHalfspace3Sphere3<double> IntrHalfspace3Sphere3d;

}

#endif
