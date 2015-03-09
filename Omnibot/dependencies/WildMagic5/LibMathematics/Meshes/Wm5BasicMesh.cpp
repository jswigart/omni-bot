// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5BasicMesh.h"
#include "Wm5EdgeKey.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
BasicMesh::BasicMesh (int numVertices, const void* points, int numTriangles,
    const int* indices)
{
    assertion(numVertices > 0 && numTriangles > 0 && indices,
        "Invalid input\n");

    if (numVertices <= 0 || numTriangles <= 0 || !indices)
    {
        mNumVertices = 0;
        mNumEdges = 0;
        mNumTriangles = 0;
        mVertices = 0;
        mEdges = 0;
        mTriangles = 0;
        mPoints = 0;
        mIndices = 0;
        mIsValid = false;
        return;
    }

    mNumVertices = numVertices;
    mNumEdges = 0;
    mNumTriangles = numTriangles;
    mPoints = points;
    mIndices = indices;
    mIsValid = true;

    // Dynamically construct triangle mesh from input.
    mVertices = new1<Vertex>(mNumVertices);
    mEdges = new1<Edge>(3*mNumTriangles);
    mTriangles = new1<Triangle>(mNumTriangles);
    std::map<EdgeKey,int> edgeMap;
    for (int t = 0; t < mNumTriangles; t++)
    {
        // Update triangle.
        Triangle& tri = mTriangles[t];
        tri.V[0] = *indices++;
        tri.V[1] = *indices++;
        tri.V[2] = *indices++;

        // Add edges to mesh.
        for (int i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
        {
            // Update vertices.
            mVertices[tri.V[i1]].InsertTriangle(t);

            EdgeKey key(tri.V[i0], tri.V[i1]);
            std::map<EdgeKey,int>::iterator eiter = edgeMap.find(key);
            if (eiter == edgeMap.end())
            {
                // First time edge encountered.
                edgeMap[key] = mNumEdges;

                // Update edge.
                Edge& edge = mEdges[mNumEdges];
                edge.V[0] = tri.V[i0];
                edge.V[1] = tri.V[i1];
                edge.T[0] = t;

                // Update vertices.
                mVertices[edge.V[0]].InsertEdge(edge.V[1],
                    mNumEdges);
                mVertices[edge.V[1]].InsertEdge(edge.V[0],
                    mNumEdges);

                // Update triangle.
                tri.E[i0] = mNumEdges;

                ++mNumEdges;
            }
            else
            {
                // Second time edge encountered.
                int e = eiter->second;
                Edge& edge = mEdges[e];

                // update edge
                assertion(edge.T[1] == -1, "Mesh must be manifold\n");
                if (edge.T[1] != -1)
                {
                    delete1(mVertices);
                    delete1(mEdges);
                    delete1(mTriangles);
                    mNumVertices = 0;
                    mNumEdges = 0;
                    mNumTriangles = 0;
                    mVertices = 0;
                    mEdges = 0;
                    mTriangles = 0;
                    mPoints = 0;
                    mIndices = 0;
                    mIsValid = false;
                }
                edge.T[1] = t;

                // Update triangles.
                int a = edge.T[0];
                Triangle& adj = mTriangles[a];
                for (int j = 0; j < 3; ++j)
                {
                    if (adj.E[j] == e)
                    {
                        adj.T[j] = t;
                        break;
                    }
                }
                tri.E[i0] = e;
                tri.T[i0] = a;
            }
        }
    }
}
//----------------------------------------------------------------------------
BasicMesh::~BasicMesh ()
{
    delete1(mVertices);
    delete1(mEdges);
    delete1(mTriangles);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// BasicMesh::Vertex
//----------------------------------------------------------------------------
BasicMesh::Vertex::Vertex ()
    :
    NumVertices(0),
    V(0),
    E(0),
    NumTriangles(0),
    T(0)
{
}
//----------------------------------------------------------------------------
BasicMesh::Vertex::~Vertex ()
{
    delete1(V);
    delete1(E);
    delete1(T);
}
//----------------------------------------------------------------------------
void BasicMesh::Vertex::InsertEdge (int v, int e)
{
    // Check whether vertex/edge in adjacency array (nothing to do if in
    // array).
    for (int i = 0; i < NumVertices; ++i)
    {
        if (v == V[i])
        {
            return;
        }
    }

    if ((NumVertices % MV_CHUNK) == 0)
    {
        int* save = V;
        V = new1<int>(NumVertices + MV_CHUNK);
        if (save)
        {
            memcpy(V, save, NumVertices*sizeof(int));
            delete1(save);
        }

        save = E;
        E = new1<int>(NumVertices + MV_CHUNK);
        if (save)
        {
            memcpy(E, save, NumVertices*sizeof(int));
            delete1(save);
        }
    }

    V[NumVertices] = v;
    E[NumVertices] = e;
    ++NumVertices;
}
//----------------------------------------------------------------------------
void BasicMesh::Vertex::InsertTriangle (int t)
{
    // Check whether triangle in adjacency array (nothing to do if in array).
    for (int i = 0; i < NumTriangles; ++i)
    {
        if (t == T[i])
        {
            return;
        }
    }

    if ((NumTriangles % MV_CHUNK) == 0)
    {
        int* save = T;
        T = new1<int>(NumTriangles + MV_CHUNK);
        if (save)
        {
            memcpy(T, save, NumTriangles*sizeof(int));
            delete1(save);
        }
    }

    T[NumTriangles] = t;
    ++NumTriangles;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// BasicMesh::Edge
//----------------------------------------------------------------------------
BasicMesh::Edge::Edge ()
{
    for (int i = 0; i < 2; ++i)
    {
        V[i] = -1;
        T[i] = -1;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// BasicMesh::Triangle
//----------------------------------------------------------------------------
BasicMesh::Triangle::Triangle ()
{
    for (int i = 0; i < 3; ++i)
    {
        V[i] = -1;
        E[i] = -1;
        T[i] = -1;
    }
}
//----------------------------------------------------------------------------
