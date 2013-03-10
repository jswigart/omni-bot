// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5SURFACEMESH_H
#define WM5SURFACEMESH_H

#include "Wm5GraphicsLIB.h"
#include "Wm5TriMesh.h"
#include "Wm5EdgeKey.h"
#include "Wm5Float2Array.h"
#include "Wm5SurfacePatch.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM SurfaceMesh : public TriMesh
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(SurfaceMesh);

public:
    // Construction and destruction.  If there are T triangles, ibuffer
    // has 3*T elements, patches has T elements, and spkParams has 3*T
    // elements.  Each triangle has a patch for constructing vertices in the
    // subdivision.  The subdivision requires knowing the parameter values
    // at the vertices of the original triangles.  SurfaceMesh assumes
    // responsibility for deleting the patches array (so it must be
    // dynamically allocated).  The parameter allowDynamicChange should be
    // set to 'true' when you plan on dynamically changing the surface
    // patches.  For example, you might modify the control points in a Bezier
    // surface patch.
    SurfaceMesh (VertexFormat* vformat, VertexBuffer* vbuffer,
        IndexBuffer* ibuffer, Float2Array* params, SurfacePatchPtr* patches,
        bool allowDynamicChange);

    virtual ~SurfaceMesh ();

    // Level of subdivision.  The 'level' value must be nonnegative.
    void SetLevel (int level);
    int GetLevel () const;

    // Call this function when one or more of your surface patches has
    // dynamically changed.
    void OnDynamicChange ();

    // Free up all memory used by the object, other than the TriMesh data.
    // The intent is that the surface mesh is in its final form.  Once this
    // function is called, SetLevel is ignored.
    void Lock ();
    bool IsLocked () const;

protected:
    VertexBufferPtr mOrigVBuffer;
    IndexBufferPtr mOrigIBuffer;
    Float2ArrayPtr mOrigParams;
    SurfacePatchPtr* mPatches;
    int mNumFullVertices, mNumPatches, mLevel;

private:
    // Support for subdivision of the mesh.
    class Edge
    {
    public:
        Edge (int v0 = -1, int v1 = -1);

        // Support for std::set.
        bool operator< (const Edge& edge) const;

        // Surface for subdivision evaluations.
        SurfacePatchPtr Patch;

        // Indices for the vertices.
        int V[2];

        // Surface parameter values for the endpoints.
        Float2 Param[2];

        // Index for the midpoint.
        int VMid;

        // Surface parameter value for the midpoint.
        Float2 ParamMid;

        // Number of triangles sharing the edge.
        int References;
    };

    typedef std::map<EdgeKey,Edge> EdgeMap;

    class Triangle
    {
    public:
        Triangle ();

        // The surface for subdivision evaluations.
        SurfacePatchPtr Patch;

        // Indices for the vertices.
        int V[3];
    };

    void Allocate (int& numTotalVertices, int& numTotalEdges,
        EdgeMap& edgeMap, int& numTotalTriangles, Triangle*& triangles);

    void Subdivide (int& numVertices, int& numEdges, EdgeMap& edgeMap,
        int& numTriangles, Triangle* triangles);

    void InsertInto (EdgeMap& edgeMap, SurfacePatch* patch, int v0, int v1,
        const Float2& param0, const Float2& param1, int newReferences);

    // Support for dynamic changes in the surface patches.
    class SurfaceInfo
    {
    public:
        SurfaceInfo ();

        SurfacePatchPtr Patch;
        Float2 Param;
    };

    void InitializeSurfaceInfo ();

    bool mAllowDynamicChange;
    SurfaceInfo* mSInfo;
};

WM5_REGISTER_STREAM(SurfaceMesh);
typedef Pointer0<SurfaceMesh> SurfaceMeshPtr;

}

#endif
