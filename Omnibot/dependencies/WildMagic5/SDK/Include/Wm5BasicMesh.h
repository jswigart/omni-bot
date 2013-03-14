// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5BASICMESH_H
#define WM5BASICMESH_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"

namespace Wm5
{

class WM5_MATHEMATICS_ITEM BasicMesh
{
public:
    // Construction and destruction.  The number of vertices in the input is
    // numVertices.  The vertex array is usually passed as points, but this
    // input can be any data type you prefer (points+attributes).  The number
    // of triangles is numTriangles.  The triangles are represented as triples
    // of indices into the vertex array.  These triples are stored in indices.
    // The caller is responsible for deleting the input arrays.
    BasicMesh (int numVertices, const void* points, int numTriangles,
        const int* indices);

    virtual ~BasicMesh ();

    // The input must correspond to a mesh whose edges are shared by no more
    // than two triangles.  If an edge with three (or more) edges is
    // encountered in the construction, IsValid returns 'false'.  Otherwise
    // it returns 'true'.
    inline bool IsValid () const;

    class WM5_MATHEMATICS_ITEM Vertex
    {
    public:
        Vertex ();
        ~Vertex ();

        enum { MV_CHUNK = 8 };

        void InsertEdge (int v, int e);
        void InsertTriangle (int t);

        int NumVertices;
        int* V;
        int* E;
        int NumTriangles;
        int* T;
    };

    class WM5_MATHEMATICS_ITEM Edge
    {
    public:
        Edge ();

        int V[2];
        int T[2];
    };

    class WM5_MATHEMATICS_ITEM Triangle
    {
    public:
        Triangle ();

        int V[3];
        int E[3];
        int T[3];
    };

    // Member access.
    inline int GetNumVertices () const;
    inline int GetNumEdges () const;
    inline int GetNumTriangles () const;
    inline const void* GetPoints () const;
    inline const int* GetIndices () const;
    inline const Vertex* GetVertices () const;
    inline const Edge* GetEdges () const;
    inline const Triangle* GetTriangles () const;

protected:
    int mNumVertices, mNumEdges, mNumTriangles;
    const void* mPoints;
    const int* mIndices;
    Vertex* mVertices;
    Edge* mEdges;
    Triangle* mTriangles;
    bool mIsValid;
};

#include "Wm5BasicMesh.inl"

}

#endif
