// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/04/18)

#include "Wm5MathematicsPCH.h"
#include "Wm5ETManifoldMesh.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
ETManifoldMesh::ETManifoldMesh (ECreator eCreator, TCreator tCreator)
{
    mECreator = (eCreator ? eCreator : CreateEdge);
    mTCreator = (tCreator ? tCreator : CreateTriangle);
}
//----------------------------------------------------------------------------
ETManifoldMesh::~ETManifoldMesh ()
{
    EMapIterator eiter = mEMap.begin();
    EMapIterator eend = mEMap.end();
    for (/**/; eiter != eend; ++eiter)
    {
        Edge* edge = eiter->second;
        delete0(edge);
    }

    TMapIterator titer = mTMap.begin();
    TMapIterator tend = mTMap.end();
    for (/**/; titer != tend; ++titer)
    {
        Triangle* tri = titer->second;
        delete0(tri);
    }
}
//----------------------------------------------------------------------------
ETManifoldMesh::EPtr ETManifoldMesh::CreateEdge (int v0, int v1)
{
    return new0 Edge(v0, v1);
}
//----------------------------------------------------------------------------
ETManifoldMesh::TPtr ETManifoldMesh::CreateTriangle (int v0, int v1, int v2)
{
    return new0 Triangle(v0, v1, v2);
}
//----------------------------------------------------------------------------
ETManifoldMesh::TPtr ETManifoldMesh::InsertTriangle (int v0, int v1, int v2)
{
    TriangleKey tkey(v0, v1, v2);
    TMapIterator titer = mTMap.find(tkey);
    if (titer != mTMap.end())
    {
        // Triangle already exists.
        return 0;
    }

    // Add new triangle.
    TPtr tri = mTCreator(v0, v1, v2);
    mTMap[tkey] = tri;

    // Add edges to mesh.
    for (int i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        EdgeKey ekey(tri->V[i0], tri->V[i1]);
        EPtr edge;
        EMapIterator eiter = mEMap.find(ekey);
        if (eiter == mEMap.end())
        {
            // First time edge encountered.
            edge = mECreator(tri->V[i0], tri->V[i1]);
            mEMap[ekey] = edge;

            // Update edge and triangle.
            edge->T[0] = tri;
            tri->E[i0] = edge;
        }
        else
        {
            // Second time edge encountered.
            edge = eiter->second;
            assertion(edge != 0, "Unexpected condition\n");

            // Update edge.
            if (edge->T[1])
            {
                assertion(false, "Mesh must be manifold\n");
                return 0;
            }
            edge->T[1] = tri;

            // Update adjacent triangles.
            TPtr adjacent = edge->T[0];
            assertion(adjacent != 0, "Unexpected condition\n");
            for (int i = 0; i < 3; i++)
            {
                if (adjacent->E[i] == edge)
                {
                    adjacent->T[i] = tri;
                    break;
                }
            }

            // Update triangle.
            tri->E[i0] = edge;
            tri->T[i0] = adjacent;
        }
    }

    return tri;
}
//----------------------------------------------------------------------------
bool ETManifoldMesh::RemoveTriangle (int v0, int v1, int v2)
{
    TriangleKey tkey(v0, v1, v2);
    TMapIterator titer = mTMap.find(tkey);
    if (titer == mTMap.end())
    {
        // Triangle does not exist.
        return false;
    }

    TPtr tri = titer->second;
    for (int i = 0; i < 3; ++i)
    {
        // Inform edges you are going away.
        Edge* edge = tri->E[i];
        assertion(edge != 0, "Unexpected condition\n");
        if (edge->T[0] == tri)
        {
            // One-triangle edges always have pointer in slot zero.
            edge->T[0] = edge->T[1];
            edge->T[1] = 0;
        }
        else if (edge->T[1] == tri)
        {
            edge->T[1] = 0;
        }
        else
        {
            assertion(false, "Unexpected condition\n");
            return false;
        }

        // Remove edge if you had the last reference to it.
        if (!edge->T[0] && !edge->T[1])
        {
            EdgeKey ekey(edge->V[0], edge->V[1]);
            mEMap.erase(ekey);
            delete0(edge);
        }

        // Inform adjacent triangles you are going away.
        TPtr adjacent = tri->T[i];
        if (adjacent)
        {
            for (int j = 0; j < 3; ++j)
            {
                if (adjacent->T[j] == tri)
                {
                    adjacent->T[j] = 0;
                    break;
                }
            }
        }
    }

    mTMap.erase(tkey);
    delete0(tri);
    return true;
}
//----------------------------------------------------------------------------
bool ETManifoldMesh::IsClosed () const
{
    EMapCIterator eiter = mEMap.begin();
    EMapCIterator eend = mEMap.end();
    for (/**/; eiter != eend; ++eiter)
    {
        const Edge* pkEdge = eiter->second;
        if (!pkEdge->T[0] || !pkEdge->T[1])
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
void ETManifoldMesh::Print (const char* filename)
{
    std::ofstream outFile(filename);
    if (!outFile)
    {
        return;
    }

    // Assign unique indices to the edges.
    std::map<EPtr,int> edgeIndex;
    edgeIndex[(EPtr)0] = 0;
    EMapIterator eiter = mEMap.begin();
    EMapIterator eend = mEMap.end();
    int i;
    for (i = 1; eiter != eend; ++eiter)
    {
        if (eiter->second)
        {
            edgeIndex[eiter->second] = i;
            ++i;
        }
    }

    // Assign unique indices to the triangles.
    std::map<TPtr,int> triIndex;
    triIndex[(TPtr)0] = 0;
    TMapIterator titer = mTMap.begin();
    TMapIterator tend = mTMap.end();
    for (i = 1; titer != tend; ++titer)
    {
        if (titer->second)
        {
            triIndex[titer->second] = i;
            ++i;
        }
    }

    // Print edges.
    outFile << "edge quantity = " << (int)mEMap.size() << std::endl;
    eiter = mEMap.begin();
    eend = mEMap.end();
    for (/**/; eiter != eend; ++eiter)
    {
        const Edge& edge = *eiter->second;
        outFile << 'e' << edgeIndex[eiter->second] << " <"
              << 'v' << edge.V[0] << ",v" << edge.V[1] << "; ";
        if (edge.T[0])
        {
            outFile << 't' << triIndex[edge.T[0]];
        }
        else
        {
            outFile << '*';
        }
        outFile << ',';
        if (edge.T[1])
        {
            outFile << 't' << triIndex[edge.T[1]];
        }
        else
        {
            outFile << '*';
        }
        outFile << '>' << std::endl;
    }
    outFile << std::endl;

    // Print triangles.
    outFile << "triangle quantity = " << (int)mTMap.size() << std::endl;
    titer = mTMap.begin();
    tend = mTMap.end();
    for (/**/; titer != tend; ++titer)
    {
        const Triangle& tri = *titer->second;
        outFile << 't' << triIndex[titer->second] << " <"
              << 'v' << tri.V[0] << ",v" << tri.V[1] << ",v"
              << tri.V[2] << "; ";
        if (tri.E[0])
        {
            outFile << 'e' << edgeIndex[tri.E[0]];
        }
        else
        {
            outFile << '*';
        }
        outFile << ',';
        if (tri.E[1])
        {
            outFile << 'e' << edgeIndex[tri.E[1]];
        }
        else
        {
            outFile << '*';
        }
        outFile << ',';
        if (tri.E[2])
        {
            outFile << 'e' << edgeIndex[tri.E[2]];
        }
        else
        {
            outFile << '*';
        }
        outFile << "; ";

        if (tri.T[0])
        {
            outFile << 't' << triIndex[tri.T[0]];
        }
        else
        {
            outFile << '*';
        }
        outFile << ',';
        if (tri.T[1])
        {
            outFile << 't' << triIndex[tri.T[1]];
        }
        else
        {
            outFile << '*';
        }
        outFile << ',';
        if (tri.T[2])
        {
            outFile << 't' << triIndex[tri.T[2]];
        }
        else
        {
            outFile << '*';
        }
        outFile << '>' << std::endl;
    }
    outFile << std::endl;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ETManifoldMesh::Edge
//----------------------------------------------------------------------------
ETManifoldMesh::Edge::Edge (int v0, int v1)
{
    V[0] = v0;
    V[1] = v1;
    T[0] = 0;
    T[1] = 0;
}
//----------------------------------------------------------------------------
ETManifoldMesh::Edge::~Edge ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ETManifoldMesh::Triangle
//----------------------------------------------------------------------------
ETManifoldMesh::Triangle::Triangle (int v0, int v1, int v2)
{
    V[0] = v0;
    V[1] = v1;
    V[2] = v2;

    for (int i = 0; i < 3; ++i)
    {
        E[i] = 0;
        T[i] = 0;
    }
}
//----------------------------------------------------------------------------
ETManifoldMesh::Triangle::~Triangle ()
{
}
//----------------------------------------------------------------------------
