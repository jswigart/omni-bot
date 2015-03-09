// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5AXISALIGNEDBOX2_H
#define WM5AXISALIGNEDBOX2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector2.h"

namespace Wm5
{

template <typename Real>
class AxisAlignedBox2
{
public:
    // Construction and destruction.
    AxisAlignedBox2 ();  // uninitialized
    ~AxisAlignedBox2 ();

    // The caller must ensure that xmin <= xmax and ymin <= ymax.
    AxisAlignedBox2 (Real xmin, Real xmax, Real ymin, Real ymax);

    // Compute the center of the box and the extents (half-lengths)
    // of the box edges.
    void GetCenterExtents (Vector2<Real>& center, Real extent[2]);

    // Overlap testing is in the strict sense.  If the two boxes are just
    // touching along a common edge, the boxes are reported as overlapping.
    bool HasXOverlap (const AxisAlignedBox2& box) const;
    bool HasYOverlap (const AxisAlignedBox2& box) const;
    bool TestIntersection (const AxisAlignedBox2& box) const;

    // The return value is 'true' if there is overlap.  In this case the
    // intersection is stored in 'intersection'.  If the return value is
    // 'false', there is no overlap.  In this case 'intersection' is
    // undefined.
    bool FindIntersection (const AxisAlignedBox2& box,
        AxisAlignedBox2& intersection) const;

    Real Min[2], Max[2];
};

#include "Wm5AxisAlignedBox2.inl"

typedef AxisAlignedBox2<float> AxisAlignedBox2f;
typedef AxisAlignedBox2<double> AxisAlignedBox2d;

}

#endif
