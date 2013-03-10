// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTLINE3RAY3_H
#define WM5DISTLINE3RAY3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Line3.h"
#include "Wm5Ray3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistLine3Ray3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistLine3Ray3 (const Line3<Real>& line, const Ray3<Real>& ray);

    // Object access.
    const Line3<Real>& GetLine () const;
    const Ray3<Real>& GetRay () const;

    // Static distance queries.
    virtual Real Get ();
    virtual Real GetSquared ();

    // Function calculations for dynamic distance queries.
    virtual Real Get (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);
    virtual Real GetSquared (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Information about the closest points.
    Real GetLineParameter () const;
    Real GetRayParameter () const;

private:
    using Distance<Real,Vector3<Real> >::mClosestPoint0;
    using Distance<Real,Vector3<Real> >::mClosestPoint1;

    const Line3<Real>* mLine;
    const Ray3<Real>* mRay;

    // Information about the closest points.
    Real mLineParameter;  // closest0 = line.origin+param*line.direction
    Real mRayParameter;  // closest1 = ray.origin+param*ray.direction
};

typedef DistLine3Ray3<float> DistLine3Ray3f;
typedef DistLine3Ray3<double> DistLine3Ray3d;

}

#endif
