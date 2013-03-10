// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CREATECLODMESH_H
#define WM5CREATECLODMESH_H

#include "Wm5CollapseRecord.h"
#include "Wm5EdgeKey.h"
#include "Wm5MinHeap.h"
#include "Wm5TriangleKey.h"
#include "Wm5TriMesh.h"

// CreateClodMesh computes an array of CollapseRecord that represents edge
// collapses in a triangle mesh.  An edge <V0,V1> is collapsed by replacing
// V1 (the "throw" vertex) by V0 (the "keep" vertex).  No new vertices or
// surface attributes are introduced.  Moreover, the vertex and index buffers
// are reordered so that vertices and triangles that are removed first in the
// collapse process occur at the ends of their arrays.  This supports sending
// the vertices as static geometry to the hardware renderer.  The level of
// detail selection only needs to (1) change the active vertex and triangle
// counts, and (2) update the index buffer based on the CollapseRecord
// index values.
//
// Some lighting artifacts may occur when normal vectors are present in the
// vertex buffer.  The artifacts can be reduced if the normals are
// recalculated using the triangle topology of the index buffer each time
// the target record changes.  In this case, the vertex buffer becomes
// dynamic, so you have to tell the renderer to update the video memory
// copy each frame.
//
// The CollapseRecord class can be modified to store new vertices and
// attributes for incremental updates.  For example, you might want to
// replace V0 and V1 by (V0+V1)/2, and then average their corresponding
// attributes.  In this case, the vertex buffer becomes dynamic, and you have
// to tell the renderer to update the video memory copy each frame.

#include "Wm5GraphicsLIB.h"

namespace Wm5
{

class WM5_GRAPHICS_ITEM CreateClodMesh
{
public:
    // Construction and destruction.
    CreateClodMesh (TriMesh* mesh, int& numRecords, CollapseRecord*& records);
    ~CreateClodMesh ();

private:
    // Vertex-edge-triangle graph.
    typedef std::set<TriangleKey> TriangleKeySet;
    typedef std::set<EdgeKey> EdgeKeySet;

    class Vertex
    {
    public:
        Vertex ();

        EdgeKeySet AdjEdges;
        TriangleKeySet AdjTriangles;
        bool Collapsible;
    };

    class Edge
    {
    public:
        Edge ();

        TriangleKeySet AdjTriangles;
        const MinHeapRecord<EdgeKey,float>* Record;
    };

    typedef int Triangle;

    typedef std::vector<Vertex> VertexArray;
    typedef std::map<EdgeKey,Edge> EdgeMap;
    typedef std::map<TriangleKey,Triangle> TriangleMap;

    // Information about the edge collapse.
    class CollapseInfo
    {
    public:
        CollapseInfo (int vKeep = -1, int vThrow = -1);

        int VKeep, VThrow, TThrow0, TThrow1;
    };


    bool ValidBuffers () const;
    void InsertTriangle (const TriangleKey& tKey, Triangle t);
    void RemoveTriangle (const TriangleKey& tKey);
    void ClassifyCollapsibleVertices ();
    float ComputeMetric (const EdgeKey& eKey);
    int CanCollapse (const EdgeKey& eKey);
    void Collapse (const EdgeKey& eKey, int indexThrow);
    bool ValidResults ();
    void ReorderBuffers ();
    void ComputeRecords (int& numRecords, CollapseRecord*& records);

    // Triangle mesh to be decimated.
    int mNumVertices, mNumIndices, mNumTriangles;
    int* mIndices;
    VertexBufferAccessor mVBA;

    // The vertex-edge-triangle graph.
    VertexArray mVertices;
    EdgeMap mEdges;
    TriangleMap mTriangles;

    // The edge heap to support collapse operations.
    MinHeap<EdgeKey,float> mHeap;

    // The sequence of edge collapses.
    std::vector<CollapseInfo> mCollapses;

    // Postprocessing of the edge collapses.
    std::vector<int> mVerticesRemaining;
    std::vector<int> mTrianglesRemaining;
};

}

#endif
