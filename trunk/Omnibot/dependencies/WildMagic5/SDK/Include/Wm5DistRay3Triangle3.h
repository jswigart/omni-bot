// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTRAY3TRIANGLE3_H
#define WM5DISTRAY3TRIANGLE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Ray3.h"
#include "Wm5Triangle3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistRay3Triangle3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistRay3Triangle3 (const Ray3<Real>& ray,
        const Triangle3<Real>& triangle);

    // Object access.
    const Ray3<Real>& GetRay () const;
    const Triangle3<Real>& GetTriangle () const;

    // Static distance queries.
    virtual Real Get ();
    virtual Real GetSquared ();

    // Function calculations for dynamic distance queries.
    virtual Real Get (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    // Information about the closest points.
    Real GetRayParameter () const;
    Real GetTriangleBary (int i) const;

private:
    using Distance<Real,Vector3<Real> >::mClosestPoint0;
    using Distance<Real,Vector3<Real> >::mClosestPoint1;

    const Ray3<Real>* mRay;
    const Triangle3<Real>* mTriangle;

    // Information about the closest points.
    Real mRayParameter;  // closest0 = ray.origin+param*ray.direction
    Real mTriangleBary[3];  // closest1 = sum_{i=0}^2 bary[i]*tri.vertex[i]
};

typedef DistRay3Triangle3<float> DistRay3Triangle3f;
typedef DistRay3Triangle3<double> DistRay3Triangle3d;

}

#endif
