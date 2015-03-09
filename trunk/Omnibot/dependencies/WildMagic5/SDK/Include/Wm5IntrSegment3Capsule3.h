// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INTRSEGMENT3CAPSULE3_H
#define WM5INTRSEGMENT3CAPSULE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Segment3.h"
#include "Wm5Capsule3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrSegment3Capsule3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSegment3Capsule3 (const Segment3<Real>& segment,
        const Capsule3<Real>& capsule);

    // Object access.
    const Segment3<Real>& GetSegment () const;
    const Capsule3<Real>& GetCapsule () const;

    // Static intersection queries.
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set.
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;
    Real GetParameter (int i) const;  // segment parameters for points

private:
    using Intersector<Real,Vector3<Real> >::IT_EMPTY;
    using Intersector<Real,Vector3<Real> >::IT_POINT;
    using Intersector<Real,Vector3<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector3<Real> >::mIntersectionType;

    // The objects to intersect.
    const Segment3<Real>* mSegment;
    const Capsule3<Real>* mCapsule;

    // Information about the intersection set.
    int mQuantity;
    Vector3<Real> mPoint[2];
    Real mParameter[2];
};

typedef IntrSegment3Capsule3<float> IntrSegment3Capsule3f;
typedef IntrSegment3Capsule3<double> IntrSegment3Capsule3d;

}

#endif
