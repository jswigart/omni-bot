// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5SEGMENT2_H
#define WM5SEGMENT2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector2.h"

namespace Wm5
{

template <typename Real>
class Segment2
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
    Segment2 ();  // uninitialized
    ~Segment2 ();

    // The constructor computes C, D, and E from P0 and P1.
    Segment2 (const Vector2<Real>& p0, const Vector2<Real>& p1);

    // The constructor computes P0 and P1 from C, D, and E.
    Segment2 (const Vector2<Real>& center, const Vector2<Real>& direction,
        Real extent);

    // Call this function when you change P0 or P1.
    void ComputeCenterDirectionExtent ();

    // Call this function when you change C, D, or e.
    void ComputeEndPoints ();

    // End-point representation.
    Vector2<Real> P0, P1;

    // Center-direction-extent representation.
    Vector2<Real> Center;
    Vector2<Real> Direction;
    Real Extent;
};

#include "Wm5Segment2.inl"

typedef Segment2<float> Segment2f;
typedef Segment2<double> Segment2d;

}

#endif
