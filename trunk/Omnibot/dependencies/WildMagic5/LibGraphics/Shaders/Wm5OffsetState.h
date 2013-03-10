// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5OFFSETSTATE_H
#define WM5OFFSETSTATE_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM OffsetState : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(OffsetState);

public:
    // Construction and destruction.
    OffsetState ();
    virtual ~OffsetState ();

    // Set whether offset should be enabled for the various polygon drawing
    // modes (fill, line, point).
    bool FillEnabled;   // default: false
    bool LineEnabled;   // default: false
    bool PointEnabled;  // default: false

    // The offset is Scale*dZ + Bias*r where dZ is the change in depth
    // relative to the screen space area of the poly, and r is the smallest
    // resolvable depth difference.  Negative values move polygons closer to
    // the eye.
    float Scale;  // default: 0
    float Bias;   // default: 0
};

WM5_REGISTER_STREAM(OffsetState);
typedef Pointer0<OffsetState> OffsetStatePtr;

}

#endif
