// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5PhysicsPCH.h"
#include "Wm5ExtremalQuery3BSP.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
ExtremalQuery3BSP<Real>::ExtremalQuery3BSP (
    const ConvexPolyhedron3<Real>* polytope)
    :
    ExtremalQuery3<Real>(polytope)
{
    // Create the adjacency information for the polytope.
    int numVertices = mPolytope->GetNumVertices();
    const Vector3<Real>* vertices = mPolytope->GetVertices();
    int numTriangles = mPolytope->GetNumTriangles();
    const int* indices = mPolytope->GetIndices();
    BasicMesh mesh(numVertices, vertices, numTriangles, indices);

    // Create the set of unique arcs which are used to create the BSP tree.
    std::multiset<SphericalArc> arcs;
    CreateSphericalArcs(mesh, arcs);

    // Create the BSP tree to be used in the extremal query.
    std::vector<SphericalArc> nodes;
    CreateBSPTree(arcs, nodes);

    // Copy the nodes into a single, nonresizeable array.
    mNumNodes = (int)nodes.size();
    mNodes = new1<SphericalArc>(mNumNodes);
    memcpy(mNodes, &nodes.front(), mNumNodes*sizeof(SphericalArc));
}
//----------------------------------------------------------------------------
template <typename Real>
ExtremalQuery3BSP<Real>::~ExtremalQuery3BSP ()
{
    delete1(mNodes);
}
//----------------------------------------------------------------------------
template <typename Real>
void ExtremalQuery3BSP<Real>::GetExtremeVertices (
    const Vector3<Real>& direction, int& positiveDirection,
    int& negativeDirection)
{
    // Do a nonrecursive depth-first search of the BSP tree to determine
    // spherical polygon contains the incoming direction D.  Index 0 is the
    // root of the BSP tree.
    int current = 0;
    while (current >= 0)
    {
        SphericalArc& node = mNodes[current];
        int sign = (int)Math<Real>::Sign(direction.Dot(node.Normal));
        if (sign >= 0)
        {
            current = node.PosChild;
            if (current == -1)
            {
                // At a leaf node.
                positiveDirection = node.PosVertex;
            }
        }
        else
        {
            current = node.NegChild;
            if (current == -1)
            {
                // At a leaf node.
                positiveDirection = node.NegVertex;
            }
        }
    }

    // Do a nonrecursive depth-first search of the BSP tree to determine
    // spherical polygon contains the reverse incoming direction -D.
    current = 0;  // the root of the BSP tree
    while (current >= 0)
    {
        SphericalArc& node = mNodes[current];
        int sign = (int)-Math<Real>::Sign(direction.Dot(node.Normal));
        if (sign >= 0)
        {
            current = node.PosChild;
            if (current == -1)
            {
                // At a leaf node.
                negativeDirection = node.PosVertex;
            }
        }
        else
        {
            current = node.NegChild;
            if (current == -1)
            {
                // At a leaf node.
                negativeDirection = node.NegVertex;
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
int ExtremalQuery3BSP<Real>::GetNumNodes () const
{
    return mNumNodes;
}
//----------------------------------------------------------------------------
template <typename Real>
int ExtremalQuery3BSP<Real>::GetTreeDepth () const
{
    return mTreeDepth;
}
//----------------------------------------------------------------------------
template <typename Real>
void ExtremalQuery3BSP<Real>::SortVertexAdjacents (BasicMesh& mesh)
{
    // The typecast is to allow modifying the vertices.  As long as the
    // sorting algorithm is correct, this is a safe thing to do.
    int numVertices = mesh.GetNumVertices();
    BasicMesh::Vertex* vertices = (BasicMesh::Vertex*)mesh.GetVertices();

    const BasicMesh::Triangle* triangles = mesh.GetTriangles();
    for (int i = 0; i < numVertices; ++i)
    {
        // This copy circumvents the constness of the mesh vertices, which
        // allows the sorting of the mesh triangles shared by a mesh vertex.
        BasicMesh::Vertex& vertex = vertices[i];

        // This is a consequence of the mesh being a polyhedron.
        assertion(vertex.NumVertices == vertex.NumTriangles,
            "Unexpected condition\n");

        // Once we have the first vertex to sort and an initial triangle
        // sharing it, we can walk around the vertex following triangle
        // adjacency links.  It is safe to overwrite the vertex data.
        int t = vertex.T[0];
        const BasicMesh::Triangle* tri = &triangles[t];

        for (int adj = 0; adj < vertex.NumVertices; ++adj)
        {
            int prev, curr;
            for (prev = 2, curr = 0; curr < 3; prev = curr++)
            {
                if (tri->V[curr] == i)
                {
                    vertex.V[adj] = tri->V[prev];
                    vertex.E[adj] = tri->E[prev];
                    vertex.T[adj] = t;

                    // The next triangle to visit.
                    t = tri->T[prev];
                    tri = &triangles[t];
                    break;
                }
            }
            assertion(curr < 3, "Unexpected condition\n");
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void ExtremalQuery3BSP<Real>::CreateSphericalArcs (BasicMesh& mesh,
    std::multiset<SphericalArc>& arcs)
{
    int numEdges = mesh.GetNumEdges();
    const BasicMesh::Edge* edges = mesh.GetEdges();
    const BasicMesh::Triangle* triangles = mesh.GetTriangles();

    const int prev[3] = { 2, 0, 1 };
    const int next[3] = { 1, 2, 0 };

    for (int i = 0; i < numEdges; ++i)
    {
        const BasicMesh::Edge& edge = edges[i];

        SphericalArc arc;
        arc.NIndex[0] = edge.T[0];
        arc.NIndex[1] = edge.T[1];
        arc.Separation = 1;

        arc.Normal = mFaceNormals[arc.NIndex[0]].Cross(
            mFaceNormals[arc.NIndex[1]]);

        const BasicMesh::Triangle& adj = triangles[edge.T[0]];
        int j;
        for (j = 0; j < 3; ++j)
        {
            if (adj.V[j] != edge.V[0]
            &&  adj.V[j] != edge.V[1])
            {
                arc.PosVertex = adj.V[prev[j]];
                arc.NegVertex = adj.V[next[j]];
                break;
            }
        }
        assertion(j < 3, "Unexpected condition\n");

        arcs.insert(arc);
    }

    CreateSphericalBisectors(mesh, arcs);
}
//----------------------------------------------------------------------------
template <typename Real>
void ExtremalQuery3BSP<Real>::CreateSphericalBisectors (BasicMesh& mesh,
    std::multiset<SphericalArc>& arcs)
{
    // For each vertex, sort the normals into a counterclockwise spherical
    // polygon when viewed from outside the sphere.
    SortVertexAdjacents(mesh);

    int numVertices = mesh.GetNumVertices();
    const BasicMesh::Vertex* vertices = mesh.GetVertices();
    std::queue<std::pair<int,int> > queue;
    for (int i = 0; i < numVertices; ++i)
    {
        const BasicMesh::Vertex& vertex = vertices[i];

        queue.push(std::make_pair(0, vertex.NumTriangles));
        while (!queue.empty())
        {
            std::pair<int,int> arc = queue.front();
            queue.pop();
            int i0 = arc.first, i1 = arc.second;
            int separation = i1 - i0;
            if (separation > 1 && separation != vertex.NumTriangles - 1)
            {
                if (i1 < vertex.NumTriangles)
                {
                    SphericalArc arc;
                    arc.NIndex[0] = vertex.T[i0];
                    arc.NIndex[1] = vertex.T[i1];
                    arc.Separation = separation;

                    arc.Normal = mFaceNormals[arc.NIndex[0]].Cross(
                        mFaceNormals[arc.NIndex[1]]);

                    arc.PosVertex = i;
                    arc.NegVertex = i;
                    arcs.insert(arc);
                }
                int iMid = (i0 + i1 + 1)/2;
                if (iMid != i1)
                {
                    queue.push(std::make_pair(i0, iMid));
                    queue.push(std::make_pair(iMid, i1));
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void ExtremalQuery3BSP<Real>::CreateBSPTree (
    std::multiset<SphericalArc>& arcs, std::vector<SphericalArc>& nodes)
{
    // The tree has at least a root.
    mTreeDepth = 1;

    typename std::multiset<SphericalArc>::reverse_iterator iter;
    for (iter = arcs.rbegin(); iter != arcs.rend(); ++iter)
    {
        InsertArc(*iter, nodes);
    }

    // The leaf nodes are not counted in the traversal of InsertArc.  The
    // depth must be incremented to account for leaves.
    ++mTreeDepth;
}
//----------------------------------------------------------------------------
template <typename Real>
void ExtremalQuery3BSP<Real>::InsertArc (const SphericalArc& arc,
    std::vector<SphericalArc>& nodes)
{
    // The incoming arc is stored at the end of the nodes array.
    if (nodes.size() > 0)
    {
        // Do a nonrecursive depth-first search of the current BSP tree to
        // place the incoming arc.  Index 0 is the root of the BSP tree.
        std::stack<int> candidates;
        candidates.push(0);
        while (!candidates.empty())
        {
            int current = candidates.top();
            candidates.pop();
            SphericalArc* node = &nodes[current];

            int sign0;
            if (arc.NIndex[0] == node->NIndex[0]
            ||  arc.NIndex[0] == node->NIndex[1])
            {
                sign0 = 0;
            }
            else
            {
                sign0 = (int)Math<Real>::Sign(
                    mFaceNormals[arc.NIndex[0]].Dot(node->Normal));
            }

            int sign1;
            if (arc.NIndex[1] == node->NIndex[0]
            ||  arc.NIndex[1] == node->NIndex[1])
            {
                sign1 = 0;
            }
            else
            {
                sign1 = (int)Math<Real>::Sign(
                    mFaceNormals[arc.NIndex[1]].Dot(node->Normal));
            }

            int doTest = 0;
            if (sign0*sign1 < 0)
            {
                // The new arc straddles the current arc, so propagate it
                // to both child nodes.
                doTest = 3;
            }
            else if (sign0 > 0 || sign1 > 0)
            {
                // The new arc is on the positive side of the current arc.
                doTest = 1;
            }
            else if (sign0 < 0 || sign1 < 0)
            {
                // The new arc is on the negative side of the current arc.
                doTest = 2;
            }
            // else: sign0 = sign1 = 0, in which case no propagation is
            // needed since the current BSP node will handle the correct
            // partitioning of the arcs during extremal queries.

            int depth;

            if (doTest & 1)
            {
                if (node->PosChild != -1)
                {
                    candidates.push(node->PosChild);
                    depth = (int)candidates.size();
                    if (depth > mTreeDepth)
                    {
                        mTreeDepth = depth;
                    }
                }
                else
                {
                    node->PosChild = (int)nodes.size();
                    nodes.push_back(arc);

                    // The push_back can cause a reallocation, so the current
                    // pointer must be refreshed.
                    node = &nodes[current];
                }
            }

            if (doTest & 2)
            {
                if (node->NegChild != -1)
                {
                    candidates.push(node->NegChild);
                    depth = (int)candidates.size();
                    if (depth > mTreeDepth)
                    {
                        mTreeDepth = depth;
                    }
                }
                else
                {
                    node->NegChild = (int)nodes.size();
                    nodes.push_back(arc);
                }
            }
        }
    }
    else
    {
        // root node
        nodes.push_back(arc);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ExtremalQueryBSP::SphericalArc
//----------------------------------------------------------------------------
template <typename Real>
ExtremalQuery3BSP<Real>::SphericalArc::SphericalArc ()
    :
    PosChild(-1),
    NegChild(-1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
bool ExtremalQuery3BSP<Real>::SphericalArc::operator< (
    const SphericalArc& arc) const
{
    return Separation < arc.Separation;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_PHYSICS_ITEM
class ExtremalQuery3BSP<float>;

template WM5_PHYSICS_ITEM
class ExtremalQuery3BSP<double>;
//----------------------------------------------------------------------------
}
