// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5RECTANGLESURFACE_H
#define WM5RECTANGLESURFACE_H

#include "Wm5GraphicsLIB.h"
#include "Wm5ParametricSurface.h"
#include "Wm5TriMesh.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM RectangleSurface : public TriMesh
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(RectangleSurface);

public:
    // Construction and destruction.  The input surface must be rectangular,
    // not triangular.  RectangleSurface accepts responsibility for deleting
    // the input surface.  Any texture coordinates in the vertex format must
    // be 2D.  The minimum and maximum texture coordinates are specified in
    // the constructor.  If you have none, set tcoordMin and tcoordMax to
    // null.
    RectangleSurface (ParametricSurfacef* surface, int numUSamples,
        int numVSamples, VertexFormat* vformat, const Float2* tcoordMin,
        const Float2* tcoordMax);

    virtual ~RectangleSurface ();

    // Member access.
    inline void SetSurface (ParametricSurfacef* surface);
    inline const ParametricSurfacef* GetSurface () const;
    inline int GetNumUSamples () const;
    inline int GetNumVSamples () const;

    // If the surface is modified, for example if it is control point
    // based and the control points are modified, then you should call this
    // update function to recompute the rectangle surface geometry.
    void UpdateSurface ();

protected:
    ParametricSurfacef* mSurface;
    int mNumUSamples, mNumVSamples;
};

WM5_REGISTER_STREAM(RectangleSurface);
typedef Pointer0<RectangleSurface> RectangleSurfacePtr;
#include "Wm5RectangleSurface.inl"

}

#endif
