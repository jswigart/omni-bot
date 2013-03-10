// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5Delaunay2.h"
#include "Wm5Query2Filtered.h"
#include "Wm5Query2Int64.h"
#include "Wm5Query2Integer.h"
#include "Wm5Query2Rational.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
Delaunay2<Real>::Delaunay2 (int numVertices, Vector2<Real>* vertices,
    Real epsilon, bool owner, Query::Type queryType)
    :
    Delaunay<Real>(numVertices, epsilon, owner, queryType),
    mVertices(vertices),
    mNumUniqueVertices(0),
    mSVertices(0),
    mQuery(0),
    mLineOrigin(Vector2<Real>::ZERO),
    mLineDirection(Vector2<Real>::ZERO),
    mPathLast(-1),
    mPath(0),
    mLastEdgeV0(-1),
    mLastEdgeV1(-1),
    mLastEdgeOpposite(-1),
    mLastEdgeOppositeIndex(-1)
{
    typename Vector2<Real>::Information info;
    Vector2<Real>::GetInformation(mNumVertices, mVertices, mEpsilon, info);
    if (info.mDimension == 0)
    {
        // The values of mDimension, mIndices, and mAdjacencies were
        // already initialized by the Delaunay base class.
        return;
    }

    if (info.mDimension == 1)
    {
        // The set is (nearly) collinear.  The caller is responsible for
        // creating a Delaunay1 object.
        mDimension = 1;
        mLineOrigin = info.mOrigin;
        mLineDirection = info.mDirection[0];
        return;
    }

    mDimension = 2;

    // Allocate storage for the input vertices and the supertriangle
    // vertices.
    mSVertices = new1<Vector2<Real> >(mNumVertices + 3);
    int i;

    if (queryType != Query::QT_RATIONAL && queryType != Query::QT_FILTERED)
    {
        // Transform the vertices to the square [0,1]^2.
        mMin = Vector2<Real>(info.mMin[0], info.mMin[1]);
        mScale = ((Real)1)/info.mMaxRange;
        for (i = 0; i < mNumVertices; ++i)
        {
            mSVertices[i] = (mVertices[i] - mMin)*mScale;
        }

        // Construct the supertriangle to contain [0,1]^2.
        mSupervertices[0] = mNumVertices++;
        mSupervertices[1] = mNumVertices++;
        mSupervertices[2] = mNumVertices++;
        mSVertices[mSupervertices[0]] = Vector2<Real>((Real)-1,(Real)-1);
        mSVertices[mSupervertices[1]] = Vector2<Real>((Real)+4,(Real)-1);
        mSVertices[mSupervertices[2]] = Vector2<Real>((Real)-1,(Real)+4);

        Real expand;
        if (queryType == Query::QT_INT64)
        {
            // Scale the vertices to the square [0,2^{16}]^2 to allow use of
            // 64-bit integers for triangulation.
            expand = (Real)(1 << 16);
            mQuery = new0 Query2Int64<Real>(mNumVertices, mSVertices);
        }
        else if (queryType == Query::QT_INTEGER)
        {
            // Scale the vertices to the square [0,2^{20}]^2 to get more
            // precision for TInteger than for 64-bit integers for
            // triangulation.
            expand = (Real)(1 << 20);
            mQuery = new0 Query2Integer<Real>(mNumVertices, mSVertices);
        }
        else // queryType == Query::QT_REAL
        {
            // No scaling for floating point.
            expand = (Real)1;
            mQuery = new0 Query2<Real>(mNumVertices, mSVertices);
        }

        mScale *= expand;
        for (i = 0; i < mNumVertices; ++i)
        {
            mSVertices[i] *= expand;
        }
    }
    else
    {
        // No transformation needed for exact rational arithmetic.
        mMin = Vector2<Real>::ZERO;
        mScale = (Real)1;
        memcpy(mSVertices, mVertices, mNumVertices*sizeof(Vector2<Real>));

        // Construct the supertriangle to contain [min,max].
        Vector2<Real> minValue = Vector2<Real>(info.mMin[0], info.mMin[1]);
        Vector2<Real> maxValue = Vector2<Real>(info.mMax[0], info.mMax[1]);
        Vector2<Real> delta = maxValue - minValue;
        Vector2<Real> superMin = minValue - delta;
        Vector2<Real> superMax = maxValue + delta*((Real)3);
        mSupervertices[0] = mNumVertices++;
        mSupervertices[1] = mNumVertices++;
        mSupervertices[2] = mNumVertices++;
        mSVertices[mSupervertices[0]] = superMin;
        mSVertices[mSupervertices[1]] =
            Vector2<Real>(superMax[0],superMin[1]);
        mSVertices[mSupervertices[2]] =
            Vector2<Real>(superMin[0],superMax[1]);

        if (queryType == Query::QT_RATIONAL)
        {
            mQuery = new0 Query2Rational<Real>(mNumVertices, mSVertices);
        }
        else // queryType == Query::QT_FILTERED
        {
            mQuery = new0 Query2Filtered<Real>(mNumVertices, mSVertices,
                mEpsilon);
        }
    }

    Triangle* tri = new0 Triangle(mSupervertices[0], mSupervertices[1],
        mSupervertices[2]);
    mTriangles.insert(tri);

    // Incrementally update the triangulation.  The set of processed points
    // is maintained to eliminate duplicates, either in the original input
    // points or in the points obtained by snap rounding.
    std::set<Vector2<Real> > processed;
    for (i = 0; i < mNumVertices - 3; ++i)
    {
        if (processed.find(mSVertices[i]) == processed.end())
        {
            Update(i);
            processed.insert(mSVertices[i]);
        }
    }
    mNumUniqueVertices = (int)processed.size();

    // Remove triangles sharing a vertex of the supertriangle.
    RemoveTriangles();

    // Assign integer values to the triangles for use by the caller.
    std::map<Triangle*,int> permute;
    typename std::set<Triangle*>::iterator iter = mTriangles.begin();
    typename std::set<Triangle*>::iterator end = mTriangles.end();
    for (i = 0; iter != end; ++iter)
    {
        tri = *iter;
        permute[tri] = i++;
    }
    permute[(Triangle*)0] = -1;

    // Put Delaunay triangles into an array (vertices and adjacency info).
    mNumSimplices = (int)mTriangles.size();
    if (mNumSimplices > 0)
    {
        mIndices = new1<int>(3*mNumSimplices);
        mAdjacencies = new1<int>(3*mNumSimplices);
        i = 0;
        iter = mTriangles.begin();
        end = mTriangles.end();
        for (/**/; iter != end; ++iter)
        {
            tri = *iter;
            mIndices[i] = tri->V[0];
            mAdjacencies[i] = permute[tri->Adj[0]];
            ++i;
            mIndices[i] = tri->V[1];
            mAdjacencies[i] = permute[tri->Adj[1]];
            ++i;
            mIndices[i] = tri->V[2];
            mAdjacencies[i] = permute[tri->Adj[2]];
            ++i;
        }
        assertion(i == 3*mNumSimplices, "Unexpected mismatch\n");

        mPathLast = -1;
        mPath = new1<int>(mNumSimplices + 1);
        memset(mPath, 0, (mNumSimplices + 1)*sizeof(int));
    }

    // Restore the vertex count to the original (discards the vertices of the
    // supertriangle).
    mNumVertices -= 3;

    iter = mTriangles.begin();
    end = mTriangles.end();
    for (/**/; iter != end; ++iter)
    {
        tri = *iter;
        delete0(tri);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Delaunay2<Real>::~Delaunay2 ()
{
    delete0(mQuery);
    delete1(mSVertices);
    delete1(mPath);
    if (mOwner)
    {
        delete1(mVertices);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>* Delaunay2<Real>::GetVertices () const
{
    return mVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
int Delaunay2<Real>::GetNumUniqueVertices () const
{
    return mNumUniqueVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& Delaunay2<Real>::GetLineOrigin () const
{
    return mLineOrigin;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& Delaunay2<Real>::GetLineDirection () const
{
    return mLineDirection;
}
//----------------------------------------------------------------------------
template <typename Real>
Delaunay1<Real>* Delaunay2<Real>::GetDelaunay1 () const
{
    assertion(mDimension == 1, "The dimension must be 1\n");
    if (mDimension != 1)
    {
        return 0;
    }

    Real* projection = new1<Real>(mNumVertices);
    for (int i = 0; i < mNumVertices; ++i)
    {
        Vector2<Real> diff = mVertices[i] - mLineOrigin;
        projection[i] = mLineDirection.Dot(diff);
    }

    return new0 Delaunay1<Real>(mNumVertices, projection, mEpsilon, true,
        mQueryType);
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay2<Real>::GetHull (int& numEdges, int*& indices)
{
    assertion(mDimension == 2, "The dimension must be 2\n");
    if (mDimension != 2)
    {
        return false;
    }

    numEdges = 0;
    indices = 0;

    // Count the number of edges that are not shared by two triangles.
    int i, numAdjacent = 3*mNumSimplices;
    for (i = 0; i < numAdjacent; ++i)
    {
        if (mAdjacencies[i] == -1)
        {
            numEdges++;
        }
    }
    assertion(numEdges > 0, "There must be at least one triangle\n");
    if (numEdges == 0)
    {
        return false;
    }

    // Enumerate the edges.
    indices = new1<int>(2*numEdges);
    int* currentIndex = indices;
    for (i = 0; i < numAdjacent; ++i)
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
int Delaunay2<Real>::GetContainingTriangle (const Vector2<Real>& p) const
{
    assertion(mDimension == 2, "The dimension must be 2\n");
    if (mDimension != 2)
    {
        return -1;
    }

    // Convert to scaled coordinates.
    Vector2<Real> scP = (p - mMin)*mScale;

    // Start at first triangle in mesh.
    int index = (mPathLast >= 0 ? mPath[mPathLast] : 0);
    mPathLast = -1;
    mLastEdgeV0 = -1;
    mLastEdgeV1 = -1;
    mLastEdgeOpposite = -1;
    mLastEdgeOppositeIndex = -1;

    // Use triangle edges as binary separating lines.
    for (int i = 0; i < mNumSimplices; ++i)
    {
        mPath[++mPathLast] = index;

        int* vertices = &mIndices[3*index];

        if (mQuery->ToLine(scP, vertices[0], vertices[1]) > 0)
        {
            index = mAdjacencies[3*index];
            if (index == -1)
            {
                mLastEdgeV0 = vertices[0];
                mLastEdgeV1 = vertices[1];
                mLastEdgeOpposite = vertices[2];
                mLastEdgeOppositeIndex = 2;
                return -1;
            }
            continue;
        }

        if (mQuery->ToLine(scP, vertices[1], vertices[2]) > 0)
        {
            index = mAdjacencies[3*index + 1];
            if (index == -1)
            {
                mLastEdgeV0 = vertices[1];
                mLastEdgeV1 = vertices[2];
                mLastEdgeOpposite = vertices[0];
                mLastEdgeOppositeIndex = 0;
                return -1;
            }
            continue;
        }

        if (mQuery->ToLine(scP, vertices[2], vertices[0]) > 0)
        {
            index = mAdjacencies[3*index + 2];
            if (index == -1)
            {
                mLastEdgeV0 = vertices[2];
                mLastEdgeV1 = vertices[0];
                mLastEdgeOpposite = vertices[1];
                mLastEdgeOppositeIndex = 1;
                return -1;
            }
            continue;
        }

        mLastEdgeV0 = -1;
        mLastEdgeV1 = -1;
        mLastEdgeOpposite = -1;
        mLastEdgeOppositeIndex = -1;
        return index;
    }

    return -1;
}
//----------------------------------------------------------------------------
template <typename Real>
int Delaunay2<Real>::GetPathLast () const
{
    return mPathLast;
}
//----------------------------------------------------------------------------
template <typename Real>
const int* Delaunay2<Real>::GetPath () const
{
    return mPath;
}
//----------------------------------------------------------------------------
template <typename Real>
int Delaunay2<Real>::GetLastEdge (int& v0, int& v1, int& v2) const
{
    v0 = mLastEdgeV0;
    v1 = mLastEdgeV1;
    v2 = mLastEdgeOpposite;
    return mLastEdgeOppositeIndex;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay2<Real>::GetVertexSet (int i, Vector2<Real> vertices[3]) const
{
    assertion(mDimension == 2, "The dimension must be 2\n");
    if (mDimension != 2)
    {
        return false;
    }

    if (0 <= i && i < mNumSimplices)
    {
        vertices[0] = mVertices[mIndices[3*i  ]];
        vertices[1] = mVertices[mIndices[3*i + 1]];
        vertices[2] = mVertices[mIndices[3*i + 2]];
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay2<Real>::GetIndexSet (int i, int indices[3]) const
{
    assertion(mDimension == 2, "The dimension must be 2\n");
    if (mDimension != 2)
    {
        return false;
    }

    if (0 <= i && i < mNumSimplices)
    {
        indices[0] = mIndices[3*i  ];
        indices[1] = mIndices[3*i + 1];
        indices[2] = mIndices[3*i + 2];
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay2<Real>::GetAdjacentSet (int i, int adjacencies[3]) const
{
    assertion(mDimension == 2, "The dimension must be 2\n");
    if (mDimension != 2)
    {
        return false;
    }

    if (0 <= i && i < mNumSimplices)
    {
        adjacencies[0] = mAdjacencies[3*i  ];
        adjacencies[1] = mAdjacencies[3*i + 1];
        adjacencies[2] = mAdjacencies[3*i + 2];
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay2<Real>::GetBarycentricSet (int i, const Vector2<Real>& p,
    Real bary[3]) const
{
    assertion(mDimension == 2, "The dimension must be 2\n");
    if (mDimension != 2)
    {
        return false;
    }

    if (0 <= i && i < mNumSimplices)
    {
        Vector2<Real> v0 = mVertices[mIndices[3*i  ]];
        Vector2<Real> v1 = mVertices[mIndices[3*i + 1]];
        Vector2<Real> v2 = mVertices[mIndices[3*i + 2]];
        p.GetBarycentrics(v0, v1, v2, bary);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
Delaunay2<Real>::Delaunay2 (const char* filename, int mode)
    :
    Delaunay<Real>(0, (Real)0, false, Query::QT_REAL),
    mVertices(0),
    mSVertices(0),
    mQuery(0),
    mPath(0)
{
    bool loaded = Load(filename, mode);
    assertion(loaded, "Cannot open file %s\n", filename);
    WM5_UNUSED(loaded);
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay2<Real>::Load (const char* filename, int mode)
{
    FileIO inFile(filename, mode);
    if (!inFile)
    {
        return false;
    }

    Delaunay<Real>::Load(inFile);

    delete0(mQuery);
    delete1(mSVertices);
    delete1(mPath);
    if (mOwner)
    {
        delete1(mVertices);
    }

    mOwner = true;
    mVertices = new1<Vector2<Real> >(mNumVertices);
    mSVertices = new1<Vector2<Real> >(mNumVertices + 3);
    mPath = new1<int>(mNumSimplices + 1);

    inFile.Read(sizeof(int), &mNumUniqueVertices);
    inFile.Read(sizeof(int), 3, mSupervertices);
    inFile.Read(sizeof(int), &mPathLast);
    inFile.Read(sizeof(int), &mLastEdgeV0);
    inFile.Read(sizeof(int), &mLastEdgeV1);
    inFile.Read(sizeof(int), &mLastEdgeOpposite);
    inFile.Read(sizeof(int), &mLastEdgeOppositeIndex);
    inFile.Read(sizeof(int), mNumSimplices + 1, mPath);

    inFile.Read(sizeof(Real), 2*mNumVertices, mVertices);
    inFile.Read(sizeof(Real), 2*(mNumVertices + 3), mSVertices);
    inFile.Read(sizeof(Real), 2, &mMin);
    inFile.Read(sizeof(Real), 2, &mScale);
    inFile.Read(sizeof(Real), 2, &mLineOrigin);
    inFile.Read(sizeof(Real), 2, &mLineDirection);

    inFile.Close();

    switch (mQueryType)
    {
    case Query::QT_INT64:
    {
        mQuery = new0 Query2Int64<Real>(mNumVertices, mSVertices);
        break;
    }
    case Query::QT_INTEGER:
    {
        mQuery = new0 Query2Integer<Real>(mNumVertices, mSVertices);
        break;
    }
    case Query::QT_RATIONAL:
    {
        mQuery = new0 Query2Rational<Real>(mNumVertices, mSVertices);
        break;
    }
    case Query::QT_REAL:
    {
        mQuery = new0 Query2<Real>(mNumVertices, mSVertices);
        break;
    }
    case Query::QT_FILTERED:
    {
        mQuery = new0 Query2Filtered<Real>(mNumVertices, mSVertices,
            mEpsilon);
        break;
    }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay2<Real>::Save (const char* filename, int mode) const
{
    FileIO outFile(filename, mode);
    if (!outFile)
    {
        return false;
    }

    Delaunay<Real>::Save(outFile);

    outFile.Write(sizeof(int), &mNumUniqueVertices);
    outFile.Write(sizeof(int), 3, mSupervertices);
    outFile.Write(sizeof(int), &mPathLast);
    outFile.Write(sizeof(int), &mLastEdgeV0);
    outFile.Write(sizeof(int), &mLastEdgeV1);
    outFile.Write(sizeof(int), &mLastEdgeOpposite);
    outFile.Write(sizeof(int), &mLastEdgeOppositeIndex);
    outFile.Write(sizeof(int), mNumSimplices + 1, mPath);

    outFile.Write(sizeof(Real), 2*mNumVertices, mVertices);
    outFile.Write(sizeof(Real), 2*(mNumVertices + 3), mVertices);
    outFile.Write(sizeof(Real), 2, &mMin);
    outFile.Write(sizeof(Real), 2, &mScale);
    outFile.Write(sizeof(Real), 2, &mLineOrigin);
    outFile.Write(sizeof(Real), 2, &mLineDirection);

    outFile.Close();
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
void Delaunay2<Real>::Update (int i)
{
    // Locate the triangle containing vertex i.
    Triangle* tri = GetContainingTriangle(i);

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

                if (adj->IsInsertionComponent(i, tri, mQuery, mSupervertices))
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
                if (IsSupervertex(v0))
                {
                    v1 = tri->V[(j+1)%3];
                    if (IsSupervertex(v1))
                    {
                        edge = (Edge*)polygon.InsertEdge(v0, v1);
                        edge->NullIndex = -1;
                        edge->Tri = 0;
                    }
                }
            }
        }
        mTriangles.erase(tri);
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
        tri = new0 Triangle(i, edge->V[0], edge->V[1]);
        mTriangles.insert(tri);

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
}
//----------------------------------------------------------------------------
template <typename Real>
typename Delaunay2<Real>::Triangle*
Delaunay2<Real>::GetContainingTriangle (int i) const
{
    // Locate which triangle in the current mesh contains vertex i.  By
    // construction, there must be such a triangle (the vertex cannot be
    // outside the supertriangle).

    Triangle* tri = *mTriangles.begin();
    const int numTriangles = (int)mTriangles.size();
    for (int t = 0; t < numTriangles; ++t)
    {
        int* vertices = tri->V;

        if (mQuery->ToLine(i, vertices[0], vertices[1]) > 0)
        {
            tri = tri->Adj[0];
            if (!tri)
            {
                break;
            }
            continue;
        }

        if (mQuery->ToLine(i, vertices[1], vertices[2]) > 0)
        {
            tri = tri->Adj[1];
            if (!tri)
            {
                break;
            }
            continue;
        }

        if (mQuery->ToLine(i, vertices[2], vertices[0]) > 0)
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
void Delaunay2<Real>::RemoveTriangles ()
{
    // Identify those triangles sharing a vertex of the supertriangle.
    std::set<Triangle*> removeTri;
    Triangle* tri;
    int j;

    typename std::set<Triangle*>::iterator iter = mTriangles.begin();
    typename std::set<Triangle*>::iterator end = mTriangles.end();
    for (/**/; iter != end; ++iter)
    {
        tri = *iter;
        for (j = 0; j < 3; ++j)
        {
            if (IsSupervertex(tri->V[j]))
            {
                removeTri.insert(tri);
                break;
            }
        }
    }

    // Remove the triangles from the mesh.
    iter = removeTri.begin();
    end = removeTri.end();
    for (/**/; iter != end; ++iter)
    {
        tri = *iter;
        for (j = 0; j < 3; ++j)
        {
            // Break the links with adjacent triangles.
            Triangle* adj = tri->Adj[j];
            if (adj)
            {
                for (int k = 0; k < 3; ++k)
                {
                    if (adj->Adj[k] == tri)
                    {
                        adj->Adj[k] = 0;
                        break;
                    }
                }
            }
        }
        mTriangles.erase(tri);
        delete0(tri);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool Delaunay2<Real>::IsSupervertex (int i) const
{
    for (int j = 0; j < 3; ++j)
    {
        if (i == mSupervertices[j])
        {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Delaunay2::Triangle
//----------------------------------------------------------------------------
template <typename Real>
Delaunay2<Real>::Triangle::Triangle (int v0, int v1, int v2)
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
bool Delaunay2<Real>::Triangle::IsInsertionComponent (int i, Triangle* adj,
    const Query2<Real>* query, const int* supervertices)
{
    if (i != Time)
    {
        Time = i;

        // Determine the number of vertices in common with the supertriangle.
        // The supertriangle vertices have indices VQ-3, VQ-2, and VQ-1, where
        // VQ is the quantity of input vertices.
        int common = 0, svIndex = -1, j;
        for (j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                if (V[j] == supervertices[k])
                {
                    ++common;
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
            relation = query->ToCircumcircle(i, V[0], V[1],
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
                v0 = V[(svIndex+1)%3];
                v1 = V[(svIndex+2)%3];
            }
            else  // common == 2
            {
                for (j = 0; j < 3; ++j)
                {
                    if (Adj[j] != 0 && Adj[j] != adj)
                    {
                        break;
                    }
                }
                v0 = V[j];
                v1 = V[(j+1)%3];
            }
            relation = query->ToLine(i, v0, v1);
        }

        IsComponent = (relation < 0);
    }

    return IsComponent;
}
//----------------------------------------------------------------------------
template <typename Real>
int Delaunay2<Real>::Triangle::DetachFrom (int adjIndex, Triangle* adj)
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
// Delaunay2::Edge
//----------------------------------------------------------------------------
template <typename Real>
Delaunay2<Real>::Edge::Edge (int v0, int v1, int nullIndex,
    Triangle* tri)
    :
    VEManifoldMesh::Edge(v0, v1),
    NullIndex(nullIndex),
    Tri(tri)
{
}
//----------------------------------------------------------------------------
template <typename Real>
VEManifoldMesh::EPtr Delaunay2<Real>::Edge::ECreator (int v0, int v1)
{
    return new0 Edge(v0, v1, 0, 0);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class Delaunay2<float>;

template WM5_MATHEMATICS_ITEM
class Delaunay2<double>;
//----------------------------------------------------------------------------
}
