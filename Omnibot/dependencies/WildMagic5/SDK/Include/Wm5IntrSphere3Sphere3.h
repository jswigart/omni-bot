// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2010/10/01)

#ifndef WM5INTRSPHERE3SPHERE3_H
#define WM5INTRSPHERE3SPHERE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Circle3.h"
#include "Wm5Sphere3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrSphere3Sphere3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSphere3Sphere3 (const Sphere3<Real>& sphere0,
        const Sphere3<Real>& sphere1);

    // Object access.
    const Sphere3<Real>& GetSphere0 () const;
    const Sphere3<Real>& GetSphere1 () const;

    // Static test-intersection query.
    virtual bool Test ();

    // Static find-intersection query.  The enumerated values are in addition
    // to those in Intersector.  Use GetIntersectionType() to return the enum
    // after the "bool Find()" call.
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_OTHER;

    enum
    {
        // IT_EMPTY:  spheres are disjoint/separated
        // IT_POINT:  spheres touch at point, each sphere outside the other
        IT_CIRCLE = IT_OTHER + 1,  // spheres intersect in a circle
        IT_SPHERE0,        // sphere0 strictly contained in sphere1
        IT_SPHERE0_POINT,  // sphere0 contained in sphere1, share common point
        IT_SPHERE1,        // sphere1 strictly contained in sphere0
        IT_SPHERE1_POINT   // sphere1 contained in sphere0, share common point
    };

    virtual bool Find ();

    // Intersection set for static find-intersection query.
    const Circle3<Real>& GetCircle () const;

    // Dynamic test-intersection query.
    virtual bool Test (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Dynamic find-intersection query.
    virtual bool Find (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Intersection set for dynamic find-intersection query.
    const Vector3<Real>& GetContactPoint () const;

private:
    using Intersector<Real,Vector3<Real> >::mContactTime;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;

    // The objects to intersect.
    const Sphere3<Real>* mSphere0;
    const Sphere3<Real>* mSphere1;

    // Circle of intersection for static spheres.
    Circle3<Real> mCircle;

    // Point of intersection for dynamic spheres.
    Vector3<Real> mContactPoint;
};

typedef IntrSphere3Sphere3<float> IntrSphere3Sphere3f;
typedef IntrSphere3Sphere3<double> IntrSphere3Sphere3d;

}

#endif
