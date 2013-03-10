// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRSEGMENT3BOX3_H
#define WM5INTRSEGMENT3BOX3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Segment3.h"
#include "Wm5Box3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrSegment3Box3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSegment3Box3 (const Segment3<Real>& segment, const Box3<Real>& box,
        bool solid);

    // Object access.
    const Segment3<Real>& GetSegment () const;
    const Box3<Real>& GetBox () const;

    // Static test-intersection query.
    virtual bool Test ();

    // Static find-intersection query.  The point(s) of intersection are
    // accessed by GetQuantity() and GetPoint(i).
    virtual bool Find ();

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
    const Box3<Real>* mBox;
    bool mSolid;

    // Information about the intersection set.
    int mQuantity;
    Vector3<Real> mPoint[2];
};

typedef IntrSegment3Box3<float> IntrSegment3Box3f;
typedef IntrSegment3Box3<double> IntrSegment3Box3d;

}

#endif
