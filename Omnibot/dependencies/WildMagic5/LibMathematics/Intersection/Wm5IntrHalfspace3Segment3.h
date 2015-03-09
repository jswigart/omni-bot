// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRHALFSPACE3SEGMENT3_H
#define WM5INTRHALFSPACE3SEGMENT3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Plane3.h"
#include "Wm5Segment3.h"

// A halfspace is the set of points on the side of a plane to which the plane
// normal points.  The queries here are for intersection of a line segment and
// a halfspace.  In the dynamice find query, if the segment is already
// intersecting the halfspace, the return value is 'false'.  The idea is to
// find first time of contact.

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrHalfspace3Segment3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrHalfspace3Segment3 (const Plane3<Real>& halfspace,
        const Segment3<Real>& segment);

    // Object access.
    const Plane3<Real>& GetHalfspace () const;
    const Segment3<Real>& GetSegment () const;

    // Static queries.
    virtual bool Test ();
    virtual bool Find ();

    // Dynamic queries.
    virtual bool Test (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    virtual bool Find (Real tmax, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // The intersection set is empty, a point, or a segment.  The function
    // GetQuantity() returns 0, 1, or 2.
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

protected:
    using Intersector<Real,Vector3<Real> >::mContactTime;

    // The objects to intersect.
    const Plane3<Real>* mHalfspace;
    const Segment3<Real>* mSegment;

    // Information about the intersection set.
    int mQuantity;
    Vector3<Real> mPoint[2];
};

typedef IntrHalfspace3Segment3<float> IntrHalfspace3Segment3f;
typedef IntrHalfspace3Segment3<double> IntrHalfspace3Segment3d;

}

#endif
