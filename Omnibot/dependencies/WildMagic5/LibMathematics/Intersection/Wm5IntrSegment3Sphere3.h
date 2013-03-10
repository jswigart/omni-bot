// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRSEGMENT3SPHERE3_H
#define WM5INTRSEGMENT3SPHERE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Segment3.h"
#include "Wm5Sphere3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrSegment3Sphere3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSegment3Sphere3 (const Segment3<Real>& segment,
        const Sphere3<Real>& sphere);

    // Object access.
    const Segment3<Real>& GetSegment () const;
    const Sphere3<Real>& GetSphere () const;

    // Static intersection queries.
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set.
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;
    Real GetSegmentParameter (int i) const;

    // Dynamic test-intersection query.
    virtual bool Test (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Dynamic find-intersection query.  The first point of contact is
    // accessed by GetPoint(0).  The first time of contact is accessed by
    // GetContactTime().
    virtual bool Find (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    Real ZeroThreshold;  // default = Math<Real>::ZERO_TOLERANCE

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector3<Real> >::IT_OTHER;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;
    using Intersector<Real,Vector3<Real> >::mContactTime;

    // The objects to intersect.
    const Segment3<Real>* mSegment;
    const Sphere3<Real>* mSphere;

    // Information about the intersection set.
    int mQuantity;
    Vector3<Real> mPoint[2];
    Real mSegmentParameter[2];
};

typedef IntrSegment3Sphere3<float> IntrSegment3Sphere3f;
typedef IntrSegment3Sphere3<double> IntrSegment3Sphere3d;

}

#endif
