// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTRAY3SEGMENT3_H
#define WM5DISTRAY3SEGMENT3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Ray3.h"
#include "Wm5Segment3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistRay3Segment3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistRay3Segment3 (const Ray3<Real>& ray, const Segment3<Real>& segment);

    // Object access.
    const Ray3<Real>& GetRay () const;
    const Segment3<Real>& GetSegment () const;

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
    Real GetSegmentParameter () const;

private:
    using Distance<Real,Vector3<Real> >::mClosestPoint0;
    using Distance<Real,Vector3<Real> >::mClosestPoint1;

    const Ray3<Real>* mRay;
    const Segment3<Real>* mSegment;

    // Information about the closest points.
    Real mRayParameter;  // closest0 = ray.origin+param*ray.direction
    Real mSegmentParameter;  // closest1 = seg.origin+param*seg.direction
};

typedef DistRay3Segment3<float> DistRay3Segment3f;
typedef DistRay3Segment3<double> DistRay3Segment3d;

}

#endif
