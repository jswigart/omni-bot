// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BOXSURFACE_H
#define WM5BOXSURFACE_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Node.h"
#include "Wm5BSplineVolume.h"
#include "Wm5TriMesh.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM BoxSurface : public Node
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(BoxSurface);

public:
    // Construction and destruction.  BoxSurface accepts responsibility for
    // deleting the input volume.
    //
    // The six TriMesh children of this object are initially stored in the
    // order: u=0 face, u=1 face, v=0 face, v=1 face, w=0 face, w=1 face.  The
    // texture coordinate ordering preserves the (u,v,w) ordering of the box
    // coordinate space.
    //   u=0 face:  (v,w) in [0,1]^2, origin at vertex (0,0,0)
    //   u=1 face:  (v,w) in [0,1]^2, origin at vertex (1,0,0)
    //   v=0 face:  (u,w) in [0,1]^2, origin at vertex (0,0,0)
    //   v=1 face:  (u,w) in [0,1]^2, origin at vertex (0,1,0)
    //   w=0 face:  (u,v) in [0,1]^2, origin at vertex (0,0,0)
    //   w=1 face:  (u,v) in [0,1]^2, origin at vertex (0,0,1)

    BoxSurface (BSplineVolumef* volume, int numUSamples, int numVSamples,
        int numWSamples, VertexFormat* vformat[6]);

    virtual ~BoxSurface ();

    // Member access.
    inline const BSplineVolumef* GetVolume () const;
    inline int GetNumUSamples () const;
    inline int GetNumVSamples () const;
    inline int GetNumWSamples () const;

    // When the volume is modified, call this update function to recompute
    // the box surface geometry.
    void UpdateSurface ();

    // The order of the faces can change by sorting.  Given a direction,
    // the faces are ordered so that the back-facing ones occur first in
    // the list followed by the front-facing ones.  This supports correct
    // alpha blending, at least when the faces are only moderately perturbed
    // by the B-spline volume.  The process of enabling and disabling
    // causes changes in the render state (cull state, zbuffer state).
    void EnableSorting ();
    void DisableSorting ();
    void SortFaces (const AVector& worldViewDirection);

protected:
    TriMesh* CreateFace (int numRows, int numCols, VertexFormat* vformat,
        bool ccw, float faceValue, int permute[3]);

    void UpdateFace (int numRows, int numCols, VertexFormat* vformat,
        VertexBuffer* vbuffer, bool ccw, float faceValue, int permute[3]);

    BSplineVolumef* mVolume;
    int mNumUSamples, mNumVSamples, mNumWSamples;
    bool mDoSort;
};

WM5_REGISTER_STREAM(BoxSurface);
typedef Pointer0<BoxSurface> BoxSurfacePtr;
#include "Wm5BoxSurface.inl"

}

#endif
