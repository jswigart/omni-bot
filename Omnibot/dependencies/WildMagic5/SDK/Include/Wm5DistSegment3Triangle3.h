// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTSEGMENT3TRIANGLE3_H
#define WM5DISTSEGMENT3TRIANGLE3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Segment3.h"
#include "Wm5Triangle3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistSegment3Triangle3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistSegment3Triangle3 (const Segment3<Real>& segment,
        const Triangle3<Real>& triangle);

    // Object access.
    const Segment3<Real>& GetSegment () const;
    const Triangle3<Real>& GetTriangle () const;

    // Static distance queries.
    virtual Real Get ();
    virtual Real GetSquared ();

    // Function calculations for dynamic distance queries.
    virtual Real Get (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);
    virtual Real GetSquared (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Information about the closest points.
    Real GetSegmentParameter () const;
    Real GetTriangleBary (int i) const;

private:
    using Distance<Real,Vector3<Real> >::mClosestPoint0;
    using Distance<Real,Vector3<Real> >::mClosestPoint1;

    const Segment3<Real>* mSegment;
    const Triangle3<Real>* mTriangle;

    // Information about the closest points.
    Real mSegmentParameter;  // closest0 = seg.origin+param*seg.direction
    Real mTriangleBary[3];  // closest1 = sum_{i=0}^2 bary[i]*tri.vertex[i]
};

typedef DistSegment3Triangle3<float> DistSegment3Triangle3f;
typedef DistSegment3Triangle3<double> DistSegment3Triangle3d;

}

#endif
