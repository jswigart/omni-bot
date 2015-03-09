// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTRAY3RECTANGLE3_H
#define WM5DISTRAY3RECTANGLE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Rectangle3.h"
#include "Wm5Ray3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistRay3Rectangle3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistRay3Rectangle3 (const Ray3<Real>& ray,
        const Rectangle3<Real>& rectangle);

    // Object access.
    const Ray3<Real>& GetRay () const;
    const Rectangle3<Real>& GetRectangle () const;

    // Static distance queries.
    virtual Real Get ();
    virtual Real GetSquared ();

    // Function calculations for dynamic distance queries.
    virtual Real Get (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);
    virtual Real GetSquared (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Information about the closest points.
    Real GetRayParameter () const;
    Real GetRectangleCoordinate (int i) const;

private:
    using Distance<Real,Vector3<Real> >::mClosestPoint0;
    using Distance<Real,Vector3<Real> >::mClosestPoint1;

    const Ray3<Real>* mRay;
    const Rectangle3<Real>* mRectangle;

    // Information about the closest points.

    // closest0 = ray.origin + param*ray.direction
    Real mRayParameter;

    // closest1 = rect.center + param0*rect.dir0 + param1*rect.dir1
    Real mRectCoord[2];
};

typedef DistRay3Rectangle3<float> DistRay3Rectangle3f;
typedef DistRay3Rectangle3<double> DistRay3Rectangle3d;

}

#endif
