// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5SEGMENT3_H
#define WM5SEGMENT3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{

template <typename Real>
class Segment3
{
public:
    // The segment is represented as (1-s)*P0+s*P1, where P0 and P1 are the
    // endpoints of the segment and 0 <= s <= 1.
    //
    // Some algorithms involving segments might prefer a centered
    // representation similar to how oriented bounding boxes are defined.
    // This representation is C+t*D, where C = (P0+P1)/2 is the center of
    // the segment, D = (P1-P0)/Length(P1-P0) is a unit-length direction
    // vector for the segment, and |t| <= e.  The value e = Length(P1-P0)/2
    // is the 'extent' (or radius or half-length) of the segment.
    
    // Construction and destruction.
    Segment3 ();  // uninitialized
    ~Segment3 ();

    // The constructor computes C, D, and E from P0 and P1.
    Segment3 (const Vector3<Real>& p0, const Vector3<Real>& p1);

    // The constructor computes P0 and P1 from C, D, and E.
    Segment3 (const Vector3<Real>& center, const Vector3<Real>& direction,
        Real extent);

    // Call this function when you change P0 or P1.
    void ComputeCenterDirectionExtent ();

    // Call this function when you change C, D, or e.
    void ComputeEndPoints ();

    // End-point representation.
    Vector3<Real> P0, P1;

    // Center-direction-extent representation.
    Vector3<Real> Center;
    Vector3<Real> Direction;
    Real Extent;
};

#include "Wm5Segment3.inl"

typedef Segment3<float> Segment3f;
typedef Segment3<double> Segment3d;

}

#endif
