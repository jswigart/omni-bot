// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5TUBESURFACE_H
#define WM5TUBESURFACE_H

#include "Wm5GraphicsLIB.h"
#include "Wm5Curve3.h"
#include "Wm5TriMesh.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM TubeSurface : public TriMesh
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(TubeSurface);

public:
    typedef float (*RadialFunction)(float);

    // Construction and destruction.  TubeSurface accepts responsibility
    // for deleting the input curve.  If upVector is not the zero vector,
    // it will be used as 'up' in the frame calculations.  If it is the zero
    // vector, the Frenet frame will be used.  If bWantColors is 'true',
    // the vertex colors are allocated and set to black.  The application
    // needs to assign colors as needed.  If either of tcoordMin or tcoordMax is
    // not null, both must be not null.  In this case, texture coordinates are
    // generated for the surface.
    TubeSurface (Curve3f* medial, RadialFunction radial, bool closed,
        const Vector3f& upVector, int numMedialSamples, int mNumSliceSamples,
        bool sampleByArcLength, bool insideView, const Float2* tcoordMin,
        const Float2* tcoordMax, VertexFormat* vformat);

    virtual ~TubeSurface ();

    // Member access.
    inline void SetMedial (Curve3f* medial);
    inline const Curve3f* GetMedial () const;
    inline void SetRadial (RadialFunction radial);
    inline RadialFunction GetRadial () const;
    inline void SetUpVector (const Vector3f& upVector);
    inline const Vector3f& GetUpVector () const;
    inline int GetNumMedialSamples () const;
    inline int GetNumSliceSamples () const;

    // Generate vertices for the end slices.  These are useful when you build
    // an open tube and want to attach meshes at the ends to close the tube.
    // The input array must have size (at least) S+1 where S is the number
    // returned by GetSliceSamples.  Function GetTMinSlice is used to access
    // the slice corresponding to the medial curve evaluated at its domain
    // minimum, tmin.  Function GetTMaxSlice accesses the slice for the
    // domain maximum, tmax.  If the curve is closed, the slices are the same.
    void GetTMinSlice (Vector3f* slice);
    void GetTMaxSlice (Vector3f* slice);

    // If the medial curve is modified, for example if it is control point
    // based and the control points are modified, then you should call this
    // update function to recompute the tube surface geometry.
    void UpdateSurface ();

protected:
    // Tessellation support.
    int Index (int s, int m);
    void ComputeSinCos ();
    void ComputeVertices ();
    void ComputeNormals ();
    void ComputeUVs (const Float2& tcoordMin, const Float2& tcoordMax);
    void ComputeIndices (bool insideView);

    Curve3f* mMedial;
    RadialFunction mRadial;
    int mNumMedialSamples, mNumSliceSamples;
    Vector3f mUpVector;
    float* mSin;
    float* mCos;
    bool mClosed, mSampleByArcLength;
};

WM5_REGISTER_STREAM(TubeSurface);
typedef Pointer0<TubeSurface> TubeSurfacePtr;
#include "Wm5TubeSurface.inl"

}

#endif
