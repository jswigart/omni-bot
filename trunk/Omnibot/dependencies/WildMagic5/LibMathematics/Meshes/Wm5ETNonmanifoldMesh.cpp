// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/05/11)

#include "Wm5MathematicsPCH.h"
#include "Wm5ETNonmanifoldMesh.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
ETNonmanifoldMesh::ETNonmanifoldMesh (ECreator eCreator, TCreator tCreator)
{
    mECreator = (eCreator ? eCreator : CreateEdge);
    mTCreator = (tCreator ? tCreator : CreateTriangle);
}
//----------------------------------------------------------------------------
ETNonmanifoldMesh::~ETNonmanifoldMesh ()
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
ETNonmanifoldMesh::EPtr ETNonmanifoldMesh::CreateEdge (int v0, int v1)
{
    return new0 Edge(v0, v1);
}
//----------------------------------------------------------------------------
ETNonmanifoldMesh::TPtr ETNonmanifoldMesh::CreateTriangle (int v0, int v1,
    int v2)
{
    return new0 Triangle(v0, v1, v2);
}
//----------------------------------------------------------------------------
ETNonmanifoldMesh::TPtr ETNonmanifoldMesh::InsertTriangle (int v0, int v1,
    int v2)
{
    TriangleKey tKey(v0, v1, v2);
    TMapIterator titer = mTMap.find(tKey);
    if (titer != mTMap.end())
    {
        // Triangle already exists.
        return 0;
    }

    // Add new triangle.
    TPtr tri = mTCreator(v0, v1, v2);
    mTMap[tKey] = tri;

    // Add edges to mesh.
    for (int i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        EdgeKey eKey(tri->V[i0], tri->V[i1]);
        EPtr edge;
        EMapIterator eiter = mEMap.find(eKey);
        if (eiter == mEMap.end())
        {
            // First time edge encountered.
            edge = mECreator(tri->V[i0], tri->V[i1]);
            mEMap[eKey] = edge;
        }
        else
        {
            // Edge previously encountered and created.
            edge = eiter->second;
        }

        // Update edge and triangle.
        edge->T.insert(tri);
        tri->E[i0] = edge;
    }

    return tri;
}
//----------------------------------------------------------------------------
bool ETNonmanifoldMesh::RemoveTriangle (int v0, int v1, int v2)
{
    TriangleKey tKey(v0, v1, v2);
    TMapIterator titer = mTMap.find(tKey);
    if (titer == mTMap.end())
    {
        // Triangle does not exist.
        return false;
    }

    TPtr tri = titer->second;
    for (int i = 0; i < 3; ++i)
    {
        // Inform edges you are going away.
        EPtr edge = tri->E[i];
        edge->T.erase(tri);

        // Remove edge if you had the last reference to it.
        if (edge->T.size() == 0)
        {
            EdgeKey eKey(edge->V[0], edge->V[1]);
            mEMap.erase(eKey);
            delete0(edge);
        }
    }

    mTMap.erase(tKey);
    delete0(tri);
    return true;
}
//----------------------------------------------------------------------------
bool ETNonmanifoldMesh::IsManifold () const
{
    EMapCIterator eiter = mEMap.begin();
    EMapCIterator eend = mEMap.end();
    for (/**/; eiter != eend; ++eiter)
    {
        if (eiter->second->T.size() > 2)
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
bool ETNonmanifoldMesh::IsClosed () const
{
    EMapCIterator eiter = mEMap.begin();
    EMapCIterator eend = mEMap.end();
    for (/**/; eiter != eend; ++eiter)
    {
        if (eiter->second->T.size() != 2)
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
bool ETNonmanifoldMesh::IsConnected () const
{
    // Perform a breadth-first search to locate the connected component
    // containing the first triangle in the triangle map.
    if (mTMap.begin() == mTMap.end())
    {
        // No triangles in the mesh, by default mesh is connected.
        return true;
    }

    // Start search at first triangle in mesh.
    std::set<Triangle*> component, boundary;
    boundary.insert(mTMap.begin()->second);

    while (boundary.size() > 0)
    {
        std::set<Triangle*> exterior;

        // Process boundary triangles.
        std::set<Triangle*>::iterator titer = boundary.begin();
        std::set<Triangle*>::iterator tend = boundary.end();
        for (/**/; titer != tend; ++titer)
        {
            // Boundary triangle is adjacent to current connected component.
            TPtr tri = *titer;
            component.insert(tri);

            // Locate adjacent, exterior triangles for later processing.
            for (int i = 0; i < 3; ++i)
            {
                EPtr edge = tri->E[i];
                std::set<Triangle*>::const_iterator aiter =
                    edge->T.begin();
                std::set<Triangle*>::const_iterator aend =
                    edge->T.end();
                for (/**/; aiter != aend; ++aiter)
                {
                    if (*aiter != tri
                    &&  component.find(*aiter) == component.end()
                    &&  boundary.find(*aiter) == boundary.end())
                    {
                        exterior.insert(*aiter);
                    }
                }
            }
        }

        // Exterior triangles are next in line to be processed.
        boundary = exterior;
    }

    return component.size() == mTMap.size();
}
//----------------------------------------------------------------------------
void ETNonmanifoldMesh::RemoveComponent (int& numIndices, int* indices)
{
    // Do a breadth-first search of the mesh to find connected components.
    // The input array is assumed to be large enough to hold the component.
    // (See the comments in ETNonmanifoldMesh.h for RemoveComponent.)
    numIndices = 0;

    if (mTMap.size() == 0)
    {
        return;
    }

    // Find the connected component containing the first triangle in the mesh.
    std::set<Triangle*> visited;
    visited.insert(mTMap.begin()->second);

    // Traverse the connected component.
    while (!visited.empty())
    {
        // Start at the current triangle.
        Triangle* tri = *visited.begin();

        // Add adjacent triangles to the set for recursive processing.
        for (int i = 0; i < 3; ++i)
        {
            EPtr edge = tri->E[i];
            std::set<Triangle*>::const_iterator aiter =
                edge->T.begin();
            std::set<Triangle*>::const_iterator aend =
                edge->T.end();
            for (/**/; aiter != aend; ++aiter)
            {
                if (*aiter != tri)
                {
                    visited.insert(*aiter);
                }
            }
        }

        // Add triangle to connectivity array.
        indices[numIndices++] = tri->V[0];
        indices[numIndices++] = tri->V[1];
        indices[numIndices++] = tri->V[2];

        // Remove the current triangle (visited, no longer needed).
        visited.erase(tri);
        RemoveTriangle(tri->V[0], tri->V[1], tri->V[2]);
    }
}
//----------------------------------------------------------------------------
void ETNonmanifoldMesh::Print (const char* filename)
{
    std::ofstream outfile(filename);
    if (!outfile)
    {
        return;
    }

    // Assign unique indices to the edges.
    std::map<Edge*,int> eIndex;
    eIndex[(Edge*)0] = 0;
    int i = 1;
    EMapIterator eiter = mEMap.begin();
    EMapIterator eend = mEMap.end();
    for (/**/; eiter != eend; ++eiter)
    {
        if (eiter->second)
        {
            eIndex[eiter->second] = i;
            ++i;
        }
    }

    // Assign unique indices to the triangles.
    std::map<Triangle*,int> tIndex;
    tIndex[(Triangle*)0] = 0;
    i = 1;
    TMapIterator titer = mTMap.begin();
    TMapIterator tend = mTMap.end();
    for (/**/; titer != tend; ++titer)
    {
        if (titer->second)
        {
            tIndex[titer->second] = i;
            ++i;
        }
    }

    // Print edges.
    outfile << "edge quantity = " << (int)mEMap.size() << std::endl;
    eiter = mEMap.begin();
    eend = mEMap.end();
    for (/**/; eiter != eend; ++eiter)
    {
        Edge* edge = eiter->second;
        outfile << 'e' << eIndex[edge] << " <"
            << 'v' << edge->V[0] << ",v" << edge->V[1] << "; ";

        std::set<Triangle*>::const_iterator aiter = edge->T.begin();
        std::set<Triangle*>::const_iterator aend = edge->T.end();
        for (/**/; aiter != aend; ++aiter)
        {
            outfile << 't' << tIndex[*aiter] << ',';
        }
        outfile << '>' << std::endl;
    }
    outfile << std::endl;

    // Print triangles.
    outfile << "triangle quantity = " << (int)mTMap.size() << std::endl;
    titer = mTMap.begin();
    tend = mTMap.end();
    for (/**/; titer != tend; ++titer)
    {
        Triangle* tri = titer->second;
        outfile << 't' << tIndex[tri] << " <"
              << 'v' << tri->V[0] << ",v" << tri->V[1] << ",v"
              << tri->V[2] << "; ";
        if (tri->E[0])
        {
            outfile << 'e' << eIndex[tri->E[0]];
        }
        else
        {
            outfile << '*';
        }
        outfile << ',';
        if (tri->E[1])
        {
            outfile << 'e' << eIndex[tri->E[1]];
        }
        else
        {
            outfile << '*';
        }
        outfile << ',';
        if (tri->E[2])
        {
            outfile << 'e' << eIndex[tri->E[2]];
        }
        else
        {
            outfile << '*';
        }
        outfile << '>' << std::endl;
    }
    outfile << std::endl;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ETNonmanifoldMesh::Edge
//----------------------------------------------------------------------------
ETNonmanifoldMesh::Edge::Edge (int v0, int v1)
{
    V[0] = v0;
    V[1] = v1;
}
//----------------------------------------------------------------------------
ETNonmanifoldMesh::Edge::~Edge ()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ETNonmanifoldMesh::Triangle
//----------------------------------------------------------------------------
ETNonmanifoldMesh::Triangle::Triangle (int v0, int v1, int v2)
{
    V[0] = v0;
    V[1] = v1;
    V[2] = v2;

    for (int i = 0; i < 3; ++i)
    {
        E[i] = 0;
    }
}
//----------------------------------------------------------------------------
ETNonmanifoldMesh::Triangle::~Triangle ()
{
}
//----------------------------------------------------------------------------
