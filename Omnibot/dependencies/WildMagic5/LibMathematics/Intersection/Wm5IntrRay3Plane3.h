// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRRAY3PLANE3_H
#define WM5INTRRAY3PLANE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Ray3.h"
#include "Wm5Plane3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrRay3Plane3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrRay3Plane3 (const Ray3<Real>& ray, const Plane3<Real>& plane);

    // Object access.
    const Ray3<Real>& GetRay () const;
    const Plane3<Real>& GetPlane () const;

    // Test-intersection query.
    virtual bool Test ();

    // Find-intersection query.  The point of intersection is
    // P = origin + t*direction, with t >= 0.
    virtual bool Find ();
    Real GetRayParameter () const;

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;

    // The objects to intersect.
    const Ray3<Real>* mRay;
    const Plane3<Real>* mPlane;

    // Information about the intersection set.
    Real mRayParameter;
};

typedef IntrRay3Plane3<float> IntrRay3Plane3f;
typedef IntrRay3Plane3<double> IntrRay3Plane3d;

}

#endif
