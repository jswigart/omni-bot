// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IncrementalDelaunay2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IncrementalDelaunay2<Real>::IncrementalDelaunay2 (Real xmin, Real ymin,
    Real xmax, Real ymax, Real uncertainty)
    :
    mXMin(xmin),
    mXMax(xmax),
    mYMin(ymin),
    mYMax(ymax),
    mUncertainty(uncertainty),
    mNumTriangles(0),
    mIndices(0),
    mAdjacencies(0),
    mPathLast(-1),
    mPath(0),
    mLastEdgeV0(-1),
    mLastEdgeV1(-1),
    mLastEdgeOpposite(-1),
    mLastEdgeOppositeIndex(-1)
{
    assertion(mXMin < mXMax && mYMin < mYMax,
        "Invalid bounding rectangle\n");
    assertion((Real)0 <= mUncertainty && mUncertainty <= (Real)1,
        "Invalid uncertainty\n");

    if (mUncertainty > (Real)0)
    {
        mRatVertexPool = new0 std::vector<QRVector>();
        mRatVertexEvaluated = new0 std::vector<bool>();
    }
    else
    {
        mRatVertexPool = 0;
        mRatVertexEvaluated = 0;
    }

    // Create a supertriangle that contains the input rectangle.
    Real x0 = ((Real)2)*xmin - xmax;
    Real y0 = ((Real)2)*ymin - ymax;
    Real x1 = ((Real)2)*xmax - xmin + ((Real)3)*(ymax - ymin);
    Real y1 = y0;
    Real x2 = x0;
    Real y2 = ((Real)2)*ymax - ymin + ((Real)3)*(xmax - xmin);
    Vector2<Real> superVertex0(x0, y0);
    Vector2<Real> superVertex1(x1, y1);
    Vector2<Real> superVertex2(x2, y2);

    // Insert the supertriangle vertices into the vertex storage.
    mVMap[superVertex0] = 0;
    mVMap[superVertex1] = 1;
    mVMap[superVertex2] = 2;
    mVertexPool.push_back(superVertex0);
    mVertexPool.push_back(superVertex1);
    mVertexPool.push_back(superVertex2);

    // Inert the supertriangle into the mesh.
    mTriangle.insert(new0 Triangle(0, 1, 2));
}
//----------------------------------------------------------------------------
template <typename Real>
IncrementalDelaunay2<Real>::~IncrementalDelaunay2 ()
{
    delete0(mRatVertexPool);
    delete0(mRatVertexEvaluated);
    delete1(mIndices);
    delete1(mAdjacencies);
    delete1(mPath);

    typename std::set<Triangle*>::iterator iter = mTriangle.begin();
    typename std::set<Triangle*>::iterator end = mTriangle.end();
    for (/**/; iter != end; ++iter)
    {
        Triangle* tri = *iter;
        delete0(tri);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
int IncrementalDelaunay2<Real>::Insert (const Vector2<Real>& position)
{
    if (position[0] < mXMin || position[0] > mXMax
    ||  position[1] < mYMin || position[1] > mYMax)
    {
        // The vertex is outside the domain specified in the constructor.
        return -1;
    }

    typename VertexMap::iterator viter = mVMap.find(position);
    if (viter != mVMap.end())
    {
        // The vertex already exists, so just return its index.
        return viter->second;
    }

    // Store the position in the various pools.
    int posIndex = (int)mVertexPool.size();
    mVMap[position] = posIndex;
    mVertexPool.push_back(position);
    if (mUncertainty > (Real)0)
    {
        (*mRatVertexPool).push_back(QRVector());
        (*mRatVertexEvaluated).push_back(false);
    }

    Triangle* tri = GetContainingTriangleInternal(position);
    if (!tri)
    {
        // All points must lie in the supertriangle, so each point must have
        // a containing triangle.
        assertion(false, "Unexpected condition\n");
        return -1;
    }

    // Locate and remove the triangles forming the insertion polygon.
    std::stack<Triangle*> triStack;
    VEManifoldMesh polygon(0, Edge::ECreator);
    triStack.push(tri);
    tri->OnStack = true;
    int j, v0, v1;
    Edge* edge;
    while (!triStack.empty())
    {
        tri = triStack.top();
        triStack.pop();
        tri->OnStack = false;
        for (j = 0; j < 3; ++j)
        {
            Triangle* adj = tri->Adj[j];
            if (adj)
            {
                // Detach triangle and adjacent triangle from each other.
                int nullIndex = tri->DetachFrom(j, adj);

                if (adj->IsInsertionComponent(posIndex, position, tri, this))
                {
                    if (!adj->OnStack)
                    {
                        // Adjacent triangle inside insertion polygon.
                        triStack.push(adj);
                        adj->OnStack = true;
                    }
                }
                else
                {
                    // Adjacent triangle outside insertion polygon.
                    v0 = tri->V[j];
                    v1 = tri->V[(j+1)%3];
                    edge = (Edge*)polygon.InsertEdge(v0, v1);
                    edge->NullIndex = nullIndex;
                    edge->Tri = adj;
                }
            }
            else
            {
                // The triangle is in the insertion polygon, but the adjacent
                // one does not exist.  This means one of two things:
                // (1) We are at an edge of the supertriangle, and that edge
                //     is part of the insertion polygon.
                // (2) We are at an edge that was recently shared by the
                //     triangle and the adjacent, but we detached those
                //     triangles from each other.  These edges should be
                //     ignored.
                v0 = tri->V[j];
                if (0 <= v0 && v0 <= 2)
                {
                    // v0 is a supervertex index.
                    v1 = tri->V[(j+1)%3];
                    if (0 <= v1 && v1 <= 2)
                    {
                        // v1 is a supervertex index.
                        edge = (Edge*)polygon.InsertEdge(v0, v1);
                        edge->NullIndex = -1;
                        edge->Tri = 0;
                    }
                }
            }
        }
        mTriangle.erase(tri);
        delete0(tri);
    }

    // Insert the new triangles formed by the input point and the edges of
    // the insertion polygon.
    const VEManifoldMesh::EMap& edgeMap = polygon.GetEdges();
    assertion(edgeMap.size() >= 3 && polygon.IsClosed(),
                "Polygon must be at least a triangle\n");
    typename VEManifoldMesh::EMapCIterator iter = edgeMap.begin();
    typename VEManifoldMesh::EMapCIterator end = edgeMap.end();
    for (/**/; iter != end; ++iter)
    {
        edge = (Edge*)iter->second;

        // Create and insert the new triangle.
        tri = new0 Triangle(posIndex, edge->V[0], edge->V[1]);
        mTriangle.insert(tri);

        // Establish the adjacency links across the polygon edge.
        tri->Adj[1] = edge->Tri;
        if (edge->Tri)
        {
            edge->Tri->Adj[edge->NullIndex] = tri;
        }

        // Update the edge's triangle pointer to point to the newly created
        // triangle.  This information is used later to establish the links
        // between the new triangles.
        edge->Tri = tri;
    }

    // Establish the adjacency links between the new triangles.
    Edge* adjEdge;
    iter = edgeMap.begin();
    end = edgeMap.end();
    for (/**/; iter != end; ++iter)
    {
        edge = (Edge*)iter->second;
        adjEdge = (Edge*)edge->E[0];
        edge->Tri->Adj[0] = adjEdge->Tri;
        adjEdge = (Edge*)edge->E[1];
        edge->Tri->Adj[2] = adjEdge->Tri;
    }

    return posIndex;
}
//----------------------------------------------------------------------------
template <typename Real>
int IncrementalDelaunay2<Real>::Remove (const Vector2<Real>& position)
{
    typename VertexMap::iterator iter = mVMap.find(position);
    if (iter == mVMap.end())
    {
        // The vertex does not exists, so return an invalid index.
        return -1;
    }
    int posIndex = iter->second;

    Triangle* initialTri = GetContainingTriangleInternal(position);
    if (!initialTri)
    {
        // All points must lie in the supertriangle, so each point must have
        // a containing triangle.  Moreover, in the Remove operation, the
        // point must be a vertex of a triangle.
        assertion(false, "Unexpected condition\n");
        return -1;
    }

    // Construct the removal polygon.
    std::vector<RPVertex> polygon;
    Triangle* tri = initialTri;
    do
    {
        // Locate the vertex for the removal point.  The opposite edge is an
        // edge of the removal polygon.
        int i;
        for (i = 0; i < 3; ++i)
        {
            if (tri->V[i] == posIndex)
            {
                break;
            }
        }
        if (i == 3)
        {
            assertion(false, "Removal point must be a triangle vertex\n");
            return -1;
        }

        // The removal point is P = Tri.V[i].  The edge of the removal polygon
        // is <V1,V2>, where V1 = Tri.V[(i+1)%3] and V2 = Tri.V[(i+2)%3)].
        // The edge <P,V1> is shared by Tri and Adj = Tri.A[i].
        polygon.push_back(RPVertex(tri->V[(i+1)%3], tri,
            tri->Adj[i]));
        tri = tri->Adj[(i+2)%3];
    }
    while (tri != initialTri);

    // Triangulate the removal polygon.
    Triangulate(polygon, posIndex, this);

    mVMap.erase(iter);
    return posIndex;
}
//----------------------------------------------------------------------------
template <typename Real>
void IncrementalDelaunay2<Real>::GetAllTriangles (int& numTriangles,
    int*& indices)
{
    numTriangles = (int)mTriangle.size();
    indices = new1<int>(3*numTriangles);

    int* currIndex = indices;
    typename std::set<Triangle*>::iterator iter = mTriangle.begin();
    typename std::set<Triangle*>::iterator end = mTriangle.end();
    for (/**/; iter != end; ++iter)
    {
        Triangle* tri = *iter;
        for (int i = 0; i < 3; ++i)
        {
            *currIndex++ = tri->V[i];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void IncrementalDelaunay2<Real>::GenerateRepresentation ()
{
    delete1(mIndices);
    mIndices = 0;
    delete1(mAdjacencies);
    mAdjacencies = 0;
    delete1(mPath);
    mPath = 0;

    // Assign integer values to the triangles for use by the caller.
    std::map<Triangle*,int> permute;
    typename std::set<Triangle*>::iterator iter = mTriangle.begin();
    typename std::set<Triangle*>::iterator end = mTriangle.end();
    mNumTriangles = (int)mTriangle.size();
    Triangle* tri;
    int i;
    for (i = 0; iter != end; ++iter)
    {
        tri = *iter;

        // Skip triangles that share a supervertex.
        if ((0 <= tri->V[0] && tri->V[0] <= 2)
        ||  (0 <= tri->V[1] && tri->V[1] <= 2)
        ||  (0 <= tri->V[2] && tri->V[2] <= 2))
        {
            --mNumTriangles;
            continue;
        }

        permute[tri] = i;
        ++i;
    }
    permute[(Triangle*)0] = -1;

    // Put Delaunay triangles into an array (vertices and adjacency info).
    if (mNumTriangles > 0)
    {
        mIndices = new1<int>(3*mNumTriangles);
        mAdjacencies = new1<int>(3*mNumTriangles);
        i = 0;
        iter = mTriangle.begin();
        end = mTriangle.end();
        for (/**/; iter != end; ++iter)
        {
            tri = *iter;

            // Skip triangles that share a supervertex.
            if ((0 <= tri->V[0] && tri->V[0] <= 2)
            ||  (0 <= tri->V[1] && tri->V[1] <= 2)
            ||  (0 <= tri->V[2] && tri->V[2] <= 2))
            {
                continue;
            }

            mIndices[i] = tri->V[0];
            if (ContainsSupervertex(tri->Adj[0]))
            {
                mAdjacencies[i++] = -1;
            }
            else
            {
                mAdjacencies[i++] = permute[tri->Adj[0]];
            }

            mIndices[i] = tri->V[1];
            if (ContainsSupervertex(tri->Adj[1]))
            {
                mAdjacencies[i++] = -1;
            }
            else
            {
                mAdjacencies[i++] = permute[tri->Adj[1]];
            }

            mIndices[i] = tri->V[2];
            if (ContainsSupervertex(tri->Adj[2]))
            {
                mAdjacencies[i++] = -1;
            }
            else
            {
                mAdjacencies[i++] = permute[tri->Adj[2]];
            }
        }
        assertion(i == 3*mNumTriangles, "Inconsistent condition\n");

        mPathLast = -1;
        mPath = new1<int>(mNumTriangles + 1);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
int IncrementalDelaunay2<Real>::GetNumTriangles () const
{
    return mNumTriangles;
}
//----------------------------------------------------------------------------
template <typename Real>
const int* IncrementalDelaunay2<Real>::GetIndices () const
{
    return mIndices;
}
//----------------------------------------------------------------------------
template <typename Real>
const int* IncrementalDelaunay2<Real>::GetAdjacencies () const
{
    return mAdjacencies;
}
//----------------------------------------------------------------------------
template <typename Real>
const std::vector<Vector2<Real> >&
IncrementalDelaunay2<Real>::GetVertices () const
{
    return mVertexPool;
}
//----------------------------------------------------------------------------
template <typename Real>
const std::map<Vector2<Real>,int>&
IncrementalDelaunay2<Real>::GetUniqueVertices () const
{
    return mVMap;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IncrementalDelaunay2<Real>::GetHull (int& numEdges, int*& indices)
{
    numEdges = 0;
    indices = 0;

    // Count the number of edges that are shared by triangles containing a
    // supervertex.
    int i, adjQuantity = 3*mNumTriangles;
    for (i = 0; i < adjQuantity; ++i)
    {
        if (mAdjacencies[i] == -1)
        {
            ++numEdges;
        }
    }
    assertion(numEdges > 0, "Unexpected condition\n");
    if (numEdges == 0)
    {
        return false;
    }

    // Enumerate the edges.
    indices = new1<int>(2*numEdges);
    int* currentIndex = indices;
    for (i = 0; i < adjQuantity; ++i)
    {
        if (mAdjacencies[i] == -1)
        {
            int tri = i/3, j = i%3;
            *currentIndex++ = mIndices[3*tri + j];
            *currentIndex++ = mIndices[3*tri + ((j+1)%3)];
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
int IncrementalDelaunay2<Real>::GetContainingTriangle (
    const Vector2<Real>& test) const
{
    // The mesh might not have any triangles (only collinear points were
    // inserted).
    if (!mPath)
    {
        return -1;
    }

    // Start at first triangle in mesh.
    int iIndex = (mPathLast >= 0 ? mPath[mPathLast] : 0);
    mPathLast = -1;
    mLastEdgeV0 = -1;
    mLastEdgeV1 = -1;
    mLastEdgeOpposite = -1;
    mLastEdgeOppositeIndex = -1;

    // Use triangle edges as binary separating lines.
    for (int i = 0; i < mNumTriangles; i++)
    {
        mPath[++mPathLast] = iIndex;

        int* aiV = &mIndices[3*iIndex];

        if (ToLine(test,aiV[0],aiV[1]) > 0)
        {
            iIndex = mAdjacencies[3*iIndex];
            if (iIndex == -1)
            {
                mLastEdgeV0 = aiV[0];
                mLastEdgeV1 = aiV[1];
                mLastEdgeOpposite = aiV[2];
                mLastEdgeOppositeIndex = 2;
                return -1;
            }
            continue;
        }

        if (ToLine(test,aiV[1],aiV[2]) > 0)
        {
            iIndex = mAdjacencies[3*iIndex+1];
            if (iIndex == -1)
            {
                mLastEdgeV0 = aiV[1];
                mLastEdgeV1 = aiV[2];
                mLastEdgeOpposite = aiV[0];
                mLastEdgeOppositeIndex = 0;
                return -1;
            }
            continue;
        }

        if (ToLine(test,aiV[2],aiV[0]) > 0)
        {
            iIndex = mAdjacencies[3*iIndex+2];
            if (iIndex == -1)
            {
                mLastEdgeV0 = aiV[2];
                mLastEdgeV1 = aiV[0];
                mLastEdgeOpposite = aiV[1];
                mLastEdgeOppositeIndex = 1;
                return -1;
            }
            continue;
        }

        mLastEdgeV0 = -1;
        mLastEdgeV1 = -1;
        mLastEdgeOpposite = -1;
        mLastEdgeOppositeIndex = -1;
        return iIndex;
    }

    return -1;
}
//----------------------------------------------------------------------------
template <typename Real>
int IncrementalDelaunay2<Real>::GetPathLast () const
{
    return mPathLast;
}
//----------------------------------------------------------------------------
template <typename Real>
const int* IncrementalDelaunay2<Real>::GetPath () const
{
    return mPath;
}
//----------------------------------------------------------------------------
template <typename Real>
int IncrementalDelaunay2<Real>::GetLastEdge (int& riV0, int& riV1, int& riV2)
    const
{
    riV0 = mLastEdgeV0;
    riV1 = mLastEdgeV1;
    riV2 = mLastEdgeOpposite;
    return mLastEdgeOppositeIndex;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IncrementalDelaunay2<Real>::GetVertexSet (int i, Vector2<Real> akV[3])
    const
{
    if (0 <= i && i < mNumTriangles)
    {
        akV[0] = mVertexPool[mIndices[3*i  ]];
        akV[1] = mVertexPool[mIndices[3*i+1]];
        akV[2] = mVertexPool[mIndices[3*i+2]];
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IncrementalDelaunay2<Real>::GetIndexSet (int i, int aiIndex[3]) const
{
    if (0 <= i && i < mNumTriangles)
    {
        aiIndex[0] = mIndices[3*i  ];
        aiIndex[1] = mIndices[3*i+1];
        aiIndex[2] = mIndices[3*i+2];
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IncrementalDelaunay2<Real>::GetAdjacentSet (int i, int aiAdjacent[3])
    const
{
    if (0 <= i && i < mNumTriangles)
    {
        aiAdjacent[0] = mAdjacencies[3*i  ];
        aiAdjacent[1] = mAdjacencies[3*i+1];
        aiAdjacent[2] = mAdjacencies[3*i+2];
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IncrementalDelaunay2<Real>::GetBarycentricSet (int i,
    const Vector2<Real>& test, Real afBary[3]) const
{
    if (0 <= i && i < mNumTriangles)
    {
        Vector2<Real> kV0 = mVertexPool[mIndices[3*i  ]];
        Vector2<Real> kV1 = mVertexPool[mIndices[3*i+1]];
        Vector2<Real> kV2 = mVertexPool[mIndices[3*i+2]];
        test.GetBarycentrics(kV0,kV1,kV2,afBary);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
int IncrementalDelaunay2<Real>::ToLine (const Vector2<Real>& test, int v0,
    int v1) const
{
    if (mUncertainty < (Real)1)
    {
        // Order the points so that ToLine(test,v0,v1) and ToLine(test,v1,v0)
        // return the same geometric result.
        Vector2<Real> vertex0 = mVertexPool[v0];
        Vector2<Real> vertex1 = mVertexPool[v1];
        bool positive;
        if (vertex0 < vertex1)
        {
            positive = true;
        }
        else
        {
            Vector2<Real> kSave = vertex0;
            vertex0 = vertex1;
            vertex1 = kSave;
            positive = false;
        }

        Real x0 = test[0] - vertex0[0];
        Real y0 = test[1] - vertex0[1];
        Real x1 = vertex1[0] - vertex0[0];
        Real y1 = vertex1[1] - vertex0[1];
        Real det = x0*y1 - x1*y0;
        if (!positive)
        {
            det = -det;
        }

        if (mUncertainty == (Real)0)
        {
            // Compute the sign test using floating-point arithmetic.
            return (det > (Real)0 ? +1 : (det < (Real)0 ? -1 : 0));
        }

        // Use filtered predicates.
        Real length0 = Math<Real>::Sqrt(x0*x0 + y0*y0);
        Real length1 = Math<Real>::Sqrt(x1*x1 + y1*y1);
        Real scaledUncertainty = mUncertainty*length0*length1;
        if (Math<Real>::FAbs(det) >= scaledUncertainty)
        {
            // The floating-point sign test is deemed to be certain.
            return (det > (Real)0 ? +1 : (det < (Real)0 ? -1 : 0));
        }
    }

    // Compute the determinant using exact rational arithmetic.
    QRVector ratTest;
    ratTest[0] = QRational(test[0]);
    ratTest[1] = QRational(test[1]);
    int indices[2] = { v0, v1 };
    for (int i = 0; i < 2; ++i)
    {
        int j = indices[i];
        if (!(*mRatVertexEvaluated)[j])
        {
            (*mRatVertexEvaluated)[j] = true;
            (*mRatVertexPool)[j][0] = QRational(mVertexPool[j][0]);
            (*mRatVertexPool)[j][1] = QRational(mVertexPool[j][1]);
        }
    }

    // Compute the sign test using rational arithmetic.
    const QRVector& ratV0 = (*mRatVertexPool)[v0];
    const QRVector& ratV1 = (*mRatVertexPool)[v1];
    QRational ratX0 = ratTest[0] - ratV0[0];
    QRational ratY0 = ratTest[1] - ratV0[1];
    QRational ratX1 = ratV1[0] - ratV0[0];
    QRational ratY1 = ratV1[1] - ratV0[1];
    QRational ratDet = ratX0*ratY1 - ratX1*ratY0;
    return (ratDet > 0 ? +1 : (ratDet < 0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
int IncrementalDelaunay2<Real>::ToTriangle (const Vector2<Real>& test,
    int v0, int v1, int v2) const
{
    int sign0 = ToLine(test, v1, v2);
    if (sign0 > 0)
    {
        return +1;
    }

    int sign1 = ToLine(test, v0, v2);
    if (sign1 < 0)
    {
        return +1;
    }

    int sign2 = ToLine(test, v0, v1);
    if (sign2 > 0)
    {
        return +1;
    }

    return ((sign0 && sign1 && sign2) ? -1 : 0);
}
//----------------------------------------------------------------------------
template <typename Real>
int IncrementalDelaunay2<Real>::ToCircumcircle (const Vector2<Real>& test,
    int v0, int v1, int v2) const
{
    if (mUncertainty < (Real)1)
    {
        // Order the points so that ToCircumcircle(test,u0,u1,u2) returns the
        // same containment result for any permutation (u0,u1,u2) of
        // (v0,v1,v2).
        Vector2<Real> vertex0 = mVertexPool[v0];
        Vector2<Real> vertex1 = mVertexPool[v1];
        Vector2<Real> vertex2 = mVertexPool[v2];
        Vector2<Real> save;
        bool positive;
        if (vertex0 < vertex1)
        {
            if (vertex2 < vertex0)
            {
                // (2,0,1)
                save = vertex2;
                vertex2 = vertex1;
                vertex1 = vertex0;
                vertex0 = save;
                positive = true;
            }
            else if (vertex2 < vertex1)
            {
                // (0,2,1)
                save = vertex1;
                vertex1 = vertex2;
                vertex2 = save;
                positive = false;
            }
            else
            {
                // (0,1,2)
                positive = true;
            }
        }
        else
        {
            if (vertex2 < vertex1)
            {
                // (2,1,0)
                save = vertex0;
                vertex0 = vertex2;
                vertex2 = save;
                positive = false;
            }
            else if (vertex2 < vertex0)
            {
                // (1,2,0)
                save = vertex0;
                vertex0 = vertex1;
                vertex1 = vertex2;
                vertex2 = save;
                positive = true;
            }
            else
            {
                // (1,0,2)
                save = vertex0;
                vertex0 = vertex1;
                vertex1 = save;
                positive = false;
            }
        }

        Real s0x = vertex0[0] + test[0];
        Real d0x = vertex0[0] - test[0];
        Real s0y = vertex0[1] + test[1];
        Real d0y = vertex0[1] - test[1];
        Real s1x = vertex1[0] + test[0];
        Real d1x = vertex1[0] - test[0];
        Real s1y = vertex1[1] + test[1];
        Real d1y = vertex1[1] - test[1];
        Real s2x = vertex2[0] + test[0];
        Real d2x = vertex2[0] - test[0];
        Real s2y = vertex2[1] + test[1];
        Real d2y = vertex2[1] - test[1];
        Real z0 = s0x*d0x + s0y*d0y;
        Real z1 = s1x*d1x + s1y*d1y;
        Real z2 = s2x*d2x + s2y*d2y;
        Real c00 = d1y*z2 - d2y*z1;
        Real c01 = d2y*z0 - d0y*z2;
        Real c02 = d0y*z1 - d1y*z0;
        Real det = d0x*c00 + d1x*c01 + d2x*c02;
        if (!positive)
        {
            det = -det;
        }

        if (mUncertainty == (Real)0)
        {
            // Compute the sign test using floating-point arithmetic.
            return (det < (Real)0 ? +1 : (det > (Real)0 ? -1 : 0));
        }

        // Use filtered predicates.
        Real length0 = Math<Real>::Sqrt(d0x*d0x + d0y*d0y + z0*z0);
        Real length1 = Math<Real>::Sqrt(d1x*d1x + d1y*d1y + z1*z1);
        Real length2 = Math<Real>::Sqrt(d2x*d2x + d2y*d2y + z2*z2);
        Real scaledUncertainty = mUncertainty*length0*length1*length2;
        if (Math<Real>::FAbs(det) >= scaledUncertainty)
        {
            return (det < (Real)0 ? 1 : (det > (Real)0 ? -1 : 0));
        }
    }

    // Compute the sign test using rational arithmetic.
    QRVector ratTest;
    ratTest[0] = QRational(test[0]);
    ratTest[1] = QRational(test[1]);
    int indices[3] = { v0, v1, v2 };
    for (int i = 0; i < 3; ++i)
    {
        int j = indices[i];
        if (!(*mRatVertexEvaluated)[j])
        {
            (*mRatVertexEvaluated)[j] = true;
            (*mRatVertexPool)[j][0] = QRational(mVertexPool[j][0]);
            (*mRatVertexPool)[j][1] = QRational(mVertexPool[j][1]);
        }
    }

    QRVector& ratV0 = (*mRatVertexPool)[v0];
    QRVector& ratV1 = (*mRatVertexPool)[v1];
    QRVector& ratV2 = (*mRatVertexPool)[v2];
    QRational ratS0x = ratV0[0] + ratTest[0];
    QRational ratD0x = ratV0[0] - ratTest[0];
    QRational ratS0y = ratV0[1] + ratTest[1];
    QRational ratD0y = ratV0[1] - ratTest[1];
    QRational ratS1x = ratV1[0] + ratTest[0];
    QRational ratD1x = ratV1[0] - ratTest[0];
    QRational ratS1y = ratV1[1] + ratTest[1];
    QRational ratD1y = ratV1[1] - ratTest[1];
    QRational ratS2x = ratV2[0] + ratTest[0];
    QRational ratD2x = ratV2[0] - ratTest[0];
    QRational ratS2y = ratV2[1] + ratTest[1];
    QRational ratD2y = ratV2[1] - ratTest[1];
    QRational ratZ0 = ratS0x*ratD0x + ratS0y*ratD0y;
    QRational ratZ1 = ratS1x*ratD1x + ratS1y*ratD1y;
    QRational ratZ2 = ratS2x*ratD2x + ratS2y*ratD2y;
    QRational ratC00 = ratD1y*ratZ2 - ratD2y*ratZ1;
    QRational ratC01 = ratD2y*ratZ0 - ratD0y*ratZ2;
    QRational ratC02 = ratD0y*ratZ1 - ratD1y*ratZ0;
    QRational ratDet = ratD0x*ratC00 + ratD1x*ratC01 + ratD2x*ratC02;
    return (ratDet < 0 ? +1 : (ratDet > 0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
typename IncrementalDelaunay2<Real>::Triangle*
IncrementalDelaunay2<Real>::GetContainingTriangleInternal (
    const Vector2<Real>& position) const
{
    // Locate which triangle in the current mesh contains vertex i.  By
    // construction, there must be such a triangle (the vertex cannot be
    // outside the supertriangle).

    Triangle* tri = *mTriangle.begin();
    int numTriangles = (int)mTriangle.size();
    for (int t = 0; t < numTriangles; ++t)
    {
        int* vertices = tri->V;

        if (ToLine(position, vertices[0], vertices[1]) > 0)
        {
            tri = tri->Adj[0];
            if (!tri)
            {
                break;
            }
            continue;
        }

        if (ToLine(position, vertices[1], vertices[2]) > 0)
        {
            tri = tri->Adj[1];
            if (!tri)
            {
                break;
            }
            continue;
        }

        if (ToLine(position, vertices[2], vertices[0]) > 0)
        {
            tri = tri->Adj[2];
            if (!tri)
            {
                break;
            }
            continue;
        }

        return tri;
    }

    assertion(false, "Delaunay vertices must lie in some triangle\n");
    return 0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IncrementalDelaunay2<Real>::ContainsSupervertex (Triangle* tri) const
{
    for (int i = 0; i < 3; ++i)
    {
        if (0 <= tri->V[i] && tri->V[i] <= 2)
        {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
void IncrementalDelaunay2<Real>::SwapEdge (Triangle* tri0, Triangle* tri1)
{
    int i0, i0p1, i0p2, i1, i1p1, i1p2, j;
    Triangle* adj;

    // Locate the indices of the shared edge.
    for (i0 = 0; i0 < 3; ++i0)
    {
        if (tri1 == tri0->Adj[i0])
        {
            break;
        }
    }
    if (i0 == 3)
    {
        assertion(false, "Unexpected condition.\n");
        return;
    }
    i0p1 = (i0 + 1) % 3;
    i0p2 = (i0 + 2) % 3;

    for (i1 = 0; i1 < 3; ++i1)
    {
        if (tri0 == tri1->Adj[i1])
        {
            break;
        }
    }
    if (i1 == 3)
    {
        assertion(false, "Unexpected condition.\n");
        return;
    }
    i1p1 = (i1 + 1) % 3;
    i1p2 = (i1 + 2) % 3;

    tri0->V[i0p1] = tri1->V[i1p2];
    tri1->V[i1p1] = tri0->V[i0p2];

    adj = tri1->Adj[i1p1];
    tri0->Adj[i0] = adj;
    if (adj)
    {
        for (j = 0; j < 3; ++j)
        {
            if (adj->Adj[j] == tri1)
            {
                adj->Adj[j] = tri0;
                break;
            }
        }
        if (j == 3)
        {
            assertion(false, "Unexpected condition.\n");
            return;
        }
    }

    adj = tri0->Adj[i0p1];
    tri1->Adj[i1] = adj;
    if (adj)
    {
        for (j = 0; j < 3; ++j)
        {
            if (adj->Adj[j] == tri0)
            {
                adj->Adj[j] = tri1;
                break;
            }
        }
        if (j == 3)
        {
            assertion(false, "Unexpected condition.\n");
            return;
        }
    }

    tri0->Adj[i0p1] = tri1;
    tri1->Adj[i1p1] = tri0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// IncrementalDelaunay2::Triangle
//----------------------------------------------------------------------------
template <typename Real>
IncrementalDelaunay2<Real>::Triangle::Triangle (int v0, int v1, int v2)
    :
    Time(-1),
    IsComponent(false),
    OnStack(false)
{
    V[0] = v0;
    V[1] = v1;
    V[2] = v2;
    Adj[0] = 0;
    Adj[1] = 0;
    Adj[2] = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IncrementalDelaunay2<Real>::Triangle::IsInsertionComponent (int posIndex,
    const Vector2<Real>& test, Triangle* adj,
    const IncrementalDelaunay2* delaunay)
{
    if (posIndex != Time)
    {
        Time = posIndex;

        // Determine the number of vertices in common with the supertriangle.
        // The supertriangle vertices have indices VQ-3, VQ-2, and VQ-1, where
        // VQ is the quantity of input vertices.
        int common = 0, svIndex = -1, j;
        for (j = 0; j < 3; ++j)
        {
            // The supervertices are at indices 0, 1, and 2, so loop counter
            // 'k' is the index into the supervertices.
            for (int k = 0; k < 3; ++k)
            {
                if (V[j] == k)
                {
                    common++;
                    svIndex = j;
                }
            }
        }

        int relation;
        if (common == 0)
        {
            // The classic case is that a point is in the mesh formed only by
            // the input vertices, in which case we only test for containment
            // in the circumcircle of the triangle.
            relation = delaunay->ToCircumcircle(test, V[0], V[1],
                V[2]);
        }
        else
        {
            // The classic problem is that points outside the mesh formed
            // only by the input vertices must be handled from a visibility
            // perspective rather than using circumcircles (compare with
            // convex hull construction).  By not doing this, you can run into
            // the pitfall that has snared many folks--the boundary edges of
            // the final triangulation do not form a convex polygon.
            int v0, v1;
            if (common == 1)
            {
                v0 = V[(svIndex + 1) % 3];
                v1 = V[(svIndex + 2) % 3];
            }
            else  // iCommon == 2
            {
                for (j = 0; j < 3; ++j)
                {
                    if (Adj[j] != 0 && Adj[j] != adj)
                    {
                        break;
                    }
                }
                v0 = V[j];
                v1 = V[(j + 1) % 3];
            }
            relation = delaunay->ToLine(test, v0, v1);
        }

        IsComponent = (relation < 0);
    }

    return IsComponent;
}
//----------------------------------------------------------------------------
template <typename Real>
int IncrementalDelaunay2<Real>::Triangle::DetachFrom (int adjIndex,
    Triangle* adj)
{
    assertion(0 <= adjIndex && adjIndex < 3 && Adj[adjIndex] == adj,
        "Invalid inputs\n");
    Adj[adjIndex] = 0;
    for (int i = 0; i < 3; ++i)
    {
        if (adj->Adj[i] == this)
        {
            adj->Adj[i] = 0;
            return i;
        }
    }
    return -1;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// IncrementalDelaunay2::Edge
//----------------------------------------------------------------------------
template <typename Real>
IncrementalDelaunay2<Real>::Edge::Edge (int v0, int v1, int nullIndex,
    Triangle* tri)
    :
    VEManifoldMesh::Edge(v0, v1)
{
    NullIndex = nullIndex;
    Tri = tri;
}
//----------------------------------------------------------------------------
template <typename Real>
VEManifoldMesh::EPtr IncrementalDelaunay2<Real>::Edge::ECreator (int v0,
    int v1)
{
    return new0 Edge(v0, v1, 0, 0);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// IncrementalDelaunay2::RPVertex
//----------------------------------------------------------------------------
template <typename Real>
IncrementalDelaunay2<Real>::RPVertex::RPVertex (int index, Triangle* tri,
    Triangle* adj)
    :
    Index(index),
    Tri(tri),
    Adj(adj),
    IsConvex(false),
    IsEarTip(false),
    IsSuperVertex(false),
    Weight(Math<Real>::MAX_REAL),
    VPrev(-1),
    VNext(-1),
    SPrev(-1),
    SNext(-1),
    EarRecord(0)
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// IncrementalDelaunay2::Triangulate
//----------------------------------------------------------------------------
template <typename Real>
IncrementalDelaunay2<Real>::Triangulate::Triangulate (
    std::vector<RPVertex>& polygon, int removal,
    IncrementalDelaunay2* delaunay)
    :
    mPolygon(polygon),
    mNumVertices((int)polygon.size()),
    mDelaunay(delaunay),
    mCFirst(-1),
    mCLast(-1),
    mRFirst(-1),
    mRLast(-1),
    mEHeap((int)polygon.size(), 1, Math<Real>::MAX_REAL)
{
    // Create a circular list of the polygon vertices for dynamic removal of
    // vertices.
    int numVerticesM1 = mNumVertices - 1;
    int i;
    for (i = 0; i <= numVerticesM1; ++i)
    {
        RPVertex& vertex = V(i);
        vertex.VPrev = (i > 0 ? i-1 : numVerticesM1);
        vertex.VNext = (i < numVerticesM1 ? i+1 : 0);
    }

    // Create a circular list of the polygon vertices for dynamic removal of
    // vertices.  Keep track of two linear sublists, one for the convex
    // vertices and one for the reflex vertices.  This is an O(N) process
    // where N is the number of polygon vertices.
    for (i = 0; i <= numVerticesM1; ++i)
    {
        if (IsConvex(i))
        {
            InsertAfterC(i);
        }
        else
        {
            InsertAfterR(i);
        }
    }

    // Identify the ear tips and build a circular list of them.  Let V0, V1,
    // and/ V2 be consecutive vertices forming a triangle T (the ear).  The
    // vertex V1 is an ear tip if no other vertices of the polygon lie inside
    // T.  Although it is enough to show that V1 is not an ear by finding at
    // least one other vertex inside T, it is sufficient to search only the
    // reflex vertices.  This is an O(C*R) process, where C is the number of
    // convex vertices and R is the number of reflex vertices with N = C+R.
    // The order is O(N^2), for example when C = R = N/2.
    Real weight;
    int vPrev, vNext;
    for (i = mCFirst; i != -1; i = V(i).SNext)
    {
        if (IsEarTip(i))
        {
            weight = ComputeWeight(i, removal);
            V(i).EarRecord = mEHeap.Insert(i, weight);
            V(i).IsEarTip = true;
        }
    }

    // Remove the ears, one at a time.
    while (mNumVertices >= 3)
    {
        if (mNumVertices == 3)
        {
            // Only one triangle remains.  Erase the three subtriangles
            // linked to removal point P and then insert the remaining
            // triangle.
            assertion(mEHeap.GetNumElements() == 3,
                "Unexpected condition\n");

            mEHeap.Remove(i, weight);
            RPVertex& vertex0 = V(i);
            vPrev = vertex0.VPrev;
            vNext = vertex0.VNext;
            RPVertex& vertexP = V(vPrev);
            RPVertex& vertexN = V(vNext);
            Triangle* tri0 = vertex0.Tri;
            Triangle* triP = vertexP.Tri;
            Triangle* triN = vertexN.Tri;

            int i0;
            for (i0 = 0; i0 < 3; ++i0)
            {
                if (tri0->V[i0] == removal)
                {
                    break;
                }
            }
            if (i0 == 3)
            {
                assertion(false, "Unexpected condition\n");
                break;
            }
            tri0->V[i0] = vertexP.Index;

            int ip;
            for (ip = 0; ip < 3; ++ip)
            {
                if (triP->V[ip] == vertexP.Index)
                {
                    break;
                }
            }
            if (ip == 3)
            {
                assertion(false, "Unexpected condition\n");
                break;
            }
            Triangle* adj = triP->Adj[ip];
            tri0->Adj[i0] = adj;
            if (adj)
            {
                for (i = 0; i < 3; ++i)
                {
                    if (adj->V[i] == vertex0.Index)
                    {
                        adj->Adj[i] = tri0;
                        break;
                    }
                }
                if (i == 3)
                {
                    assertion(false, "Unexpected condition\n");
                    break;
                }
            }


            int in;
            for (in = 0; in < 3; ++in)
            {
                if (triN->V[in] == vertexN.Index)
                {
                    break;
                }
            }
            if (in == 3)
            {
                assertion(false, "Unexpected condition\n");
                break;
            }
            adj = triN->Adj[in];
            tri0->Adj[(i0 + 2) % 3] = adj;
            if (adj)
            {
                for (i = 0; i < 3; ++i)
                {
                    if (adj->V[i] == vertexP.Index)
                    {
                        adj->Adj[i] = tri0;
                        break;
                    }
                }
                if (i == 3)
                {
                    assertion(false, "Unexpected condition\n");
                    break;
                }
            }

            mDelaunay->mTriangle.erase(triP);
            mDelaunay->mTriangle.erase(triN);
            delete0(triP);
            delete0(triN);
            break;
        }

        mEHeap.Remove(i, weight);
        vPrev = V(i).VPrev;
        vNext = V(i).VNext;
        mDelaunay->SwapEdge(V(i).Adj, V(i).Tri);
        V(vPrev).Tri = V(i).Tri;
        RemoveV(i);

        // Removal of the ear can cause an adjacent vertex to become an ear
        // or to stop being an ear.
        RPVertex& vertexP = V(vPrev);
        if (vertexP.IsEarTip)
        {
            if (!IsEarTip(vPrev))
            {
                mEHeap.Update(V(vPrev).EarRecord, (Real)-1);
                mEHeap.Remove(i, weight);
                assertion(i == vPrev && weight == (Real)-1,
                    "Unexpected condition\n");
            }
        }
        else
        {
            bool wasReflex = !vertexP.IsConvex;
            if (IsConvex(vPrev))
            {
                if (wasReflex)
                {
                    RemoveR(vPrev);
                }

                if (IsEarTip(vPrev))
                {
                    weight = ComputeWeight(vPrev, removal);
                    V(vPrev).EarRecord = mEHeap.Insert(vPrev, weight);
                    V(vPrev).IsEarTip = true;
                }
            }
        }

        RPVertex& vertexN = V(vNext);
        if (vertexN.IsEarTip)
        {
            if (!IsEarTip(vNext))
            {
                mEHeap.Update(V(vNext).EarRecord, (Real)-1);
                mEHeap.Remove(i, weight);
                assertion(i == vNext && weight == (Real)-1,
                    "Unexpected condition\n");
            }
        }
        else
        {
            bool wasReflex = !vertexN.IsConvex;
            if (IsConvex(vNext))
            {
                if (wasReflex)
                {
                    RemoveR(vNext);
                }

                if (IsEarTip(vNext))
                {
                    weight = ComputeWeight(vNext, removal);
                    V(vNext).EarRecord = mEHeap.Insert(vNext, weight);
                    V(vNext).IsEarTip = true;
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
typename IncrementalDelaunay2<Real>::RPVertex&
IncrementalDelaunay2<Real>::Triangulate::V (int i)
{
    return mPolygon[i];
}
//----------------------------------------------------------------------------
template <typename Real>
bool IncrementalDelaunay2<Real>::Triangulate::IsConvex (int i)
{
    RPVertex& vertex = V(i);
    Vector2<Real> curr = mDelaunay->GetVertices()[vertex.Index];
    int prev = V(vertex.VPrev).Index;
    int next = V(vertex.VNext).Index;
    vertex.IsConvex = (mDelaunay->ToLine(curr, prev, next) > 0);
    return vertex.IsConvex;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IncrementalDelaunay2<Real>::Triangulate::IsEarTip (int i)
{
    RPVertex& vertex = V(i);

    if (mRFirst == -1)
    {
        // The remaining polygon is convex.
        vertex.IsEarTip = true;
        return true;
    }

    // Search the reflex vertices and test if any are in the triangle
    // <V[prev],V[curr],V[next]>.
    int prev = V(vertex.VPrev).Index;
    int curr = vertex.Index;
    int next = V(vertex.VNext).Index;
    vertex.IsEarTip = true;
    for (int j = mRFirst; j != -1; j = V(j).SNext)
    {
        // Check if the test vertex is already one of the triangle vertices.
        if (j == vertex.VPrev || j == i || j == vertex.VNext)
        {
            continue;
        }

        // Test if the vertex is inside or on the triangle.  When it is, it
        // causes V[curr] not to be an ear.
        Vector2<Real> test = mDelaunay->GetVertices()[V(j).Index];
        if (mDelaunay->ToTriangle(test, prev, curr, next) <= 0)
        {
            vertex.IsEarTip = false;
            break;
        }
    }

    return vertex.IsEarTip;
}
//----------------------------------------------------------------------------
template <typename Real>
void IncrementalDelaunay2<Real>::Triangulate::InsertAfterC (int i)
{
    if (mCFirst == -1)
    {
        // add first convex vertex
        mCFirst = i;
    }
    else
    {
        V(mCLast).SNext = i;
        V(i).SPrev = mCLast;
    }
    mCLast = i;
}
//----------------------------------------------------------------------------
template <typename Real>
void IncrementalDelaunay2<Real>::Triangulate::InsertAfterR (int i)
{
    if (mRFirst == -1)
    {
        // add first reflex vertex
        mRFirst = i;
    }
    else
    {
        V(mRLast).SNext = i;
        V(i).SPrev = mRLast;
    }
    mRLast = i;
}
//----------------------------------------------------------------------------
template <typename Real>
void IncrementalDelaunay2<Real>::Triangulate::RemoveV (int i)
{
    int currVPrev = V(i).VPrev;
    int currVNext = V(i).VNext;
    V(currVPrev).VNext = currVNext;
    V(currVNext).VPrev = currVPrev;
    --mNumVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
void IncrementalDelaunay2<Real>::Triangulate::RemoveR (int i)
{
    assertion(mRFirst != -1 && mRLast != -1, "List must be nonempty\n");

    if (i == mRFirst)
    {
        mRFirst = V(i).SNext;
        if (mRFirst != -1)
        {
            V(mRFirst).SPrev = -1;
        }
        V(i).SNext = -1;
    }
    else if (i == mRLast)
    {
        mRLast = V(i).SPrev;
        if (mRLast != -1)
        {
            V(mRLast).SNext = -1;
        }
        V(i).SPrev = -1;
    }
    else
    {
        int currSPrev = V(i).SPrev;
        int currSNext = V(i).SNext;
        V(currSPrev).SNext = currSNext;
        V(currSNext).SPrev = currSPrev;
        V(i).SNext = -1;
        V(i).SPrev = -1;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Real IncrementalDelaunay2<Real>::Triangulate::ComputeWeight (int v0, int p)
{
    RPVertex& vertex0 = V(v0);
    assertion(vertex0.IsEarTip, "Vertex must be an ear tip\n");
    if (0 <= vertex0.Index && vertex0.Index <= 2)
    {
        // The vertex is a supervertex.  Return infinite weight so that the
        // supervertices are processed last.
        vertex0.IsSuperVertex = true;
        vertex0.Weight = Math<Real>::MAX_REAL;
        return vertex0.Weight;
    }

    // Get the adjacent vertices.
    int prev = vertex0.VPrev;
    int next = vertex0.VNext;
    RPVertex& vertexP = V(prev);
    RPVertex& vertexN = V(next);

    const Vector2<Real>& posP = mDelaunay->GetVertices()[vertexP.Index];
    const Vector2<Real>& pos0 = mDelaunay->GetVertices()[vertex0.Index];
    const Vector2<Real>& posN = mDelaunay->GetVertices()[vertexN.Index];
    const Vector2<Real>& posR = mDelaunay->GetVertices()[p];

    // Compute D.
    Real x0 = pos0[0] - posP[0];
    Real y0 = pos0[1] - posP[1];
    Real x1 = posN[0] - posP[0];
    Real y1 = posN[1] - posP[1];
    Real denom = x0*y1 - x1*y0;

    // Compute H.
    Real s0x = posP[0] + posR[0];
    Real d0x = posP[0] - posR[0];
    Real s0y = posP[1] + posR[1];
    Real d0y = posP[1] - posR[1];
    Real s1x = pos0[0] + posR[0];
    Real d1x = pos0[0] - posR[0];
    Real s1y = pos0[1] + posR[1];
    Real d1y = pos0[1] - posR[1];
    Real s2x = posN[0] + posR[0];
    Real d2x = posN[0] - posR[0];
    Real s2y = posN[1] + posR[1];
    Real d2y = posN[1] - posR[1];
    Real z0 = s0x*d0x + s0y*d0y;
    Real z1 = s1x*d1x + s1y*d1y;
    Real z2 = s2x*d2x + s2y*d2y;
    Real c00 = d1y*z2 - d2y*z1;
    Real c01 = d2y*z0 - d0y*z2;
    Real c02 = d0y*z1 - d1y*z0;
    Real numer = d0x*c00 + d1x*c01 + d2x*c02;

    vertex0.Weight = numer/denom;
    return vertex0.Weight;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IncrementalDelaunay2<float>;

template WM5_MATHEMATICS_ITEM
class IncrementalDelaunay2<double>;
//----------------------------------------------------------------------------
}
