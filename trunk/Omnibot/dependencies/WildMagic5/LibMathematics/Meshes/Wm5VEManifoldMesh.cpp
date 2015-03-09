// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/04/18)

#include "Wm5MathematicsPCH.h"
#include "Wm5VEManifoldMesh.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
VEManifoldMesh::VEManifoldMesh (VCreator vCreator, ECreator eCreator)
{
    mVCreator = (vCreator ? vCreator : CreateVertex);
    mECreator = (eCreator ? eCreator : CreateEdge);
}
//----------------------------------------------------------------------------
VEManifoldMesh::~VEManifoldMesh ()
{
    VMapIterator viter = mVMap.begin();
    VMapIterator vend = mVMap.end();
    for (/**/; viter != vend; ++viter)
    {
        Vertex* vertex = viter->second;
        delete0(vertex);
    }

    EMapIterator eiter = mEMap.begin();
    EMapIterator eend = mEMap.end();
    for (/**/; eiter != eend; ++eiter)
    {
        Edge* edge = eiter->second;
        delete0(edge);
    }
}
//----------------------------------------------------------------------------
VEManifoldMesh::VPtr VEManifoldMesh::CreateVertex (int v)
{
    return new0 Vertex(v);
}
//----------------------------------------------------------------------------
VEManifoldMesh::EPtr VEManifoldMesh::CreateEdge (int v0, int v1)
{
    return new0 Edge(v0, v1);
}
//----------------------------------------------------------------------------
VEManifoldMesh::EPtr VEManifoldMesh::InsertEdge (int v0, int v1)
{
    std::pair<int,int> ekey(v0, v1);
    EMapIterator eiter = mEMap.find(ekey);
    if (eiter != mEMap.end())
    {
        // Edge already exists.
        return 0;
    }

    // Add new edge.
    EPtr edge = mECreator(v0,v1);
    mEMap[ekey] = edge;

    // Add vertices to mesh.
    for (int i = 0; i < 2; ++i)
    {
        int v = edge->V[i];
        VPtr vertex;
        VMapIterator viter = mVMap.find(v);
        if (viter == mVMap.end())
        {
            // First time vertex encountered.
            vertex = mVCreator(v);
            mVMap[v] = vertex;

            // Update vertex.
            vertex->E[0] = edge;
        }
        else
        {
            // Second time vertex encountered.
            vertex = viter->second;
            assertion(vertex != 0, "Unexpected condition\n");

            // Update vertex.
            if (vertex->E[1])
            {
                assertion(false, "Mesh must be manifold\n");
                return 0;
            }
            vertex->E[1] = edge;

            // Update adjacent edge.
            EPtr adjacent = vertex->E[0];
            assertion(adjacent != 0, "Unexpected condition\n");
            for (int j = 0; j < 2; ++j)
            {
                if (adjacent->V[j] == v)
                {
                    adjacent->E[j] = edge;
                    break;
                }
            }

            // Update edge.
            edge->E[i] = adjacent;
        }
    }

    return edge;
}
//----------------------------------------------------------------------------
bool VEManifoldMesh::RemoveEdge (int v0, int v1)
{
    std::pair<int,int> ekey(v0, v1);
    EMapIterator eiter = mEMap.find(ekey);
    if (eiter == mEMap.end())
    {
        // Edge does not exist.
        return false;
    }

    EPtr edge = eiter->second;
    for (int i = 0; i < 2; ++i)
    {
        // Inform vertices you are going away.
        VMapIterator viter = mVMap.find(edge->V[i]);
        assertion(viter != mVMap.end(), "Unexpected condition\n");
        Vertex* vertex = viter->second;
        assertion(vertex != 0, "Unexpected condition\n");
        if (vertex->E[0] == edge)
        {
            // One-edge vertices always have pointer in slot zero.
            vertex->E[0] = vertex->E[1];
            vertex->E[1] = 0;
        }
        else if (vertex->E[1] == edge)
        {
            vertex->E[1] = 0;
        }
        else
        {
            assertion(false, "Unexpected condition\n");
            return false;
        }

        // Remove vertex if you had the last reference to it.
        if (!vertex->E[0] && !vertex->E[1])
        {
            mVMap.erase(vertex->V);
            delete0(vertex);
        }

        // Inform adjacent edges you are going away.
        EPtr adjacent = edge->E[i];
        if (adjacent)
        {
            for (int j = 0; j < 2; ++j)
            {
                if (adjacent->E[j] == edge)
                {
                    adjacent->E[j] = 0;
                    break;
                }
            }
        }
    }

    mEMap.erase(ekey);
    delete0(edge);
    return true;
}
//----------------------------------------------------------------------------
bool VEManifoldMesh::IsClosed () const
{
    VMapCIterator viter = mVMap.begin();
    VMapCIterator vend = mVMap.end();
    for (/**/; viter != vend; ++viter)
    {
        const Vertex* vertex = viter->second;
        if (!vertex->E[0] || !vertex->E[1])
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
void VEManifoldMesh::Print (const char* filename)
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
    for (int i = 1; eiter != eend; ++eiter)
    {
        if (eiter->second)
        {
            edgeIndex[eiter->second] = i;
            ++i;
        }
    }

    // Print vertices.
    outFile << "vertex quantity = " << (int)mVMap.size() << std::endl;
    VMapIterator viter = mVMap.begin();
    VMapIterator vend = mVMap.end();
    for (/**/; viter != vend; ++viter)
    {
        const Vertex& vertex = *viter->second;
        outFile << 'v' << vertex.V << " <";
        if (vertex.E[0])
        {
            outFile << 'e' << edgeIndex[vertex.E[0]];
        }
        else
        {
            outFile << '*';
        }
        outFile << ',';
        if (vertex.E[1])
        {
            outFile << 'e'  << edgeIndex[vertex.E[1]];
        }
        else
        {
            outFile << '*';
        }
        outFile << '>' << std::endl;
    }

    // Print edges.
    outFile << "edge quantity = " << (int)mEMap.size() << std::endl;
    for (eiter = mEMap.begin(); eiter != eend; ++eiter)
    {
        const Edge& edge = *eiter->second;
        outFile << 'e' << edgeIndex[eiter->second] << " <"
              << 'v' << edge.V[0] << ",v" << edge.V[1] << "; ";
        if (edge.E[0])
        {
            outFile << 'e' << edgeIndex[edge.E[0]];
        }
        else
        {
            outFile << '*';
        }
        outFile << ',';
        if (edge.E[1])
        {
            outFile << 'e' << edgeIndex[edge.E[1]];
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
// VEManifoldMesh::Vertex
//----------------------------------------------------------------------------
VEManifoldMesh::Vertex::Vertex (int v)
{
    V = v;
    E[0] = 0;
    E[1] = 0;
}
//----------------------------------------------------------------------------
VEManifoldMesh::Vertex::~Vertex ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// VEManifoldMesh::Edge
//----------------------------------------------------------------------------
VEManifoldMesh::Edge::Edge (int v0, int v1)
{
    V[0] = v0;
    V[1] = v1;
    E[0] = 0;
    E[1] = 0;
}
//----------------------------------------------------------------------------
VEManifoldMesh::Edge::~Edge ()
{
}
//----------------------------------------------------------------------------
