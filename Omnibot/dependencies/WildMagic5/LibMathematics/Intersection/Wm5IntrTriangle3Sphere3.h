// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRTRIANGLE3SPHERE3_H
#define WM5INTRTRIANGLE3SPHERE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Triangle3.h"
#include "Wm5Sphere3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrTriangle3Sphere3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrTriangle3Sphere3 (const Triangle3<Real>& triangle,
        const Sphere3<Real>& sphere);

    // Object access.
    const Triangle3<Real>& GetTriangle () const;
    const Sphere3<Real>& GetSphere () const;

    // Static test-intersection query.
    virtual bool Test ();

    // Dynamic find-intersection query.  If the query returns 'true', use
    // GetContactTime() to obtain the first time of contact.  If this time
    // is zero, the triangle is already intersecting the sphere and no
    // contact set is computed.  If this time is positive, obtain the
    // first point of contact using GetPoint().
    virtual bool Find (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // The contact point.
    const Vector3<Real>& GetPoint () const;

private:
    using Intersector<Real,Vector3<Real> >::mContactTime;

    // Support for the dynamic query.
    bool FindTriangleSphereCoplanarIntersection (int vertex,
        const Vector3<Real> V[3], const Vector3<Real>& sideNorm,
        const Vector3<Real>& side, Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    bool FindSphereVertexIntersection (const Vector3<Real>& vertex,
        Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // The objects to intersect.
    const Triangle3<Real>* mTriangle;
    const Sphere3<Real>* mSphere;

    // The point of contact for dynamic queries.
    Vector3<Real> mPoint;
};

typedef IntrTriangle3Sphere3<float> IntrTriangle3Sphere3f;
typedef IntrTriangle3Sphere3<double> IntrTriangle3Sphere3d;

}

#endif
