// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/10/01)

#ifndef WM5PICKRECORD_H
#define WM5PICKRECORD_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Spatial.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM PickRecord
{
public:
    // Construction and destruction.  The default constructor does not
    // initialize any members.  It exists to support the construction
    // of the const static PickRecord in the Picker class.
    inline PickRecord ();
    inline ~PickRecord ();

    // For sorting purposes.
    inline bool operator== (const PickRecord& record) const;
    inline bool operator!= (const PickRecord& record) const;
    inline bool operator<  (const PickRecord& record) const;
    inline bool operator<= (const PickRecord& record) const;
    inline bool operator>  (const PickRecord& record) const;
    inline bool operator>= (const PickRecord& record) const;

    // The intersected object.
    SpatialPtr Intersected;

    // The linear component is parameterized by P + t*D.  The T member is
    // the value of parameter t at the intersection point.
    float T;

    // The index of the triangle that is intersected by the ray.
    int Triangle;

    // The barycentric coordinates of the point of intersection.  All of the
    // coordinates are in [0,1] and b0 + b1 + b2 = 1.
    float Bary[3];
};

#include "Wm5PickRecord.inl"

}

#endif
