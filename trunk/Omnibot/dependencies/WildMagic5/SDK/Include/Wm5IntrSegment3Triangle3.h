// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRSEGMENT3TRIANGLE3_H
#define WM5INTRSEGMENT3TRIANGLE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Segment3.h"
#include "Wm5Triangle3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrSegment3Triangle3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSegment3Triangle3 (const Segment3<Real>& segment,
        const Triangle3<Real>& triangle);

    // Object access.
    const Segment3<Real>& GetSegment () const;
    const Triangle3<Real>& GetTriangle () const;

    // Test-intersection query.
    virtual bool Test ();

    // Find-intersection query.  The point of intersection is
    //   P = origin + t*direction = b0*V0 + b1*V1 + b2*V2
    virtual bool Find ();
    Real GetSegmentParameter () const;
    Real GetTriBary0 () const;
    Real GetTriBary1 () const;
    Real GetTriBary2 () const;

    // Dynamic test-intersection query.
    virtual bool Test (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Dynamic find-intersection query.  The first point of contact is
    // accessed by GetPoint(0), when there is a single contact, or by
    // GetPoint(0) and GetPoint(1), when the contact is a segment, in which
    // case the fetched points are the segment endpoints.  The first time of
    // contact is accessed by GetContactTime().
    virtual bool Find (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector3<Real> >::mContactTime;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;

    // The objects to intersect.
    const Segment3<Real>* mSegment;
    const Triangle3<Real>* mTriangle;

    // Information about the stationary intersection set.
    Real mSegmentParameter, mTriBary0, mTriBary1, mTriBary2;

    // Information about the dynamic intersection set.
    int mQuantity;
    Vector3<Real> mPoint[2];
};

typedef IntrSegment3Triangle3<float> IntrSegment3Triangle3f;
typedef IntrSegment3Triangle3<double> IntrSegment3Triangle3d;

}

#endif
