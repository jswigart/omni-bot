// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DISTSEGMENT3SEGMENT3_H
#define WM5DISTSEGMENT3SEGMENT3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Distance.h"
#include "Wm5Segment3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM DistSegment3Segment3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistSegment3Segment3 (const Segment3<Real>& segment0,
        const Segment3<Real>& segment1);

    // Object access.
    const Segment3<Real>& GetSegment0 () const;
    const Segment3<Real>& GetSegment1 () const;

    // Static distance queries.
    virtual Real Get ();
    virtual Real GetSquared ();

    // Function calculations for dynamic distance queries.
    virtual Real Get (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);
    virtual Real GetSquared (Real t, const Vector3<Real>& velocity0,
        const Vector3<Real>& velocity1);

    // Information about the closest points.
    Real GetSegment0Parameter () const;
    Real GetSegment1Parameter () const;

private:
    using Distance<Real,Vector3<Real> >::mClosestPoint0;
    using Distance<Real,Vector3<Real> >::mClosestPoint1;

    const Segment3<Real>* mSegment0;
    const Segment3<Real>* mSegment1;

    // Information about the closest points.
    Real mSegment0Parameter;  // closest0 = seg0.origin+param*seg0.direction
    Real mSegment1Parameter;  // closest1 = seg1.origin+param*seg1.direction
};

typedef DistSegment3Segment3<float> DistSegment3Segment3f;
typedef DistSegment3Segment3<double> DistSegment3Segment3d;

}

#endif
