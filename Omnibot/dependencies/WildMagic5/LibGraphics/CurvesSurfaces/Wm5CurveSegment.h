// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CURVESEGMENT_H
#define WM5CURVESEGMENT_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"
#include "Wm5APoint.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM CurveSegment : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(CurveSegment);

protected:
    // Construction.
    CurveSegment (float umin, float umax);
public:
    // Abstract base class.
    virtual ~CurveSegment ();

    // The parametric domain is umin <= u <= umax.
    float GetUMin () const;
    float GetUMax () const;

    // Position and derivatives up to third order.
    virtual APoint P (float u) const = 0;
    virtual AVector PU (float u) const = 0;
    virtual AVector PUU (float u) const = 0;
    virtual AVector PUUU (float u) const = 0;

    // Differential geometric quantities.
    AVector Tangent (float u) const;
    AVector Normal (float u) const;
    AVector Binormal (float u) const;
    void GetFrame (float u, APoint& position, AVector& tangent,
        AVector& normal, AVector& binormal) const;
    float Curvature (float u) const;
    float Torsion (float u) const;

protected:
    float mUMin, mUMax;
};

WM5_REGISTER_STREAM(CurveSegment);
typedef Pointer0<CurveSegment> CurveSegmentPtr;

}

#endif
