// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRSEGMENT3PLANE3_H
#define WM5INTRSEGMENT3PLANE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Segment3.h"
#include "Wm5Plane3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrSegment3Plane3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSegment3Plane3 (const Segment3<Real>& segment,
        const Plane3<Real>& plane);

    // Object access.
    const Segment3<Real>& GetSegment () const;
    const Plane3<Real>& GetPlane () const;

    // Test-intersection query.
    virtual bool Test ();

    // Find-intersection query.  The point of intersection is
    // P = origin + t*direction, with |t| <= e, where e is the segment
    // extent.
    virtual bool Find ();
    Real GetSegmentParameter () const;

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;

    // The objects to intersect.
    const Segment3<Real>* mSegment;
    const Plane3<Real>* mPlane;

    // Information about the intersection set.
    Real mSegmentParameter;
};

typedef IntrSegment3Plane3<float> IntrSegment3Plane3f;
typedef IntrSegment3Plane3<double> IntrSegment3Plane3d;

}

#endif
