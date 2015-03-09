// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5SURFACEPATCH_H
#define WM5SURFACEPATCH_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Object.h"
#include "Wm5APoint.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM SurfacePatch : public Object
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(SurfacePatch);

protected:
    // Construction.
    SurfacePatch (float umin, float umax, float vmin, float vmax,
        bool rectangular);
public:
    // Abstract base class.
    virtual ~SurfacePatch ();

    // The parametric domain is either rectangular or triangular.  Valid (u,v)
    // values for a rectangular domain satisfy
    //   umin <= u <= umax,  vmin <= v <= vmax
    // Valid (u,v) values for a triangular domain satisfy
    //   umin <= u <= umax,  vmin <= v <= vmax,
    //   (vmax-vmin)*(u-umin)+(umax-umin)*(v-vmax) <= 0
    float GetUMin () const;
    float GetUMax () const;
    float GetVMin () const;
    float GetVMax () const;
    bool IsRectangular () const;

    // Position and derivatives up to second order.
    virtual APoint P (float u, float v) const = 0;
    virtual AVector PU (float u, float v) const = 0;
    virtual AVector PV (float u, float v) const = 0;
    virtual AVector PUU (float u, float v) const = 0;
    virtual AVector PUV (float u, float v) const = 0;
    virtual AVector PVV (float u, float v) const = 0;

    // Compute a coordinate frame.  The set {T0,T1,N} is a right-handed
    // orthonormal set.
    AVector Tangent0 (float u, float v) const;
    AVector Tangent1 (float u, float v) const;
    AVector Normal (float u, float v) const;
    void GetFrame (float u, float v, APoint& position, AVector& tangent0,
        AVector& tangent1, AVector& normal) const;

    // Differential geometric quantities.  The returned scalars are the
    // principal curvatures and the returned vectors are the corresponding
    // principal directions.
    void ComputePrincipalCurvatureInfo (float u, float v, float& curv0,
        float& curv1, AVector& dir0, AVector& dir1);

protected:
    float mUMin, mUMax, mVMin, mVMax;
    bool mRectangular;
};

WM5_REGISTER_STREAM(SurfacePatch);
typedef Pointer0<SurfacePatch> SurfacePatchPtr;

}

#endif
