// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTPOINT3SEGMENT3_H
#define WM5DISTPOINT3SEGMENT3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Segment3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistPoint3Segment3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistPoint3Segment3 (const Vector3<Real>& rkVector,
        const Segment3<Real>& rkSegment);

    // Object access.
    const Vector3<Real>& GetPoint () const;
    const Segment3<Real>& GetSegment () const;

    // Static distance queries.
    virtual Real Get ();
    virtual Real GetSquared ();

    // Function calculations for dynamic distance queries.
    virtual Real Get (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);
    virtual Real GetSquared (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Information about the closest segment point.
    Real GetSegmentParameter () const;

private:
    using Distance<Real,Vector3<Real> >::mClosestPoint0;
    using Distance<Real,Vector3<Real> >::mClosestPoint1;

    const Vector3<Real>* mPoint;
    const Segment3<Real>* mSegment;

    // Information about the closest segment point.
    Real mSegmentParameter;  // closest1 = seg.origin+param*seg.direction
};

typedef DistPoint3Segment3<float> DistPoint3Segment3f;
typedef DistPoint3Segment3<double> DistPoint3Segment3d;

}

#endif
