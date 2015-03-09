// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BSPLINESURFACEPATCH_H
#define WM5BSPLINESURFACEPATCH_H

#include "Wm5GraphicsLIB.h"
#include "Wm5SurfacePatch.h"
#include "Wm5BSplineRectangle.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM BSplineSurfacePatch : public SurfacePatch
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(BSplineSurfacePatch);

public:
    // Construction and destruction.   The caller is responsible for deleting
    // the input arrays if they were dynamically allocated.  Internal copies
    // of the arrays are made, so to dynamically change control points or
    // knots you must use the 'SetControlPoint' and 'SetKnot' member functions
    // of BSplineRectanglef.  Spline types for curves are
    //   open uniform (OU)
    //   periodic uniform (PU)
    //   open nonuniform (ON)
    // For tensor product surfaces, you have to choose a type for each of two
    // dimensions, leading to nine possible spline types for surfaces.  The
    // constructors below represent these choices.

    // (OU,OU), (OU,PU), (PU,OU), or (PU,PU)
    BSplineSurfacePatch (int numUCtrlPoints, int numVCtrlPoints,
        Vector3f** ctrlPoints, int uDegree, int vDegree, bool uLoop,
        bool vLoop, bool uOpen, bool vOpen);

    // (OU,ON) or (PU,ON)
    BSplineSurfacePatch (int numUCtrlPoints, int numVCtrlPoints,
        Vector3f** ctrlPoints, int uDegree, int vDegree, bool uLoop,
        bool vLoop, bool uOpen, float* vKnots);

    // (ON,OU) or (ON,PU)
    BSplineSurfacePatch (int numUCtrlPoints, int numVCtrlPoints,
        Vector3f** ctrlPoints, int uDegree, int vDegree, bool uLoop,
        bool vLoop, float* uKnots, bool vOpen);

    // (ON,ON)
    BSplineSurfacePatch (int numUCtrlPoints, int numVCtrlPoints,
        Vector3f** ctrlPoints, int uDegree, int vDegree, bool uLoop,
        bool vLoop, float* uKnots, float* vKnots);

    virtual ~BSplineSurfacePatch ();

    // Position and derivatives up to second order.
    virtual APoint P (float u, float v) const;
    virtual AVector PU (float u, float v) const;
    virtual AVector PV (float u, float v) const;
    virtual AVector PUU (float u, float v) const;
    virtual AVector PUV (float u, float v) const;
    virtual AVector PVV (float u, float v) const;

protected:
    // The class has four constructors, not counting the default one used
    // for streaming.  The correct constructor needs to be called on a stream
    // load operation, so this data member stores that information.  The
    // value is 1, 2, 3, or 4 and refers to the order of the four nondefault
    // constructors listed previously.
    int mConstructor;

    BSplineRectanglef* mPatch;
};

WM5_REGISTER_STREAM(BSplineSurfacePatch);
typedef Pointer0<SurfacePatch> BSplineSurfacePatchPtr;

}

#endif
